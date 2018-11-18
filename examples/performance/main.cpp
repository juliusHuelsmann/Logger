#include <Logger.hpp>
#include <outputHandler/NetIo.h>
#include <thread>
#include <unistd.h>
#include <executionHandler/Profiler.hpp>
#include <muParser.h>

using namespace slog;


//XXX: 1: create Profiler, which offers one function that can be implemented.
//

#define ETW wrapper::Wrapper<mu::Parser>
class ExecutionTimeWrapper : public wrapper::Profiler<mu::Parser> {
  public:

  ExecutionTimeWrapper(mu::Parser* t, std::string customPrefix) :
      Profiler<mu::Parser>(t, customPrefix) {


  }


  /**
   * Log the
   * @param duration        time in ms that was required for the last funciton
   *                        call.
   * @param functionCall    the name of the function that was just called.
   * @param customPrefix    custom prefix that be provided to the
   *                         #ExecutionTimeWrapper. Is provided in this case
   *                        to see if there are differneces in the execution
   *                        time with a different exponent.
   */
  virtual void handleDuration(long duration,
  std::string functionCall, std::string customPrefix) {
    (void) functionCall;
    TOPIC("execTime[" + customPrefix + "]", duration);
    //TOPIC("execTime." + customPrefix + "", duration);
    std::cout << customPrefix << " " << duration << "\n";
  }



  static ETW create(mu::Parser* t, std::string customPrefix,
                     bool handleMemory=true) {
    return ETW(t, new ExecutionTimeWrapper(t, customPrefix), handleMemory);
  }

};


void evaluateFunction(ETW &w, uint i) {

  double t = 0;
  w->DefineVar("t", &t);


  w->SetExpr("t^" + std::to_string(i*10) + " + sin(t) * 3.2");
  //for (auto i = 0; i < rand() % 255500; i++) {
    for (auto k = 0; i < 255500; i++) {

      auto j = k / 1000.;
      auto j2 = j + 1./2000.;

    TOPIC("nonsense", j, j2);
    TOPIC("nonsense[1]", j, j2);
    TOPIC("nonsense[2]", j, j2, j2, j);
    t = w->Eval();
    usleep(10000 * (rand() % 10));
  }


}
void info() {
  int i = 2;
  (void) i;
}
void * get_pc () { return __builtin_return_address(0); }
int main2 (){
  auto **array = (void**) malloc(8);
  array[0] = malloc(8);
  auto sz = 8;
  (void)sz;

  info();
  auto k = (char*)get_pc()-10;
  info();
  auto k2 = (char*)get_pc()-10;
  info();
  auto k3 = (char*)get_pc()-10;
  (void) k;
  (void) k2;
  (void) k3;
  memcpy(array, k, sz);
  memcpy(array+sz, k2, sz);
  memcpy(array+2*sz, k3, sz);
  auto bts = backtrace_symbols(array, 1);
  (void)bts;

  printf("%p\n",k);
  printf("%p\n", k2);
  printf("%p\n", k3);

    return 0;
}

int main(int argc, char const * const* const argv) {
  (void)argv;


  //
  //Check if zmq is installed. If not, exit.
#ifdef FOUND_ZMQ
  auto streamMethod = std::shared_ptr<outputHandler::OutputHandler>(
      new outputHandler::NetIo(true, true, 5555));
  LOG().setStreamMethod(streamMethod);
  LOG(INFO) << "network logger found!" << std::endl;
#else
  LOG(FATAL) << "Error, ZMQ is not installed, but required for this demo."
            << std::endl;
  return -1;
#endif


  //
  // Create three kinds of different functions and measure the time that
  // this operation takes in miliseconds.

  LOG(INFO) << "This will run indefinetely. Evaluates functions" << std::endl;
  LOG().enableTopic("execTime", nullptr, 100, "", "histogram");
  LOG().enableTopic("nonsense"); // < live logging (without buffering)
  LOG().enableTopic("nonsense[1]", nullptr, 0, "", "interval");

  // create multiple threads.
  switch (argc)  {
    case 2:
    case 1:
      break;
    default:
      LOG(FATAL) << "Error, I do not know what to do with >3 arguments\n";

  }



  auto amountThreads = 1;
  for (auto i = 1; i <= amountThreads; i++) {
    auto parser = new mu::Parser();
    auto etw = ExecutionTimeWrapper::create(parser, std::to_string(i));
    evaluateFunction(etw, i);
  }
  return 0;
}


