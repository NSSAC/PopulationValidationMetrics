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

#ifndef RELIABILITY_PARAM_H
#define RELIABILITY_PARAM_H

#include <algorithm>
#include <vector>
#include <typeinfo>
#include <string>

#include "../BitArray.h"

// A class designed to make it easy to add parameters to an executable and construct web interfaces.
// Intended for use with codes that generate or parse a key/value pair configuration file

using namespace std;

class BaseParam
{
	public :

		BaseParam(const string & name, const string & typNam, bool req = false, const string & val = string(""));
		virtual ~BaseParam(void) {};

		virtual void SetHint(const string & hint); // a brief "tooltip"
		void SetHelp(const string & help);         // something more extensive
		void SetGroup(const string & s);

		void   SetStringVal(const string & s);
		string GetStringVal(void) const {return fStringVal;}
		virtual void SetValFromString(void) = 0;  // sets the values in templated parameters from the string

		string GetName(void)     const {return fName;};
		string GetKey(void)      const {return fKey;};
		string GetTypeName(void) const {return fTypeName;};
		string GetHint(void)     const {return fHint;};
		string GetHelp(void)     const {return fHelp;};
		string GetGroup(void)    const {return fGroup;};
		
		int GetEnumValue(string = "") const;

		void SetPossibles(const vector<string> & possibles);  // vector maintains order in menu
		void AddPossible(const string & possible, bool expose = true);
		void Hide(int index) { fExposed.Clear(index);} ;   // don't expose the "index"-th possible value in the user interface

		void SetExposed(void) {fParamExposed = true;};
		void SetUnexposed(void) {fParamExposed = false;};
		bool IsExposed(void) {return fParamExposed;};

		void SetRequired(bool req = true) {fRequired = req;};
		bool IsRequired(void) const {return fRequired;};
		void SetDefault(const string & def) {fDefaultStringVal = def; fStringVal = def; fDefaulted = true; SetValFromString();}
		bool HasDefault(void) const {return fDefaulted;};
		bool IsSet(void) const {return fSet;};
		bool IsDefault(void) const;

		bool IsReadable(bool quiet = false) const;  // only makes sense for filenames
		bool IsWritable(void) const;  // only makes sense for filenames
		virtual bool IsValid(void) const {return false;};

		friend ostream & operator<<(ostream & os, BaseParam & bp);
		void print(ostream & os) const;
		void printHTML(ostream & os) const;
		void printXSLT(ostream & os) const;

	protected :

		string fStringVal;
		string fDefaultStringVal;

		string fTypeName; // e.g., int bool
		string fName;    // unique name by which this param is known in the code
		string fKey;     // unique name used in any user interface 
		string fHint;    // short usage notes
		string fHelp;    // longer explanation of what the parameter controls
		string fGroup;   // one level of hierarchy for grouping parameters in user interface

		vector<string> fPossibles;  // only string values make sense to me for this ...
		BitArray fExposed; // Expose the corresponding fPossibles value through the user interface

		bool fParamExposed; // the parameter itself should be exposed to the user

		bool fRequired;  // can the code execute without a value for this parameter?
		bool fSet;       // has a value been specified?
		bool fDefaulted; // does the code supply a default value?
		bool fControlledVocab;  // is there a pre-defined list of possible values?
	
		bool fMinSet;
		bool fMaxSet;
};

template <typename T> 
class Param : public BaseParam
{

	public :

		// separate constructors so you don't need to reserve a special value
		Param(const string & name, bool req = false);
		Param(const string & name, bool req, const string & def);

		T  Get(void) const;
		void Set(const T & v);
		void SetValFromString(void);

		void SetMin(const T & min);
		void SetMax(const T & max);

		void SetHint(const string & hint);

		bool IsValid(void) const;  // within range? file readable?

	protected :

		T fVal;

		// Set a range for fVal: fMinVal <= fVal <= fMaxVal
		T fMinVal;  
		T fMaxVal;
};

// default implementation
template <typename T>
struct TypeName
{
    static const char* Get()
    {
        return typeid(T).name();
    }
};

// a specialization for each type of those you want to support
// and don't like the string returned by typeid
#define ENABLE_TYPENAME(A) template<> struct TypeName<A> { static const char *Get() { return #A; }};
ENABLE_TYPENAME(bool);
ENABLE_TYPENAME(int);
ENABLE_TYPENAME(float);
ENABLE_TYPENAME(string);
ENABLE_TYPENAME(vector<float>);
ENABLE_TYPENAME(vector<int>);

template <typename T> 
class Param<vector<T> > : public BaseParam
{

	public :

		// separate constructors so you don't need to reserve a special value
		Param(const string & name, bool req = false);
		Param(const string & name, bool req, const string & def);

		vector<T>  Get(void) const;
		void Set(const vector<T> & v) {fVal = v; fSet = true; ostringstream oss(fStringVal); int i; for (i=0; i<v.size()-1; i++) oss << v[i] << ", "; oss << v[i];};
		void SetValFromString(void);

		void SetMin(const T & min);
		void SetMax(const T & max);

		void SetHint(const string & hint);

		bool IsValid(void) const;  // within range? file readable?

	protected :

		vector<T> fVal;

		// Set a range for fVal: fMinVal <= fVal <= fMaxVal
		T fMinVal;  
		T fMaxVal;
};

template <typename T>
void Param<vector<T> >::SetValFromString(void)
{
	std::replace( fStringVal.begin(), fStringVal.end(), ',', ' '); 
	std::replace( fStringVal.begin(), fStringVal.end(), '\t', ' '); 
  istringstream iss(fStringVal);
	T val;
	int count = 0;
	// cerr << "Setting values for " << fKey << " from string '" << fStringVal << "'" << endl;
	while (iss)
	{
  	iss >> val;
		if (iss)
		{
			fVal.resize(count+1);
			fVal[count++] = val;
			// cerr << "Set value " << count-1 << " to '" << fVal[count-1] << "'" << endl;
		}
	}
	fSet = true;
}

