// Copyright 2013-2018 Stephen Eubank, Virginia Polytechnic Institute and State University.
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


#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <vector>
#include <list>
#include <queue>


#include "Utilities.h"
#include "BitArray.h"

using namespace std;

// #define TESTING 1

bool resetCout(const string & fname, int id)
{
	bool rtn = true;

	string name(fname);
	size_t len = name.length();
	if (id != -1)
	{
		ostringstream oss;
		if (len > 0 && name[len-1] != '/' && name[len-1] != '.' && name[len-1] != '-')
			oss << '-';
		oss << id;
		name += oss.str();
	}

	ofstream *ofc = new ofstream(name, ios::app);  // pointer so it doesn't go out of scope and get destroyed
  if (ofc->good())
		cout.rdbuf(ofc->rdbuf());
	else
  {
    rtn = false;
    cerr << "Cannot create file '" << name << "' for writing " << endl;
  }
	return rtn;
}

bool resetCerr(const string & fname, int id)
{
	bool rtn = true;

	string name(fname);
	size_t len = name.length();
	if (id != -1)
	{
		ostringstream oss;
		if (len > 0 && name[len-1] != '/' && name[len-1] != '.' && name[len-1] != '-')
			oss << '-';
		oss << id;
		name += oss.str();
	}
	len = name.length();
	if (len > 0 && name[len-1] != '.' && name[len-1] != '/' && name[len-1] != '-')
		name += '.';
	name += "err";

	ofstream *ofc = new ofstream(name);  // pointer so it doesn't go out of scope and get destroyed
  if (ofc->good())
		cerr.rdbuf(ofc->rdbuf());
	else
  {
    rtn = false;
    cerr << "Cannot create file '" << name << "' for writing " << endl;
  }
	return rtn;
}

bool resetClog(const string & fname, int id)
{
	bool rtn = true;

	string name(fname);
	size_t len = name.length();
	if (id != -1)
	{
		ostringstream oss;
		if (len > 0 && name[len-1] != '/' && name[len-1] != '.' && name[len-1] != '-')
			oss << '-';
		oss << id;
		name += oss.str();
	}
	len = name.length();
	if (len > 0 && name[len-1] != '/' && name[len-1] != '.' && name[len-1] != '-')
		name += '.';
	name += "log";

	ofstream *ofc = new ofstream(name);  // pointer so it doesn't go out of scope and get destroyed
  if (ofc->good())
		clog.rdbuf(ofc->rdbuf());
	else
  {
    rtn = false;
    cerr << "Cannot create file '" << name << "' for writing " << endl;
  }
	return rtn;
}

bool fileExists(const string & fname)
{
    struct stat fileInfo;
    return stat(fname.c_str(), &fileInfo) == 0;
}

bool fileIsEmpty(const string & fname)
{
    struct stat fileInfo;
	if (! fileExists(fname) )
		return true;

    stat(fname.c_str(), &fileInfo);
	return (fileInfo.st_size == 0);
}

bool fileIsReadable(const string & fname)
{
	ifstream my_file(fname.c_str());
	return (my_file.good());
}

bool fileIsWritable(const string & fname)
{
	bool empty = fileIsEmpty(fname);  // or doesn't exist
	ofstream my_file(fname.c_str(), ios::app);
	bool rtn = my_file.good();
	if (rtn && empty) 
		remove(fname.c_str());

	return rtn;
}

uint32_t adler32(const char *data, size_t len){
	const int MOD_ADLER = 65521;
	uint32_t a = 1, b = 0;
	size_t idx;
	for( idx = 0; idx < len; ++idx ){
		a = (a + data[idx] ) % MOD_ADLER;
		b = (a + b ) % MOD_ADLER;
	}

	return (b << 16 ) | a;
}

// uniform in the closed interval [0,1]
double myDoubleRandom(void)
{
        static const double invMaxRandom = 1.0 / RAND_MAX;
        double rtn = 0.0;
        rtn = ((double) random()) * invMaxRandom;
        return rtn;
}

long pickRandomIndex(const vector<double> & weights, double sumWeights)
{
	double ran = myDoubleRandom();
	// clog << "Ran: " << ran << " numWeights: " << weights.size() << " sum: " << sumWeights << endl;
	ran *= sumWeights;
	for (long i = 0; i < weights.size(); i++)
	{
		// clog << "ran " << ran << " weight[i] " << weights[i] << endl;
		ran -= weights[i];
		if (ran <= 0.0)
			return i;
	}
	return -1;
}

