#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <outputHandler/OutputHandler.h>
#include <Topics.hpp>
#include <Log.hpp>

#include <vector>
#include <memory>

namespace slog {


#define LOG Logger::getInstance
#define TOPIC(str, val, ...) \
  Logger::getInstance().topic(str, std::vector<typeof(val)>({val, __VA_ARGS__}));


  /**
   * Contains functionality for logging and for specification of topics and
   * topic-related settings and functionality for making the logging thread
   * safe.
   */
  class Logger : public Log, public Topics {

    public:

      /**
       * Utility #getInstance(LogLevel::Info
       * @return default instance of the logger.
       */
      static Logger& getInstance();

      /**
       * @param e the log level of the message that is to be emitted.
       * @return  default instance of the logger for emitting message with log
       *          level e
       */
      static Logger& getInstance(LogLevel e);


    /*
     * Sets the stream method. Attention: the deletion of #out is onw handled
     * by the logger.
     */
    void setStreamMethod(std::shared_ptr<outputHandler::OutputHandler> nOut);

      /*
       * delete copy, move constructors and assign operators.
       */
      Logger(Logger const&)             = delete;
      Logger(Logger&&)                  = delete;
      Logger& operator=(Logger const &) = delete;
      Logger& operator=(Logger const &&)= delete;
      Logger& operator=(Logger &&)      = delete;

      virtual ~Logger() = default;


    protected:

    /**
     * Shielded constructor: calls Log and Topics constructor and sets the base
     * context of the topic according to the Log default output.
     */
    Logger();

  };
}


#endif // _LOGGER_H_
