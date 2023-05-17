/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#define LOG_TAG "RdbSyncer"
#include "rdb_syncer.h"

#include <chrono>

#include "accesstoken_kit.h"
#include "account/account_delegate.h"
#include "checker/checker_manager.h"
#include "crypto_manager.h"
#include "device_manager_adapter.h"
#include "directory_manager.h"
#include "kvstore_utils.h"
#include "log_print.h"
#include "metadata/appid_meta_data.h"
#include "metadata/meta_data_manager.h"
#include "metadata/store_meta_data.h"
#include "rdb_result_set_impl.h"
#include "types.h"
#include "utils/constant.h"
#include "utils/converter.h"
#include "types_export.h"

using OHOS::DistributedKv::KvStoreUtils;
using OHOS::DistributedKv::AccountDelegate;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::DistributedData;
using system_clock = std::chrono::system_clock;
using DmAdapter = OHOS::DistributedData::DeviceManagerAdapter;

constexpr uint32_t ITERATE_TIMES = 10000;
namespace OHOS::DistributedRdb {
RdbSyncer::RdbSyncer(const RdbSyncerParam& param, RdbStoreObserverImpl* observer)
    : param_(param), observer_(observer)
{
    ZLOGI("construct %{public}s", param_.storeName_.c_str());
}

RdbSyncer::~RdbSyncer() noexcept
{
    param_.password_.assign(param_.password_.size(), 0);
    ZLOGI("destroy %{public}s", param_.storeName_.c_str());
    if ((manager_ != nullptr) && (delegate_ != nullptr)) {
        manager_->CloseStore(delegate_);
    }
    delete manager_;
    if (observer_ != nullptr) {
        delete observer_;
    }
}

void RdbSyncer::SetTimerId(uint64_t timerId)
{
    timerId_ = timerId;
}

uint64_t RdbSyncer::GetTimerId() const
{
    return timerId_;
}

pid_t RdbSyncer::GetPid() const
{
    return pid_;
}

std::string RdbSyncer::GetIdentifier() const
{
    return DistributedDB::RelationalStoreManager::GetRelationalStoreIdentifier(GetUserId(), GetAppId(), GetStoreId());
}

std::string RdbSyncer::GetUserId() const
{
    return std::to_string(AccountDelegate::GetInstance()->GetUserByToken(token_));
}

std::string RdbSyncer::GetBundleName() const
{
    return param_.bundleName_;
}

std::string RdbSyncer::GetAppId() const
{
    return DistributedData::CheckerManager::GetInstance().GetAppId({ uid_, token_, param_.bundleName_ });
}

std::string RdbSyncer::GetStoreId() const
{
    return RemoveSuffix(param_.storeName_);
}

int32_t RdbSyncer::Init(pid_t pid, pid_t uid, uint32_t token, const StoreMetaData &meta)
{
    ZLOGI("enter");
    pid_ = pid;
    uid_ = uid;
    token_ = token;

    if (InitDBDelegate(meta) != RDB_OK) {
        ZLOGE("delegate is nullptr");
        return RDB_ERROR;
    }

    if (meta.storeType == RDB_DEVICE_COLLABORATION && meta.version < StoreMetaData::UUID_CHANGED_TAG) {
        delegate_->RemoveDeviceData();
    }

    ZLOGI("success");
    return RDB_OK;
}

bool RdbSyncer::GetPassword(const StoreMetaData &metaData, DistributedDB::CipherPassword &password)
{
    if (!metaData.isEncrypt) {
        return true;
    }

    std::string key = metaData.GetSecretKey();
    DistributedData::SecretKeyMetaData secretKeyMeta;
    MetaDataManager::GetInstance().LoadMeta(key, secretKeyMeta, true);
    std::vector<uint8_t> decryptKey;
    CryptoManager::GetInstance().Decrypt(secretKeyMeta.sKey, decryptKey);
    if (password.SetValue(decryptKey.data(), decryptKey.size()) != DistributedDB::CipherPassword::OK) {
        std::fill(decryptKey.begin(), decryptKey.end(), 0);
        ZLOGE("Set secret key value failed. len is (%{public}d)", int32_t(decryptKey.size()));
        return false;
    }
    std::fill(decryptKey.begin(), decryptKey.end(), 0);
    return true;
}

std::string RdbSyncer::RemoveSuffix(const std::string& name)
{
    std::string suffix(".db");
    auto pos = name.rfind(suffix);
    if (pos == std::string::npos || pos < name.length() - suffix.length()) {
        return name;
    }
    return std::string(name, 0, pos);
}

int32_t RdbSyncer::InitDBDelegate(const StoreMetaData &meta)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (manager_ == nullptr) {
        manager_ = new(std::nothrow) DistributedDB::RelationalStoreManager(meta.appId, meta.user, meta.instanceId);
    }
    if (manager_ == nullptr) {
        ZLOGE("malloc manager failed");
        return RDB_ERROR;
    }

