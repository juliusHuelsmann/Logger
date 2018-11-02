
#include <iostream>

#include <outputHandler/Stdio.h>

void jlog::outputHandler::Stdio::handle(
    std::string st, jlog::LogLevel msgLogLevel) {
  if (msgLogLevel >= jlog::WARN)  std::cerr << st;
  else std::cout << st;
}

jlog::outputHandler::Stdio::Stdio(){}
jlog::outputHandler::Stdio::~Stdio(){}
