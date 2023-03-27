/*
* Copyright (c) 2023 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef DOC_COMMON_H
#define DOC_COMMON_H

#include <cstdint>
#include <vector>
#include "json_common.h"

class JsonCommon;
namespace DocumentDB {
class CheckCommon
{
public:
    CheckCommon() = default;
    ~CheckCommon();
    
    static bool CheckCollectionName(const std::string &collectionName);
    static bool CheckFilter(const std::string &filter);
    static bool CheckIdFormat(const std::string &data);
    static bool CheckDocument(const std::string &document);
};
using Key = std::vector<uint8_t>;
using Value = std::vector<uint8_t>;

} // DocumentDB
#endif // DOC_COMMON_H