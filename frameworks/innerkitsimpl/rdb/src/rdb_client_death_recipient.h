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

#ifndef IRDB_CLIENT_DEATH_RECIPIENT_H
#define IRDB_CLIENT_DEATH_RECIPIENT_H

#include <iremote_broker.h>
#include <iremote_proxy.h>
#include <iremote_stub.h>

namespace OHOS::DistributedRdb {
class IRdbClientDeathRecipient : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedRdb.IRdbClientDeathRecipient");
};

class RdbClientDeathRecipientProxy : public IRemoteProxy<IRdbClientDeathRecipient> {
public:
    explicit RdbClientDeathRecipientProxy(const sptr<IRemoteObject>& object);
    virtual ~RdbClientDeathRecipientProxy();

private:
    static inline BrokerDelegator<RdbClientDeathRecipientProxy> delegator_;
};

class RdbClientDeathRecipientStub : public IRemoteStub<IRdbClientDeathRecipient> {
public:
    RdbClientDeathRecipientStub();
    virtual ~RdbClientDeathRecipientStub();
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
};
}
#endif
