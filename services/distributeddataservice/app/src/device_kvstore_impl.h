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

#ifndef DEVICE_KVSTORE_IMPL_H
#define DEVICE_KVSTORE_IMPL_H

#include "single_kvstore_impl.h"

namespace OHOS::DistributedKv {
union KeyEncap {
    int len;
    uint8_t byteLen[sizeof(int)];
} __attribute__((packed));

class DeviceKvStoreImpl : public SingleKvStoreImpl {
public:
    DeviceKvStoreImpl(const Options &options, const std::string &userId,
        const std::string &bundleName, const std::string &storeId, const std::string &appId,
        const std::string &directory, DistributedDB::KvStoreNbDelegate *delegate);
    ~DeviceKvStoreImpl();
    Status Put(const Key &key, const Value &value) override;
    Status Delete(const Key &key) override;
    Status Get(const Key &key, Value &value) override;
    Status GetEntries(const Key &prefixKey, std::vector<Entry> &entries) override;
    Status GetEntriesWithQuery(const std::string &query, std::vector<Entry> &entries) override;
    void GetResultSet(const Key &prefixKey, std::function<void(Status, sptr<IKvStoreResultSet>)> callback) override;
    Status PutBatch(const std::vector<Entry> &entries) override;
    Status DeleteBatch(const std::vector<Key> &keys) override;
    static std::string GetLocalDeviceId();

protected:
    KvStoreObserverImpl *CreateObserver(const SubscribeType subscribeType, sptr<IKvStoreObserver> observer) override;
    KvStoreResultSetImpl *CreateResultSet(
        DistributedDB::KvStoreResultSet *resultSet, const DistributedDB::Key &prix) override;

private:
    bool AddKeyPrefixAndSuffix(const Key &in, std::vector<uint8_t> &out);
    Status DeleteKeyPrefix(const Key &in, std::vector<uint8_t> &out);
    void DeletePrefixAndSuffix(const Key &in, std::vector<uint8_t> &out);
    static std::string localDeviceId_;
};
} // namespace OHOS::DistributedKv
#endif // DEVICE_KVSTORE_IMPL_H
