
#ifndef _OUTPUT_HANDLER_H
#define _OUTPUT_HANDLER_H

#include <Constants.h>

#include<string>
#include<unordered_map>
#include <cstring>


namespace slog {
  namespace outputHandler {
    class OutputHandler {

      public: 

        /**
         * This function handles a way to deal with messages:
         */
        void handle(const char*  st, LogLevel msgLogLevel) {
          handle(st,msgLogLevel, strlen(st));
        }

        virtual void handle(const char* st, LogLevel msgLogLevel, size_t len)=0;

        virtual ~OutputHandler();

    };
  }
}





namespace slog {
  namespace topic {

    struct Context {

      /**
       * Constructor that is used in case a used context is created.
       *
       * @param amount
       * @param typeSize
       */
      Context(uint amount, uint typeSize) :
          out(nullptr), memorySize(0), topicPrefix(""), nextFreeIndex(0),
          els(nullptr), amount(amount), typeSize(typeSize) { }

      /*
       * Constructors used for property inheritance
       */

      Context(outputHandler::OutputHandler* out = nullptr, uint memorySize = 0,
              std::string topicPrefix = ""):
          out(out), memorySize(memorySize), topicPrefix(topicPrefix),
          nextFreeIndex(0), els(nullptr), amount(0), typeSize(0) {

      }

      Context& operator=(Context const& d) {
        this->out = d.out;
        this->memorySize = d.memorySize;
        this->topicPrefix = d.topicPrefix;
        return *this;
      }

      Context(const Context& d) : amount(0), typeSize(0) {
        this->out = d.out;
        this->memorySize = d.memorySize;
        this->topicPrefix = d.topicPrefix;
      }
      virtual ~Context() {
        free(els);
      }

      /**
       *  Apply ancestor settings to a partially filled context item.
       */
      void apply(const Context& ancestor) {
        if (ancestor.out) this->out = ancestor.out;
        if (ancestor.memorySize) this->memorySize = ancestor.memorySize;
        if (ancestor.topicPrefix!="") this->topicPrefix += ancestor.topicPrefix;
      }

      //
      // Inherited
      //

      outputHandler::OutputHandler* out;
      uint memorySize;
      std::string topicPrefix;

      //
      // Not inherited
      //
      uint nextFreeIndex;
      char* els;
      const unsigned char amount;
      const unsigned char typeSize;
    };



    struct Topic {
      Topic(): sub(), s(nullptr) { }

      std::unordered_map<std::string, Topic*> sub;
      Context* s;
    };


  }
}







#endif // _OUTPUT_HANDLER_H
