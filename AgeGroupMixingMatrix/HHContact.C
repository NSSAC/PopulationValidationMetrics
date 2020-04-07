// Copyright 2019 Stephen Eubank, University of Virginia.

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

#include "CSVParser.h"
#include "ContactMatrix.h"

using namespace std;

typedef long countyType;
typedef long hhIdType;
typedef string myAgeType;
typedef long personIdType;

// Functions that populate the data structures above, like a SELECT
bool readPopulation(const string & fName);

map<countyType, ContactMatrix> gContacts;
ContactMatrix gState;

int main(int argc, char **argv)
{
	const bool IOTask = true;

	if (argc < 1)
	{
		cerr << "Usage: " << argv[0] << " <personFile> <networkFile>" << endl;
		exit(1);
	}

	string base(argv[1]);
	base += "-hh-cm";
	size_t start = base.find_last_of("/");
	base = base.substr(start+1, base.length()-start);

	string fName = argv[1];
	fName += ".txt";
	readPopulation(fName);

	for (auto it = gContacts.begin(); it != gContacts.end(); it++)
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

bool readPopulation(const string & popFName)
{
	CSVParser popFS(popFName);
	++popFS;
	const int idCol = popFS.getColumn("pid");
	const int hhCol = popFS.getColumn("hid");
	const int ageCol = popFS.getColumn("age_group");
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
