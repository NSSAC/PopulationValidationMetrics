//  Copyright 2019 University of Virginia
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef EXCEADS_CSVPARSER_H
#define EXCEADS_CSVPARSER_H

#include <list>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

// #include "LATypes.h"
// #include "Person.h"

// This class reads a <char>-separated values file where
// <char> can be one of ',', '\t', or ' '
// FIX:  and can be escaped within a field by prefixing it with a '\'.
// An iterator reads one line at a time and stores each field in an array of strings.
// String field names can be used as indices into the array.
// The class creates a mapping from field names to indices by parsing the first line,
// FIX:  first removing any '#' and leading white space characters, unless <char> is ' '.

class CSVParser {
	public :

	CSVParser(std::ifstream & is, char sep = ',');
	CSVParser(const std::string & fName, char sep = ',');

	CSVParser operator++(void);
	operator bool() const {return (*fIs) ? true : false;};

	int getColumn(const std::string & name);

	long getLong(int col) const;
	double getDouble(int col) const;

	const std::string & operator[](int col) const {return fData[col];};

	void print(std::ostream & os) const;

	protected :

	char fSep;
	std::ifstream *fIs;
	std::map<std::string, int> fColNames;
	std::vector<std::string> fData;

	void parseHeader(void);
};

#endif
