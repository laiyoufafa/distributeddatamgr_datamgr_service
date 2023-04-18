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

#ifndef JSON_COMMON_H
#define JSON_COMMON_H

#include <cstdint>
#include <vector>
#include <set>
#include <functional>
#include "json_object.h"

namespace DocumentDB {
class JsonCommon
{
public:
    JsonCommon() = default;
    ~JsonCommon();

    static ValueObject GetValueByFiled(JsonObject &node, const std::string& filed);
    static bool CheckJsonField(JsonObject &node);
    static bool CheckProjectionField(JsonObject &node);
    static int ParseNode(JsonObject &Node, std::vector<std::string> singlePath, std::vector<std::vector<std::string>> &resultPath, bool isFirstFloor);
    static std::vector<std::vector<std::string>> ParsePath(const JsonObject &node);
    static std::vector<ValueObject>  GetLeafValue(JsonObject &node);

    static int Append(const JsonObject &src, const JsonObject &add);
    
private:
    static bool CheckNode(JsonObject &Node, std::set<std::string> filedSet, bool &errFlag);
    static bool CheckProjectionNode(JsonObject &Node, std::set<std::string> filedSet, bool &errFlag, bool isFirstFloor);
    static int CheckLeafNode(JsonObject &Node, std::vector<ValueObject> &leafValue);
};
} // DocumentDB
#endif // JSON_COMMON_H