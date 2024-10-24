#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <ratio>

/* 时间间隔duration
 * chrono::duration<type, ratio<X, Y>>
 * type: 时间的长度的类型
 * ratio<X, Y>: 时间的刻度(X/Y 秒)
 *
 * 时间间隔 = 时间长度 * 时间刻度
 */
void duration() {
  // 构造函数
  std::chrono::duration<int, std::ratio<1000, 1>> ks(10); // 10 * (1000 / 1) 秒
  std::cout << "ks duration has " << ks.count() << " ticks" << std::endl;

  std::chrono::milliseconds ms{3}; // 3 毫秒
  std::cout << "ms has duration has " << ms.count() << " ticks" << std::endl;

  std::chrono::microseconds us = 2 * ms; // 2 * 3ms = 6000 us
  std::cout << "us has duration has " << us.count() << " ticks" << std::endl;

  std::chrono::nanoseconds ns = 3 * us; // 纳秒
  std::cout << "ns has duration has " << ns.count() << " ticks" << std::endl;

  // 运算
  std::chrono::minutes t1{2};
  std::chrono::seconds t2{3};
  std::chrono::seconds t3 = t1 - t2;
  std::cout << "t3 has duration has " << t3.count() << " ticks" << std::endl;

  // auto t4 = t2 * t3; // error
  // std::cout << "t4 has duration has " << t4.count() << " ticks" << std::endl;

  // 乘法
  t3 *= 3;
  std::cout << "t3 has duration has " << t3.count() << " ticks" << std::endl;

  // 不同时间刻度之间的运算
  std::chrono::duration<double, std::ratio<9, 7>> d1(3);
  std::chrono::duration<double, std::ratio<6, 5>> d2(1);
  // d1 和 d2 统一之后的时钟周期
  std::chrono::duration<double, std::ratio<3, 35>> d3 = d1 - d2;

  std::cout << "d3 has duration has " << d3.count() << " ticks" << std::endl;
}

void time_point() {

  // system_clock
  typedef std::chrono::system_clock::time_point stp;
  typedef std::chrono::duration<int, std::ratio<60 * 60 * 24>> dday;
  stp epoch; // 新纪元时间点

  dday day(1); // 一天的时间间隔

  // 新纪元的时间 + 1 天
  stp t1(day);
  time_t tm = std::chrono::system_clock::to_time_t(t1);
  std::cout << "tm time: " << ctime(&tm) << std::endl;

  // 现在的时间
  stp today = std::chrono::system_clock::now();
  time_t tm1 = std::chrono::system_clock::to_time_t(today);
  std::cout << "tm1 time: " << ctime(&tm1) << std::endl;

  // 明天的时间
  //
  stp t2 = stp(today + day);
  time_t tm2 = std::chrono::system_clock::to_time_t(t2);
  std::cout << "tm2 time: " << ctime(&tm2) << std::endl;

  // steady_clock

  std::chrono::steady_clock::time_point tp1 = std::chrono::steady_clock::now();

  std::cout << "print 1000 * ..." << std::endl;
  for (int i = 0; i < 1000; ++i)
    std::cout << "*";
  std::cout << std::endl;
  std::chrono::steady_clock::time_point tp2 = std::chrono::steady_clock::now();

  auto diff = tp2 - tp1;
  std::cout << "diff duration has " << diff.count() << "ns" << std::endl;

  auto diff2 = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "diff2 duration has " << diff2.count() << "ms" << std::endl;

  std::chrono::duration<double, std::ratio<1, 1000>> diff3 = (tp2 - tp1);
  std::cout << "diff3 duration has " << diff3.count() << "ms" << std::endl;
}

int main() {
  // duration();
  time_point();
  return 0;
}
