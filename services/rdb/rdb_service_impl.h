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

#ifndef DISTRIBUTEDDATASERVICE_RDB_SERVICE_H
#define DISTRIBUTEDDATASERVICE_RDB_SERVICE_H

#include "rdb_service_stub.h"

#include <map>
#include <mutex>
#include <string>
#include "rdb_syncer_impl.h"
#include "concurrent_map.h"

namespace OHOS::DistributedRdb {
class RdbServiceImpl : public RdbServiceStub {
public:
    sptr<IRdbSyncer> GetRdbSyncerInner(const RdbSyncerParam& param) override;
    
    int RegisterClientDeathRecipient(const std::string& bundleName, sptr<IRemoteObject> proxy) override;
    
    void OnClientDied(const std::string& bundleName, sptr<IRemoteObject>& proxy);

private:
    bool CheckAccess(const RdbSyncerParam& param) const;
    
    sptr<IRdbSyncer> CreateSyncer(const RdbSyncerParam& param);
    
    void ClearClientRecipient(const std::string& bundleName, sptr<IRemoteObject>& proxy);
    
    void ClearClientSyncers(const std::string& bundleName);
    
    class ClientDeathRecipient : public DeathRecipient {
    public:
        using DeathCallback = std::function<void(sptr<IRemoteObject>&)>;
        explicit ClientDeathRecipient(DeathCallback& callback);
        ~ClientDeathRecipient() override;
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    private:
        DeathCallback callback_;
    };
    
    ConcurrentMap<std::string, sptr<RdbSyncerImpl>> syncers_; // identifier
    ConcurrentMap<sptr<IRemoteObject>, sptr<ClientDeathRecipient>> recipients_;
};
}
#endif
