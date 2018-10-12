#include <Logger.hpp>

using namespace log;
int main() {

  Logger::getInstance(INFO) << "This logger seems to work." << std::endl;

}
