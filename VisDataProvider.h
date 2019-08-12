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

#ifndef VISDATAPROVIDER_H_
#define VISDATAPROVIDER_H_

#include <android/hardware/gnss/1.0/types.h>
#include <uWS.h>
#include <string>

using android::hardware::gnss::V1_0::GnssLocation;

namespace android {
namespace hardware {
namespace gnss {
namespace V1_1 {
namespace xenvm {

enum ConnState {
    STATE_DISCONNECTED,
    STATE_CONNECTING,
    STATE_CONNECTED
};

class VisDataProvider {
 public:
    VisDataProvider()
    :mConnectedState(STATE_DISCONNECTED) { }

    int init();
    int pull();
    GnssLocation getLocation() const { return mLocation; }

 private:
    std::string mUri;
    uWS::Hub mHub;
    ConnState mConnectedState;
    GnssLocation mLocation;

    static constexpr const char* paramValueName = "value";
    static constexpr const char* paramTimestampName = "timestamp";
    static constexpr const char* paramLonName = "Signal.Cabin.Infotainment.Navigation.CurrentLocation.Longitude";
    static constexpr const char* paramLatName = "Signal.Cabin.Infotainment.Navigation.CurrentLocation.Latitude";
    static constexpr const char* paramVehSpeedName = "Signal.Cabin.Infotainment.Navigation.CurrentLocation.Speed";
    static constexpr const char* paramTelemtryAll = "Signal.Cabin.Infotainment.Navigation.CurrentLocation.*";
    static constexpr const char* requestTemplateGet = "{\
            \"action\": \"get\",\
            \"path\": \"%s\",\
            \"requestId\": \"%d\"\
    }";
    static const int kMaxBufferLength = 1024;
};


}  // namespace xenvm
}  // namespace V1_1
}  // namespace gnss
}  // namespace hardware
}  // namespace android

#endif  // VISDATAPROVIDER_H_
