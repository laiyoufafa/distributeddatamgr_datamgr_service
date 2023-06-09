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

#ifndef DISTRIBUTEDDATAMGR_KVSTORE_RESULTSET_CLIENT_H
#define DISTRIBUTEDDATAMGR_KVSTORE_RESULTSET_CLIENT_H

#include "ikvstore_resultset.h"
#include "kvstore_result_set.h"

namespace OHOS::DistributedKv {
class KvStoreResultSetClient : public KvStoreResultSet {
public:
    explicit KvStoreResultSetClient(sptr<IKvStoreResultSet> kvStoreProxy);

    ~KvStoreResultSetClient()
    {}

    int GetCount() const override;

    int GetPosition() const override;

    bool MoveToFirst() override;

    bool MoveToLast() override;

    bool MoveToNext() override;

    bool MoveToPrevious() override;

    bool Move(int offset) override;

    bool MoveToPosition(int position) override;

    bool IsFirst() const override;

    bool IsLast() const override;

    bool IsBeforeFirst() const override;

    bool IsAfterLast() const override;

    Status GetEntry(Entry &entry) const override;

    sptr<IKvStoreResultSet> GetKvStoreResultSetProxy() const;

private:
    sptr<IKvStoreResultSet> kvStoreResultSetProxy_;
};
} // namespace OHOS::DistributedKv
#endif // DISTRIBUTEDDATAMGR_KVSTORE_RESULTSET_CLIENT_H
