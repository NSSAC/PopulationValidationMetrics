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

#include <sys/stat.h>
#include <errno.h>

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string.h>

#include "ConfigFile.h"
#include "ContactConfig.h"
#include "ContactConfigDefaults.h"
#include "ContactConfigToolTips.h"

using namespace std;

ContactConfigStrings & ContactConfig::fCCS(ContactConfigStrings::getInstance());

map<string, vector<string> > ContactConfig::fValStrings;
map<string, string>          ContactConfig::ConfigNameToKey;

map<string, string>      ContactConfig::fParamNames;
vector<string>           ContactConfig::fGroups;
map<string, BaseParam *> ContactConfig::fParams;
vector<string>           ContactConfig::fOrder;
int                      ContactConfig::fRefCount = 1;   // the static one

ContactConfig::ContactConfig(void)
{
	Initialize();
	fRefCount++;
}

ContactConfig::~ContactConfig() 
{
	fRefCount--;
	if (fRefCount > 0)
		return;

/* This screws everything up for no apparent reason!!!
	for (auto elem : fParams)
	{
		delete elem.second;
	}
*/
}

const string & ContactConfig::getKey(const string & name)
{
	if (ConfigNameToKey.find(name) == ConfigNameToKey.end())
	{
		cerr << "ERROR: unknown key " << name << " requested" << endl;
		ConfigNameToKey[name] = name;
	}
	return ConfigNameToKey[name];
}

vector<string> ContactConfig::AllowedValues(const string & name)
{
	return fValStrings[name];
}

int ContactConfig::valueToInt(const string & name, const string & val)
{
	if (fValStrings.find(name) == fValStrings.end())
		return -1;

	vector<string> vals = fValStrings[name];
	for (int i=0; i<vals.size(); i++)
	{
		if (vals[i] == val)
			return i;
	}
	return -1;
}

void ContactConfig::Initialize(void)
{
	static bool fInitialized = false;

	if (fInitialized)
		return;

	fInitialized = true;

	bool req = true;
	bool notReq = false;

	Param<string> * sp;
	// Param<bool> * bp;
	// Param<float> * fp = 0;
	Param<int> * ip = 0;
	// Param<vector<float> > * vfp = 0;
	// Param<vector<int> > * vip = 0;

	string fileNames("File names");
	string tasks("Tasks");
	string fieldNames("Field names");
	// string formats("Formats");
	string unexposed ("unexposed");  // won't show up in the user interface

	sp = new Param<string>(fCCS.ConfigFileKey, req);
	sp->SetGroup(fileNames);
	addParam(sp);
	sp->SetHint(kConfigFileToolTip);
	
	sp = new Param<string>(fCCS.NetworkFileKey, notReq, kDefNetworkFile);
	sp->SetGroup(fileNames);
	addParam(sp);
	sp->SetHint(kNetworkFileToolTip);

	sp = new Param<string>(fCCS.PopFileKey, req);
	sp->SetGroup(fileNames);
	addParam(sp);
	sp->SetHint(kPopFileToolTip);

	ip = new Param<int>(fCCS.VerbosityKey, notReq, kDefVerbosity);
	ip->SetGroup(tasks);
	addParam(ip); 
	ip->SetHint(kVerbosityToolTip);

	sp = new Param<string>(fCCS.OutputDirectoryKey, notReq);
	sp->SetGroup(fileNames);
	addParam(sp);
	sp->SetHint(kOutputDirectoryToolTip);

	// sp = new Param<string>(fCCS.HHIdFieldNameKey, notReq, kDefHHIdFieldName);
	// sp->SetGroup(fieldNames);
	// addParam(sp);
	// sp->SetHint(kHHIdFieldToolTip);

	// sp = new Param<string>(fCCS.PersonIdFieldNameKey, notReq, kDefPersonIdFieldName);
	// sp->SetGroup(fieldNames);
	// addParam(sp);
	// sp->SetHint(kPersonIdFieldToolTip);

	// sp = new Param<string>(fCCS.AgeFieldNameKey, notReq, kDefAgeFieldName);
	// sp->SetGroup(fieldNames);
	// addParam(sp);
	// sp->SetHint(kAgeFieldToolTip);

	// sp = new Param<string>(fCCS.GenderFieldNameKey, notReq, kDefGenderFieldName);
	// sp->SetGroup(fieldNames);
	// addParam(sp);
	// sp->SetHint(kGenderFieldToolTip);

	// sp = new Param<string>(fCCS.GradeFieldNameKey, notReq, kDefGradeFieldName);
	// sp->SetGroup(fieldNames);
	// addParam(sp);
	// sp->SetHint(kGradeFieldToolTip);

	sp = new Param<string>(fCCS.OutputFileKey);
	sp->SetGroup(fileNames);
	addParam(sp);
	sp->SetHint(kOutputFileToolTip);

	sp = new Param<string>(fCCS.AgeGroupKey);
	sp->SetGroup(fileNames);
	vector<string> agePossibles(2);
	agePossibles[0] = "CDC";
	agePossibles[1] = "PolyMod";
	sp->SetPossibles(agePossibles);
	sp->SetDefault("CDC");
	addParam(sp);
	sp->SetHint(kAgeGroupToolTip);

	sp = new Param<string>(fCCS.ConfigVersionKey, notReq, CURRENT_VERSION);
	sp->SetGroup(unexposed);
	addParam(sp);
	sp->SetHint(kConfigVersionToolTip);
	sp->SetUnexposed();

  // instantiating these things *and using them* makes sure all their static strings are defined and inserted into the list of parameters
  // const ModeConfig & MC(ModeConfig::getInstance()); MC.noOp();

}

