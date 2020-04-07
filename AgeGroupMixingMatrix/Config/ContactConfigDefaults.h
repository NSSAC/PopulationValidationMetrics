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

#ifndef CONTACT_CONFIG_DEFAULTS_H
#define CONTACT_CONFIG_DEFAULTS_H

#include <string>

using namespace std;

const string kDefVerbosity = "2";
const string kDefConfigVersion = "2016";

const string kDefAgeGroup = "CDC";

const string kDefHHIdFieldName = "HID";
const string kDefPersonIdFieldName = "PID";
const string kDefAgeFieldName = "AGE";
const string kDefGenderFieldName = "SEX";
const string kDefGradeFieldName = "GRADE_LEVEL_ATTENDING";
const string kDefAreaFieldName = "area_sqm";
const string kDefPreKFieldName = "pub_pk";

#endif
