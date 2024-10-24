#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;
class ThreadPool {
public:
  ThreadPool(int min_thread,
             int max_thread = std::thread::hardware_concurrency());
  ~ThreadPool();
  void AddTask(std::function<void()> task);

  template <typename F, typename... Args>
  auto addTask(F &&f, Args &&...args)
      -> future<typename result_of<F(Args...)>::type> {
    using returnType = typename result_of<F(Args...)>::type;
    auto task = make_shared<packaged_task<returnType()>>(
        bind(forward<F>(f), forward<Args>(args)...));
    future<returnType> res = task->get_future();
    {
      unique_lock<mutex> lock(m_quemutex);
      m_tasks.emplace([task]() { (*task)(); });
    }
    m_condition.notify_one();
    return res;
  }

private:
  void manager();
  void worker();

  std::queue<std::function<void()>> m_tasks;
  std::thread *m_manager;
  std::map<std::thread::id, std::thread> m_workers;
  std::vector<std::thread::id> m_exitthreads;

  int m_minthread;
  int m_maxthread;
  std::atomic_int m_curthread;
  std::atomic_int m_idlethread;
  std::atomic_int m_exitthead;
  std::atomic_bool m_stop;

  std::mutex m_quemutex;
  std::mutex m_vecmutex;

  std::condition_variable m_condition;
};

ThreadPool::ThreadPool(int min_thread, int max_thread)
    : m_minthread(min_thread), m_maxthread(max_thread), m_stop(false) {

  this->m_idlethread = this->m_curthread = m_minthread;
  this->m_manager = new std::thread(std::bind(&ThreadPool::manager, this));
  for (int i = 0; i < this->m_curthread; i++) {
    std::thread th(std::bind(&ThreadPool::worker, this));
    this->m_workers.insert(std::make_pair(th.get_id(), std::move(th)));
  }
}

ThreadPool::~ThreadPool() {
  m_stop = true;
  m_condition.notify_all();
  for (auto &it : m_workers) {
    auto &th = it.second;
    if (th.joinable()) {
      std::cout << "worker: " << th.get_id() << " will exit" << std::endl;
      th.join();
    }
  }
  if (m_manager->joinable()) {
    std::cout << "manager: " << m_manager->get_id() << " will exit"
              << std::endl;
    m_manager->join();
  }

  delete m_manager;
}
void ThreadPool::AddTask(std::function<void()> task) {

  {
    std::unique_lock<std::mutex> locker(m_quemutex);
    m_tasks.emplace(std::move(task));
    std::cout << "add a task to queue" << std::endl;
  }
  m_condition.notify_one();
}

// 管理线程执行的任务: 动态调整线程池中的线程数
void ThreadPool::manager() {

  while (!m_stop.load()) {

    // 每隔三秒管理一次线程池
    std::this_thread::sleep_for(std::chrono::seconds(3));

    auto a = m_idlethread.load(); // 空闲线程数
    auto b = m_curthread.load();  // 所有线程数

    // 如果空闲数大于整体一半, 且整体还少于最低限额
    if (a * 2 > b && b > m_minthread) {
      // 设置要杀死俩个线程
      m_exitthead.store(2);
      m_condition.notify_all();
      // 互斥访问要清理的线程集合
      std::lock_guard<std::mutex> locker(m_vecmutex);
      for (auto &id : m_exitthreads) {
        auto work = m_workers.find(id);
        if (work != m_workers.end()) {
          work->second.join(); // 等待线程的最后一个任务完成
          std::cout << "worker: " << id << " exited" << std::endl;
          m_workers.erase(work); // 从工作线程队列中删除该线程
        }
      }
      m_exitthreads.clear();
    } else if (a * 2 < b && b < m_maxthread) { // 添加新的线程
      std::thread th(std::bind(&ThreadPool::worker, this));
      m_workers.insert(std::make_pair(th.get_id(), std::move(th)));
      std::cout << "add worker: " << th.get_id() << std::endl;
      m_idlethread++;
      m_curthread++;
    }
  }
}
void ThreadPool::worker() {

  while (!m_stop.load()) {
    std::function<void()> task; // 想要执行的任务
    {
      std::unique_lock<std::mutex> locker(m_quemutex); // 为任务队列添加互斥锁
      // 等待条件锁成立：线程池stop/任务队列不为空
      m_condition.wait(
          locker, [this]() { return this->m_stop || !this->m_tasks.empty(); });

      // 如果管理线程发出要结束线程的数量还么有满足，就结束当前线程
      if (!this->m_exitthead.load()) {
        this->m_exitthead--;
        this->m_curthread--;
        // 将当前线程的id添加到结束集合中，等待管理线程清理
        std::lock_guard<std::mutex> veclocker(m_vecmutex);
        std::cout << "worker: " << std::this_thread::get_id() << " will exit"
                  << std::endl;
        m_exitthreads.emplace_back(std::this_thread::get_id());
        return;
      }

      // 获取任务
      if (!m_tasks.empty()) {
        std::cout << "worker: " << std::this_thread::get_id() << " get task"
                  << std::endl;
        task = std::move(m_tasks.front());
        m_tasks.pop();
      }
    }
    // 执行任务
    if (task) {
      // 空闲线程--
      m_idlethread--;
      task();
      m_idlethread++;
    }
  }
}

std::atomic_int g_count = 0;
void calc(int x, int y) {
  std::cout << x << " + " << y << " = " << x + y << std::endl;
  g_count += x + y;
}

int main() {

  ThreadPool tp(4);

  for (int i = 0; i < 10; ++i) {
    tp.AddTask(std::move(std::bind(calc, i, rand() % 10)));
  }

  std::getchar();
  std::cout << "g_count = " << g_count.load() << std::endl;
  return 0;
}
