#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <outputHandler/OutputHandler.h>
#include <outputHandler/StdIo.h>
#include <Constants.h>

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include <stdarg.h>
#include <mutex>
#include <thread>

namespace slog {

#define LOG Logger::getInstance
#define TOPIC(str, val, ...) \
  Logger::topic(str, std::vector<typeof(val)>({val, __VA_ARGS__}));
#define SUB_EQ(sub, str) str.size() >= sub.size() && \
  std::equal(sub.begin(), sub.begin()+sub.size(), str.begin())

  typedef std::ostream& (*ManipFn)(std::ostream&);
  typedef std::ios_base& (*FlagsFn)(std::ios_base&);

  /**
   * Contains functionality for logging and for specification of topics and
   * topic-related settings and functionality for making the logging thread
   * safe.
   */
  class Logger {

  private:


    struct UnlockMutexReturnHelper{ //XXX: move back to mutexGuard
      ~UnlockMutexReturnHelper() {
        Logger::getInstance().mtx.unlock();
      }
    };


    /**
     * Guards the mutex and ensures that std::endl is not forgotten!
     *
     * Attention: The mutex is required to be locked when creating an instance
     * of MutexGuard, as that is the way to make counting thread-specific.
     */
    struct MutexGuard {
    public:

      MutexGuard(Logger* l): l(l) {
        // the first MutexGuard has to lock one times more
        if (!l->mutexGuardCount++) {
          l->mtx.lock();
        }
      }

      ~MutexGuard() {
        l->mtx.unlock();
        if (!--l->mutexGuardCount) {
          // 1) lock, 2) lock (in constructor), 3) unlock (here) 4) unlock (here)
          if (!l->flushed) *this << std::endl;
          l->mtx.unlock();
        }
      }

      template<class T>
      MutexGuard operator<<(const std::vector<T>& m) {
        l->flushed = false;
        return *l << m;
      }
      template<class T>
      MutexGuard operator<<(const T& m) {
        l->flushed = false;
        return  *l<<m;
      }
      MutexGuard operator<<(ManipFn m) {
        l->flushed = true;
        return  *l<<m;
      }

    private:
      Logger* l;
    };

    /**
     * Count of the mutex guards that are currently active for this instance 
     * of a logger.
     */
    uint mutexGuardCount; //  friend MutexGuard; // XXX: friend scope.
    bool flushed;

  public:

      /*
       * delete, copy, move constructors and assign operators
       */
      Logger(Logger const&) = delete;
      Logger(Logger&&) = delete;
      Logger& operator=(Logger const &) = delete;
      Logger& operator=(Logger const &&) = delete;
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
       *  Topics
       */

      /**
       * Set the default configuration which is applied in case no more
       * specific sub-configuration is given.
       */
      static void setBaseContext(const topic::Context& baseContext) {
        getInstance().mtx.lock();
        auto mg = UnlockMutexReturnHelper();
        Logger::baseContext = baseContext;
        for (auto c: launchedTopics) {
          recomputeSettings(c.first, c.second);
          assert(c.second);
        }
      }

      topic::Context const *getBaseContext() {
        return &baseContext;
      }

      /**
       *  helper function for extracting subtopics from string.
       */
      static void subTopic(std::vector<std::string>& topBranch,
                           const std::string& topId) {
          auto c = boost::split(topBranch, topId,
                       [](char c){return c == '.' || c == '[';});
      }

    private:

      /**
       * Utility method for computing the context for a given branch if enabled.
       *
       * @param topicBranchId       a string that uniquely identifies the
       *                            branch in the settings tree.
       * @param amountParameters    the typical amount of parameters that is
       *                            logged under the specific topic.
       * @param sizeType            the type of the size that is to be stored
       *                            under topic.
       *
       * @return    the context that is attached for the provided settings
       *            branch identifier or nullptr if the settings in question
       *            are not logged.
       * @see recomputeSettings
       */
      static topic::Context* computeSettings(const std::string& topic,
          uint amountParameters, uint sizeType, const char dataType) {
        topic::Context* ctx =
            new topic::Context(amountParameters, sizeType, dataType);
        recomputeSettings(topic, ctx);
        return ctx;
      }

