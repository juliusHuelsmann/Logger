
#ifndef _OUTPUT_HANDLER_H
#define _OUTPUT_HANDLER_H

#include <Constants.h>

namespace log {
  namespace outputHandler {
    class OutputHandler {

      public: 

        /**
         * This function handles a way to deal with messages:
         */
        virtual void handle(std::string st, LogLevel msgLogLevel);
        virtual ~OutputHandler();

    };
  }
}
#endif // _OUTPUT_HANDLER_H
