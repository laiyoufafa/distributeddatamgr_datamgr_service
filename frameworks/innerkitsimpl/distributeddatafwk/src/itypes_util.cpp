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

#define LOG_TAG "ITypesUtil"

#include "itypes_util.h"

#include "autils/constant.h"
#include "log_print.h"

namespace OHOS::DistributedKv {
bool ITypesUtil::Marshalling(MessageParcel &data)
{
    return true;
}

bool ITypesUtil::Unmarshalling(MessageParcel &data)
{
    return true;
}

bool ITypesUtil::Marshalling(const std::string &input, MessageParcel &data)
{
    return data.WriteString(input);
}

bool ITypesUtil::Unmarshalling(MessageParcel &data, std::string &output)
{
    return data.ReadString(output);
}

bool ITypesUtil::Marshalling(const Blob &blob, MessageParcel &data)
{
    return data.WriteUInt8Vector(blob.Data());
}

bool ITypesUtil::Unmarshalling(MessageParcel &data, Blob &output)
{
    std::vector<uint8_t> blob;
    bool result = data.ReadUInt8Vector(&blob);
    output = blob;
    return result;
}

bool ITypesUtil::Marshalling(const Entry &entry, MessageParcel &data)
{
    if (!Marshalling(entry.key, data)) {
        return false;
    }
    return Marshalling(entry.value, data);
}

bool ITypesUtil::Unmarshalling(MessageParcel &data, Entry &output)
{
    if (!Unmarshalling(data, output.key)) {
        return false;
    }
    return Unmarshalling(data, output.value);
}

bool ITypesUtil::Marshalling(const DeviceInfo &entry, MessageParcel &data)
{
    if (!data.WriteString(entry.deviceId)) {
        return false;
    }
    if (!data.WriteString(entry.deviceName)) {
        return false;
    }
    return data.WriteString(entry.deviceType);
}

bool ITypesUtil::Unmarshalling(MessageParcel &data, DeviceInfo &output)
{
    if (!data.ReadString(output.deviceId)) {
        return false;
    }
    if (!data.ReadString(output.deviceName)) {
        return false;
    }
    return data.ReadString(output.deviceType);
}

bool ITypesUtil::Marshalling(const ChangeNotification &notification, MessageParcel &parcel)
{
    if (!Marshalling(notification.GetInsertEntries(), parcel)) {
        return false;
    }

    if (!Marshalling(notification.GetUpdateEntries(), parcel)) {
        return false;
    }

    if (!Marshalling(notification.GetDeleteEntries(), parcel)) {
        return false;
    }
    if (!parcel.WriteString(notification.GetDeviceId())) {
        ZLOGE("WriteString deviceId_ failed.");
        return false;
    }

    return parcel.WriteBool(notification.IsClear());
}

bool ITypesUtil::Unmarshalling(MessageParcel &parcel, ChangeNotification &output)
{
    std::vector<Entry> insertEntries;
    if (!Unmarshalling(parcel, insertEntries)) {
        return false;
    }
    std::vector<Entry> updateEntries;
    if (!Unmarshalling(parcel, updateEntries)) {
        return false;
    }
    std::vector<Entry> deleteEntries;
    if (!Unmarshalling(parcel, deleteEntries)) {
        return false;
    }
    std::string deviceId;
    if (!parcel.ReadString(deviceId)) {
        ZLOGE("WriteString deviceId_ failed.");
        return false;
    }
    bool isClear = false;
    if (!parcel.ReadBool(isClear)) {
        ZLOGE("WriteString deviceId_ failed.");
        return false;
    }
    output = ChangeNotification(
        std::move(insertEntries), std::move(updateEntries), std::move(deleteEntries), deviceId, isClear);
    return true;
}

bool ITypesUtil::Marshalling(const DistributedRdb::RdbSyncerParam &param, MessageParcel &parcel)
{
    if (!parcel.WriteString(param.bundleName_)) {
        ZLOGE("RdbStoreParam write bundle name failed");
        return false;
    }
    if (!parcel.WriteString(param.relativePath_)) {
        ZLOGE("RdbStoreParam write directory failed");
        return false;
    }
    if (!parcel.WriteString(param.storeName_)) {
        ZLOGE("RdbStoreParam write store name failed");
        return false;
    }
    if (!parcel.WriteString(param.encryptLevel_)) {
        ZLOGE("RdbStoreParam write security level failed");
        return false;
    }
    if (!parcel.WriteInt32(param.type_)) {
        ZLOGE("RdbStoreParam write type failed");
        return false;
    }
    if (!parcel.WriteBool(param.isAutoSync_)) {
        ZLOGE("RdbStoreParam write auto sync failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Unmarshalling(MessageParcel &parcel, DistributedRdb::RdbSyncerParam &param)
{
    if (!parcel.ReadString(param.bundleName_)) {
        ZLOGE("RdbStoreParam read bundle name failed");
        return false;
    }
    if (!parcel.ReadString(param.relativePath_)) {
        ZLOGE("RdbStoreParam read directory failed");
        return false;
    }
    if (!parcel.ReadString(param.storeName_)) {
        ZLOGE("RdbStoreParam read store name failed");
        return false;
    }
    if (!parcel.ReadString(param.encryptLevel_)) {
        ZLOGE("RdbStoreParam read security level failed");
        return false;
    }
    if (!parcel.ReadInt32(param.type_)) {
        ZLOGE("RdbStoreParam read type failed");
        return false;
    }
    if (!parcel.ReadBool(param.isAutoSync_)) {
        ZLOGE("RdbStoreParam read auto sync failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Marshalling(const DistributedRdb::SyncResult &result, MessageParcel &parcel)
{
    if (!parcel.WriteInt32(static_cast<int32_t>(result.size()))) {
        ZLOGE("SyncResult write size failed");
        return false;
    }

    for (const auto &entry : result) {
        if (!parcel.WriteString(entry.first)) {
            ZLOGE("SyncResult write device failed");
            return false;
        }
        if (!parcel.WriteInt32(entry.second)) {
            ZLOGE("SyncResult write int failed");
            return false;
        }
    }
    return true;
}

bool ITypesUtil::Unmarshalling(MessageParcel &parcel, DistributedRdb::SyncResult &result)
{
    int32_t size = 0;
    if (!parcel.ReadInt32(size)) {
        ZLOGE("SyncResult read size failed");
        return false;
    }
    if (size <= 0) {
        ZLOGE("SyncResult size invalid");
        return false;
    }

    for (int32_t i = 0; i < size; i++) {
        std::string device;
        if (!parcel.ReadString(device)) {
            ZLOGE("SyncResult read device failed");
            return false;
        }
        int32_t error;
        if (!parcel.ReadInt32(error)) {
            ZLOGE("SyncResult read int failed");
            return false;
        }
        result.insert({ device, error });
    }
    return true;
}

bool ITypesUtil::Marshalling(const DistributedRdb::SyncOption &option, MessageParcel &parcel)
{
    if (!parcel.WriteInt32(option.mode)) {
        ZLOGE("SyncOption write mode failed");
        return false;
    }
    if (!parcel.WriteBool(option.isBlock)) {
        ZLOGE("SyncOption write isBlock failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Unmarshalling(MessageParcel &parcel, DistributedRdb::SyncOption &option)
{
    int32_t mode;
    if (!parcel.ReadInt32(mode)) {
        ZLOGE("SyncOption read mode failed");
        return false;
    }
    option.mode = static_cast<DistributedRdb::SyncMode>(mode);
    if (!parcel.ReadBool(option.isBlock)) {
        ZLOGE("SyncOption read isBlock failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Marshalling(const DistributedRdb::RdbPredicates &predicates, MessageParcel &parcel)
{
    if (!parcel.WriteString(predicates.table_)) {
        ZLOGE("predicate write table failed");
        return false;
    }
    if (!parcel.WriteStringVector(predicates.devices_)) {
        ZLOGE("predicate write devices failed");
        return false;
    }
    if (!parcel.WriteUint32(predicates.operations_.size())) {
        ZLOGE("predicate write operation size failed");
        return false;
    }
    for (const auto &operation : predicates.operations_) {
        if (!parcel.WriteInt32(operation.operator_)) {
            ZLOGE("predicate write operator failed");
            return false;
        }
        if (!parcel.WriteString(operation.field_)) {
            ZLOGE("predicate write field failed");
            return false;
        }
        if (!parcel.WriteStringVector(operation.values_)) {
            ZLOGE("predicate write values failed");
            return false;
        }
    }
    return true;
}

bool ITypesUtil::Unmarshalling(MessageParcel &parcel, DistributedRdb::RdbPredicates &predicates)
{
    if (!parcel.ReadString(predicates.table_)) {
        ZLOGE("predicate read table failed");
        return false;
    }
    if (!parcel.ReadStringVector(&predicates.devices_)) {
        ZLOGE("predicate read devices failed");
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        ZLOGE("predicate read operation size failed");
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        int32_t op;
        if (!parcel.ReadInt32(op)) {
            ZLOGE("predicate read operator failed");
            return false;
        }
        DistributedRdb::RdbPredicateOperation operation;
        operation.operator_ = static_cast<DistributedRdb::RdbPredicateOperator>(op);
        if (!parcel.ReadString(operation.field_)) {
            ZLOGE("predicate read field failed");
            return false;
        }
        if (!parcel.ReadStringVector(&operation.values_)) {
            ZLOGE("predicate read values failed");
            return false;
        }
        predicates.operations_.push_back(std::move(operation));
    }
    return true;
}

bool ITypesUtil::Marshalling(const Options &input, MessageParcel &data)
{
    if (!data.WriteString(input.schema)) {
        ZLOGE("schema is failed");
        return false;
    }
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(sizeof(input));
    Options *target = reinterpret_cast<Options *>(buffer.get());
    target->createIfMissing = input.createIfMissing;
    target->encrypt = input.encrypt;
    target->persistent = input.persistent;
    target->backup = input.backup;
    target->autoSync = input.autoSync;
    target->securityLevel = input.securityLevel;
    target->syncPolicy = input.syncPolicy;
    target->kvStoreType = input.kvStoreType;
    target->syncable = input.syncable;
    target->dataOwnership = input.dataOwnership;
    return data.WriteRawData(buffer.get(), sizeof(input));
}

bool ITypesUtil::Unmarshalling(MessageParcel &data, Options &output)
{
    if (!data.ReadString(output.schema)) {
        ZLOGE("read schema failed");
        return false;
    }
    const Options *source = reinterpret_cast<const Options *>(data.ReadRawData(sizeof(output)));
    if (source == nullptr) {
        return false;
    }
    output.createIfMissing = source->createIfMissing;
    output.encrypt = source->encrypt;
    output.persistent = source->persistent;
    output.backup = source->backup;
    output.autoSync = source->autoSync;
    output.securityLevel = source->securityLevel;
    output.syncPolicy = source->syncPolicy;
    output.kvStoreType = source->kvStoreType;
    output.syncable = source->syncable;
    output.dataOwnership = source->dataOwnership;
    return true;
}

int64_t ITypesUtil::GetTotalSize(const std::vector<Entry> &entries)
{
    int64_t bufferSize = 1;
    for (const auto &item : entries) {
        if (item.key.Size() > Constant::MAX_KEY_LENGTH || item.value.Size() > Constant::MAX_VALUE_LENGTH) {
            return -bufferSize;
        }
        bufferSize += item.key.RawSize() + item.value.RawSize();
    }
    return bufferSize - 1;
}

int64_t ITypesUtil::GetTotalSize(const std::vector<Key> &entries)
{
    int64_t bufferSize = 1;
    for (const auto &item : entries) {
        if (item.Size() > Constant::MAX_KEY_LENGTH) {
            return -bufferSize;
        }
        bufferSize += item.RawSize();
    }
    return bufferSize - 1;
}
} // namespace OHOS::DistributedKv
