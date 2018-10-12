
#include <iostream>

#include <outputHandler/OutputHandler.h>

void log::outputHandler::OutputHandler::handle(
    std::string st, log::LogLevel msgLogLevel) {
  if (msgLogLevel >= log::WARN)  std::cerr << st;
  else std::cout << st;
}
log::outputHandler::OutputHandler::~OutputHandler(){}
