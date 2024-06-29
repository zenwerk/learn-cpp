// from: https://zenn.dev/nishiki/articles/01ff1417f0b85f
#include <queue>
#include <mutex>
#include <functional>
#include <thread>
#include <future>

#include <chrono>
#include <iostream>

#include <random>
#include <iomanip>
#include <sstream>
#include <unordered_map>


using time_point = std::chrono::high_resolution_clock::time_point;


time_point now() {
  return std::chrono::high_resolution_clock::now();
}


int duration(time_point start, time_point end) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}


struct JobData {
  int name{};
  time_point start;
  time_point end;
};

using JobHandle = std::shared_future<void>;

class JobSystem {
public:
  explicit JobSystem(size_t thread_count) {
    for (size_t i = 0; i < thread_count; ++i) {
      workers.emplace_back([this] { this->workerThreadFunction(); });
    }
  }

  ~JobSystem() {
    // 全てのワーカースレッドに停止信号を送る
    stop_flag.store(true);

    // 全てのワーカースレッドを起こす
    cv.notify_all();

    // 全てのワーカースレッドが終了するのを待つ
    for (auto &worker: workers) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  // 依存タスクなしのジョブ投入
  JobHandle schedule(const std::function<void()> &job) {
    // packaged_task と shared_future を作成
    std::packaged_task<void()> task(job);
    auto future = task.get_future().share();
    {
      std::lock_guard<std::mutex> lock(mtx);
      job_counter.fetch_add(1); // インクリメント
      complete_flag.store(false); // 未完了にする
      // ジョブをキューにプッシュ
      q.push(std::move(task));
    }
    // 待機中のワーカースレッドをひとつ起こす
    cv.notify_one();
    return future;
  }

  // 依存タスクを持つジョブ投入
  JobHandle schedule(const std::function<void()> &job,
                     const std::vector<JobHandle> &dependencies) {
    auto wrapper = [job, dependencies]() {
      // 全ての依存ジョブが終わるまで待機する
      for (auto& dep : dependencies) {
        if (dep.valid()) {
          dep.wait();
        }
      }

      job();
    };
    return schedule(wrapper);
  }

  void waitForAll() const {
    // trueになるまで待機する
    complete_flag.wait(false);
  }

private:
  std::queue<std::packaged_task<void()>> q;
  mutable std::mutex mtx;
  std::vector<std::thread> workers;
  std::condition_variable cv;
  std::atomic<int> job_counter = 0;
  std::atomic<bool> complete_flag = false;
  std::atomic<bool> stop_flag = false;

  void workerThreadFunction() {
    // 無限ループで待機と実行を繰り返す
    while (true) {
      std::packaged_task<void()> job; // ジョブの入れ物を用意
      {
        // ジョブキューにジョブが追加されるまで待機する
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] {
          // 停止フラグが立った場合も起きる
          return !q.empty() || stop_flag;
        });

        // 停止フラグが立っていて、ジョブキューが空なら抜ける
        if (stop_flag && q.empty()) {
          break;
        }

        // ジョブキューからジョブを取得する
        job = std::move(q.front());
        q.pop();
      } // ロックを解放

      // ジョブを実行する
      job();

      // デクリメント
      if (job_counter.fetch_sub(1) == 1) {
        // カウントが0になったら完了フラグを立てて通知
        complete_flag.store(true);
        complete_flag.notify_all();
      }
    }
  }
};


void busyWait(std::chrono::nanoseconds duration) {
  auto start_time = std::chrono::high_resolution_clock::now();
  while (std::chrono::high_resolution_clock::now() - start_time < duration) {
    // CPU時間を消費するためのアクティブな待機
    // ループの中で何もしない
  }
}


int generateRandomInt(int minNum, int maxNum) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(minNum, maxNum);
  return distribution(gen);
}


// タイムラインバーを作成する関数
std::string createTimeline(const std::vector<JobData> &jobData, time_point globalStart, int globalDuration) {
  // タイムラインを空白で初期化
  std::string timeline;
  for (const auto &data: jobData) {
    timeline = std::string(globalDuration, ' ');
  }

  // ジョブ実行中の時刻にインデックスを挿入
  for (const auto &data: jobData) {
    auto begin = timeline.begin();
    int start = duration(globalStart, data.start);
    int end = duration(globalStart, data.end);
    std::fill(begin + start, begin + end, data.name);
  }

  return timeline;
}

std::mutex dataMutex;

void func(std::unordered_map<std::thread::id, std::vector<JobData>> &jobData, int name) {
  JobData data;
  data.name = name;
  data.start = now();

  int sleepTime = generateRandomInt(5, 10);
  busyWait(std::chrono::milliseconds(sleepTime));

  data.end = now();

  auto thisId = std::this_thread::get_id();
  std::lock_guard<std::mutex> lock(dataMutex);
  if (!jobData.contains(thisId)) {
    jobData[thisId] = {};
  }
  jobData[thisId].push_back(data);
}

int main() {
  // ジョブシステムの作成
  unsigned int threadCount = 4;
  JobSystem jobSystem(threadCount);

  // ジョブデータの格納先
  std::unordered_map<std::thread::id, std::vector<JobData> > jobData;

  // 開始時間を記録
  auto globalStart = now();
  // ジョブAをスケジュールする
  std::vector<JobHandle> jobAHandles;
  for (int i = 0; i < 4; ++i) {
    auto handle = jobSystem.schedule([&jobData]() {
      func(jobData, 'A');
    });

    // ハンドルを保持しておく
    jobAHandles.push_back(handle);

    busyWait(std::chrono::milliseconds(2));
  }

  // ジョブBをスケジュールする
  for (int i = 0; i < 4; ++i) {
    // ジョブAを待機するためハンドルを渡す
    jobSystem.schedule([&jobData]() {
      func(jobData, 'B');
    }, jobAHandles);

    busyWait(std::chrono::milliseconds(2));
  }

  // 全てのジョブが完了するのを待つ
  jobSystem.waitForAll();

  // 終了時間を記録
  auto globalEnd = now();
  int globalDuration = duration(globalStart, globalEnd);
  std::cout << "Total duration: " << globalDuration << " ms" << std::endl;

  // タイムラインを表示
  for (const auto &[id, data]: jobData) {
    // スレッドIDを空白埋めして5桁にする
    std::ostringstream threadIdStream;
    threadIdStream << std::setw(5) << std::setfill(' ') << id;
    std::string threadIdStr = threadIdStream.str();

    // タイムラインを表示
    std::string timeline = createTimeline(data, globalStart, globalDuration);
    std::cout << "Thread " << threadIdStr << " |" << timeline << "| (" << data.size() << " jobs)" << std::endl;
  }
}
