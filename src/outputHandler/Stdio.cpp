
#include <outputHandler/Stdio.h>

#include <iostream>

void jlog::outputHandler::Stdio::handle(
    std::string st, jlog::LogLevel msgLogLevel) {
  if (msgLogLevel >= jlog::WARN)  std::cerr << st;
  else std::cout << st;
  std::cout << std::flush;
  std::cerr << std::flush;
}

jlog::outputHandler::Stdio::Stdio(){}
jlog::outputHandler::Stdio::~Stdio(){}
