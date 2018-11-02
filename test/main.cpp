#include <Logger.hpp>

using namespace log;
int main() {

  Logger::getInstance(INFO) << "This logger seems to work." << std::endl;

  /*jlog::LOG().setStreamMethod(new jlog::outputHandler::NetworkLogger(true, true));
  jlog::LOG().setLogLevel(jlog::INFO);
  jlog::LOG().setExitLevel(jlog::WARN);
  jlog::LOG() << "Sense launched!" << std::endl;
*/
}
