
#ifndef _OUTPUT_HANDLER_H
#define _OUTPUT_HANDLER_H

#include <Constants.h>

#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>


namespace slog {
  namespace outputHandler {
    class OutputHandler {

      public: 
          
        virtual ~OutputHandler() = default;

        /**
         * This function handles a way to deal with messages:
         */
        void handle(const char*  st, LogLevel msgLogLevel);

        virtual void handle(const char* sts, LogLevel msgLogLevel, size_t len) 
          final;

        virtual void handle(std::vector<std::pair<const char*, size_t>> sts,
            slog::LogLevel msgLogLevel) = 0;

    };
  }
}

#endif // _OUTPUT_HANDLER_H
