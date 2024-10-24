#include <atomic>
#include <iostream>
#include <thread>
std::atomic_int count = 0;
// int count = 0;

void inc() {
  for (int i = 0; i < 10000; i++) {
    ++count;
  }
}

void dec() {
  for (int i = 0; i < 10000; i++) {
    --count;
  }
}

int main() {
  std::thread th1(inc);
  std::thread th2(dec);

  th1.join();
  th2.join();
  std::cout << "count: " << count << std::endl;
  return 0;
}
