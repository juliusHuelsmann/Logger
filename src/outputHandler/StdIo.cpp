
#include <outputHandler/StdIo.h>

#include <iterator>
#include <algorithm>


void slog::outputHandler::StdIo::handle(
    std::vector<std::pair<const char*, size_t>> sts, 
    slog::LogLevel msgLogLevel) {
  auto stream = msgLogLevel >= slog::LogLevel::WARN ? &std::cerr : &std::cout;
  for (auto st:sts) std::copy(st.first,st.first+st.second,
      std::ostream_iterator<unsigned char>(*stream));
  *stream << std::endl;
}
