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

#ifndef CONTACT_CONFIG_TOOLTIPS_H
#define CONTACT_CONFIG_TOOLTIPS_H

#include <string>

using namespace std;

const string kVerbosityToolTip = "Higher numbers produce more logging information";

const string kRandomSeedToolTip = "Initializes the random number generator";

const string kPopFileToolTip = "File containing population with age and gender";
const string kNetworkFileToolTip = "File containing contact network";
const string kAgeGroupToolTip = "Whether to use CDC or PolyMod age groups";

const string kHHIdFieldToolTip = "Label (in header line) of column in csv file containing Household ID";
const string kPersonIdFieldToolTip = "Label (in header line) of column in csv file containing Person ID in Person file";
const string kAgeFieldToolTip = "Label (in header line) of column in csv file containing Age";
const string kGenderFieldToolTip = "Label (in header line) of column in csv file containing Gender";
const string kGradeFieldToolTip = "Label (in header line) of column in csv file containing Grade";
const string kAreaFieldToolTip = "Label (in header line) of column in csv file containing Location Area";

const string kOutputFileToolTip = "Output location assignment file name";

const string kOutputDirectoryToolTip = "All output will be written to a directory with this name (created at run time, if necessary and permitted). Default value is current working directory.";

const string kConfigFileToolTip = "Name of the input configuration file";

const string kConfigVersionToolTip = "To support (future) versioning";

#endif
