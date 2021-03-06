#include <Logger.hpp>
#include <outputHandler/NetIo.h>
#include <memory>

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
  auto sm = std::shared_ptr<outputHandler::OutputHandler>
    (new outputHandler::NetIo(true, true, 8001));
  LOG().setStreamMethod(sm);
  LOG(LogLevel::INFO) << "network logger found!" << std::endl;
#else
  LOG(LogLevel::INFO) << "ZMQ not installed, fall back to standard io@" << "."
            << std::endl;
#endif
  LOG(LogLevel::INFO) << "neu" << std::endl;


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
  LOG().enableTopic("time[0]" , nullptr, sizeof(int)*4);
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

  LOG().enableTopic("time[0]", nullptr, 8*9); // write out the first 4 * 8 values ot cli
  LOG() << "logged once again" << std::endl;
  TOPIC("time[0]", 2, 2);  //< buffer
  TOPIC("time[0]", 2, 2);  //< buffer
  LOG() << "not logged" << std::endl;



  LOG() << "Logger test finished. Now, the clean up routine will log the \n"
        << "last chunk of data." << std::endl;






  // test lots of topics
  LOG().enableTopic("time" , nullptr, sizeof(int)*4);
  LOG().enableTopic("money" , nullptr, sizeof(int)*4);
  LOG().enableTopic("money.lending" , nullptr, sizeof(int)*4);
  LOG().enableTopic("time[0].ente.haus.not.explicitely.enabled" , nullptr, sizeof(int)*4);
  LOG().enableTopic("time.ente.." , nullptr, sizeof(int)*4);


}



void execute(uint id) {

  std::cout << std::numeric_limits<ushort>::max() << "\n";

  // set default levels.
  LOG().setLogLevel(LogLevel::INFO);
  LOG().setExitLevel(LogLevel::WARNING);

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
  LOG().enableTopic("time", nullptr, 8*5);
  for (auto i = 0 ; i < 50; i++) {
    LOG().enableTopic("time[" + std::to_string(i%4)  +"]" ,
                        nullptr, sizeof(int)*2 + i%50);
    TOPIC("someDisabledStuff.enabled?.not[0]", 2, 3);
    for (auto _ = 0; _ < rand() % 10; _++)
      TOPIC("time[" + std::to_string(rand() % 5) + ")]", 2, 2);
    usleep(1);

  }
  LOG().enableTopic("time", nullptr, 8*5);

  TOPIC("time[0]", 2, 2); //< logged, buffer size = 8 -> use the buffer
  TOPIC("trash", 2, 2);   //< not logged
  TOPIC("time[0]", 2, 2); //< the buffer is cleared  here, and output is logged for the first time.

  TOPIC("time[0]", 2, 2); //< Fill the first 4 * 8 values
  LOG().enableTopic("time[0]", nullptr, 8*9); // write out the first 4 * 8 values ot cli
  TOPIC("time[0]", 2, 2);  //< buffer
  TOPIC("time[0]", 2, 2);  //< buffer
  //< print out and clear buffer.
  for (auto i = 0; i < 45; i++) TOPIC("time[0]", 2, 2);
  LOG() << "A thread " << "finished" << std::endl;

}



