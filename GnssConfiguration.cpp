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

#define LOG_TAG "GnssConfiguration"

#include "GnssConfiguration.h"
#include <log/log.h>

namespace android {
namespace hardware {
namespace gnss {
namespace V1_1 {
namespace xenvm {

// Methods from ::android::hardware::gnss::V1_0::IGnssConfiguration follow.
Return<bool> GnssConfiguration::setSuplEs(bool) {
    // TODO implement
    return bool{};
}

Return<bool> GnssConfiguration::setSuplVersion(uint32_t) {
    // TODO implement
    return bool{};
}

Return<bool> GnssConfiguration::setSuplMode(hidl_bitfield<SuplMode>) {
    // TODO implement
    return bool{};
}

Return<bool> GnssConfiguration::setGpsLock(hidl_bitfield<GpsLock>) {
    // TODO implement
    return bool{};
}

Return<bool> GnssConfiguration::setLppProfile(hidl_bitfield<LppProfile>) {
    // TODO implement
    return bool{};
}

Return<bool> GnssConfiguration::setGlonassPositioningProtocol(hidl_bitfield<GlonassPosProtocol>) {
    // TODO implement
    return bool{};
}

Return<bool> GnssConfiguration::setEmergencySuplPdn(bool) {
    // TODO implement
    return bool{};
}

// Methods from ::android::hardware::gnss::V1_1::IGnssConfiguration follow.
Return<bool> GnssConfiguration::setBlacklist(const hidl_vec<BlacklistedSource>& sourceList) {
    std::unique_lock<std::recursive_mutex> lock(mMutex);
    mBlacklistedConstellationSet.clear();
    mBlacklistedSourceSet.clear();
    for (auto source : sourceList) {
        if (source.svid == 0) {
            // Wildcard blacklist, i.e., blacklist entire constellation.
            mBlacklistedConstellationSet.insert(source.constellation);
        } else {
            mBlacklistedSourceSet.insert(source);
        }
    }
    return true;
}

Return<bool> GnssConfiguration::isBlacklisted(const GnssSvInfo& gnssSvInfo) const {
    std::unique_lock<std::recursive_mutex> lock(mMutex);
    if (mBlacklistedConstellationSet.find(gnssSvInfo.constellation) !=
        mBlacklistedConstellationSet.end()) {
        return true;
    }
    BlacklistedSource source = {.constellation = gnssSvInfo.constellation, .svid = gnssSvInfo.svid};
    return (mBlacklistedSourceSet.find(source) != mBlacklistedSourceSet.end());
}

std::recursive_mutex& GnssConfiguration::getMutex() const {
    return mMutex;
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace hardware
}  // namespace android
