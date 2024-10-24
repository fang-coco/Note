#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

void fun1(std::string str, int id) {
  for (int i = 0; i < 10; ++i) {
    if (1 == i) {
      auto now = std::chrono::system_clock::now();
      auto t2 = std::chrono::seconds(3);
      std::this_thread::sleep_until(now + t2); // 到某个时间点唤醒

    } else
      std::this_thread::sleep_for(std::chrono::seconds(2)); // 睡眠某个时间段
    std::cout << "this is " << id << " " << i << "th print " << str
              << std::endl;
  }
}

void test1() {
  std::thread th1(fun1, "i love you", 520);
  std::thread th2(fun1, "i hate you", 4848);

  std::cout << "main thread id: " << std::this_thread::get_id() << std::endl;
  std::cout << "son1 thread id: " << th1.get_id() << std::endl;
  std::cout << "son2 thread id: " << th2.get_id() << std::endl;

  th1.join();
  th2.join();
  std::cout << std::thread::hardware_concurrency() << std::endl;
}

// 互斥锁
int g_buffer = 0;
std::mutex g_buffer_mutex;

void fun2(std::string name) {
  for (int i = 0; i < 3; ++i) {
    auto num = std::rand() % 100;

    std::lock_guard<std::mutex> lock(g_buffer_mutex);
    // g_buffer_mutex.lock();
    g_buffer += num;
    std::cout << name << " add " << num << " to " << g_buffer << std::endl;
    // g_buffer_mutex.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

void test2() {

  std::thread th1(fun2, "Bob");
  std::thread th2(fun2, "Alice");
  th1.join();
  th2.join();
}
// 超时互斥锁
std::timed_mutex g_mutex;
void work() {

  std::chrono::seconds timeout(1);

  while (true) {
    if (g_mutex.try_lock_for(timeout)) {

      std::cout << "Thread " << std::this_thread::get_id() << " get timed_mutex"
                << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(10));
      g_mutex.unlock();
      break;
    } else {
      std::cout << "Thread " << std::this_thread::get_id()
                << " not get timed_mutex" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

void test3() {
  std::thread th1(work);
  std::thread th2(work);
  th1.join();
  th2.join();
  std::cout << "Main thread over" << std::endl;
}

int main() {

  test3();
  return 0;
}
