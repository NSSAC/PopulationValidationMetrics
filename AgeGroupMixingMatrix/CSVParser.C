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

#include "CSVParser.h"
// #include "LAErr.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// trim leading '#' and spaces and trailing spaces and make lowercase
std::string normalize(std::string s)
{
	size_t start = s.find_first_not_of("# ");
	if (start == std::string::npos)
		return "";

	size_t stop = s.find_last_not_of(" ");
	std::string rtn = s.substr(start, stop-start+1);
	std::transform(rtn.begin(), rtn.end(), rtn.begin(),
    	     [](unsigned char c){ return std::tolower(c); });
	return rtn;
}

CSVParser::CSVParser(const std::string & fName, char sep)
	: fSep(sep)
{
	fIs = new std::ifstream(fName.c_str());
	if (! *fIs)
	{
		std::cerr << "Couldn't open file '" << fName << "' for reading" << std::endl;
	}
	else
	{
		parseHeader();
	}
	fData.resize(fColNames.size());
}

CSVParser::CSVParser(std::ifstream & fs, char sep)
	: fSep(sep), fIs(&fs)
{
	parseHeader();
	fData.resize(fColNames.size());
}

// Read header line with field names
void CSVParser::parseHeader(void)
{
	std::string buf;
	getline(*fIs, buf, '\n');  // eat schema line
	getline(*fIs, buf, '\n');
	size_t start = buf.find_first_not_of("# \t");
	if (start != std::string::npos)
	{
		buf = buf.substr(start);
	}
	std::istringstream is(buf);
	int index = 0;
	std::string name;
	while (1)
	{
		getline(is, name, fSep);
		name = normalize(name);
		if (! is) break;
		fColNames[name] = index;
		// std::cerr << "Set column name for col " << index << " to " << name << std::endl;
		index++;
	}
}

CSVParser CSVParser::operator++(void)
{
	std::string buf;
	getline(*fIs, buf, '\n');
	std::istringstream is(buf);
	// std::cerr << "Read line '" << buf << "'" << std::endl;
	int index = 0;
	std::string val;
	while (1)
	{
		getline(is, val, fSep);
		if (! is) break;
		fData[index] = val;
		// std::cerr << "Read val in col " << index << " -> " << val << std::endl;
		index++;
	}
	return *this;
}

int CSVParser::getColumn(const std::string & name)
{
	std::string normName = normalize(name);
	auto it = fColNames.find(normName);
	if (it == fColNames.end())
	{
		std::cerr << "No field named '" << normName << "'" << std::endl;
		print(std::cerr);
		return -1;
	}
	std::clog << "Field '" << normName << "' is in col " << it->second << std::endl;
	return it->second;
}

long CSVParser::getLong(int column) const
{
	if (column < 0 || column >= fData.size())
	{
		std::cerr << "Invalid column '" << column << "' in CSVParser::getLong" << std::endl;
	}
	long rtn = -1;
	std::istringstream is(fData[column]);
	is >> rtn;
	return rtn;
}

double CSVParser::getDouble(int column) const
{
	if (column < 0 || column >= fData.size())
	{
		std::cerr << "Invalid column '" << column << "' in CSVParser::getLong" << std::endl;
	}
	double rtn = -1;
	std::istringstream is(fData[column]);
	is >> rtn;
	return rtn;
}

void CSVParser::print(std::ostream & os) const
{
	os << fColNames.size() << " columns: " << std::endl;
	for (auto it = fColNames.begin(); it != fColNames.end(); it++)
	{
		os << "'" << it->first << "' ";
	}
	os << std::endl;
	for (int i=0; i< fData.size(); i++)
		os << "'" << fData[i] << "' ";
	os << std::endl;
}