ContactConfig::ContactConfig(const char *F_Config) 
{
	Initialize();

	if (F_Config == 0 || strncmp(F_Config, "", 1) == 0)
		return;

	Param<string> * cf = ((Param<string> *)getParam(fCCS.ConfigFileKey));
	cf->SetStringVal(F_Config);
	if (! cf->IsReadable())
	{
		cerr << "ERROR: Configuration file '" << F_Config << "' is not readable" << endl;
	}

	string defOutputFile(F_Config);
	size_t pos = defOutputFile.rfind('/');
	if (pos != string::npos)
	{
		defOutputFile = defOutputFile.substr(pos);
	}
	defOutputFile += "-out";
	getParam(fCCS.OutputFileKey)->SetStringVal(defOutputFile);

  ifstream IFS_Config(F_Config, ios::in);
  IFS_Config >> (*this);

  IFS_Config.close();
}

void ContactConfig::addParam(BaseParam * bp) 
{
	if (fParams.find(bp->GetName()) != fParams.end())
	{
		cerr << "ERROR: adding parameter '" << bp->GetName() << "' more than once." << endl;
		return;
	}
	fParams[bp->GetName()] = bp;
	fOrder.push_back(bp->GetName());
	for (int i=0; i<fGroups.size(); i++)
	{
		if (fGroups[i] == bp->GetGroup())
			return;
	}
	fGroups.push_back(bp->GetGroup());
}

BaseParam * ContactConfig::getParam(const string & s)
{
	map<string, BaseParam *>::const_iterator it = fParams.find(s);
	if (it == fParams.end())
	{
		cerr << "ERROR: requested parameter " << s << " unknown" << endl;
		return new Param<int>(s, true);
	}
	return it->second;
}

