
#ifndef _OUTPUT_HANDLER_H
#define _OUTPUT_HANDLER_H

#include <Constants.h>

#include<string>
#include<unordered_map>
#include <cstring>
#include <vector>


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

        virtual void handle(const char* sts,
                            LogLevel msgLogLevel, size_t len) final {
          std::vector<std::pair<const char*, size_t>> d;
          d.push_back(std::make_pair(sts, len));
          handle(d, msgLogLevel);
        }

        virtual void handle(std::vector<std::pair<const char*, size_t>> sts,
                            slog::LogLevel msgLogLevel) = 0;
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
      Context(uint amount, uint typeSize, const char dataType) :
          out(nullptr), memorySize(0), topicPrefix(""), plotStyle(""),
          nextFreeIndex(0), els(nullptr), amount(amount), typeSize(typeSize), 
          dataType(dataType) {

      }

      /*
       * Constructors used for property inheritance
       */

      Context(outputHandler::OutputHandler* out = nullptr, uint memorySize = 0,
              std::string topicPrefix = "", std::string plotStyle=""):
          out(out), memorySize(memorySize), topicPrefix(topicPrefix),
          plotStyle(plotStyle),
          nextFreeIndex(0), els(nullptr), amount(0), typeSize(0),
          dataType(' ') { }

      Context& operator=(Context const& d) {
        this->out = d.out;
        this->memorySize = d.memorySize;
        this->topicPrefix = d.topicPrefix;
        this->plotStyle = d.plotStyle;

        els = nullptr;

        return *this;
      }

      Context(const Context& d) : els(nullptr), amount(0), typeSize(0),
                                  dataType(' ') {
        this->out = d.out;
        this->memorySize = d.memorySize;
        this->topicPrefix = d.topicPrefix;
        this->plotStyle = d.plotStyle;
      }

      virtual ~Context() {
        if (els) free(els);
      }

      /**
       *  Apply ancestor settings to a partially filled context item.
       */
      void apply(const Context& ancestor) {
        if (ancestor.out) this->out = ancestor.out;
        if (ancestor.memorySize) this->memorySize = ancestor.memorySize;
        if (ancestor.topicPrefix!="") this->topicPrefix += ancestor.topicPrefix;
        if (ancestor.plotStyle!="") this->plotStyle = ancestor.plotStyle;
      }

      //
      // Inherited
      //
      outputHandler::OutputHandler* out;
      uint memorySize;
      std::string topicPrefix;
      std::string plotStyle;

      std::string topic;
      std::string subTopic;

      //
      // Not inherited
      //
      uint nextFreeIndex;
      char* els;
      const unsigned char amount;
      const unsigned char typeSize;
      const char dataType;
    };



    struct Topic {
      Topic(): sub(), s(nullptr) { }

      std::unordered_map<std::string, Topic*> sub;
      Context* s;
    };


  }
}







#endif // _OUTPUT_HANDLER_H
