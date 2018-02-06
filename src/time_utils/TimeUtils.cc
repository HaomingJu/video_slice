#include <string.h>
#include <chrono>
#include <ctime>
#include <thread>

#define MODULE_TAG "TIMEUTILS"
#include <string.h>
#include <string>
#include "logging/DMSLog.h"
#include "time_utils/TimeUtils.h"

int64_t TimeUtils::mStartEpochTime = 0L;
int64_t TimeUtils::mInitTimestamp;
int TimeUtils::mMultiple = 1;
int TimeUtils::mDivider = 1;

#ifdef android
#include <sstream>
int stoi(const std::string &s) {
  std::stringstream stream;
  int i;
  stream << s;
  stream >> i;
  return i;
}
#endif  // android

void TimeUtils::reset() {
  std::chrono::milliseconds time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
  mStartEpochTime = time.count();
  mInitTimestamp = mStartEpochTime;
  mMultiple = 1;
  mDivider = 1;
}

void TimeUtils::initTimeUtil(int64_t curEpochTime, int multiple, int divider) {
  std::chrono::milliseconds time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
  mStartEpochTime = time.count();
  mInitTimestamp = curEpochTime;
  mMultiple = multiple;
  mDivider = divider;
}

int64_t TimeUtils::getEpochTimeMs() {
  std::chrono::milliseconds time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
  return mInitTimestamp +
         (time.count() - mStartEpochTime) * mMultiple / mDivider;
}

void TimeUtils::getTimeRatio(int *pMultiple, int *pDivider) {
  *pMultiple = mMultiple;
  *pDivider = mDivider;
}

void TimeUtils::sleepUtil(int64_t epochTimeMs) {
  int64_t time =
      mStartEpochTime + (epochTimeMs - mInitTimestamp) * mDivider / mMultiple;
  std::this_thread::sleep_until(
      std::chrono::system_clock::time_point(std::chrono::milliseconds(time)));
}

void TimeUtils::sleepFor(int64_t timeMs) {
  std::this_thread::sleep_for(
      std::chrono::milliseconds(timeMs * mMultiple / mDivider));
}

void TimeUtils::StringMstoTimMs(const std::string &result, int64_t &timeMs,
                                const char *format) {
  // result = "20091231-195124_275"
  // last "_XXX" is appended ms
  if (result.length() < 18) {
    timeMs = -1;
    return;
  }
  std::tm ttm;
  memset(&ttm, 0, sizeof(ttm));
  std::string yyyymmdd_hhmmss(result.substr(0, result.length() - 4));
  strptime(yyyymmdd_hhmmss.c_str(), format, &ttm);
  // timeMs = mktime(&ttm) * 1000 + stoi(result.substr(result.length() - 3, 3));
  timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::from_time_t(std::mktime(&ttm))
                   .time_since_epoch()).count() +
           stoi(result.substr(result.length() - 3, 3));
}

void TimeUtils::TimeMstoStringMs(int64_t timeMs, std::string &result,
                                 const char *format) {
  auto ttime_t = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::time_point(std::chrono::milliseconds(timeMs)));
  auto tp_sec = std::chrono::system_clock::from_time_t(ttime_t);
  int ms = (timeMs % 1000);

  std::tm *ttm = localtime(&ttime_t);
  // std::tm *ttm = gmtime(&ttime_t);
  ttm->tm_isdst = 0;
  char time_str[64] = {0};
  strftime(time_str, sizeof(time_str), format, ttm);
  sprintf(&time_str[strlen(time_str)], "_%03d", ms);
  result = time_str;
}

#ifdef DMS_TIME_DEBUG
void TimeCheck::setAnchTimePoint() {
  std::chrono::milliseconds time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
  mAnchorTimeB = mAnchorTimeA = time.count();
}

void TimeCheck::PrintAnchDuraTms(const std::string &tag,
                                 const std::string &message) {
  std::chrono::milliseconds time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
  mAnchorTimeC = time.count();
  LOGI_T(MODULE_TAG) << "cost: " << tag << " " << message << " ("
                     << mAnchorTimeC - mAnchorTimeB
                     << " total: " << mAnchorTimeC - mAnchorTimeA << ")";
  mAnchorTimeB = mAnchorTimeC;
}
#endif
