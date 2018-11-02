#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <Constants.h>
#include <outputHandler/OutputHandler.h>
#include <outputHandler/Stdio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <iostream>
#include <vector>

namespace jlog {
#define LOG Logger::getInstance

  /**
   * Logging class 
   */
  class Logger {

    public:
      typedef std::ostream& (*ManipFn)(std::ostream&);
      typedef std::ios_base& (*FlagsFn)(std::ios_base&);
      const std::string separator = "-----------------------------";
      const std::string end = "                                                                              □ ";

      /*
       * delete, copy, move constructors and assign operators
       */
      Logger(Logger const&) = delete;
      Logger(Logger&&) = delete;
      Logger& operator=(Logger const&) = delete;
      Logger& operator=(Logger &&) = delete;
      static Logger& getInstance() {
        static Logger instance;
        return instance;
      }

      static Logger& getInstance(LogLevel e) {
        Logger::getInstance().logLevelCurrentMessage = e;
        return Logger::getInstance();
      }

      /*
       * Overloaded operators
       */
      template<class T> Logger& operator<<(const std::vector<T>& output) {
        for (auto i = output.begin(); i != output.end(); ++i)
          sstream << *i;
        return *this;
      }

      /*
       * int, double, strings, ...
       */
      template<class T> Logger& operator<<(const T& output) {
        sstream << output;
        return *this;
      }

      /*
       * endl, flush, sentw, setfill, ...
       */
      Logger& operator<<(ManipFn manip) { 
        manip(sstream);
        if (manip == static_cast<ManipFn>(std::flush)
            || manip == static_cast<ManipFn>(std::endl ) ) this->flush();
        return *this;
      }

      /**
       *  For setiosflags + resetiosflags.
       */
      Logger& operator<<(FlagsFn manip) {
        manip(sstream);
        return *this;
      }
      

      /**
       * Operator for specifying the log level of the logging to be executed
       * next.
       */
      Logger& operator()(LogLevel e) {
        this->logLevelCurrentMessage = e;
        return *this;
      }


      /**
       * Actual handling of the message
       */
      void flush() {

        // Check if the message is to be logged.
        if (this->logLevelCurrentMessage >= this->logLevel) {

          std::stringstream pre;
          pre << separator;
          switch (this->logLevelCurrentMessage) {
            case DEBUG:
              pre << "DEBUG";
              break;
            case INFO:
              pre << "INFO ";
              break;
            case WARN:
              pre << "WARN ";
              break;
            case ERROR:
              pre << "ERROR";
              break;
            case FATAL:
              pre << "FATAL";
              break;
            case NEVER:
            default:
              pre << "Fatal error occurred while logging a message: "
                  << " Log level of message corrupted : "
                  << this->logLevelCurrentMessage
                  << "\n\nOriginal message:\n";

              // ensure to exit
              this->logLevelCurrentMessage  = NEVER;
          }
          //prepend current time.
          auto now = std::time(0);
          auto local = std::localtime(&now);
          this->msg = (this->msg + 1) % 100;
          pre <<  " " <<local->tm_mday << "." << (local->tm_mon + 1) << ' '
              << local->tm_hour << ":" << local->tm_min << ":" << local->tm_sec
              << "." << (msg < 10 ? " " : "")  << msg;


          // append the message.
          pre << separator << "\n" << sstream.str() << "\n" << end << "\n";

          out->handle(pre.str(), logLevelCurrentMessage);

          // Check if the program has to exit according to the log level
          if (this->logLevelCurrentMessage >= this->exitLevel) {

            std::stringstream error;
            error << "\n The logger-exit level is set to exit on messages of level "
                  << this->exitLevel << " or higher. \n Bye!\n";
            out->handle(error.str(), INFO);
            std::exit(-1);
          }
        }

        // reset the standard log level to INFO (in case no log level is explicitely
        // provided) and clear + empty the string stream
        sstream.str( std::string() );
        sstream.clear();
        this->logLevelCurrentMessage = INFO;
      }

      /*
       * Setters for properties
       */

      void setExitLevel(LogLevel ll) {
        this->exitLevel = ll;
      }

      void setLogLevel(LogLevel ll) {
        this->logLevel = ll;
      }

      /*
       * Sets the stream method. Attention: the deletion of #out is onw handled
       * by the logger.
       */
      void setStreamMethod(outputHandler::OutputHandler* out) {
        if (this->out) delete(this->out);
        this->out = out;
      }

    protected:

      Logger(): logLevelCurrentMessage(INFO), logLevel(INFO), exitLevel(ERROR) {
        out = new outputHandler::Stdio();
        this->msg = 0;
      }

      virtual ~Logger() {
        if (out) delete(out);
      }

    private: 

      /**
       * Contains the log level of the currently constructed message.
       */
      LogLevel logLevelCurrentMessage;

      /**
       * Contains the log level that underneath which no message is logged.
       */
      LogLevel logLevel;

      /**
       * Contains the the level at / above which the program exits.
       */
      LogLevel exitLevel;

      /**
       * Stream for the current log message.
       */
      std::stringstream sstream;

      /**
       *  Output handler that is currently applied.
       */
      outputHandler::OutputHandler* out;

      /**
       * Message counter.
       */
      uint64_t msg;
  };
}

#endif // _LOGGER_H_
