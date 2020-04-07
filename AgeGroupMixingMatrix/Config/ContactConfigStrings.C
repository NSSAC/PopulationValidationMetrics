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

#include "ContactConfigStrings.h"

#include <string.h>
#include <iostream>

using namespace std;

ContactConfigStrings::ContactConfigStrings(void)
:	ConfigFileKey (      "Config File"),
	OutputFileKey (      "Output File"),
	OutputDirectoryKey ( "Output Directory"),
	VerbosityKey (       "Verbosity"),
	RandomSeedKey (      "Random Seed"),
	ConfigVersionKey (   "Config File Version"),

	PopFileKey (     "Population File"),
	NetworkFileKey ( "Network File"),
	AgeGroupKey (    "Age Groups"),

	HHIdFieldNameKey("Household ID Field Name"),
	PersonIdFieldNameKey("Person ID Field Name"),
	AgeFieldNameKey("Age Field Name"),
	GenderFieldNameKey("Gender Field Name"),
	GradeFieldNameKey("Grade Field Name")
{ 
}

ContactConfigStrings & ContactConfigStrings::getInstance(void)
{
	static bool inited = false;
	static ContactConfigStrings * instance = 0;

	if (! inited)
	{
		inited = true;
		instance = new ContactConfigStrings;
	}
	return *instance;
}
