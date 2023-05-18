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

#include "result_set_common.h"

#include <string>
#include <vector>

#include "doc_errno.h"
#include "grd_base/grd_error.h"

namespace DocumentDB {
class ValueObject;
int InitResultSet(std::shared_ptr<QueryContext> &context, DocumentStore *store, ResultSet &resultSet, bool ifField)
{
    if (ifField == false) {
        if (context->projectionTree.ParseTree(context->projectionPath) == -E_INVALID_ARGS) {
            GLOGE("Parse ProjectionTree failed");
            return -E_INVALID_ARGS;
        }
    }
    return resultSet.Init(context, store, ifField);
}
} // namespace DocumentDB
