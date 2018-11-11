
#include <outputHandler/Stdio.h>

#include <iostream>
#include <iterator>
#include <algorithm>

void slog::outputHandler::Stdio::handle(
    std::vector<std::pair<const char*, size_t>> sts, slog::LogLevel msgLogLevel) {

  auto stream = msgLogLevel >= slog::WARN ? &std::cerr : &std::cout;
  for (auto st:sts) std::copy(st.first,st.first+st.second,
                              std::ostream_iterator<unsigned char>(*stream));
  *stream << std::endl;
}


slog::outputHandler::Stdio::Stdio(){}
slog::outputHandler::Stdio::~Stdio(){}
