#include <Logger.hpp>

using namespace jlog;
int main() {

  Logger::getInstance(INFO) << "This logger seems to work." << std::endl;

}
