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
				cerr << "Don't recognize age group " << ageGroup << endl;
				exit(403);
		}
		return rtn;
	}

	switch ( ageGroup )
	{
		case 0:
			rtn = "0-4";
			break;

		case 1:
			rtn = "5-9";
			break;

		case 2:
			rtn = "10-14";
			break;

		case 3:
			rtn = "15-19";
			break;

		case 4:
			rtn = "20-24";
			break;

		case 5:
			rtn = "25-29";
			break;

		case 6:
			rtn = "30-34";
			break;

		case 7:
			rtn = "35-39";
			break;

		case 8:
			rtn = "40-44";
			break;

		case 9:
			rtn = "45-49";
			break;

		case 10:
			rtn = "50-54";
			break;

		case 11:
			rtn = "55-59";
			break;

		case 12:
			rtn = "60-64";
			break;

		case 13:
			rtn = "65-69";
			break;

// HSM: adding and adjusting cases.

		case 14:
			rtn = "70-74";
			break;

		case 15:
			rtn = "75+";
			break;
			default:
				cerr << "Don't recognize age group " << ageGroup << endl;
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
		cerr << "Don't recognize age group '" << a << "'" << endl;
		exit(404);
	}
	istringstream is(a);
	int age;
	is >> age;
	int rtn = age / 5;
	if (age >= 70)
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
