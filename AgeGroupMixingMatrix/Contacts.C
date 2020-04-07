// Copyright 2020 Stephen Eubank, University of Virginia.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <math.h>

#include "Utilities.h"
#include "ContactErr.h"
#include "CSVParser.h"
#include "ContactMatrix.h"
#include "Config/ContactConfig.h"

using namespace std;

typedef long countyType;
typedef string myAgeType;
typedef long personIdType;
typedef long hhIdType;

// mappings from an id to an instance of the appropriate object with that id
map<personIdType, pair<myAgeType, countyType> > gPeople;

// from a FIPS code to a ContactMatrix
map<countyType, ContactMatrix> gCounts;

// Function that populates the gPeople map
bool readPopulation(const string & fName, bool useCDCAgeGroups);

map<countyType, ContactMatrix> gContacts;
ContactMatrix gState;

// Function that populates the gContacts network if there's no network file
bool readAtHomeNetwork(const string & fName, bool useCDCAgeGroups);

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " <configFile>" << endl;
		ContactConfig & config = *ContactConfig::getInstance();
		cerr << config;
		exit(1);
	}

	string name = argv[1];
	size_t pos = name.rfind("/");
	if (pos != string::npos)
		name = name.substr(pos);

	ContactConfig & config = *ContactConfig::getInstance(argv[1]);  // forces assignment of key values
	const bool IOTask = true;
	if (! config.IsValid(IOTask) )	 // munges filenames, too
        {
		cerr << "Invalid configuration file '" << argv[1] << "'" << endl;
		exit(kBadConfig);
	}

	ContactErr::gVerbosity = ContactConfig::GetVerbosity();
	const ContactConfigStrings & CCS(ContactConfigStrings::getInstance());
	Param<string> * cfp = (Param<string> *) config.getParam(CCS.ConfigFileKey);  
	cfp->SetStringVal(name);

	string outFName = config.GetOutputFile();   // after IsValid, has a reasonable directory + file
	const int useId = -1;
	resetClog(outFName, useId);
	resetCerr(outFName, useId);

	clog << config << endl;

	string ageGroups = config.GetAgeGroups();
	const bool useCDCAgeGroups = (ageGroups == "CDC");
	ContactMatrix::setAgeGroup(useCDCAgeGroups);

	string fName = config.GetPopFile();
	string netFile = config.GetNetworkFile();
	const bool atHome = (netFile.length() == 0);
	if (atHome)
	{
		readAtHomeNetwork(fName, useCDCAgeGroups);
        	for (auto it = gContacts.begin(); it != gContacts.end(); it++)
        	{
                	stringstream oss;
                	oss << it->first;
                	string fName = netFile + "-" + oss.str();
                	ofstream os(fName);
	
                	const ContactMatrix & cm = it->second;
                	os << cm;
                	os.close();
        	}
        	ofstream os(netFile);
        	os << gState;
        	os.close();
		return 0;
	}

	readPopulation(fName, useCDCAgeGroups);

	CSVParser netFS(netFile);
	++netFS;
	const int srcIdCol = netFS.getColumn("sourcePID");
	const int dstIdCol = netFS.getColumn("targetPID");
	const int durCol = netFS.getColumn("duration");
	long srcCounty = -1;
	long added = 0;
	while (netFS)
	{
		personIdType src = netFS.getLong(srcIdCol);
		personIdType dst = netFS.getLong(dstIdCol);
		double dur = netFS.getLong(durCol);
		myAgeType srcAge = gPeople[src].first;
		countyType county = gPeople[src].second;
		myAgeType dstAge = gPeople[dst].first;
		ContactMatrix & cm = gCounts[county];
		cm.addDuration(srcAge, dstAge, dur);
		gState.addDuration(srcAge, dstAge, dur);
		++netFS;
		added++;
		if (added % 1000000 == 0)
			cout << "Added " << added/1000000 << " million contacts" << endl;
	}

	// size_t start = netFile.find_last_of("/");
	// netFile = netFile.substr(start+1, netFile.length()-start);
	string base = outFName;
	base += "-cm";
	for (auto it = gCounts.begin(); it != gCounts.end(); it++)
	{
		stringstream oss;
		oss << it->first;
		string fName = base + "-" + oss.str();
		ofstream os(fName);
		
		const ContactMatrix & cm = it->second;
		os << cm;
		os.close();
	}
	ofstream os(base);
	os << gState;
	os.close();

	return 0;
}

bool readPopulation(const string & popFName, bool useCDCAgeGroups)
{
	CSVParser popFS(popFName);
	++popFS;
	const int idCol = popFS.getColumn("pid");
	const int ageCol = (useCDCAgeGroups) ? popFS.getColumn("age_group") : popFS.getColumn("age");
	const int fipsCol = popFS.getColumn("county_fips");
	while (popFS)
	{
		personIdType pid = popFS.getLong(idCol);
		myAgeType age = popFS[ageCol];
		countyType county = popFS.getLong(fipsCol);
		if (! popFS)
			break;
		ContactMatrix & cm = gCounts[county];
		cm.addPerson(age);
		gState.addPerson(age);
		gPeople.insert(make_pair(pid, make_pair(age, county)));
		++popFS;
	}
	clog << "Read " << gPeople.size() << " people from '" << popFName 
	     << "'" << endl;
	return true;
}


bool readAtHomeNetwork(const string & popFName, bool useCDCAgeGroups)
{
	CSVParser popFS(popFName);
	++popFS;
	const int idCol = popFS.getColumn("pid");
	const int hhCol = popFS.getColumn("hid");
	const int ageCol = (useCDCAgeGroups) ? popFS.getColumn("age_group") : popFS.getColumn("age");
	const int fipsCol = popFS.getColumn("county_fips");
	hhIdType prev = -1;
	vector<myAgeType> ages(100);
	int numInHH = 0;
	countyType county = -1;
	while (popFS)
	{
		personIdType pid = popFS.getLong(idCol);
		hhIdType hhid = popFS.getLong(hhCol);
		myAgeType age = popFS[ageCol];
		if (! popFS || hhid != prev)
		{
			ContactMatrix & cm = gContacts[county];
			for (int i=0; i<numInHH; i++)
			{
				cm.addPerson(ages[i]);
				gState.addPerson(ages[i]);
				for (int j=i+1; j<numInHH; j++)
				{
					cm.addCount(ages[i], ages[j]);
					gState.addCount(ages[i], ages[j]);
				}
			}
			prev = hhid;
			numInHH = 0;
		}
		if (! popFS)
			break;
		county = popFS.getLong(fipsCol);
		ages[numInHH] = age;
		numInHH++;
		++popFS;
	}
	return true;
}
