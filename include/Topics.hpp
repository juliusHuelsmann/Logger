
#ifndef _TOPICS_HPP_
#define _TOPICS_HPP_

#include <outputHandler/OutputHandler.h>
#include <outputHandler/StdIo.h>
#include <Constants.h>
#include <guard/MutexReturn.h>
#include <topics/Topic.h>

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
#include <cstdarg>
#include <mutex>
#include <thread>

namespace slog {

#define SUB_EQ(sub, str) (str).size() >= (sub).size() && \
  std::equal((sub).begin(), (sub).begin()+(sub).size(), (str).begin())

  typedef std::ostream& (*ManipFn)(std::ostream&);
  typedef std::ios_base& (*FlagsFn)(std::ios_base&);

  /**
   * Contains functionality for logging and for specification of topics and
   * topic-related settings and functionality for making the logging thread
   * safe.
   */
  class Topics {

    public:

      virtual ~Topics();

      /**
       * Set the default configuration which is applied in case no more
       * specific sub-configuration is given.
       */
      void setBaseContext(const topic::Context& baseContext);

      /**
       * Function used for enabling a topic. After it is enabled, the specified
       * topic or any of its descendants are enabled and processed as specified
       * via preference - parameters.
       *
       * Logging preferences (called "Context") for topics are inherited on
       * each level of the topic tree if provided.
       * The root of the tree is the base configuration "baseContext" which
       * must always specify an outputHandler that might be "overwritten" by
       * a more specific outputHandler specified for a descendant of the topic.
       *
       * If this function is called multiple times for the same topic, the
       * information provided last are taken into account and the topics
       * that already exist are updated.
       */
      void enableTopic(std::string topic,
                 std::shared_ptr<outputHandler::OutputHandler> out= nullptr,
                 uint memorySize=0, std::string topicPrefix="",
                 std::string plotStyle=LINE_PLOT,
                 unsigned char plotBufferSize=0);


      /**
       * Log values provided in #vec via outputStream provided by the topic
       * configuration iferred from #tIdent.
       *
       * This function is not recommended to be called directly, the
       * preprocessor macro #TOPIC provides a more convenient api.
       *
       * For a topic, the type and the vector are required to remain the
       * same during an entire session.
       */
template <typename T> void topic(std::string tIdent, std::vector<T> vec) {

  lock();
  volatile MutexReturn mr(this);
  preprocessTopic(tIdent);

  // The topic is implicitly disabled (check returned false before); exit
  if (disabledTopics.count(tIdent)) return;

  const auto sizeType = sizeof(T);
  const auto sizeVec = vec.size();
  auto type = typeid(T).name()[0];
  type = type == 'x' ? 'q': type; //XXX: fix this generally

  if (!launchedTopics.count(tIdent)) {

    // the exact topic has not been used yet, but it is possible that
    // it is a descendant of a topic that is enabled.

    assert(sizeVec < std::numeric_limits<unsigned char>::max());
    assert(sizeVec < std::numeric_limits<unsigned char>::max());
    auto contextToAdd = computeSettings(tIdent,
                                        (unsigned char) sizeVec,
                                        (unsigned char) sizeType,
                                        type);
    if (contextToAdd) {
      launchedTopics[tIdent] = contextToAdd;
      launchedTopics[tIdent]->els = (char* ) malloc(contextToAdd->memorySize);
    }
    else {
      disabledTopics[tIdent] = true;
      return;
    }
  }

  // The topic can be found in the enabled-mapping.
  // Log the provided parameters.

  //
  // Sanity checks.
  assert(launchedTopics.count(tIdent));
  auto topic = launchedTopics[tIdent];
  assert(topic->out);
  // the amount of parameters and the size of the data type is assumed
  // to remain unchanged.
  assert(sizeVec == topic->amount && sizeType == topic->typeSize
         && topic->dataType == type);
  // Size of the topic and subTopic string has to fit into an unsigned char.
  // The size of the plot style is guaranteed to be < 255 as its contents are
  // predefined.
  const auto limitSize = std::numeric_limits<unsigned char>::max();
  assert (topic->topic.size() <= limitSize);
  assert (topic->subTopic.size() <= limitSize);
  assert (topic->plotStyle.size() <= limitSize);

  // Find out if the new parameters have space inside the buffer or if the
  // buffer has to be flushed
  const auto sizeToAdd = vec.size() * sizeof(T);
  if (topic->memorySize <= sizeToAdd + topic->nextFreeIndex) {
    topic->out->logTopic(topic, (const char*) vec.data(), (size_t) sizeToAdd);
  } else {
    //
    // write to buffer and increase buffer index.
    memcpy(topic->els+topic->nextFreeIndex, vec.data(), sizeToAdd);
    topic->nextFreeIndex += sizeToAdd;
    assert(topic->nextFreeIndex < topic->memorySize);
  }
}


      topic::Context const & getBaseContext() const;

    protected:

    void lock();

    void unlock();

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
    topic::Context* computeSettings(const std::string& topic,
       unsigned char amountParameters, unsigned char sizeType, char dataType);

    /**
     *
     * @param topic         a string that uniquely identifies the
     *                      branch in the settings tree.
     * @param ctx           Handle to a pointer of context, is deleted  and
     *                      set to nullptr if the subject branch is not
     *                      enabled, otherwise contains the context
     *                      associated with the settingds branch.
     */
    void recomputeSettings(const std::string& topic,
       topic::Context*& ctx, bool enabled=false);


    inline void preprocessTopic(std::string& raw) {
      while(raw.find("..") != std::string::npos) boost::replace_all(raw, "..", ".");
      while(raw.find("].") != std::string::npos) boost::replace_all(raw, "].", "]");
    }

    static void iterate(slog::topic::Topic* top, size_t sz, std::stringstream& sstr);
      /**
       *  Root of the settings tree, guaranteed to have an outputHandler
       *  configured.
       */
      topic::Context baseContext;

      /**
       * Root of the topics tree.
       */
      topic::Topic topics;

      /**
       * Map for quick lookup of topics that have already been launched
       */
      std::unordered_map<std::string, topic::Context*> launchedTopics;

      /**
       * Map for quick lookup of topics that have already been used in #log
       * but not been enabled yet.
       */
      std::unordered_map<std::string, bool> disabledTopics;



    std::recursive_mutex mtx;
    friend MutexReturn;

  };
}

#endif // _TOPICS_HPP_
