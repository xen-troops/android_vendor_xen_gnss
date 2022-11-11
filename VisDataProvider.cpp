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

// #define LOG_NDEBUG 0
#define LOG_TAG "gnss.vis-provider.xenvm"

#include <cutils/properties.h>
#include <log/log.h>
#include <json/reader.h>

#include <string>

#include <GnssConstants.h>

#include "VisDataProvider.h"

using Json::Reader;
using Json::Value;

namespace android {
namespace hardware {
namespace gnss {
namespace V1_1 {
namespace xenvm {

const int32_t cookie = 0xdeadbeef;

GnssLocation VisDataProvider::getLocation() const{
    const std::lock_guard<std::recursive_mutex> lock(mProtector);
    return mLocation;
}

int VisDataProvider::init() {
    const std::lock_guard<std::recursive_mutex> lock(mProtector);

    char propValue[PROPERTY_VALUE_MAX] = {};
    property_get("persist.vendor.vis.uri", propValue, "wss://wwwivi:8088");
    mUri = propValue;
    static unsigned int requestid = 0;
    mLocation.gnssLocationFlags = 0xFF;
    mLocation.altitudeMeters = kMockAltitudeMeters;
    mLocation.speedMetersPerSec = kMockSpeedMetersPerSec;
    mLocation.bearingDegrees = kMockBearingDegrees;
    mLocation.horizontalAccuracyMeters = kMockHorizontalAccuracyMeters;
    mLocation.verticalAccuracyMeters = kMockVerticalAccuracyMeters;
    mLocation.speedAccuracyMetersPerSecond = kMockSpeedAccuracyMetersPerSecond;
    mLocation.bearingAccuracyDegrees = kMockBearingAccuracyDegrees;

    mHub.onError([this](void *code) {
        const std::lock_guard<std::recursive_mutex> lock(mProtector);

        ALOGE("Websocket FAILURE: %p", code);
        if (mConnectedState == STATE_CONNECTING) {
            mConnectedState = STATE_DISCONNECTED;
        }
    });

    mHub.onTransfer([this](uWS::WebSocket<uWS::CLIENT> *ws) {
        const std::lock_guard<std::recursive_mutex> lock(mProtector);

        ALOGV("onTransfer");
        ws->close();
    });

    mHub.onMessage([this](uWS::WebSocket<uWS::CLIENT> *ws, char * c, size_t t, uWS::OpCode op) {
        const std::lock_guard<std::recursive_mutex> lock(mProtector);

        ALOGV("onMessage");
        if (op == uWS::OpCode::TEXT) {
            std::string str(c, t);
            Json::Value rootVal;
            Json::Reader reader;
            if (reader.parse(str, rootVal)) {
                Json::Value value = rootVal[paramValueName];
                if (!value.isNull()) {
                    if (!value[paramLonName].isNull()) {
                        mLocation.longitudeDegrees =
                                std::stof(value[paramLonName].asString().c_str());
                    } else {
                        ALOGE("Parameter %s not found", paramLonName);
                    }
                    if (!value[paramLatName].isNull()) {
                        mLocation.latitudeDegrees =
                                std::stof(value[paramLatName].asString().c_str());
                    } else {
                        ALOGE("Parameter %s not found", paramLatName);
                    }
                    if (!value[paramVehSpeedName].isNull()) {
                        mLocation.speedMetersPerSec =
                                ((value[paramVehSpeedName].asUInt() * 1000) / (60*60));
                    } else {
                        ALOGE("Parameter %s not found", paramVehSpeedName);
                    }
                }
                if (!rootVal[paramTimestampName].isNull()) {
                    mLocation.timestamp = rootVal[paramTimestampName].asInt64();
                } else {
                    ALOGE("Parameter %s not found", paramTimestampName);
                }
            }

            char buffer[kMaxBufferLength];
            snprintf(buffer, kMaxBufferLength, requestTemplateGet, paramTelemtryAll, requestid++);
            ws->send(buffer);
            return;
        }
        ws->close();
    });

    mHub.onConnection([this](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest /*req*/) {
        const std::lock_guard<std::recursive_mutex> lock(mProtector);

        switch (reinterpret_cast<int64_t>(ws->getUserData())) {
        case cookie:
            ALOGI("Client established a remote connection over SSL");
            mConnectedState = STATE_CONNECTED;
            char buffer[kMaxBufferLength];
            snprintf(buffer, kMaxBufferLength, requestTemplateGet, paramTelemtryAll, requestid++);
            ws->send(buffer);
            break;
        default:
            ALOGE("Unexpected user data: %ld!", reinterpret_cast<int64_t>(ws->getUserData()));
            ws->close();
        }
    });

    mHub.onDisconnection([this](uWS::WebSocket<uWS::CLIENT> *ws, int code,
            char *message, size_t length) {
        const std::lock_guard<std::recursive_mutex> lock(mProtector);

        ALOGI("Client got disconnected with data: %ld code %d message:%s",
                reinterpret_cast<int64_t>(ws->getUserData()), code,
                std::string(message, length).c_str());
        mConnectedState = STATE_DISCONNECTED;
    });

    ALOGI("Will try uri[%s]", mUri.c_str());
    mHub.connect(mUri.c_str(), (void *) cookie);
    mConnectedState = STATE_CONNECTING;
    return 0;
}

int VisDataProvider::pull() {
    if (mConnectedState == STATE_DISCONNECTED) {
        init();
    }
    mHub.poll();
    return 0;
}

bool VisDataProvider::waitConnection(int s) const {
    ALOGV("waitConnectionMs");
    int minSleepTime = std::min(s, 1);
    int timeSleeped = 0;
    while (true) {
        sleep(minSleepTime);
        timeSleeped += minSleepTime;
        if (timeSleeped >= s) {
            return mConnectedState == STATE_CONNECTED;
        } else if (mConnectedState == STATE_CONNECTED) {
            return true;
        }
    }

    return true;
}

}  // namespace xenvm
}  // namespace V1_1
}  // namespace gnss
}  // namespace hardware
}  // namespace android

