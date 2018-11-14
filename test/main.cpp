#include <Logger.hpp>
#include <outputHandler/NetIo.h>

using namespace slog;


/**
 * Multi-threaded execution
 * @param id  simplified thread id.
 */
void execute(uint id);


int main() {


  //
  // Print information on whether zmq is found. If yes, initialize the network
  // logger.
#ifdef FOUND_ZMQ
  LOG().setStreamMethod(new outputHandler::NetIo(true, true, 8001));
  LOG(INFO) << "network logger found!" << std::endl;
#else
  LOG(INFO) << "ZMQ not installed, fall back to standard io@" << "."
            << std::endl;
#endif
  LOG(INFO) << "neu" << std::endl;


  //
  // Threaded execution of logger and topic.
  volatile bool testThreads = false;
  if (testThreads) {
    const auto amount = 100;
    std::thread threads[amount];
    for (int i=0; i<amount; ++i) threads[i] = std::thread(execute,i+1);
    for (auto& th : threads) th.join();
  }

  LOG() << "-------------------------------------------------------------------"
        << "\nMulti- thread topic + logging example finished\n"
        << "-------------------------------------------------------------------"
        << std::endl;



  //
  // Topic-testing
  Logger::enableTopic("time[0]" , nullptr, sizeof(int)*4);
  LOG() << "topic enabled (and possibly emitted remaining values from buffer)"
        << std::endl;

  TOPIC("time[0]", 2, 2); //< tracked, buffer size = 8 -> use the buffer
  LOG() << "nothing logged 'till here" << std::endl;

  TOPIC("trash", 2, 2);   //< not tracked logged
  LOG() << "nothing logged 'till here" << std::endl;

  TOPIC("time[0]", 2, 2); //< the buffer is cleared  here, and output is logged for the first time.
  LOG() << "logged. " << std::endl;

  TOPIC("time[0]", 2, 2); //< Fill the first 4 * 8 values
  LOG() << "no log" << std::endl;

  Logger::enableTopic("time[0]", nullptr, 8*9); // write out the first 4 * 8 values ot cli
  LOG() << "logged once again" << std::endl;
  TOPIC("time[0]", 2, 2);  //< buffer
  TOPIC("time[0]", 2, 2);  //< buffer
  LOG() << "not logged" << std::endl;



  LOG() << "Logger test finished" << std::endl;
}



void execute(uint id) {

  // set default levels.
  LOG().setLogLevel(INFO);
  LOG().setExitLevel(WARN);

  // show that the log operation is not interrupted, even if the operator '<<'
  // is used and the input is not explicitly terminated.
  for (auto i = 1; i < 10;++i) {
    LOG() << id << "Here " << "i " << "do " "not "
          << " use the logger properly.\n"
          << "." << "." << "." << "." << "." << "." << "." << "." << "."
          << "." << "." << "." << "." << "." << "." << "." << "." << "."
          << "." << "." << "." << "." << "." << "." << "." << "." << "."
          << "I forgot std::endl in the end!"
          << " That is managed by the Logger!";
    usleep(1);
  }

  // test topics: initialize with different settings and check that the topics
  // and settings remain consistent via assertions inside the logger code.
  Logger::enableTopic("time", nullptr, 8*5);
  for (auto i = 0 ; i < 50; i++) {
    Logger::enableTopic("time[" + std::to_string(i%4)  +"]" ,
                        nullptr, sizeof(int)*2 + i%50);
    TOPIC("someDisabledStuff.enabled?.not[0]", 2, 3);
    for (auto _ = 0; _ < rand() % 10; _++)
      TOPIC("time[" + std::to_string(rand() % 5) + ")]", 2, 2);
    usleep(1);

  }
  Logger::enableTopic("time", nullptr, 8*5);

  TOPIC("time[0]", 2, 2); //< logged, buffer size = 8 -> use the buffer
  TOPIC("trash", 2, 2);   //< not logged
  TOPIC("time[0]", 2, 2); //< the buffer is cleared  here, and output is logged for the first time.

  TOPIC("time[0]", 2, 2); //< Fill the first 4 * 8 values
  Logger::enableTopic("time[0]", nullptr, 8*9); // write out the first 4 * 8 values ot cli
  TOPIC("time[0]", 2, 2);  //< buffer
  TOPIC("time[0]", 2, 2);  //< buffer
  //< print out and clear buffer.
  for (auto i = 0; i < 45; i++) TOPIC("time[0]", 2, 2);
  LOG() << "A thread " << "finished" << std::endl;

}



