#ifndef UTILS_TIMEUTILS_H__
#define UTILS_TIMEUTILS_H__

#include <ctime>

#if defined(__linux) and not defined(__ANDROID__)
#include <cstdint>
#endif
#include <string>

class TimeUtils {
 public:
  static int64_t getEpochTimeMs();
  static void sleepUtil(int64_t epochTimeMs);
  static void sleepFor(int64_t timeMs);
  static void reset();
  static void initTimeUtil(int64_t curEpochTime, int multiple, int divider);
  static void getTimeRatio(int *pMultiple, int *pDivider);
  static void TimeMstoStringMs(int64_t timeMs, std::string &result);
  static void StringMstoTimMs(const std::string &result, int64_t& timeMs);

 private:
  static int64_t mStartEpochTime;
  static int64_t mInitTimestamp;
  static int mMultiple;
  static int mDivider;
};

#ifdef DMS_TIME_DEBUG
class TimeCheck {
 public:
  void setAnchTimePoint(void);
  void PrintAnchDuraTms(const std::string &tag, const std::string &message);

 private:
  int64_t mAnchorTimeA = 0l;
  int64_t mAnchorTimeB = 0l;
  int64_t mAnchorTimeC = 0l;
};
#define TIME_CHECK_INIT(name) \
  TimeCheck _tc_##name;       \
  _tc_##name.setAnchTimePoint();
#define TIME_CHECK_PRINT(name, msg) _tc_##name.PrintAnchDuraTms(#name, msg);
#else
#define TIME_CHECK_INIT(name)
#define TIME_CHECK_PRINT(name, msg)
#endif

#endif  // UTILS_TIMEUTILS_H__
