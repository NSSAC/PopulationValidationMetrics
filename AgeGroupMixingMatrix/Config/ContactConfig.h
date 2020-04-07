// Copyright 2014-2018 Stephen Eubank, Virginia Polytechnic Institute and State University.
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

#ifndef CONTACT_CONFIG_H
#define CONTACT_CONFIG_H

// (key, value) pairs, where key string is easy to localize, and value may be limited to a fixed set of strings

#include <set>
#include <map>
#include <string>
#include <stdint.h>

#include "ConfigFile.h"
#include "ConfigParam.h"
#include "ContactConfigStrings.h"

using namespace std;

#define CURRENT_VERSION "2016"

// An entirely static class, instead of having a global object

class ContactConfig {
 public:

	static ContactConfig * getInstance(const char * fName = 0)
		{return (fName) ? new ContactConfig(fName) : new ContactConfig;};

	static void addParam(BaseParam * p);
	static BaseParam * getParam(const string & s);

	static bool IsValid(bool testFileAccess = true);

	friend ostream& operator<<(ostream& OS_, const ContactConfig& Config);
	friend istream& operator>>(istream& IS_, ContactConfig& Config);
 
	static void read(istream & IS_);

	static void print(ostream & os);
	// static void printHTML(ostream & os);
	// static void printXSLT(ostream & os);

	static const string & getKey(const string & name);

	static int valueToInt(const string & name, const string & val);
	static const string & valueToString(const string & name, const string & val);
	static void setStringValues(const string & name, const vector<string> & m)
			{fValStrings.insert(make_pair(name, m));}

	static vector<string> AllowedValues(const string & name);

	static string GetStringParam(const string & nm);
	static bool   GetBoolParam(const string & nm);
	static int    GetIntParam(const string & nm);
	static int    GetEnumParam(const string & nm);
	static float  GetFloatParam(const string & nm);
	static vector<float>  GetFloatVecParam(const string & nm);
	static vector<int>    GetIntVecParam(const string & nm);
	static vector<int> GetEdgeIdVecParam(const string & nm);

	// commonly used keys, for convenience
	static string GetConfigFile(void) {return GetStringParam(fCCS.ConfigFileKey);};
	static string GetOutputDirectory(void) {return GetStringParam(fCCS.OutputDirectoryKey);};
	static string GetNetworkFile(void)       {return GetStringParam(fCCS.NetworkFileKey);};
	static string GetPopFile(void) {return GetStringParam(fCCS.PopFileKey);};
	static string GetOutputFile(void)  {return GetStringParam(fCCS.OutputFileKey);};

	// for parsing Person files
	static string GetHHIdFieldName(void)     {return GetStringParam(fCCS.HHIdFieldNameKey);};
	static string GetPersonIdFieldName(void) {return GetStringParam(fCCS.PersonIdFieldNameKey);};
	static string GetAgeGroups(void)      {return GetStringParam(fCCS.AgeGroupKey);};
	static string GetAgeFieldName(void)      {return GetStringParam(fCCS.AgeFieldNameKey);};
	static string GetGenderFieldName(void)   {return GetStringParam(fCCS.GenderFieldNameKey);};
	static string GetGradeFieldName(void)   {return GetStringParam(fCCS.GradeFieldNameKey);};

	static int GetVerbosity(void)           {return GetIntParam(fCCS.VerbosityKey);};
	
	static const vector<string> GetGroups(void) {return fGroups;};
	static const vector<string> GetOrder(void) {return fOrder;};

 private:

	~ContactConfig();
	ContactConfig();
	ContactConfig(const char *F_Config);

	void Initialize(void);

	static ContactConfigStrings & fCCS;

	static map<string, string> ConfigNameToKey;
	static map<string, vector<string> > fValStrings;

	static map<string, string> fParamNames;  
	static vector<string> fGroups;
	static map<string, BaseParam *> fParams;
	static vector<string> fOrder;            // saves order in which parameters were added

	static int fRefCount;

};
#endif
