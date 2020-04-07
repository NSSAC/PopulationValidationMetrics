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

// mappings from an id to an instance of the appropriate object with that id
map<personIdType, pair<myAgeType, countyType> > gPeople;

// from a FIPS code to an index into an array
map<long, long> gCountyMap;

// Function that populates the gPeople map
bool readPopulation(const string & fName, bool useCDCAgeGroups);

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

	string ageGroups = config.GetAgeGroups();
	const bool useCDCAgeGroups = (ageGroups == "CDC");
	ContactMatrix::setAgeGroup(useCDCAgeGroups);

	string fName = config.GetPopFile();
	readPopulation(fName, useCDCAgeGroups);

	string base = config.GetNetworkFile();
	fName = base;
	base += "-cm";
	size_t start = base.find_last_of("/");
	base = base.substr(start+1, base.length()-start);

	CSVParser netFS(fName);
	++netFS;
	const int srcIdCol = netFS.getColumn("sourcePID");
	const int dstIdCol = netFS.getColumn("targetPID");
	const int durCol = netFS.getColumn("duration");
	long srcCounty = -1;
	vector<ContactMatrix> counts(gCountyMap.size());
	ContactMatrix state;
	long added = 0;
	while (netFS)
	{
		personIdType src = netFS.getLong(srcIdCol);
		personIdType dst = netFS.getLong(dstIdCol);
		double dur = netFS.getLong(durCol);
		myAgeType srcAge = gPeople[src].first;
		countyType county = gPeople[src].second;
		myAgeType dstAge = gPeople[dst].first;
		ContactMatrix & cm = counts[gCountyMap[county]];
		cm.addDuration(srcAge, dstAge, dur);
		state.addDuration(srcAge, dstAge, dur);
		++netFS;
		added++;
		if (added % 1000000 == 0)
			cout << "Added " << added/1000000 << " million contacts" << endl;
	}

	for (auto it = gCountyMap.begin(); it != gCountyMap.end(); it++)
	{
		stringstream oss;
		oss << it->first;
		string fName = base + "-" + oss.str();
		ofstream os(fName);
		
		const ContactMatrix & cm = counts[it->second];
		os << cm;
		os.close();
	}
	ofstream os(base);
	os << state;
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
		if (gCountyMap.find(county) == gCountyMap.end())
		{
			gCountyMap.insert(make_pair(county, gCountyMap.size()));
		}
		gPeople.insert(make_pair(pid, make_pair(age, county)));
		++popFS;
	}
	clog << "Read " << gPeople.size() << " people from '" << popFName 
	     << "'" << endl;
	return true;
}
