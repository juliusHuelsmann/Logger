
#include <outputHandler/OutputHandler.h>


void slog::outputHandler::OutputHandler::handle(const char* st, 
    LogLevel msgLogLevel) {
  handle(st,msgLogLevel, strlen(st));
}

void slog::outputHandler::OutputHandler::handle(const char* sts,
    LogLevel msgLogLevel, size_t len) {
  auto d = std::vector<std::pair<const char*,size_t>>();
  d.push_back(std::make_pair(sts, len));
  handle(d, msgLogLevel);
}