    if (delegate_ == nullptr) {
        DistributedDB::RelationalStoreDelegate::Option option;
        if (meta.isEncrypt) {
            GetPassword(meta, option.passwd);
            option.isEncryptedDb = param_.isEncrypt_;
            option.iterateTimes = ITERATE_TIMES;
            option.cipher = DistributedDB::CipherType::AES_256_GCM;
        }
        option.observer = observer_;
        std::string fileName = meta.dataDir;
        ZLOGI("path=%{public}s storeId=%{public}s", fileName.c_str(), meta.storeId.c_str());
        auto status = manager_->OpenStore(fileName, meta.storeId, option, delegate_);
        if (status != DistributedDB::DBStatus::OK) {
            ZLOGE("open store failed, path=%{public}s storeId=%{public}s status=%{public}d",
                fileName.c_str(), meta.storeId.c_str(), status);
            return RDB_ERROR;
        }
        ZLOGI("open store success");
    }

    return RDB_OK;
}

int32_t RdbSyncer::GetInstIndex(uint32_t tokenId, const std::string &bundleName)
{
    if (AccessTokenKit::GetTokenTypeFlag(tokenId) != TOKEN_HAP) {
        return 0;
    }

    HapTokenInfo tokenInfo;
    tokenInfo.instIndex = -1;
    int errCode = AccessTokenKit::GetHapTokenInfo(tokenId, tokenInfo);
    if (errCode != RET_SUCCESS) {
        ZLOGE("GetHapTokenInfo error:%{public}d, tokenId:0x%{public}x appId:%{public}s", errCode, tokenId,
            bundleName.c_str());
        return -1;
    }
    return tokenInfo.instIndex;
}

DistributedDB::RelationalStoreDelegate* RdbSyncer::GetDelegate()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return delegate_;
}

int32_t RdbSyncer::SetDistributedTables(const std::vector<std::string> &tables)
{
    auto* delegate = GetDelegate();
    if (delegate == nullptr) {
        ZLOGE("delegate is nullptr");
        return RDB_ERROR;
    }

    for (const auto& table : tables) {
        ZLOGI("%{public}s", table.c_str());
        auto dBStatus = delegate->CreateDistributedTable(table);
        if (dBStatus != DistributedDB::DBStatus::OK) {
            ZLOGE("create distributed table failed, table:%{public}s, err:%{public}d", table.c_str(), dBStatus);
            return RDB_ERROR;
        }
    }
    ZLOGI("create distributed table success");
    return RDB_OK;
}

std::vector<std::string> RdbSyncer::GetConnectDevices()
{
    auto deviceInfos = DmAdapter::GetInstance().GetRemoteDevices();
    std::vector<std::string> devices;
    for (const auto& deviceInfo : deviceInfos) {
        devices.push_back(deviceInfo.networkId);
    }
    ZLOGI("size=%{public}u", static_cast<uint32_t>(devices.size()));
    for (const auto& device: devices) {
        ZLOGI("%{public}s", KvStoreUtils::ToBeAnonymous(device).c_str());
    }
    return devices;
}

std::vector<std::string> RdbSyncer::NetworkIdToUUID(const std::vector<std::string> &networkIds)
{
    std::vector<std::string> uuids;
    for (const auto& networkId : networkIds) {
        auto uuid = DmAdapter::GetInstance().GetUuidByNetworkId(networkId);
        if (uuid.empty()) {
            ZLOGE("%{public}s failed", KvStoreUtils::ToBeAnonymous(networkId).c_str());
            continue;
        }
        uuids.push_back(uuid);
        ZLOGI("%{public}s <--> %{public}s", KvStoreUtils::ToBeAnonymous(networkId).c_str(),
              KvStoreUtils::ToBeAnonymous(uuid).c_str());
    }
    return uuids;
}

void RdbSyncer::HandleSyncStatus(const std::map<std::string, std::vector<DistributedDB::TableStatus>> &syncStatus,
                                 SyncResult &result)
{
    for (const auto& status : syncStatus) {
        auto res = DistributedDB::DBStatus::OK;
        for (const auto& tableStatus : status.second) {
            if (tableStatus.status != DistributedDB::DBStatus::OK) {
                res = tableStatus.status;
                break;
            }
        }
        std::string uuid = DmAdapter::GetInstance().ToNetworkID(status.first);
        if (uuid.empty()) {
            ZLOGE("%{public}.6s failed", status.first.c_str());
            continue;
        }
        ZLOGI("%{public}.6s=%{public}d", uuid.c_str(), res);
        result[uuid] = res;
    }
}
void RdbSyncer::EqualTo(const RdbPredicateOperation &operation, DistributedDB::Query &query)
{
    query.EqualTo(operation.field_, operation.values_[0]);
    ZLOGI("field=%{public}s value=%{public}s", operation.field_.c_str(), operation.values_[0].c_str());
}

