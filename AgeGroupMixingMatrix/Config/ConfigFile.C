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

#include "ConfigFile.h"

using namespace std;

ConfigFile::ConfigFile(string filename, string delimiter,
                       string comment, string sentry)
  : myDelimiter(delimiter), myComment(comment), mySentry(sentry) {

  // Construct a ConfigFile, getting keys and values from given file

  ifstream in(filename.c_str());

  if (!in) throw file_not_found(filename);

  in >> (*this);
}


ConfigFile::ConfigFile()
  : myDelimiter(string(1,'=')), myComment(string(1,'#')) {
  // Construct a ConfigFile without a file; empty
}


void ConfigFile::remove(const string& key) {
  // Remove key and its value
  myContents.erase(myContents.find(key));
  return;
}


bool ConfigFile::keyExists(const string& key) const {
  // Indicate whether key is found
  string sup = key;
  for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);
  mapci p = myContents.find(sup);
  return (p != myContents.end());
}


/* static */
void ConfigFile::trim(string& s) {
  // Remove leading and trailing whitespace
  static const char whitespace[] = " \n\t\v\r\f";
  s.erase(0, s.find_first_not_of(whitespace));
  s.erase(s.find_last_not_of(whitespace) + 1U);
}

void ConfigFile::mapValue( int & rtn, const vector<string> & stringVals, const string & val, bool caseSensitive ) {
	if (val.length() == 0)
		return;   // Don't modify rtn, which may have been initialized to a default value!

	string cmp = val;
	if (! caseSensitive ) {
  	for( string::iterator p = cmp.begin(); p != cmp.end(); ++p )
			*p = toupper(*p);
	}
  for( int index = 1; index < stringVals.size(); index++) {
		string s = stringVals[index];
		if (! caseSensitive) {
  		for( string::iterator p = s.begin(); p != s.end(); ++p )
				*p = toupper(*p);
		}
		if (s == cmp) {
			rtn = index;
			return;
		}
	}
	cerr << "ERROR: unknown value string '" << val << "'" << endl;
	cerr << "Recognized values are ";
  for( int index = 1; index < stringVals.size(); index++) {
		cerr << "'" << stringVals[index] << "' ";
	}
	cerr << endl;
	rtn = 0;
	
	return;
}


std::ostream& operator<<(std::ostream& os, const ConfigFile& cf) {
  // Save a ConfigFile to os
  for (ConfigFile::mapci p = cf.myContents.begin();
       p != cf.myContents.end();
       ++p) {
    os << p->first << " " << cf.myDelimiter << " ";
    os << p->second << std::endl;
  }
  return os;
}


std::istream& operator>>(std::istream& is, ConfigFile& cf) {
  // Load a ConfigFile from is
  // Read in keys and values, keeping internal whitespace
  typedef string::size_type pos;
  const string& delim  = cf.myDelimiter;  // separator
  const string& comm   = cf.myComment;    // comment
  const string& sentry = cf.mySentry;     // end of file sentry
  const pos skip = delim.length();        // length of separator

  string nextline = "";
  // might need to read ahead to see where value ends

  while (is || nextline.length() > 0) {
    // Read an entire line at a time
    string line;
    if (nextline.length() > 0) {
      line = nextline;  // we read ahead; use it now
      nextline = "";
    }
    else {
      std::getline(is, line);
    }

    // Ignore comments
    line = line.substr(0, line.find(comm));

    // Check for end of file sentry
    if (sentry != "" && line.find(sentry) != string::npos) return is;

    // Parse the line if it contains a delimiter
    pos delimPos = line.find(delim);
    if (delimPos < string::npos) {
      // Extract the key
      string key = line.substr(0, delimPos);
  		string sup = key;
  		for( string::iterator p = sup.begin(); p != sup.end(); ++p )
				*p = toupper(*p);
      line.replace(0, delimPos+skip, "");

      // See if value continues on the next line
      // Stop at blank line, next line with a key, end of stream,
      // or end of file sentry
      bool terminate = false;
      while (!terminate && is) {
        std::getline(is, nextline);
        terminate = true;

        string nlcopy = nextline;
        ConfigFile::trim(nlcopy);
        if (nlcopy == "") continue;

        nextline = nextline.substr(0, nextline.find(comm));
        if (nextline.find(delim) != string::npos)
          continue;
        if (sentry != "" && nextline.find(sentry) != string::npos)
          continue;

        nlcopy = nextline;
        ConfigFile::trim(nlcopy);
        if (nlcopy != "") line += "\n";
        line += nextline;
        terminate = false;
      }

      // Store key and value
      ConfigFile::trim(sup);
      ConfigFile::trim(line);
      cf.myContents[sup] = line;  // overwrites if key is repeated
    }
  }

  return is;
}
