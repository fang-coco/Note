#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
std::mutex g_mutex;
std::condition_variable pCon;
std::condition_variable sCon;
std::queue<int> que;
const unsigned int maxCount = 2;

void put(const int &x) {
  std::unique_lock<std::mutex> locker(g_mutex);

  // 如果满了阻塞
  pCon.wait(locker, [&]() {
    if (que.size() == maxCount) {
      std::cout << "Process thread blocked: " << std::this_thread::get_id()
                << std::endl;
      return false;
    }
    return true;
  });
  // 休眠一秒钟4， 模拟生产过程
  std::this_thread::sleep_for(std::chrono::seconds(4));
  que.push(x);
  std::cout << x << " processed" << std::endl;
  // 通知消费
  sCon.notify_one();
}

void get() {
  std::unique_lock<std::mutex> locker(g_mutex);
  sCon.wait(locker, [&]() {
    if (que.size() == 0) {
      std::cout << "Consume thread blocked: " << std::this_thread::get_id()
                << std::endl;
      return false;
    }
    return true;
  });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // 获取值
  auto x = que.front();
  que.pop();
  std::cout << x << " consumed" << std::endl;
  // 通知生产
  pCon.notify_one();
}

int main() {

  std::thread pth[30];
  std::thread sth[30];
  for (int i = 0; i < 30; i++) {
    pth[i] = std::thread(put, rand() % 10);
    sth[i] = std::thread(get);
  }
  for (int i = 0; i < 30; ++i) {
    pth[i].join();
    sth[i].join();
  }
  std::cout << "Main thread over ..." << std::endl;
  return 0;
}
