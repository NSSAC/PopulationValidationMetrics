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

#ifndef CONTACTMATRIX_H
#define CONTACTMATRIX_H 1

#include <string>
#include <iostream>
#include <vector>

using namespace std;

// Need to setAgeGroup() before constructing any ContactMatirx objects!

class ContactMatrix {
	public :
	ContactMatrix(void) : fData(getNumGroups() * getNumGroups()), fPopSize(getNumGroups()) 
		{for (int i=0; i<fData.size(); i++) {fData[i] = make_pair(0, 0.0);}};

	void addPerson(const string & a)
		{fPopSize[ageToIndex(a)]++;};
	void addCount(const string & a, const string & b, long count = 0)
		{fData[index(a,b)].first += count;};

	void addDuration(const string & a, const string & b, double dur = 86400.0)
		{int idx = index(a,b); fData[idx].first++; fData[idx].second += dur;};

	long count(const string & a, const string & b) const {return fData[index(a,b)].first;};
	long countAll(void) const 
		{int rtn=0; for (int i=0; i<fData.size(); i++) {rtn += fData[i].first;} return rtn;};

	double duration(const string & a, const string & b) const {return fData[index(a,b)].second;};
	
	void print(ostream & os) const;

	static void setAgeGroup(bool CDC = true) {fUseCDC = CDC;};

	protected :

	vector<pair<long, double> > fData;
	vector<long> fPopSize;

	string name(int ageGroup) const;
	int ageToIndex(const string & a) const;
	int index(const string & a, const string & b) const;

	enum {kCDCUnknown=-1, kCDCPreschool, kCDCSchool, kCDCAdult, kCDCOlder, kCDCGolden, kCDCNumGroups};
	enum {kPOLYMODUnknown=-1, kPOLYMODNumGroups=15};
	static bool fUseCDC;  // hack alert: OK for two age group schemes, but unwieldy for more

	static int getNumGroups(void) {return (fUseCDC) ? kCDCNumGroups : kPOLYMODNumGroups;};
};

inline ostream & operator<<(ostream & os, const ContactMatrix & cm)
{ cm.print(os); return os; }

#endif
