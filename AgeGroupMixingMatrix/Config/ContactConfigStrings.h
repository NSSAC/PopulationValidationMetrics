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

#ifndef CONTACT_CONFIG_STRINGS_H
#define CONTACT_CONFIG_STRINGS_H

#include <string>

using namespace std;

#define CURRENT_VERSION "2016"

class ContactConfigStrings
{

	public :

		static ContactConfigStrings & getInstance();

		const string ConfigFileKey;
		const string OutputFileKey;
		const string OutputDirectoryKey;
		const string VerbosityKey;
		const string RandomSeedKey;
		// const string MPINumChildrenKey;
		const string ConfigVersionKey;

		const string PopFileKey;
		const string NetworkFileKey;
        	const string AgeGroupKey;

        	// for parsing Person files
        	const string HHIdFieldNameKey;
        	const string PersonIdFieldNameKey;
        	const string AgeFieldNameKey;
        	const string GenderFieldNameKey;
        	const string GradeFieldNameKey;

		static void noOp(void) {};

	protected :

		ContactConfigStrings(void);

};

#endif