      /**
       *
       * @param topic         a string that uniquely identifies the
       *                      branch in the settings tree.
       * @param ctx           Handle to a pointer of context, is deleted  and
       *                      set to nullptr if the subject branch is not
       *                      enabled, otherwise contains the context
       *                      associated with the settingds branch.
       */
      static void recomputeSettings(const std::string& topic,
                                    topic::Context*& ctx, bool enabled=false) {

        // generate a sub-topic split for the passed identifier
        auto sub = std::vector<std::string>();
        Logger::subTopic(sub, topic);

        // sub topic configuration
        auto i = topic.find('[');
        std::string subTop = "";
        std::string top = topic;
        if (i != std::string::npos) {
          auto j = topic.find(']');
          assert(j != std::string::npos && j > i);


          subTop =  topic.substr(i+1, j-i-1);
          top = topic.substr(0,i) + topic.substr(j+1, topic.size());
        }

        // Initialize the context that is to be created with the base context.
        // Then go through the settings tree and apply the settings.
        // More specific settings have a higher priority than general settings.
        *ctx = Logger::baseContext;
        //auto enabled = false;         //< true if some parent is enabled.
        auto cTop = &topics;
        auto idx = sub.begin();
        for (cTop = cTop->sub.count(*idx)!=0 ? cTop->sub[*idx]: nullptr;
             cTop != nullptr && idx != sub.end();
             cTop = ++idx != sub.end() && cTop->sub.count(*idx)
                    ? cTop->sub[*idx]: nullptr) {

          if (cTop->s) {
            enabled = true;
            ctx->apply(*cTop->s);
          }
        }
        ctx->subTopic = subTop;
        ctx->topic = top;
        if (!enabled) {
          delete(ctx);
          ctx= nullptr;
        }
      }

    public:

      /**
       * Function used for enabling a topic. After it is enabled, topic logs
       * categorized under that topic or any of its descendants are enabled
       * and processed as specified via preference - parameters.
       *
       * Logging preferences (called "Context") for topics are inherited if
       * set. The root of inheritance is the base configuration "baseContext".
       *
       * If this function is called multiple times for the same topic, the
       * information provided last are taken into account.
       */
      static void enableTopic(const std::string& topic,
                              outputHandler::OutputHandler*out= nullptr,
          uint memorySize=0, const std::string topicPrefix="",
          const std::string plotStyle=LINE_PLOT,
                              const unsigned char plotBufferSize=0) {

        getInstance().mtx.lock();
        volatile UnlockMutexReturnHelper umrh;

        //auto thisId = std::this_thread::get_id();
        //std::cout << ":" << thisId << "";

        // erase the implicitly disabled topics that are descendants of the
        // now-enabled topic #topic.
        for (auto d = disabledTopics.begin(); d != disabledTopics.end();
             d = SUB_EQ(topic, d->first) ? disabledTopics.erase(d) : ++d);

        // generate a sub-topic split for the passed identifier
        auto sub = std::vector<std::string>();
        subTopic(sub, topic);
        if (!sub.size()) return;

        // Determine whether the topic has already been enabled, add it
        // otherwise.
        topic::Topic* t = &topics;
        for (auto cSub = sub.begin(); cSub != sub.end(); t = t->sub[*cSub++])
          if (!t->sub.count(*cSub)) {
            t->sub[*cSub] = new topic::Topic();
          }

        // Store the context inside the topic
        if (t->s) delete(t->s);
        t->s = new topic::Context(out, memorySize, topicPrefix, plotStyle,
                                  plotBufferSize);
        auto k = Logger::topics;

        // update the buffered context for all subjects that were enabled
        // before.
        for (auto c: Logger::launchedTopics) {
          if (SUB_EQ(topic, c.first)) {
            const auto memSizePrior = c.second->memorySize;
            recomputeSettings(c.first, c.second, true);
            const auto memSizeAfter = c.second->memorySize;

            if (memSizePrior != memSizeAfter) {
              if (c.second->nextFreeIndex) // if contains content
                c.second->out->handle((const char*) c.second->els, INFO,
                                      (size_t) c.second->nextFreeIndex);
              delete(c.second->els);
              c.second->els = (char* ) malloc(c.second->memorySize);
              c.second->nextFreeIndex = 0;
            }
            assert(!(memSizeAfter != memorySize && topic == c.first));
          }
        }
      }

