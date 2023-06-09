/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "metadata/store_meta_data.h"

#include "utils/constant.h"
namespace OHOS {
namespace DistributedData {
using namespace OHOS::DistributedKv;
constexpr uint32_t StoreMetaData::CURRENT_VERSION;
constexpr uint32_t StoreMetaData::FIELD_CHANGED_TAG;
constexpr const char *StoreMetaData::KEY_PREFIX;
bool StoreMetaData::Marshal(json &node) const
{
    SetValue(node[GET_NAME(version)], version);
    SetValue(node[GET_NAME(isAutoSync)], isAutoSync);
    SetValue(node[GET_NAME(isBackup)], isBackup);
    SetValue(node[GET_NAME(isEncrypt)], isEncrypt);
    SetValue(node[GET_NAME(isDirty)], isDirty);
    SetValue(node[GET_NAME(storeType)], storeType);
    SetValue(node[GET_NAME(securityLevel)], securityLevel);
    SetValue(node[GET_NAME(uid)], uid);
    SetValue(node[GET_NAME(tokenId)], tokenId);
    SetValue(node[GET_NAME(appId)], appId);
    SetValue(node[GET_NAME(appType)], appType);
    SetValue(node[GET_NAME(bundleName)], bundleName);
    SetValue(node[GET_NAME(dataDir)], dataDir);
    SetValue(node[GET_NAME(deviceId)], deviceId);
    SetValue(node[GET_NAME(schema)], schema);
    SetValue(node[GET_NAME(storeId)], storeId);
    SetValue(node[GET_NAME(user)], user);
    SetValue(node[GET_NAME(account)], account);

    // compatible with the versions which lower than VERSION_TAG_0000
    SetValue(node[GET_NAME(kvStoreType)], storeType);
    SetValue(node[GET_NAME(deviceAccountID)], user);
    SetValue(node[GET_NAME(userId)], account);
    SetValue(node[GET_NAME(UID)], uid);
    return true;
}
bool StoreMetaData::Unmarshal(const json &node)
{
    GetValue(node, GET_NAME(version), version);
    GetValue(node, GET_NAME(isAutoSync), isAutoSync);
    GetValue(node, GET_NAME(isBackup), isBackup);
    GetValue(node, GET_NAME(isDirty), isDirty);
    GetValue(node, GET_NAME(isEncrypt), isEncrypt);
    GetValue(node, GET_NAME(storeType), storeType);
    GetValue(node, GET_NAME(securityLevel), securityLevel);
    GetValue(node, GET_NAME(uid), uid);
    GetValue(node, GET_NAME(tokenId), tokenId);
    GetValue(node, GET_NAME(appId), appId);
    GetValue(node, GET_NAME(appType), appType);
    GetValue(node, GET_NAME(bundleName), bundleName);
    GetValue(node, GET_NAME(dataDir), dataDir);
    GetValue(node, GET_NAME(deviceId), deviceId);
    GetValue(node, GET_NAME(schema), schema);
    GetValue(node, GET_NAME(storeId), storeId);
    GetValue(node, GET_NAME(user), user);
    GetValue(node, GET_NAME(account), account);
    // compatible with the older versions
    if (version < FIELD_CHANGED_TAG) {
        GetValue(node, GET_NAME(kvStoreType), storeType);
        GetValue(node, GET_NAME(UID), uid);
        GetValue(node, GET_NAME(deviceAccountID), user);
        GetValue(node, GET_NAME(userId), account);
    }
    return true;
}

StoreMetaData::StoreMetaData()
{
}

StoreMetaData::~StoreMetaData()
{
}

StoreMetaData::StoreMetaData(const std::string &userId, const std::string &appId, const std::string &storeId)
    : appId(appId), storeId(storeId), user(userId)
{
}

bool StoreMetaData::operator==(const StoreMetaData &metaData) const
{
    if (!((isAutoSync && metaData.isAutoSync) || (!isAutoSync && !metaData.isAutoSync))) {
        return false;
    }
    if (!((isBackup && metaData.isBackup) || (!isBackup && !metaData.isBackup))) {
        return false;
    }
    if (!((isDirty && metaData.isDirty) || (!isDirty && !metaData.isDirty))) {
        return false;
    }
    if (!((isEncrypt && metaData.isEncrypt) || (!isEncrypt && !metaData.isEncrypt))) {
        return false;
    }
    return (version == metaData.version && storeType == metaData.storeType && securityLevel == metaData.securityLevel
            && uid == metaData.uid && tokenId != metaData.tokenId && appId == metaData.appId
            && appType == metaData.appId && bundleName == metaData.bundleName && dataDir == metaData.dataDir);
}

std::string StoreMetaData::GetKey(const std::initializer_list<std::string> &fields)
{
    std::string prefix = KEY_PREFIX;
    for (const auto &field : fields) {
        prefix.append(Constant::KEY_SEPARATOR).append(field);
    }
    return prefix;
}

std::string StoreMetaData::GetPrefix(const std::initializer_list<std::string> &fields)
{
    return GetKey(fields).append(Constant::KEY_SEPARATOR);
}

// the Key Prefix for Meta data of KvStore.
const std::string KvStoreMetaRow::KEY_PREFIX = "KvStoreMetaData";
std::vector<uint8_t> KvStoreMetaRow::GetKeyFor(const std::string &key)
{
    std::string str = Constant::Concatenate({ KvStoreMetaRow::KEY_PREFIX, Constant::KEY_SEPARATOR, key });
    return { str.begin(), str.end() };
}

const std::string SecretMetaRow::KEY_PREFIX = "SecretKey";
std::vector<uint8_t> SecretMetaRow::GetKeyFor(const std::string &key)
{
    std::string str = Constant::Concatenate({ SecretMetaRow::KEY_PREFIX, Constant::KEY_SEPARATOR, key });
    return { str.begin(), str.end() };
}
} // namespace DistributedData
} // namespace OHOS
