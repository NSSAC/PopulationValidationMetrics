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


#ifndef UTILITIES_H
#define UTILITIES_H 1

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string.h>     // for memcpy
#include <cmath>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <vector>     
#include <set>     
#include <list>
#include <queue>

#include "BitArray.h"
// class BitArray;

// #include "ReliabilityTypes.h"

using namespace std;

// Change the buffers of cout, etc. to point to files with the given name for the duration of the program
// You don't need to pass around ostreams all over the place
bool resetCout(const string & fname, int id=-1);
bool resetCerr(const string & fname, int id=-1);
bool resetClog(const string & fname, int id=-1);

// file testing
bool fileExists(const string & fname);
bool fileIsEmpty(const string & fname);
bool fileIsReadable(const string & fname);
bool fileIsWritable(const string & fname);

uint32_t adler32(const char *data, size_t len);

double myDoubleRandom(void);  // uniform in the closed interval [0,1]

long pickRandomIndex(const vector<double> & weights, double sumWeights);
#endif
