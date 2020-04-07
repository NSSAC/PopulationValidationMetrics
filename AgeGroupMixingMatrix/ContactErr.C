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


#include "ContactErr.h"

int ContactErr::gVerbosity = 0;

const char * mystrerr(int errnum)
{
	const char * rtn = 0;
	switch (errnum) 
	{

	// user error
		case kNoConfig :
			rtn = "no configuration file";
			break;

		case kBadConfig :
			rtn = "error in configuration file";
			break;

		case kBadPopFile :
			rtn = "error reading population file";
			break;

		case kBadNetworkFile :
			rtn = "error reading network file";
			break;

		default :
			rtn = "unknown error";
	}
	return rtn;
}