template <typename T>
void Param<T>::Set(const T & v)
{
	fVal = v;
  ostringstream oss;
	oss << std::boolalpha << v;
	fStringVal = oss.str();
	fSet = true; 
}

template <typename T>
void Param<vector<T> >::SetHint(const string & hint)
{
	fHint = "";
	if (fRequired)
		fHint = string("required ");
	ostringstream oss;
	oss << "comma-separated list of ";
	oss << TypeName<T>::Get();
	oss << "s";
	if (fMinSet && fMaxSet)
		oss << " in range [" << fMinVal << ", " << fMaxVal << "]";
	else if (fMinSet)
		oss << " >= " << fMinVal;
	else if (fMaxSet)
		oss << " <= " << fMaxVal;
	oss << " (-1 means to ignore the value) ";
	fHint += oss.str();
	BaseParam::SetHint(hint);
}

ostream & operator<<(ostream & os, BaseParam & bp);

template <typename T>
Param<T>::Param(const string & name, bool req)
  : BaseParam(name, TypeName<T>::Get(), req)
{}

template <typename T>
Param<T>::Param(const string & name, bool req, const string & def)
  : BaseParam(name, TypeName<T>::Get(), req, def)
{ 
  if (def.length() > 0)
  {
    fDefaulted = true;
    SetValFromString();
  }
}

template <typename T>
void Param<T>::SetHint(const string & hint) 
{
	fHint = "";
	if (fRequired)
		fHint = string("required ");
	fHint += fTypeName;
	ostringstream oss;
	if (fMinSet && fMaxSet)
		oss << " in range [" << fMinVal << ", " << fMaxVal << "]";
	else if (fMinSet)
		oss << " >= " << fMinVal;
	else if (fMaxSet)
		oss << " <= " << fMaxVal;
	fHint += oss.str();
	BaseParam::SetHint(hint);
}

template <typename T>
T Param<T>::Get(void) const
{
  if (fRequired && ! fSet)
    cerr << fKey << " is required, but not set" << endl;
  return fVal;
}

template <typename T>
bool Param<T>::IsValid(void) const
{
  if (fRequired && ! fSet)
    return false;

  if (fControlledVocab)   // make sure the value is one of the allowed ones
  {
    if (GetEnumValue() == -1)
    {
      cerr << "ERROR: " << fKey << " parameter value '" << fVal << "' is not one of recognized values: ";
      for (vector<string>::const_iterator it = fPossibles.begin(); it != fPossibles.end(); it++)
      {
        cerr << (*it) << endl;
      }
      cerr << endl;
      return false;
    }
  }

  if (fSet && fMinSet)
  {
    if (fVal < fMinVal)
    {
      cerr << "ERROR: " << fKey << " parameter value " << fVal << " is smaller than allowed minimum: " << fMinVal << endl;
      return false;
    }
  }

  if (fSet && fMaxSet)
  {
    if (fVal > fMaxVal)
    {
      cerr << "ERROR: " << fKey << " parameter value " << fVal << " is larger than allowed maximum: " << fMaxVal << endl;
      return false;
    }
  }

  return true;
}

template <typename T>
void Param<T>::SetValFromString(void)
{
  istringstream iss(fStringVal);
  T val;
  iss >> val;
  Set(val);
}

template <>
void Param<string>::SetValFromString( void ) ;

template <>
void Param<bool>::SetValFromString( void ) ;

template <typename T>
void Param<T>::SetMin(const T & min)
{
  fMinVal = min;
  fMinSet = true;
}

template <typename T>
void Param<T>::SetMax(const T & max)
{
  fMaxVal = max;
  fMaxSet = true;
}

template <typename T>
Param<vector<T> >::Param(const string & name, bool req)
  : BaseParam(name, TypeName<T>::Get(), req)
{}

template <typename T>
Param<vector<T> >::Param(const string & name, bool req, const string & def)
  : BaseParam(name, TypeName<T>::Get(), req, def)
{ 
  if (def.length() > 0)
  {
    fDefaulted = true;
    SetValFromString();
  }
}

template <typename T>
vector<T> Param<vector<T> >::Get(void) const
{
  if (fRequired && ! fSet)
    cerr << fKey << " is required, but not set" << endl;

	// if (fVal.size() == 0)
    // cerr << fKey << " is empty " << endl;
	// else
		// cerr << "Returning " ;
	// for (int i=0; i<fVal.size(); i++)
		// cerr << fVal[i] << endl;
  return fVal;
}

template <typename T>
void Param<vector<T> >::SetMin(const T & min)
{
  fMinVal = min;
  fMinSet = true;
}

template <typename T>
void Param<vector<T> >::SetMax(const T & max)
{
  fMaxVal = max;
  fMaxSet = true;
}

template <typename T>
bool Param<vector<T> >::IsValid(void) const
{
  if (fRequired && ! fSet)
    return false;

	for (int i=0; i<fVal.size(); i++)
	{
		T val = fVal[i];
  	if (fMinSet)
  	{
    	if (val < fMinVal)
    	{
      	cerr << "ERROR: " << fKey << " parameter value " << val << " is smaller than allowed minimum: " << fMinVal << endl;
      	return false;
    	}
  	}
	
  	if (fMaxSet)
  	{
    	if (val > fMaxVal)
    	{
      	cerr << "ERROR: " << fKey << " parameter value " << val << " is larger than allowed maximum: " << fMaxVal << endl;
      	return false;
    	}
  	}
	}

  return true;
}
#endif
