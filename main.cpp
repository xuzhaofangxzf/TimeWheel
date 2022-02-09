#include <iostream>
#include <string>
#include <thread>
#include <sys/time.h>

#include "time_wheel_scheduler.h"
#include "timecost.h"

// std::string timetoStr(){
//   char tmp[64] = {0};
//   time_t t = time(NULL);
//   tm* tm = localtime(&t);
//   sprintf(tmp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
//   return tmp;
// }

std::string timetoStr(){
  TimerCost tc;
  char tmp[64] = {0};
  struct timeval now;
  struct tm local_tm;
  gettimeofday(&now, NULL);
  time_t t = now.tv_sec;
  localtime_r(&t, &local_tm);
  sprintf(tmp, "%02d:%02d:%02d:%-3d", local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec, now.tv_usec / 1000);
  std::cout << "cost: " << tc.elapsed_micro() << " micro" << std::endl;
  return tmp;
}

int print_hello(int i, int j) {
  std::cout << "hello " << i << " hello " << j << " " << timetoStr() << std::endl;
}
int main() {
  // Four level time wheels: Hour, Minute, Secon, Millisecond.
  int timer_step_ms = 10;
  TimeWheelScheduler tws(timer_step_ms);
  // Hour time wheel. 24 scales, 1 scale = 60 * 60 * 1000 ms.
  tws.AppendTimeWheel(24, 60 * 60 * 1000, "HourTimeWheel");
  // Minute time wheel. 60 scales, 1 scale = 60 * 1000 ms.
  tws.AppendTimeWheel(60, 60 * 1000, "MinuteTimeWheel");
  // Second time wheel. 60 scales, 1 scale = 1000 ms.
  tws.AppendTimeWheel(60, 1000, "SecondTimeWheel");
  // Millisecond time wheel. 1000/timer_step_ms scales, 1 scale = timer_step ms.
  tws.AppendTimeWheel(1000 / timer_step_ms, timer_step_ms, "MillisecondTimeWheel");

  tws.Start();
#if 0
  tws.CreateTimerAt(GetNowTimestamp() + 10000, []() {
    std::cout << "At now+10s" << std::endl;
  });

  tws.CreateTimerAfter(500, []() {
    std::cout << "After 0.5s" << std::endl;
  });

  std::cout << timetoStr() << std::endl;
  auto timer_id = tws.CreateTimerEvery(5000, []() {
    std::cout << "Every 5s: " << timetoStr() << std::endl;
  });

  tws.CreateTimerEvery(30 * 1000, []() {
    std::cout << "Every 30s: " << timetoStr() <<std::endl;
  });

  tws.CreateTimerEvery(60 * 1000, []() {
    std::cout << "Every 1m: " << timetoStr() << std::endl;
  });

  tws.CreateTimerEvery(90 * 1000, []() {
    std::cout << "Every 1.5m: " << timetoStr() << std::endl;
  });

  tws.CreateTimerEvery(120 * 1000, []() {
    std::cout << "Every 2m: " << timetoStr() << std::endl;
  });
#endif

  auto handler = std::bind(print_hello, 1, 2);
  auto timer_id = tws.CreateTimerEvery(1000, handler);

  std::this_thread::sleep_for(std::chrono::seconds(30));
  tws.CancelTimer(timer_id);

  std::this_thread::sleep_for(std::chrono::minutes(20));
  tws.Stop();

  return 0;
}