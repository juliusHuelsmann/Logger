
#include <iostream>

#include <outputHandler/Stdio.h>

void log::outputHandler::Stdio::handle(
    std::string st, log::LogLevel msgLogLevel) {
  if (msgLogLevel >= log::WARN)  std::cerr << st;
  else std::cout << st;
}

log::outputHandler::Stdio::Stdio(){}
log::outputHandler::Stdio::~Stdio(){}
