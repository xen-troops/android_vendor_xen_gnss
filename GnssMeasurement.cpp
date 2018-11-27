/*
 * Copyright (C) 2018 The Android Open Source Project
 * Copyright (C) 2018 EPAM Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "GnssMeasurement.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_1 {
namespace xenvm {

// Methods from ::android::hardware::gnss::V1_0::IGnssMeasurement follow.
Return<::android::hardware::gnss::V1_0::IGnssMeasurement::GnssMeasurementStatus>
GnssMeasurement::setCallback(const sp<::android::hardware::gnss::V1_0::IGnssMeasurementCallback>&) {
    // TODO implement
    return ::android::hardware::gnss::V1_0::IGnssMeasurement::GnssMeasurementStatus{};
}

Return<void> GnssMeasurement::close() {
    // TODO implement
    return Void();
}

// Methods from ::android::hardware::gnss::V1_1::IGnssMeasurement follow.
Return<::android::hardware::gnss::V1_0::IGnssMeasurement::GnssMeasurementStatus>
GnssMeasurement::setCallback_1_1(
    const sp<::android::hardware::gnss::V1_1::IGnssMeasurementCallback>&, bool) {
    // TODO implement
    return ::android::hardware::gnss::V1_0::IGnssMeasurement::GnssMeasurementStatus{};
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace hardware
}  // namespace android
