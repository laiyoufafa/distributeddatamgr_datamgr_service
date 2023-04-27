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

#include "cloud/schema_meta.h"
namespace OHOS::DistributedData {
bool SchemaMeta::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(version)], version);
    SetValue(node[GET_NAME(databases)], databases);
    return true;
}

bool SchemaMeta::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(version), version);
    GetValue(node, GET_NAME(databases), databases);
    return true;
}

bool SchemaMeta::Database::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(name)], name);
    SetValue(node[GET_NAME(alias)], alias);
    SetValue(node[GET_NAME(tables)], tables);
    return true;
}

bool SchemaMeta::Database::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(name), name);
    GetValue(node, GET_NAME(alias), alias);
    GetValue(node, GET_NAME(tables), tables);
    return true;
}

bool SchemaMeta::Table::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(name)], name);
    SetValue(node[GET_NAME(alias)], alias);
    SetValue(node[GET_NAME(fields)], fields);
    return true;
}

bool SchemaMeta::Table::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(name), name);
    GetValue(node, GET_NAME(alias), alias);
    GetValue(node, GET_NAME(fields), fields);
    return true;
}

bool SchemaMeta::Field::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(colName)], colName);
    SetValue(node[GET_NAME(alias)], alias);
    SetValue(node[GET_NAME(type)], type);
    SetValue(node[GET_NAME(primary)], primary);
    SetValue(node[GET_NAME(nullable)], nullable);
    return true;
}

bool SchemaMeta::Field::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(colName), colName);
    GetValue(node, GET_NAME(alias), alias);
    GetValue(node, GET_NAME(type), type);
    GetValue(node, GET_NAME(primary), primary);
    GetValue(node, GET_NAME(nullable), nullable);
    return true;
}

SchemaMeta::Database SchemaMeta::GetDataBase(const std::string &storeId)
{
    for (const auto &database : databases) {
        if (database.name == storeId) {
            return database;
        }
    }
    return {};
}
} // namespace OHOS::DistributedData