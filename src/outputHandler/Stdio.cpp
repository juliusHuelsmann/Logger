
#include <outputHandler/Stdio.h>

#include <iostream>
#include <iterator>
#include <algorithm>

void slog::outputHandler::Stdio::handle(
    const char* st, slog::LogLevel msgLogLevel, size_t length) {
   auto stream = msgLogLevel >= slog::WARN ? &std::cerr : &std::cout;
   std::copy(st, st + length, std::ostream_iterator<unsigned char>(*stream));
   *stream << "\n";
}

slog::outputHandler::Stdio::Stdio(){}
slog::outputHandler::Stdio::~Stdio(){}
