#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <iostream>

struct Job {
  int id;
  std::chrono::steady_clock::time_point start_time;
  std::chrono::steady_clock::duration duration;
};

// Comparator for Job priority queue
struct JobCompare {
  bool operator()(const Job &lhs, const Job &rhs) const {
    return lhs.start_time > rhs.start_time;
  }
};

class RealtimeScheduler {
public:
  RealtimeScheduler() : stop_flag_(false) {}

  void start() {
    stop_flag_ = false;
    worker_thread_ = std::thread(&RealtimeScheduler::worker_loop, this);
  }

  void stop() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      stop_flag_ = true;
    }
    std::cout << "ここまできた2" << std::endl;
    condition_.notify_all();
    std::cout << "ここまできた3" << std::endl;
    worker_thread_.join();
  }

  void
  schedule_job(Job job) {
    std::unique_lock<std::mutex> lock(mutex_);
    job_queue_.push(job);
    condition_.notify_one();
  }

private:
  void worker_loop() {
    while (!stop_flag_) {
      std::unique_lock<std::mutex> lock(mutex_);
      // Wait until a job is available or stop flag is set
      // TODO: 相変わらず終了処理がおかしく、止まらないので治す
      while (job_queue_.empty() && !stop_flag_.load()) {
        condition_.wait(lock);
      }

      // Check if there's a job that should be executed
      if (!job_queue_.empty() && job_queue_.top().start_time <= std::chrono::steady_clock::now()) {
        Job job = job_queue_.top();
        job_queue_.pop();

        lock.unlock();  // Unlock while executing the job
        std::this_thread::sleep_for(job.duration);  // Simulate job execution time
        lock.lock();  // Lock again before modifying state

        // Do something with completed job
        std::cout << "Job ID: " << job.id << " done." << std::endl;

      } else {
        // No job to execute yet, wait until the next job is ready or stop flag is set
        if (!job_queue_.empty()) {
          condition_.wait_until(lock, job_queue_.top().start_time);
        } else {
          condition_.wait(lock);
        }
      }
    }
  }

  std::priority_queue<Job, std::vector<Job>, JobCompare> job_queue_;
  std::mutex mutex_;
  std::condition_variable condition_;
  std::thread worker_thread_;
  std::atomic<bool> stop_flag_;
};

int main() {
  RealtimeScheduler rs;

  rs.start();

  std::chrono::duration<int, std::milli> milliseconds{1000};
  Job job1{1, std::chrono::steady_clock::now(), milliseconds};

  rs.schedule_job(job1);

  std::this_thread::sleep_for(std::chrono::milliseconds{1000});

  Job job2{2, std::chrono::steady_clock::now(), milliseconds};

  rs.schedule_job(job2);

  std::this_thread::sleep_for(std::chrono::milliseconds{1000});

  rs.stop();

  return 0;
}