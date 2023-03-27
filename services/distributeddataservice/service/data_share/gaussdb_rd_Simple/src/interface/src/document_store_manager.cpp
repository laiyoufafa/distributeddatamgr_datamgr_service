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

#include "document_store_manager.h"
#include "doc_errno.h"
#include "doc_limit.h"
#include "grd_base/grd_type_export.h"
#include "kv_store_manager.h"
#include "log_print.h"
#include "os_api.h"

namespace DocumentDB {
int DocumentStoreManager::GetDocumentStore(const std::string &path, const std::string &config, DocumentStore *&store)
{
    std::string canonicalPath;
    std::string dbName;
    int errCode = E_OK;
    if (!CheckDBPath(path, canonicalPath, dbName, errCode)) {
        GLOGE("Check document db file path failed.");
        return errCode;
    }

    if (!CheckDBConfig(config, errCode)) {
        GLOGE("Check document db config failed.");
        return errCode;
    }

    KvStoreExecutor *executor = nullptr;
    KvStoreManager::GetKvStore(canonicalPath, executor);
    store = new (std::nothrow) DocumentStore(executor);
    return errCode;
}

int DocumentStoreManager::CloseDocumentStore(DocumentStore *store, CloseType type)
{
    if (type == CloseType::NORMAL) {
        // TODO: check result set
    }

    delete store;
    return E_OK;
}

bool DocumentStoreManager::CheckDBPath(const std::string &path, std::string &canonicalPath, std::string &dbName,
    int &errCode)
{
    if (path.empty()) {
        GLOGE("Invalid path empty");
        errCode = -E_INVALID_ARGS;
        return false;
    }

    if (path.back() == '/') {
        GLOGE("Invalid path end with slash");
        errCode = -E_INVALID_ARGS;
        return false;
    }

    std::string dirPath;
    OSAPI::SplitFilePath(path, dirPath, dbName);

    std::string canonicalDir;
    int innerErrCode = OSAPI::GetRealPath(dirPath, canonicalDir);
    if (innerErrCode != E_OK) {
        GLOGE("Get real path failed. %d", errCode);
        errCode = -E_FILE_OPERATION;
        return false;
    }

    return true;
}

bool DocumentStoreManager::CheckDBConfig(const std::string &config, int &errCode)
{
    if (config.empty()) {
        return true;
    }

    if (config.length() > MAX_JSON_LEN) {
        GLOGE("Config json string is too long.");
        errCode = -E_OVER_LIMIT;
        return false;
    }

    std::shared_ptr<JsonObject> dbConfig;
    errCode = JsonObject::Parse(config, dbConfig);
    if (errCode != E_OK) {
        GLOGE("Check DB config failed. %d", errCode);
        return false;
    }

    ValueObject maxConnNum;
    dbConfig->GetObjectByPath({"maxConnNum"}, maxConnNum);
    if (maxConnNum.valueType != ValueObject::ValueType::VALUE_NUMBER) {
        GLOGE("Check DB config failed, the field type of maxConnNum is not NUMBER. %d", errCode);
        return false;
    }

    return true;
}
} // DocumentDB