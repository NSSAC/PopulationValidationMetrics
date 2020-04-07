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


#include <stdlib.h>

#include "ConfigParamValue.h"
// #include "ReliabilityConfig.h"

const string & ConfigParamValue::help(const string & name) const
{
	static string s("Undocumented");
	map<string, string>::const_iterator it = fHelpStrings.find(name);
	if (it != fHelpStrings.end())
		return it->second;

	return s;
}

const string & ConfigParamValue::paramHelp(const string & name) const
{
	static string s("Undocumented");
	map<string, string>::const_iterator it = fParamHelpStrings.find(name);
	if (it != fParamHelpStrings.end())
		return it->second;

	return s;
}

const ConfigParamValue & ConfigParamValue::getInstance(void)
{
	static ConfigParamValue * fInstance = 0;

	if (fInstance == 0)
		fInstance = new ConfigParamValue;

	return *fInstance;
}

ConfigParamValue::ConfigParamValue(void)
{
}

int ConfigParamValue::getIndex(const string & name) const
{
	for (int i=0; i<fNames.size(); i++)
	{
		if (fNames[i] == name)
			return i;
	}
	return -1;
}
