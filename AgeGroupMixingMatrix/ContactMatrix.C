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


#include <string>
#include <sstream>
#include "ContactMatrix.h"

using namespace std;

bool ContactMatrix::fUseCDC = true;

string ContactMatrix::name(int ageGroup) const
{
	string rtn = "";
	if (fUseCDC)
	{
		switch (ageGroup)
		{
			case kCDCPreschool:
				rtn = "p";
				break;

			case kCDCSchool:
				rtn = "s";
				break;

			case kCDCAdult:
				rtn = "a";
				break;

			case kCDCOlder:
				rtn = "o";
				break;

			case kCDCGolden:
				 rtn = "g";
				break;

			default:
				cerr << "Unrecognized age group " << ageGroup << endl;
				exit(403);
		}
		return rtn;
	}

	switch ( ageGroup )
	{
		case 0:
			rtn = "age_00_04";
			break;

		case 1:
			rtn = "age_05_09";
			break;

		case 2:
			rtn = "age_10_14";
			break;

		case 3:
			rtn = "age_15_19";
			break;

		case 4:
			rtn = "age_20_24";
			break;

		case 5:
			rtn = "age_25_29";
			break;

		case 6:
			rtn = "age_30_34";
			break;

		case 7:
			rtn = "age_35_39";
			break;

		case 8:
			rtn = "age_40_44";
			break;

		case 9:
			rtn = "age_45_49";
			break;

		case 10:
			rtn = "age_50_54";
			break;

		case 11:
			rtn = "age_55_59";
			break;

		case 12:
			rtn = "age_60_64";
			break;

		case 13:
			rtn = "age_65_69";
			break;

// HSM: adding and adjusting cases.

		case 14:
			rtn = "age_70_74";
			break;

		case 15:
			rtn = "age_75_79";
			break;
			default:
				cerr << "Unrecognized age group " << ageGroup << endl;
				exit(404);
	}
	return rtn;
}

int ContactMatrix::ageToIndex(const string & a) const
{
	if (fUseCDC)
	{
		if (a == "p") return kCDCPreschool;
		if (a == "s") return kCDCSchool;
		if (a == "a") return kCDCAdult;
		if (a == "o") return kCDCOlder;
		if (a == "g") return kCDCGolden;
		cerr << "Unrecognized age group '" << a << "'" << endl;
		exit(404);
	}
	istringstream is(a);
	int age;
	is >> age;
	int rtn = age / 5;
	// HSM
	// if (age >= 70)
	if (age >= 75)
		rtn = kPOLYMODNumGroups - 1;
	return rtn;
}

int ContactMatrix::index(const string & a, const string & b) const
{
	int i1 = ageToIndex(a);
	int i2 = ageToIndex(b);
	return i1 * getNumGroups() + i2;
}

void ContactMatrix::print(ostream & os) const
{
	const string header("src_age,dst_age,num_contacts,total_duration,num_people");
	os << header << endl;
	for (int a = 0; a < getNumGroups(); a++)
	{
		long pop = fPopSize[a];
		for (int b = 0; b < getNumGroups(); b++)
		{
			int idx = a * getNumGroups() + b;
			os << name(a) << ',' << name(b)
			   << ',' << fData[idx].first
			   << ',' << fData[idx].second / 86400.0
			   << ',' << pop
			   << endl;
		}
	}
}
