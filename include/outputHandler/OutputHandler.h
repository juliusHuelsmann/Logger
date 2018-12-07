
#ifndef _OUTPUT_HANDLER_H
#define _OUTPUT_HANDLER_H

#include <Constants.h>

#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>


namespace slog {
    namespace topic {
        class Context;
    }
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


        /**
         * Default handler for topic messages that can be overwritten by a class implements
         * the outputHandler.
         *
         * @param context
         * @param additionalData
         * @param additionalSize
         */
        virtual void logTopic(topic::Context * context, const char* additionalData=nullptr,
                              size_t additionalSize=0);

    };
  }
}

#endif // _OUTPUT_HANDLER_H
