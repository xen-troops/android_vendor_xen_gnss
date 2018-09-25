/*
 * Copyright (C) 2018 The Android Open Source Project
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

//#define LOG_NDEBUG 0
#define LOG_TAG "gnss.xenvm"

#include <android/hardware/gnss/1.0/types.h>
#include <log/log.h>

#include "Gnss.h"
#include "GnssConstants.h"
#include "GnssDebug.h"
#include "GnssMeasurement.h"

#include "json/reader.h"
#include "uWS.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_1 {
namespace xenvm {

#if 0

using namespace Json;


static const char* paramLonName = "Signal.Emulator.telemetry.lon";
static const char* paramLatName = "Signal.Emulator.telemetry.lat";
static const char* paramTimestampName = "timestamp";
static const char* paramTelemtryAll = "Signal.Emulator.telemetry.*";

static const char* requestTemplateGet = "{\
		\"action\": \"get\",\
		\"path\": \"%s\",\
		\"requestId\": \"%d\"\
	}";




static int requestId = 1;
static float gMockLatitudeDegrees = kMockLatitudeDegrees;
static float gMockLongitudeDegrees = kMockLongitudeDegrees;
static int64_t gMockTimestamp = kMockTimestamp;

uWS::Hub h;


void init() {
	ALOGV("init +++");
	 h.onError([](void *user) {
	        switch ((long) user) {
	        case 1:
	            ALOGE("Client emitted error on invalid URI");
	            break;
	        case 2:
	            ALOGE("Client emitted error on resolve failure");
	            break;
	        case 3:
	            ALOGE("Client emitted error on connection timeout (non-SSL)");
	            break;
	        case 5:
	            ALOGE("Client emitted error on connection timeout (SSL)");
	            break;
	        case 6:
	            ALOGE("Client emitted error on HTTP response without upgrade (non-SSL)");
	            break;
	        case 7:
	            ALOGE("Client emitted error on HTTP response without upgrade (SSL)");
	            break;
	        case 10:
	            ALOGE("Client emitted error on poll error");
	            break;
	        case 11:
	            static int protocolErrorCount = 0;
	            protocolErrorCount++;
	            ALOGE("Client emitted error on invalid protocol");
	            if (protocolErrorCount > 1) {
	                ALOGE( "FAILURE: %d errors emitted for one connection!", protocolErrorCount);
	                //abort();
	            }
	            break;
	        default:
	            ALOGE("FAILURE: %p should not emit error!", user);
	            //abort();
	        }
	    });

	    h.onTransfer([](uWS::WebSocket<uWS::CLIENT> *ws) {
	    	ALOGV("onTransfer");
	    	ws->close(1000);
	    });

	    h.onMessage([](uWS::WebSocket<uWS::CLIENT> *ws, char * c, size_t t, uWS::OpCode op) {
	        	ALOGV("onMessage");
	        	if (op == uWS::OpCode::TEXT) {

	        		std::string str(c, t);
	        		//ALOGD("onMessage text %s",str.c_str());
	        		Json::Value root;
	        		Json::Reader reader;
	        		if (reader.parse(str, root)) {
	        			Json::Value val = root["value"];
	        			if (!val.isNull()) {
	        				std:: string lon = val[paramLonName].asString();
	        				std:: string lat = val[paramLatName].asString();

	        				//ALOGD("-> LON : %s", lon.c_str());
	        				//ALOGD("-> LAT : %s", lat.c_str());

	        				gMockLatitudeDegrees = std::stof(lat.c_str());
	        				gMockLongitudeDegrees = std::stof(lon.c_str());

	        			}
	        			val = root[paramTimestampName];
	        			if (!val.isNull()) {
	        				gMockTimestamp = val.asInt64();
	        				ALOGD("-> TS : %lld", gMockTimestamp);
	        			}
	        		}


	                char buffer [1024];
	                sprintf(buffer, requestTemplateGet, paramTelemtryAll, requestId++);
	                ws->send(buffer);
	                return;
	        	}
	        	ws->close(1000);
	    });

	    h.onConnection([](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
	        switch ((long) ws->getUserData()) {
	        case 8:
	            ALOGE("Client established a remote connection over non-SSL");
	            ws->close(1000);
	            break;
	        case 9:
	            ALOGE("Client established a remote connection over SSL");
	            //while(true) {
	            char buffer [1024];
	            sprintf(buffer, requestTemplateGet, paramTelemtryAll, requestId++);
	            ws->send(buffer);
	            //}
	            //ws->close(1000);
	            break;
	        default:
	            ALOGE("FAILURE: %ld should not connect!", ws->getUserData());
	            exit(-1);
	        }
	    });

	    h.onDisconnection([](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
	        ALOGI("Client got disconnected with data: %ld code %d message:%s", (long)ws->getUserData() , code, std::string(message, length).c_str());
	    });
	    ALOGI("Will try wss://192.168.1.100");
	    h.connect("wss://192.168.1.100:8088", (void *) 9);
}


int pull() {
    h.poll();
    return 0;
}

#endif

using GnssSvFlags = IGnssCallback::GnssSvFlags;

const uint32_t MIN_INTERVAL_MILLIS = 100;
sp<::android::hardware::gnss::V1_1::IGnssCallback> Gnss::sGnssCallback = nullptr;

Gnss::Gnss() : mMinIntervalMs(10), mGnssConfiguration{new GnssConfiguration()}, mVis{std::string("wss://192.168.1.100:8088")} {}

Gnss::~Gnss() {
    stop();
}

// Methods from ::android::hardware::gnss::V1_0::IGnss follow.
Return<bool> Gnss::setCallback(const sp<::android::hardware::gnss::V1_0::IGnssCallback>&) {
    // Mock handles only new callback (see setCallback1_1) coming from Android P+
    return false;
}

Return<bool> Gnss::start() {
    if (mIsActive) {
        ALOGW("Gnss has started. Restarting...");
        stop();
    }

    mIsActive = true;
    mThread = std::thread([this]() {
    	mVis.init();
        while (mIsActive == true) {
        	mVis.pull();
            auto svStatus = this->getMockSvStatus();
            this->reportSvStatus(svStatus);

            auto location = this->getMockLocation();
            this->reportLocation(location);

            std::this_thread::sleep_for(std::chrono::milliseconds(mMinIntervalMs));
        }
    });
    return true;
}

Return<bool> Gnss::stop() {
    mIsActive = false;
    if (mThread.joinable()) {
        mThread.join();
    }
    return true;
}

Return<void> Gnss::cleanup() {
    // TODO implement
    return Void();
}

Return<bool> Gnss::injectTime(int64_t, int64_t, int32_t) {
    // TODO implement
    return bool{};
}

Return<bool> Gnss::injectLocation(double, double, float) {
    // TODO implement
    return bool{};
}

Return<void> Gnss::deleteAidingData(::android::hardware::gnss::V1_0::IGnss::GnssAidingData) {
    return Void();
}

Return<bool> Gnss::setPositionMode(::android::hardware::gnss::V1_0::IGnss::GnssPositionMode,
                                   ::android::hardware::gnss::V1_0::IGnss::GnssPositionRecurrence,
                                   uint32_t, uint32_t, uint32_t) {
    // TODO implement
    return bool{};
}

Return<sp<::android::hardware::gnss::V1_0::IAGnssRil>> Gnss::getExtensionAGnssRil() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IAGnssRil>{};
}

Return<sp<::android::hardware::gnss::V1_0::IGnssGeofencing>> Gnss::getExtensionGnssGeofencing() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssGeofencing>{};
}

Return<sp<::android::hardware::gnss::V1_0::IAGnss>> Gnss::getExtensionAGnss() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IAGnss>{};
}

Return<sp<::android::hardware::gnss::V1_0::IGnssNi>> Gnss::getExtensionGnssNi() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssNi>{};
}

Return<sp<::android::hardware::gnss::V1_0::IGnssMeasurement>> Gnss::getExtensionGnssMeasurement() {
    // TODO implement
    return new GnssMeasurement();
}

Return<sp<::android::hardware::gnss::V1_0::IGnssNavigationMessage>>
Gnss::getExtensionGnssNavigationMessage() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssNavigationMessage>{};
}

Return<sp<::android::hardware::gnss::V1_0::IGnssXtra>> Gnss::getExtensionXtra() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssXtra>{};
}

Return<sp<::android::hardware::gnss::V1_0::IGnssConfiguration>>
Gnss::getExtensionGnssConfiguration() {
    // TODO implement
    return new GnssConfiguration();
}

Return<sp<::android::hardware::gnss::V1_0::IGnssDebug>> Gnss::getExtensionGnssDebug() {
    return new GnssDebug();
}

Return<sp<::android::hardware::gnss::V1_0::IGnssBatching>> Gnss::getExtensionGnssBatching() {
    // TODO implement
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssBatching>{};
}

// Methods from ::android::hardware::gnss::V1_1::IGnss follow.
Return<bool> Gnss::setCallback_1_1(
    const sp<::android::hardware::gnss::V1_1::IGnssCallback>& callback) {
    if (callback == nullptr) {
        ALOGE("%s: Null callback ignored", __func__);
        return false;
    }

    sGnssCallback = callback;

    uint32_t capabilities = 0x0;
    auto ret = sGnssCallback->gnssSetCapabilitesCb(capabilities);
    if (!ret.isOk()) {
        ALOGE("%s: Unable to invoke callback", __func__);
    }

    IGnssCallback::GnssSystemInfo gnssInfo = {.yearOfHw = 2018};

    ret = sGnssCallback->gnssSetSystemInfoCb(gnssInfo);
    if (!ret.isOk()) {
        ALOGE("%s: Unable to invoke callback", __func__);
    }

    auto gnssName = "Xenvm-VIS GNSS Implementation v1.1";
    ret = sGnssCallback->gnssNameCb(gnssName);
    if (!ret.isOk()) {
        ALOGE("%s: Unable to invoke callback", __func__);
    }

    return true;
}

Return<bool> Gnss::setPositionMode_1_1(
    ::android::hardware::gnss::V1_0::IGnss::GnssPositionMode,
    ::android::hardware::gnss::V1_0::IGnss::GnssPositionRecurrence, uint32_t minIntervalMs,
    uint32_t, uint32_t, bool) {
    mMinIntervalMs = (minIntervalMs < MIN_INTERVAL_MILLIS) ? MIN_INTERVAL_MILLIS : minIntervalMs;
    return true;
}

Return<sp<::android::hardware::gnss::V1_1::IGnssConfiguration>>
Gnss::getExtensionGnssConfiguration_1_1() {
    return mGnssConfiguration;
}

Return<sp<::android::hardware::gnss::V1_1::IGnssMeasurement>>
Gnss::getExtensionGnssMeasurement_1_1() {
    // TODO implement
    return new GnssMeasurement();
}

Return<bool> Gnss::injectBestLocation(const GnssLocation&) {
    return true;
}

Return<GnssLocation> Gnss::getMockLocation() const {
#if 0
    GnssLocation location = {.gnssLocationFlags = 0xFF,
                             .latitudeDegrees = gMockLatitudeDegrees,
                             .longitudeDegrees = gMockLongitudeDegrees,
                             .altitudeMeters = kMockAltitudeMeters,
                             .speedMetersPerSec = kMockSpeedMetersPerSec,
                             .bearingDegrees = kMockBearingDegrees,
                             .horizontalAccuracyMeters = kMockHorizontalAccuracyMeters,
                             .verticalAccuracyMeters = kMockVerticalAccuracyMeters,
                             .speedAccuracyMetersPerSecond = kMockSpeedAccuracyMetersPerSecond,
                             .bearingAccuracyDegrees = kMockBearingAccuracyDegrees,
                             .timestamp = kMockTimestamp};
    return location;
#endif
    return mVis.getLocation();
}

Return<GnssSvInfo> Gnss::getSvInfo(int16_t svid, GnssConstellationType type, float cN0DbHz,
                                   float elevationDegrees, float azimuthDegrees) const {
    GnssSvInfo svInfo = {.svid = svid,
                         .constellation = type,
                         .cN0Dbhz = cN0DbHz,
                         .elevationDegrees = elevationDegrees,
                         .azimuthDegrees = azimuthDegrees,
                         .svFlag = GnssSvFlags::USED_IN_FIX | GnssSvFlags::HAS_EPHEMERIS_DATA |
                                   GnssSvFlags::HAS_ALMANAC_DATA};
    return svInfo;
}

Return<GnssSvStatus> Gnss::getMockSvStatus() const {
    std::unique_lock<std::recursive_mutex> lock(mGnssConfiguration->getMutex());
    GnssSvInfo mockGnssSvInfoList[] = {
        getSvInfo(3, GnssConstellationType::GPS, 32.5, 59.1, 166.5),
        getSvInfo(5, GnssConstellationType::GPS, 27.0, 29.0, 56.5),
        getSvInfo(17, GnssConstellationType::GPS, 30.5, 71.0, 77.0),
        getSvInfo(26, GnssConstellationType::GPS, 24.1, 28.0, 253.0),
        getSvInfo(5, GnssConstellationType::GLONASS, 20.5, 11.5, 116.0),
        getSvInfo(17, GnssConstellationType::GLONASS, 21.5, 28.5, 186.0),
        getSvInfo(18, GnssConstellationType::GLONASS, 28.3, 38.8, 69.0),
        getSvInfo(10, GnssConstellationType::GLONASS, 25.0, 66.0, 247.0)};

    GnssSvStatus svStatus = {.numSvs = sizeof(mockGnssSvInfoList) / sizeof(GnssSvInfo)};
    for (uint32_t i = 0; i < svStatus.numSvs; i++) {
        if (mGnssConfiguration->isBlacklisted(mockGnssSvInfoList[i])) {
            /**
             * Note well, this is a simple, mock emulation of not using a satellite by changing the
             * used bit.  Simply blanking the used bit, as is done here, is *not* an acceptable
             * actual device implementation - actual devices *must not* use the satellite in the
             * position calculation, as specified in IGnssConfiguration.hal.
             */
            mockGnssSvInfoList[i].svFlag &=
                ~static_cast<uint8_t>(IGnssCallback::GnssSvFlags::USED_IN_FIX);
        }
        svStatus.gnssSvList[i] = mockGnssSvInfoList[i];
    }

    return svStatus;
}

Return<void> Gnss::reportLocation(const GnssLocation& location) const {
    std::unique_lock<std::mutex> lock(mMutex);
    if (sGnssCallback == nullptr) {
        ALOGE("%s: sGnssCallback is null.", __func__);
        return Void();
    }
    sGnssCallback->gnssLocationCb(location);
    return Void();
}

Return<void> Gnss::reportSvStatus(const GnssSvStatus& svStatus) const {
    std::unique_lock<std::mutex> lock(mMutex);
    if (sGnssCallback == nullptr) {
        ALOGE("%s: sGnssCallback is null.", __func__);
        return Void();
    }
    sGnssCallback->gnssSvStatusCb(svStatus);
    return Void();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace hardware
}  // namespace android
