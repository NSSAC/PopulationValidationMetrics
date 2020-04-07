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

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class ConfigFile {

  // Data

 protected:
  string myDelimiter;  // separator between key and value
  string myComment;    // separator between value and comments
  string mySentry;     // optional string to signal end of file
  map<string,string> myContents;  // extracted keys and values

  typedef map<string,string>::iterator mapi;
  typedef map<string,string>::const_iterator mapci;

  // Methods

 public:
  ConfigFile( string filename,
              string delimiter = "=",
              string comment = "#",
              string sentry = "EndConfigFile" );
  ConfigFile();

	// Iterate through all pairs as strings

	mapci begin() const { return myContents.begin(); };
	mapci end() const { return myContents.end(); };

  // Search for key and read value or optional default value
  template<typename T> T read( const string& key ) const;
  // call as read<T>
  template<typename T> T read( const string& key, const T& value ) const;
  template<typename T> bool readInto( T& var, const string& key ) const;
  template<typename T>
    bool readInto( T& var, const string& key, const T& value ) const;

  // Modify keys and values
  template<typename T> void add( string key, const T& value );
  void remove( const string& key );

  // Check whether key exists in configuration
  bool keyExists( const string& key ) const;

  // Check or change configuration syntax
  string getDelimiter() const { return myDelimiter; }
  string getComment() const { return myComment; }
  string getSentry() const { return mySentry; }
  string setDelimiter( const string& s )
  { string old = myDelimiter;  myDelimiter = s;  return old; }  
  string setComment( const string& s )
  { string old = myComment;  myComment = s;  return old; }

	// Map from string value to integer value
	// returns 0 if val is not in vector of possible stringVals
	static void mapValue( int & rtn, const vector<string> & stringVals, const string & val, bool caseSensitive = false );

  // Write or read configuration
  friend ostream& operator<<(ostream& os,
                                  const ConfigFile& cf);
  friend istream& operator>>(istream& is, ConfigFile& cf);

 protected:
  template<typename T> static string T_as_string( const T& t );
  template<typename T> static T string_as_T( const string& s );
  static void trim( string& s );


  // Exception types
 public:
  struct file_not_found {
    string filename;
  file_not_found( const string& filename_ = string() )
  : filename(filename_) {}
  };

  struct key_not_found {  // thrown only by T read(key) variant of read()
    string key;
  key_not_found( const string& key_ = string() )
  : key(key_) {}
  };
};

/* static */
template<typename T>
string ConfigFile::T_as_string( const T& t ) {
  // Convert from a T to a string
  // Type T must support << operator
  ostringstream ost;
  ost << t;
  return ost.str();
}

/* static */
template<typename T>
T ConfigFile::string_as_T( const string& s ) {
  // Convert from a string to a T
  // Type T must support >> operator
  T t;
  istringstream ist(s);
  ist >> t;
  return t;
}

/* static */
template<>
inline string ConfigFile::string_as_T<string>( const string& s ) {
  // Convert from a string to a string
  // In other words, do nothing
  return s;
}

/* static */
template<>
inline bool ConfigFile::string_as_T<bool>( const string& s ) {
  // Convert from a string to a bool
  // Interpret "false", "F", "no", "n", "0" as false
  // Interpret "true", "T", "yes", "y", "1", "-1",
  //  or anything else as true
  bool b = true;
  string sup = s;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
    *p = toupper(*p);  // make string all caps
  if( sup==string("FALSE") || sup==string("F") ||
      sup==string("NO") || sup==string("N") ||
      sup==string("0") || sup==string("NONE") )
    b = false;
  return b;
}

template<typename T>
T ConfigFile::read( const string& key ) const {
  // Read the value corresponding to key
  string sup = key;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);
  mapci p = myContents.find(sup);
  if( p == myContents.end() ) throw key_not_found(key);
  return string_as_T<T>( p->second );
}

template<typename T>
T ConfigFile::read( const string& key, const T& value ) const {
  // Return the value corresponding to key or given default value
  // if key is not found
  string sup = key;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);
  mapci p = myContents.find(sup);
  if( p == myContents.end() ) return value;
  return string_as_T<T>( p->second );
}

template<typename T>
bool ConfigFile::readInto( T& var, const string& key ) const {
  // Get the value corresponding to key and store in var
  // Return true if key is found
  // Otherwise leave var untouched
  string sup = key;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);
  mapci p = myContents.find(sup);
  bool found = ( p != myContents.end() );
  if( found ) var = string_as_T<T>( p->second );
	// cerr << "readInto for " << sup << " returns " << var << endl;
  return found;
}

template<typename T>
bool ConfigFile::readInto
( T& var, const string& key, const T& value ) const {
  // Get the value corresponding to key and store in var
  // Return true if key is found
  // Otherwise set var to given default
  string sup = key;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);
  mapci p = myContents.find(sup);
  bool found = ( p != myContents.end() );
  if( found )
    var = string_as_T<T>( p->second );
  else
    var = value;
  return found;
}

template<typename T>
void ConfigFile::add( string key, const T& value ) {
  // Add a key with given value
  string v = T_as_string( value );
  string sup = key;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);
  trim(key);
  trim(v);
  myContents[sup] = v;
  return;
}

#endif  // CONFIG_FILE_H
