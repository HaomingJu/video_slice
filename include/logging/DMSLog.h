#ifndef __DMS_DMSLOG_H__
#define __DMS_DMSLOG_H__

#include "hobotlog/hobotlog.hpp"
#include <string>

#ifndef MODULE_TAG
#define MODULE_TAG "HobotDMS"
#endif

#define LOG_FATAL_LEVEL 0
#define LOG_ERROR_LEVEL 1
#define LOG_WARN_LEVEL 2
#define LOG_INFO_LEVEL 3
#define LOG_DEBUG_LEVEL 4

#ifdef LOGE_T
#undef LOGE_T
#define LOGE_T(tag) LOG_TAG(rtc::HOBOT_LOG_ERROR, std::string("[ERRO] ") + tag)
#endif

#ifdef LOGV_T
#undef LOGV_T
#define LOGV_T(tag) LOG_TAG(rtc::HOBOT_LOG_VERBOSE, std::string("[VRBS] ") + tag)

#endif
#ifdef LOGD_T
#undef LOGD_T
#define LOGD_T(tag) LOG_TAG(rtc::HOBOT_LOG_DEBUG, std::string("[DEBG] ") + tag)
#endif

#ifdef LOGI_T
#undef LOGI_T
#define LOGI_T(tag) LOG_TAG(rtc::HOBOT_LOG_INFO, std::string("[INFO] ") + tag)
#endif

#ifdef LOGW_T
#undef LOGW_T
#define LOGW_T(tag) LOG_TAG(rtc::HOBOT_LOG_WARN, std::string("[WARN] ") + tag)
#endif


#define FUN_IN(modulename)                                   \
    do {                                                     \
        LOGV_T(modulename) << "(" << __func__<< ") ++in"; \
    } while (0);
#define FUN_OUT(modulename)                                   \
    do {                                                      \
        LOGV_T(modulename) << "(" << __func__<< ") --out"; \
    } while (0);

#endif  //__DMS_DMSLOG_H__