void RdbSyncer::NotEqualTo(const RdbPredicateOperation &operation, DistributedDB::Query &query)
{
    query.NotEqualTo(operation.field_, operation.values_[0]);
    ZLOGI("field=%{public}s value=%{public}s", operation.field_.c_str(), operation.values_[0].c_str());
}

void RdbSyncer::And(const RdbPredicateOperation &operation, DistributedDB::Query &query)
{
    query.And();
    ZLOGI("");
}

void RdbSyncer::Or(const RdbPredicateOperation &operation, DistributedDB::Query &query)
{
    query.Or();
    ZLOGI("");
}

void RdbSyncer::OrderBy(const RdbPredicateOperation &operation, DistributedDB::Query &query)
{
    bool isAsc = operation.values_[0] == "true";
    query.OrderBy(operation.field_, isAsc);
    ZLOGI("field=%{public}s isAsc=%{public}s", operation.field_.c_str(), operation.values_[0].c_str());
}

void RdbSyncer::Limit(const RdbPredicateOperation &operation, DistributedDB::Query &query)
{
    char *end = nullptr;
    int limit = static_cast<int>(strtol(operation.field_.c_str(), &end, DECIMAL_BASE));
    int offset = static_cast<int>(strtol(operation.values_[0].c_str(), &end, DECIMAL_BASE));
    if (limit < 0) {
        limit = 0;
    }
    if (offset < 0) {
        offset = 0;
    }
    query.Limit(limit, offset);
    ZLOGI("limit=%{public}d offset=%{public}d", limit, offset);
}

DistributedDB::Query RdbSyncer::MakeQuery(const RdbPredicates &predicates)
{
    ZLOGI("table=%{public}s", predicates.table_.c_str());
    auto query = DistributedDB::Query::Select(predicates.table_);
    for (const auto& operation : predicates.operations_) {
        if (operation.operator_ >= 0 && operation.operator_ < OPERATOR_MAX) {
            HANDLES[operation.operator_](operation, query);
        }
    }
    return query;
}

int32_t RdbSyncer::DoSync(const SyncOption &option, const RdbPredicates &predicates, SyncResult &result)
{
    ZLOGI("enter");
    auto* delegate = GetDelegate();
    if (delegate == nullptr) {
        ZLOGE("delegate is nullptr");
        return RDB_ERROR;
    }

    std::vector<std::string> devices;
    if (predicates.devices_.empty()) {
        devices = NetworkIdToUUID(GetConnectDevices());
    } else {
        devices = NetworkIdToUUID(predicates.devices_);
    }

    ZLOGI("delegate sync");
    return delegate->Sync(devices, static_cast<DistributedDB::SyncMode>(option.mode),
                          MakeQuery(predicates), [&result] (const auto& syncStatus) {
                              HandleSyncStatus(syncStatus, result);
                          }, true);
}

int32_t RdbSyncer::DoAsync(const SyncOption &option, const RdbPredicates &predicates, const SyncCallback& callback)
{
    auto* delegate = GetDelegate();
    if (delegate == nullptr) {
        ZLOGE("delegate is nullptr");
        return RDB_ERROR;
    }

    std::vector<std::string> devices;
    if (predicates.devices_.empty()) {
        devices = NetworkIdToUUID(GetConnectDevices());
    } else {
        devices = NetworkIdToUUID(predicates.devices_);
    }

    ZLOGI("delegate sync");
    return delegate->Sync(devices, static_cast<DistributedDB::SyncMode>(option.mode),
                          MakeQuery(predicates), [callback] (const auto& syncStatus) {
                              SyncResult result;
                              HandleSyncStatus(syncStatus, result);
                              callback(result);
                          }, false);
}

int32_t RdbSyncer::RemoteQuery(const std::string& device, const std::string& sql,
                               const std::vector<std::string>& selectionArgs, sptr<IRemoteObject>& resultSet)
{
    ZLOGI("enter");
    auto* delegate = GetDelegate();
    if (delegate == nullptr) {
        ZLOGE("delegate is nullptr");
        return RDB_ERROR;
    }

    ZLOGI("delegate remote query");
    std::shared_ptr<DistributedDB::ResultSet> dbResultSet;
    DistributedDB::DBStatus status = delegate->RemoteQuery(device, {sql, selectionArgs},
                                                           REMOTE_QUERY_TIME_OUT, dbResultSet);
    if (status != DistributedDB::DBStatus::OK) {
        ZLOGE("DistributedDB remote query failed, status is  %{public}d.", status);
        return RDB_ERROR;
    }
    resultSet = new (std::nothrow) RdbResultSetImpl(dbResultSet);
    if (resultSet == nullptr) {
        ZLOGE("resultSet is nullptr");
        return RDB_ERROR;
    }
    return RDB_OK;
}
} // namespace OHOS::DistributedRdb
