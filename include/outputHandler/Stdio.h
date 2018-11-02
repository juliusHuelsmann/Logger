#ifndef _STDIO_H_
#define _STDIO_H_

#include <outputHandler/OutputHandler.h>

namespace jlog {
  namespace outputHandler {
    class Stdio : public OutputHandler {

      public: 
        Stdio();

        /**
         * This function handles a way to deal with messages:
         * print to standard out / standard err.
         */
        virtual void handle(std::string st, LogLevel msgLogLevel) override;
        virtual ~Stdio();

    };
  }
}
#endif //_STDIO_H_
