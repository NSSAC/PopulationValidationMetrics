// Copyright 2015-2018 Stephen Eubank, Virginia Polytechnic Institute and State University.
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

#include <fstream>
#include <iostream>
#include <sstream>

#include "ConfigParam.h"
// #include "ReliabilityConfig.h"
#include "../Utilities.h"

using namespace std;

ostream & operator<<(ostream & os, BaseParam & bp)
{ bp.print(os); return(os); }

template<>
void Param<string>::SetValFromString(void)
{
  fSet = true;
}


BaseParam::BaseParam(const string & name, const string & typNam, bool req, const string & val)
	: fStringVal(val), fDefaultStringVal(val), fTypeName(typNam), fName(name), fKey(name), fHint(""), fHelp(""), fGroup("Miscellaneous"), 
	  fParamExposed(true), fRequired(req), fSet(false), fDefaulted(false), fControlledVocab(false), 
		fMinSet(false), fMaxSet(false)
{ }

void BaseParam::SetHint(const string & hint)
{
	fHint += ": ";
	fHint += hint;
}

void BaseParam::SetHelp(const string & help)
{
	fHelp = help;
}

void BaseParam::SetGroup(const string & group)
{
	fGroup = group;
}

void BaseParam::SetStringVal(const string & s)
{
	if (s.length() == 0)
		return;

	// cerr << "Setting parameter '" << fKey << "' with string '" << s << "'" << endl;
	if (fControlledVocab && (GetEnumValue(s) == -1) )
	{
		cerr << "WARNING: attempted to set parameter '" << fKey << "' with unknown value '" << s << "'" << endl;
		cerr << "\t allowed values are:" << endl;
		for (vector<string>::const_iterator it = fPossibles.begin(); it != fPossibles.end(); it++)
		{
			cerr << "\t\t'" << *it << "'" << endl;
		}
		cerr << "Reset to " << fStringVal << endl;
	}
	else
	{
		fStringVal = s;
		if (fTypeName != "string")  // I don't know why pointer and virtual functions quit working
			SetValFromString();
  	fSet = true;
	}
}

void BaseParam::SetPossibles(const vector<string> & possibles)
{
	if (possibles.size() == 0)
		return;

	fPossibles = possibles;
	fControlledVocab = true;
	fExposed = BitArray(fPossibles.size());
	fExposed.Sett();
}

void BaseParam::AddPossible(const string & possible, bool expose)
{
	fPossibles.push_back(possible);
	fControlledVocab = true;
	int num = fPossibles.size();
	fExposed.Grow(num);
	if (expose)
	{
		fExposed.Sett(num-1);
	}
}

bool BaseParam::IsReadable(bool quiet) const
{
	if (! IsSet())
		return false;

	bool rtn = fileIsReadable(fStringVal);
	if (! rtn && ! quiet)
	{
		cerr << "Cannot read from " << fKey << " '" << fStringVal << "'" << endl;
	}
	return rtn;
}

bool BaseParam::IsWritable(void) const
{
	if (! IsSet())
		return false;

	bool rtn = fileIsWritable(fStringVal);
	ofstream os(fStringVal.c_str());
	if (! rtn)
	{
		cerr << "Cannot write to " << fKey << " '" << fStringVal << "'" << endl;
	}
	return rtn;
}

bool BaseParam::IsDefault(void) const
{
  return (fDefaulted && (fStringVal == fDefaultStringVal));
}

int BaseParam::GetEnumValue(string s) const
{
	if (s.length() == 0)
		s = fStringVal;
	for (int i=0; i<fPossibles.size(); i++)
	{
		if (fPossibles[i] == s)
			return i;
	}
	return -1;
}

void BaseParam::print(ostream & os) const
{
	os << fKey << " = ";
	if (fSet)
	{
		os << fStringVal << ' '; 
	}
	else
	{
		os << ' ';
	}
}

void BaseParam::printHTML(ostream & os) const
{
	if (fHint.length() == 0)
		((BaseParam *) this)->SetHint("");  // cast away const-ness

	if (fRequired)
		os << "<font color=red>*</font>";

  os << fKey << ": ";
	os << "<mark onMouseOver=\"alert('" << GetHint() << "');\"> ? </mark>";

	if (! fRequired)
		os << "<font size=-2>(optional)</font> ";

	if (fControlledVocab)
	{
		if (fTypeName == string("bool") )
		{
			os << "\t<input type=\"radio\" name=\"" << fName << ".true" << "\" value=\"true\"";
			if (fStringVal == "true")
				os << " checked";
			os << "> true " << endl;
			os << "\t<input type=\"radio\" name=\"" << fName << ".false" << "\" value=\"false\"";
			if (fStringVal == "false")
				os << " checked";
			os << "> false " << endl;
		}
		else if (fPossibles.size() < 3 )
		{
			for (vector<string>::const_iterator it = fPossibles.begin(); it != fPossibles.end(); it++)
			{
				os << "\t<input type=\"radio\" name=\"" << fName << "\" value=\"" << *it << "\"";
				if (fStringVal == *it)
					os << " checked";
				os << "> " << *it << " " << endl;
			}
		}
		else
		{
			os << "<select>" << endl;
			for (vector<string>::const_iterator it = fPossibles.begin(); it != fPossibles.end(); it++)
			{
				os << "\t<option value=\"" << *it << "\"";
				if (fStringVal == *it)
					os << " selected";
 				os << ">" << *it << "</option>" << endl;
			}
			os << "</select>" << endl;
		}
	}
	else
	{
		os << "<input type=\"text\" name=\"" << fName << "\"";
		if (fSet)
			os << " value=\"" << fStringVal << "\"";
		if (fRequired)
			os << " required";
		os << ">" << endl;
	}
	os << "<br>" << endl;
}

void BaseParam::printXSLT(ostream & os) const
{
	if (fHint.length() == 0)
		((BaseParam *) this)->SetHint("");  // cast away const-ness

	os << "<param>" << endl;
	os << "\t<key>" << fKey << "</key>" << endl;
	os << "\t<type>" << fTypeName << "</type>" << endl;
	os << "\t<value>" << fStringVal << "</value>" << endl;
	os << "\t<required>" << fRequired << "</required>" << endl;
	os << "\t<exposed>" << fParamExposed << "</exposed>" << endl;
	if (fDefaulted)
	{
		os << "\t<default>" << fDefaultStringVal << "</default>" << endl;
	}
	os << "\t<hint>" << fHint << "</hint>" << endl;
	os << "\t<count>" << fPossibles.size() << "</count>" << endl;
	if (fControlledVocab)
	{
		int i = -1;
		for (vector<string>::const_iterator it = fPossibles.begin(); it != fPossibles.end(); it++)
		{
			i++;
			if (! fExposed.Get(i))
				continue;

			os << "\t<possible>" << endl;
			os << "\t\t<name>" << fName << "</name>" << endl;
			os << "\t\t<value>" << *it << "</value>" << endl;
			if (fStringVal == *it)
				os << "\t\t<selected>1</selected>" << endl;
			os << "\t</possible>" << endl;
		}
	}
	os << "</param>" << endl;
}

template <>
void Param<bool>::SetValFromString( void )
{
  // Convert from a string to a bool
  // Interpret "false", "F", "no", "n", "0" as false
  // Interpret "true", "T", "yes", "y", "1", "-1",
  //  or anything else as true
  bool b = true;
  string sup = fStringVal;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
    *p = toupper(*p);  // make string all caps
  if( sup==string("FALSE") || sup==string("F") ||
      sup==string("NO") || sup==string("N") ||
      sup==string("0") || sup==string("NONE") )
    b = false;
  Set(b);
}

