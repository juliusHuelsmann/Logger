#ifndef _STDIO_H_
#define _STDIO_H_

#include <outputHandler/OutputHandler.h>

namespace slog {
  namespace outputHandler {
    class Stdio : public OutputHandler {

      public: 
        Stdio();

        /**
         * This function handles a way to deal with messages:
         * print to standard out / standard err.
         */
        virtual void handle(const char* st, LogLevel msgLogLevel, size_t s)
        override;

        virtual ~Stdio();

    };
  }
}
#endif //_STDIO_H_
