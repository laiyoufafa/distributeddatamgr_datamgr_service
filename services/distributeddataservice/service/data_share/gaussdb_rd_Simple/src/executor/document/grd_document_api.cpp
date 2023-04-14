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

#include "grd_document/grd_document_api.h"
#include "grd_base/grd_error.h"
#include "grd_type_inner.h"
#include "log_print.h"
using namespace DocumentDB;

int GRD_CreateCollection(GRD_DB *db, const char *collectionName, const char *optionStr, unsigned int flags)
{
    if (db == nullptr || db->store_ == nullptr) {
        return GRD_INVALID_ARGS;
    }

    std::string name = (collectionName == nullptr ? "" : collectionName);
    std::string option = (optionStr == nullptr ? "" : optionStr);
    int ret = db->store_->CreateCollection(name, option, flags);
    return TrasnferDocErr(ret);
}

int GRD_DropCollection(GRD_DB *db, const char *collectionName, unsigned int flags)
{
    if (db == nullptr || db->store_ == nullptr) {
        return GRD_INVALID_ARGS;
    }

    std::string name = (collectionName == nullptr ? "" : collectionName);
    int ret = db->store_->DropCollection(name, flags);
    return TrasnferDocErr(ret);
}

int GRD_UpdateDoc(GRD_DB *db, const char *collectionName, const char *filter, const char *update, unsigned int flags)
{
    if (db == nullptr || db->store_ == nullptr) {
        return GRD_INVALID_ARGS;
    }

    std::string name = (collectionName == nullptr ? "" : collectionName);
    std::string filterStr = (filter == nullptr ? "" : filter);
    std::string updateStr = (update == nullptr ? "" : update);
    int ret = db->store_->UpdateDocument(name, filterStr, updateStr, flags);
    return TrasnferDocErr(ret);
}

int GRD_UpsertDoc(GRD_DB *db, const char *collectionName, const char *filter, const char *document, unsigned int flags)
{
    if (db == nullptr || db->store_ == nullptr) {
        return GRD_INVALID_ARGS;
    }

    std::string name = (collectionName == nullptr ? "" : collectionName);
    std::string filterStr = (filter == nullptr ? "" : filter);
    std::string documentStr = (document == nullptr ? "" : document);
    int ret = db->store_->UpsertDocument(name, filterStr, documentStr, flags);
    return TrasnferDocErr(ret);
}
