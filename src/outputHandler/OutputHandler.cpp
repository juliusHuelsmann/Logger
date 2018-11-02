
#include <iostream>

#include <outputHandler/OutputHandler.h>

void jlog::outputHandler::OutputHandler::handle(
    std::string st, jlog::LogLevel msgLogLevel) {
  if (msgLogLevel >= jlog::WARN)  std::cerr << st;
  else std::cout << st;
}
jlog::outputHandler::OutputHandler::~OutputHandler(){}
