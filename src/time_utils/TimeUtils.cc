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

void TimeUtils::StringMstoTimMs(const std::string &result, int64_t &timeMs) {
  // result = "20091231-195124_275"
  if (result.length() != 19) {
    timeMs = -1;
    return;
  }
  std::tm tm_;
  tm_.tm_sec = stoi(result.substr(13, 2));
  tm_.tm_min = stoi(result.substr(11, 2));
  tm_.tm_hour = stoi(result.substr(9, 2));
  tm_.tm_mday = stoi(result.substr(6, 2));
  tm_.tm_mon = stoi(result.substr(4, 2)) - 1;
  tm_.tm_year = stoi(result.substr(0, 4)) - 1900;
  timeMs = mktime(&tm_);
  // auto now2 = std::chrono::system_clock::from_time_t(std::mktime(&tm_));
}

void TimeUtils::TimeMstoStringMs(int64_t timeMs, std::string &result) {
  auto ttime_t = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::time_point(std::chrono::milliseconds(timeMs)));
  auto tp_sec = std::chrono::system_clock::from_time_t(ttime_t);
  int ms = (timeMs % 1000);

  std::tm *ttm = localtime(&ttime_t);
  char date_time_format[] = "%Y%m%d-%H%M%S";
  char time_str[] = "yyyy.mm.dd.HH-MM.SS.fff";
  strftime(time_str, strlen(time_str), date_time_format, ttm);
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
