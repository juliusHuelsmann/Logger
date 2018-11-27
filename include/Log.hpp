#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <outputHandler/OutputHandler.h>
#include <Constants.h>

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <mutex>
#include <cassert>


namespace slog {

  class Log {

    public:


      /**
       * Guards the mutex and ensures that std::endl is not forgotten!
       *
       * Attention: The mutex is required to be locked when creating an instance
       * of MutexGuard, as that is the way to make counting thread-specific.
       */

      struct MutexGuard {

        public:

          explicit MutexGuard(slog::Log * l);

          ~MutexGuard();

          template<class T> MutexGuard operator<<(const std::vector<T>& m) {
            return *l << m;
          }

          template<class T> MutexGuard operator<<(const T& m) {
            return  *l<<m;
          }


          MutexGuard operator<<(ManipFn m) {
            l->flushed = true;
            return  *l<<m;
          }

        private:

          slog::Log* l;
      };


    virtual ~Log() = default;

    /**
     * Overloaded operators
     */
    template<class T> MutexGuard operator<<(const std::vector<T>& output) {
      lock();
        flushed = false;
      if (!mutexGuardCount) start();
      for (auto i = output.begin(); i != output.end(); ++i)
        sstream << *i;
      return MutexGuard(this);
    }

    /**
     * int, double, strings, ...
     */
    template<class T> MutexGuard operator<<(const T& output) {
      lock();
      flushed = false;
      if (!mutexGuardCount) start();
      sstream << output;
      return MutexGuard(this);
    }


    /**
     * endl, flush, sentw, setfill, ...
     */
    MutexGuard operator<<(ManipFn manip) {
      lock();
      manip(sstream);
      if (manip == static_cast<ManipFn>(std::flush)
          || manip == static_cast<ManipFn>(std::endl ) ) this->end();
      return MutexGuard(this);
    }

    /**
     *  For setiosflags + resetiosflags.
     */
    MutexGuard operator<<(FlagsFn manip) {
      lock();
      manip(sstream);
      return MutexGuard(this);
    }

    /**
     * Operator for specifying the log level of the logging to be executed
     * next.
     */
    MutexGuard operator()(LogLevel e) {
      lock();
      this->logLevelCurrentMessage = e;
      return MutexGuard(this);
    }

    /*
     * Setters for properties
     */

    void setExitLevel(LogLevel ll);

    void setLogLevel(LogLevel ll);
  protected:


    explicit Log();


    void lock();

    void unlock();

  private:
    void start();

    /**
     * Actual handling of the message
     */
    void end();
  protected:


    /**
     * Contains the log level of the currently constructed message.
     */
    LogLevel logLevelCurrentMessage;

    /**
     *  Output handler that is currently applied.
     */
    std::shared_ptr<outputHandler::OutputHandler> out;

  private:

    /**
     * Contains the log level that underneath which no message is logged.
     */
    LogLevel logLevel;

    /**
     * Contains the the level at / above which the program exits.
     */
    LogLevel exitLevel;

    /**
     * String set by #start and printed in #flush; contains the correct amount
     * of spaces for aligning the output nicely.
     */
    std::string startString;

    /**
     * Stream for the current log message.
     */
    std::stringstream sstream;

    /**
     * Message counter.
     */
    uint64_t msg;

    /**
     * Is false if a pending logging operation is currently running (that is
     * not terminated via std::endl).
     *
     * Used for appending std::endl in case it was forgotten by the user.
     */
    bool flushed;

    ushort mutexGuardCount;

    std::recursive_mutex mtx;

    friend MutexGuard;


  };
}

#endif // _LOG_HPP_
