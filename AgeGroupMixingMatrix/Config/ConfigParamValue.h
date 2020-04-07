// Copyright 2016-2018 Stephen Eubank, Virginia Polytechnic Institute and State University.
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


// Derive a class from this one if you have a configuration file parameter 
// whose values should be chosen from a restricted set of strings.
// It handles instantiation so that static variables are there when you need them.

#ifndef CONFIG_PARAM_VALUE
#define CONFIG_PARAM_VALUE 1

#include <string>
#include <vector>
#include <map>

using namespace std;

// a singleton class that is read-only, but contains many static variables whose instatntiation order can't be left to the linker

class ConfigParamValue
{
	public:

		static const ConfigParamValue & getInstance(void);  // re-declaration in derived class will hide this, which is what we want

		const string & help(const string & name) const;
		const string & paramHelp(const string & name) const;
		const string & getName(int i)  const {return fNames[i];};
		int getIndex(const string & name) const;

	protected:

		ConfigParamValue(void);

		vector<string> fNames;
		map<string, string> fHelpStrings;
		map<string, string> fParamHelpStrings;
};

#endif