      /**
       * 
       */
      template <typename T> static
      void topic(std::string tIdent, std::vector<T> vec) {

        getInstance().mtx.lock();
        volatile auto mg = UnlockMutexReturnHelper();

        // The topic is implicitly disabled (check returned false before); exit
        if (disabledTopics.count(tIdent)) return;

        // The topic has been used before as it is enabled, so directly use
        // the attached context.
        const auto sizeType = sizeof(T);
        const auto sizeVec = vec.size();

        auto name = typeid(T).name();
        //const char dataType = types.count(name) ? types[name] : ' ';
        const char dataType = name[0];

        if (!launchedTopics.count(tIdent)) {

          // the exact topic has not been used yet, but it is possible that
          // it is a descendant of a topic that is enabled.

          auto contextToAdd = computeSettings(tIdent, sizeVec, sizeType,
                                              dataType);
          if (contextToAdd) {
            launchedTopics[tIdent] = contextToAdd;
            launchedTopics[tIdent]->els = (char* ) malloc(contextToAdd->memorySize);
          }
          else {
            disabledTopics[tIdent] = true;
            return;
          }
        }

        //
        // Log the values.
        assert(launchedTopics.count(tIdent));
        auto topic = launchedTopics[tIdent];
        assert(topic->out);
        assert(sizeVec == topic->amount && sizeType == topic->typeSize);
        const auto sizeToAdd = vec.size() * sizeof(T);
        auto settings = std::stringstream ();
        assert (topic->topic.size() < 255);
        settings << (unsigned char) 1
                 << (unsigned char) topic->topic.size()
                 << topic->topic.c_str()
                 << (unsigned char) topic->subTopic.size()
                 << topic->subTopic.c_str()
                 << (unsigned char) topic->plotStyle.size()
                 << topic->plotStyle
                 << topic->amount
                 << topic->typeSize
                 << topic->dataType
                 << topic->plotBufferSize;

        if (topic->memorySize <= sizeToAdd + topic->nextFreeIndex) {
          auto str = settings.str();
          auto vals = std::vector<std::pair<const char*, size_t>>();
          vals.push_back(std::make_pair(str.c_str(), str.size()));

          //
          // Directly forward to the output
          if (topic->nextFreeIndex) vals.push_back(std::make_pair(
                topic->els, (size_t) topic->nextFreeIndex));

          vals.push_back(std::make_pair((const char*) vec.data(),
                                        (size_t) sizeToAdd));
          topic->out->handle(vals, INFO);
          topic->nextFreeIndex = 0;
        } else {
          //
          // write to buffer and increase buffer index.
          memcpy(topic->els+topic->nextFreeIndex, vec.data(), sizeToAdd);
          topic->nextFreeIndex += sizeToAdd;
          assert(topic->nextFreeIndex < topic->memorySize);
        }
      }

      /*
       * Overloaded operators
       */
      template<class T> MutexGuard operator<<(const std::vector<T>& output) {
        mtx.lock();
        if (!mutexGuardCount) start();
        for (auto i = output.begin(); i != output.end(); ++i)
          sstream << *i;
        return MutexGuard(this);
      }

      /*
       * int, double, strings, ...
       */
      template<class T> MutexGuard operator<<(const T& output) {
        mtx.lock();
        if (!mutexGuardCount) start();
        sstream << output;
        return MutexGuard(this);
      }

      /*
       * endl, flush, sentw, setfill, ...
       */
      MutexGuard operator<<(ManipFn manip) {
        mtx.lock();
        manip(sstream);
        if (manip == static_cast<ManipFn>(std::flush)
            || manip == static_cast<ManipFn>(std::endl ) ) this->flush();
        return MutexGuard(this);
      }