bool ContactConfig::IsValid(bool testFileAccess) {
  bool Valid = true;

	map<string, BaseParam *>::iterator it;

	for (it = fParams.begin(); it != fParams.end(); it++)
	{
		if (! (it->second)->IsValid())
		{
			Valid = false;
			cerr << "Value '" << it->second->GetStringVal() << "' for key " << it->second->GetKey() 
			     << " is invalid " << endl;
		}
	}

	BaseParam *bp = getParam(fCCS.PopFileKey);
	if (testFileAccess &&  ! bp->IsReadable() )
	{
		cerr << "population file '" << bp->GetStringVal() << "' is not readable" << endl;
		Valid = false;
	}

	bp = getParam(fCCS.NetworkFileKey);
	if (bp->GetStringVal().length() > 0 && testFileAccess &&  ! bp->IsReadable() )
	{
		cerr << "network file '" << bp->GetStringVal() << "' is not readable" << endl;
		Valid = false;
	}

	bp = getParam(fCCS.OutputFileKey);
	string fname = bp->GetStringVal();
	bp = getParam(fCCS.OutputDirectoryKey);
	string dname = bp->GetStringVal();
	if (fname[0] != '/')
	{
		if (dname.length() == 0)
		{
			dname = "./";
		}
		else if (dname[dname.length()-1] != '/')
		{
			dname += "/";
		}
 	}
	bp->SetStringVal("");
	if (testFileAccess)   // IsWritable doesn't handle directories correctly
	{
		if (dname.length() != 0 && mkdir(dname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
		{
			if (errno != EEXIST)
			{
				int err = errno;
				cerr << "Warning: output directory '" << dname 
				     << "' does not exist and cannot be created. Error: " 
				     << strerror(err) << endl;
				Valid = false;
			}
		}
	}

	fname = dname + fname;
	bp = getParam(fCCS.OutputFileKey);
	bp->SetStringVal(fname);
	cerr << "Output file '" << fname << "'" << endl;
	if (testFileAccess && ! bp->IsWritable() )
	{
		cerr << "Can't write to output file '" << bp->GetStringVal() << "'" << endl;
		Valid = false;
	}

	return Valid;
}

/*
void ContactConfig::printXSLT(ostream & os)
{
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	os << "<?xml-stylesheet type=\"text/xsl\" href=\"config.xsl\"?>" << endl;
	os << "<paramlist>" << endl;
	for (vector<string>::const_iterator it = fGroups.begin(); it != fGroups.end(); it++)
	{
		// if (*it == "unexposed")
			// continue;

		os << "<group>" << endl;
		os << "<groupname>" << *it << "</groupname>" << endl;
		for (vector<string>::const_iterator key = fOrder.begin(); key != fOrder.end(); key++)
		{
			map<string, BaseParam *>::const_iterator it2 = fParams.find(*key);
			BaseParam * bp = it2->second;
			if (bp->GetGroup() == *it && bp->IsRequired())
				bp->printXSLT(os);
		}
		for (vector<string>::const_iterator key = fOrder.begin(); key != fOrder.end(); key++)
		{
			map<string, BaseParam *>::const_iterator it2 = fParams.find(*key);
			BaseParam * bp = it2->second;
			if (bp->GetGroup() == *it && ! bp->IsRequired())
				bp->printXSLT(os);
		}
		os << "</group>" << endl;
	}
	os << "</paramlist>" << endl;
}
*/

void ContactConfig::print(ostream & os)
{
	os << "#  ======= Required parameters ========" << endl;
	os << endl;

	map<string, BaseParam *>::const_iterator it;
	for (it = fParams.begin(); it != fParams.end(); it++)
	{
		if (it->second->IsRequired() && ! it->second->HasDefault())
		{
			it->second->print(os); os << endl;
		}
	}

	os << endl;
	os << "#  ======= Default parameters overridden ========" << endl;
	os << endl;
	for (it = fParams.begin(); it != fParams.end(); it++)
	{
		if (it->second->HasDefault() && ! it->second->IsDefault())
		{
			it->second->print(os); os << endl;
		}
	}

	os << endl;
	os << "#  ======= Default parameters ========" << endl;
	os << endl;
	for (it = fParams.begin(); it != fParams.end(); it++)
	{
		if (it->second->HasDefault() && it->second->IsDefault())
		{
			it->second->print(os); os << endl;
		}
	}

	os << endl;
	os << "#  ======= Optional parameters set ========" << endl;
	os << endl;
	for (it = fParams.begin(); it != fParams.end(); it++)
	{
		if ((! it->second->IsRequired()) && (! it->second->HasDefault()) && (it->second->IsSet()) )
		{
			it->second->print(os); os << endl;
		}
	}

	os << endl;
	os << "#  ======= Optional parameters not set ========" << endl;
	os << endl;
	for (it = fParams.begin(); it != fParams.end(); it++)
	{
		if ((! it->second->IsRequired()) && (! it->second->HasDefault()) && (! it->second->IsSet()) )
		{
			it->second->print(os); os << endl;
		}
	}
	os << endl;
	os << "EndConfigFile";      // FIX: the default "sentry" string in ConfigFile.h. Need a better way to pass it
}

struct upper {
  int operator()(int c)
  {
    return std::toupper((unsigned char)c);
  }
};
 
ostream& operator<<(ostream& os, const ContactConfig& Config) 
{
	Config.print(os);
	return os;
}

istream& operator>>(istream& is, ContactConfig& Config)
{
	Config.read(is);
	return is;
}

void ContactConfig::read(istream & is)
{
	ConfigFile *GenericConfig = new ConfigFile();
  	is >> (*GenericConfig);

	map<string, BaseParam *>::iterator it;
	for (it = fParams.begin(); it != fParams.end(); it++)
	{
   		string val;
   		GenericConfig->readInto(val, it->second->GetKey());
		it->second->SetStringVal(val);
	}

	delete GenericConfig;
}

int ContactConfig::GetIntParam(const string & s)
{
	// well you're a very trusting soul, aren't you!
	Param<int> * p = ((Param<int> *) getParam(s));
	return p->Get();
}

bool ContactConfig::GetBoolParam(const string & s)
{
	Param<bool> * p = ((Param<bool> *) getParam(s));
	return p->Get();
}

float ContactConfig::GetFloatParam(const string & s)
{
	Param<float> * p = ((Param<float> *) getParam(s));
	return p->Get();
}

int ContactConfig::GetEnumParam(const string & s)
{
	return getParam(s)->GetEnumValue();
}

string ContactConfig::GetStringParam(const string & s)
{
	return getParam(s)->GetStringVal();
}


vector<float> ContactConfig::GetFloatVecParam(const string & s)
{
	Param<vector<float> > * p = ((Param<vector<float> > *) getParam(s));
	return p->Get();
}

vector<int> ContactConfig::GetIntVecParam(const string & s)
{
	Param<vector<int> > * p = ((Param<vector<int> > *) getParam(s));
	return p->Get();
}

#ifdef Test_ContactConfig

int main (int argc, char *argv[]) {
  ContactConfig::Initialize();
  ContactConfig config;
  cout << config;
  return 0;
}

#endif
