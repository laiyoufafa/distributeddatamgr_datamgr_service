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

#include "objectservicestub_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "object_service_impl.h"
#include "message_parcel.h"
#include "securec.h"

using namespace OHOS::DistributedObject;

namespace OHOS {
const std::u16string INTERFACE_TOKEN = u"OHOS.DistributedObject.IObjectService";
const uint32_t CODE_MIN = 0;
const uint32_t CODE_MAX = 4;

bool OnRemoteRequestFuzz(const uint8_t *data, size_t size)
{
    uint32_t code = static_cast<uint32_t>(*data) % (CODE_MAX - CODE_MIN + 1) + CODE_MIN;
    MessageParcel request;
    request.WriteInterfaceToken(INTERFACE_TOKEN);
    request.WriteBuffer(data, size);
    request.RewindRead(0);
    MessageParcel reply;
    std::shared_ptr<ObjectServiceStub> objectServiceStub = std::make_shared<ObjectServiceImpl>();
    objectServiceStub->OnRemoteRequest(code, request, reply);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }

    OHOS::OnRemoteRequestFuzz(data, size);

    return 0;
}