      /**
       *  For setiosflags + resetiosflags.
       */
      MutexGuard operator<<(FlagsFn manip) {
        mtx.lock();
        manip(sstream);
        return MutexGuard(this);
      }
      

      /**
       * Operator for specifying the log level of the logging to be executed
       * next.
       */
      Logger& operator()(LogLevel e) {
        mtx.lock();
        this->logLevelCurrentMessage = e;
        mtx.unlock();
        return *this;
      }

  private:
      void start() {
          switch (this->logLevelCurrentMessage) {
            case DEBUG:
              sstream << "DEBUG";
              break;
            case INFO:
              sstream << "INFO ";
              break;
            case WARN:
              sstream << "WARN ";
              break;
            case ERROR:
              sstream << "ERROR";
              break;
            case FATAL:
              sstream << "FATAL";
              break;
            case NEVER:
            default:
              sstream << "Fatal error occurred while logging a message: "
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
          sstream <<  "["
              //<< local->tm_mday << "." << (local->tm_mon + 1) << ' '
              << local->tm_hour << ":" << local->tm_min << ":" << local->tm_sec
              << "." << (msg < 10 ? " " : "")  << msg << "] ";
        this->startString = std::string(sstream.str().size(),' ');
      }


      /**
       * Actual handling of the message
       */
      void flush() {

        // Check if the message is to be logged.
        if (this->logLevelCurrentMessage >= this->logLevel) {

          auto s = sstream.str();
          boost::replace_all(s.erase(s.size()-1),"\n","\n" + startString);

          out->handle(s.c_str(), logLevelCurrentMessage);

          // Check if the program has to exit according to the log level
          if (this->logLevelCurrentMessage >= this->exitLevel) {

            std::stringstream error;
            error << "\n The logger-exit level is set to exit on messages of level "
                  << this->exitLevel << " or higher. \n Bye!\n";
            out->handle(error.str().c_str(), INFO);
            std::exit(-1);
          }
        }

        // reset the standard log level to INFO (in case no log level is explicitely
        // provided) and clear + empty the string stream
        sstream.str( std::string() );
        sstream.clear();
        this->logLevelCurrentMessage = INFO;
      }

  public:

      /*
       * Setters for properties
       */

      void setExitLevel(LogLevel ll) {

        mtx.lock();
        this->exitLevel = ll;
        mtx.unlock();
      }

      void setLogLevel(LogLevel ll) {
        mtx.lock();
        this->logLevel = ll;
        mtx.unlock();
      }

      /*
       * Sets the stream method. Attention: the deletion of #out is onw handled
       * by the logger.
       */
      void setStreamMethod(outputHandler::OutputHandler* out) {
        mtx.lock();
        if (this->out) delete(this->out);
        this->out = out;
        this->baseContext.out = out;
        mtx.unlock();
      }

    protected:

      Logger(): logLevelCurrentMessage(INFO), logLevel(INFO), exitLevel(ERROR),
                startString("") {
        out = new outputHandler::StdIo();
        this->msg = 0;
        this->baseContext.out = out;
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
       * String set by #start and printed in #flush; contains the correct amount
       * of spaces for aligning the output nicely.
       */
      std::string startString;

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

      /**
       * 
       */
      static topic::Topic topics;
      static std::unordered_map<std::string, topic::Context*> launchedTopics;
      static std::unordered_map<std::string, bool> disabledTopics;
      static topic::Context baseContext;


      class Mut : public std::recursive_mutex {
      public:
        static uint cntr;

        void lock() {
          cntr++;
          this->std::recursive_mutex::lock();
        }
        void unlock() {
          this->std::recursive_mutex::unlock();
        }

      };
      /**
       * Mutex that is used in the middle of
       */
      Mut mtx;


      static std::unordered_map<std::string, char> types;


    friend MutexGuard;
  };
}


#endif // _LOGGER_H_
