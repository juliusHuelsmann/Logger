#include <Logger.hpp>
#include <outputHandler/Netio.h>


using namespace jlog;
int main() {
  jlog::LOG().setLogLevel(jlog::INFO);
  jlog::LOG().setExitLevel(jlog::WARN);

#ifdef FOUND_ZMQ
  jlog::LOG().setStreamMethod(new jlog::outputHandler::Netio(true, true));
  jlog::LOG(jlog::INFO) << "network logger found!" << std::endl;
#else   
  jlog::LOG(jlog::INFO) << "ZMQ not installed, fall back to standard io." 
    << std::endl;
#endif 

  jlog::LOG() << "Logger test finished" << std::endl;
}
