
// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18
//

#ifndef LOGGER_CONTEXT_H
#define LOGGER_CONTEXT_H

#include <outputHandler/OutputHandler.h>

#include <memory>

namespace slog {
  namespace topic {

    class Context {
    public:

      /**
       * Constructor that is used in case a used context is created.
       * Sets all other values to nullptr.
       *
       * @param amount
       * @param typeSize
       */
      Context(unsigned char amount, unsigned char typeSize, char dataType,
              std::string topic, std::string sub);

      /*
       * Constructors used for property inheritance
       */

      Context(std::shared_ptr<outputHandler::OutputHandler> out = nullptr,
          uint memorySize = 0,
          std::string topicPrefix = "", std::string plotStyle="",
          const unsigned char plotBufferSize=0);

      /**
       * Assignment operator: copies
       * @param d
       * @return
       */
      Context& operator=(Context const& d);

      Context(const Context& d);

      virtual ~Context();

      /**
       *  Apply ancestor settings to a partially filled context item.
       */
      void apply(const Context& ancestor);

      //
      // Inherited
      //

      /**
       *  Output handler is only used
       */
      std::shared_ptr<outputHandler::OutputHandler> out;
      uint memorySize;
      std::string topicPrefix;
      std::string plotStyle;
      unsigned char plotBufferSize;

      /**
       * Topic identifier without #subTopic.
       * "execTime.math"       in full topic name "execTime.math[1]"
       * "execTime.math.uint"  in full topic name "execTime.math[standard].uint"
       *
       */
      const std::string topic;

      /**
       * Sub topic identifier e.g.
       * "1"                   in full topic name "execTime.math[1]"
       * "standard"            in full topic name "execTime.math[standard].uint"
       * "1standard"           in full topic name "execTime[1].math[standard]"
       */
      const std::string subTopic;

      //
      // Not inherited
      //
      uint nextFreeIndex;
      char* els;
      const unsigned char amount;
      const unsigned char typeSize;
      const char dataType;
    };
  }
}

#endif //LOGGER_CONTEXT_H
