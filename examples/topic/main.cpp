
#include <slog/Logger.hpp>
#include <slog/outputHandler/NetIo.h>

#include <chrono>
#include <memory>
#include <thread>

uint32_t fac (uint32_t a) {
    return a==0?1:fac(a-1)*a;
} 

constexpr uint32_t fac2 (uint32_t a) {
    return a==0?1:fac2(a-1)*a;
} 

/// Most likely inlighned either way.
inline constexpr uint32_t fac3 (uint32_t a) {
    return a==0?1:fac3(a-1)*a;
} 




int main() {





  LOG().setStreamMethod(std::make_shared< slog::outputHandler::NetIo>(true, false, 5555));
  LOG(LogLevel::INFO) << "The topic logger is going to be launched shortly\n";
  LOG().enableTopic("time", nullptr, 50, "", HISTOGRAM);
  LOG().enableTopic("time.execution.diff", nullptr, 0, "", LINE_PLOT, 200);

  for (auto i = 0; i < 100000; ++i) {


   

    auto t0 = std::chrono::steady_clock::now();
    const auto hms = 20000000;
    std::this_thread::sleep_for(std::chrono::nanoseconds(hms));
    auto tE = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(tE-t0).count();

    TOPIC("time.scheduling.diff", duration-hms);


    t0 = std::chrono::steady_clock::now();
    fac(i*100);
    tE = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(tE-t0).count();


    t0 = std::chrono::steady_clock::now();
    fac2(i*100);
    tE = std::chrono::steady_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(tE-t0).count();


    t0 = std::chrono::steady_clock::now();
    fac3(i*100);
    tE = std::chrono::steady_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(tE-t0).count();

    TOPIC("time.execution.diff", i, duration, duration2, duration3);

    TOPIC("time.execution.diffHist", duration, duration2, duration3);
    //TOPIC("time.execution.diffHist", 10, 20, 30, 50, 60 );
    /*TOPIC("time.execution.diffHist[normal]", duration, bins);
    TOPIC("time.execution.diffHist[constexpr]", duration2, bins);
    TOPIC("time.execution.diffHist[inline]", duration3, bins);*/


  }


}
