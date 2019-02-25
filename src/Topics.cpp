// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/15/18

#include <Topics.hpp>

void slog::Topics::iterate(slog::topic::Topic* top, size_t sz, std::stringstream& sstr) {
  if (top->s) {
    sstr << "(w. config)" << std::flush;
    sz += 11;
  }
  uint it = 0;
  for (const auto c : top->sub) {
    if (it++) sstr << "\n" << std::string(sz , ' ');
    sstr <<  " -> " << c.first;
    iterate(c.second, sz + c.first.size() + 4, sstr);
    delete(c.second);
  }
  std::cout << "<";
}

void slog::Topics::flush() {
  for (auto c : launchedTopics) {
    c.second->out->logTopic(c.second);
  }
}

slog::Topics::~Topics() {

  auto info = std::stringstream();

  if (launchedTopics.size() || disabledTopics.size()) {
    info << "\nThe logger is shutting down and hence cleaning up the topics.\n"
         << "List of active topics:   ";
    for (auto c = launchedTopics.begin(); launchedTopics.size();
         c = launchedTopics.erase(c)) {
      info << c->first << (launchedTopics.size() == 1  ? "."  : ", ");
      c->second->out->logTopic(c->second);
      delete(c->second);
    }
    info << "\nList of disabled topics: ";
    for (auto c = disabledTopics.begin(); disabledTopics.size();
         c = disabledTopics.erase(c)) {
      info << c->first << (disabledTopics.size() == 1  ? "."  : ", ");
    }
    info << "\nSettings tree:\n";
    iterate(&topics, 0, info);

  } else {
    info << "\nThe topics logger shut down unused.\n";
  }
  baseContext.out->handle(info.str().c_str(), LogLevel::INFO);
}


/**
 * Helper function that extracts subtopics - settings - branch from the raw
 * topic string.
 *
 * @param tBranch           return parameter: overwritten with the branch.
 * @param rawString         the raw topics string
 */
inline void computeSettingsTopicStructure(std::vector<std::string>& tBranch,
    std::string rawString) {
  boost::split(tBranch, rawString, [](char c){return c == '.'
                                                     || c == '[' || c == ']';});
  if (tBranch.rbegin()->size() == 0) tBranch.pop_back();
}

/**
 * Helper function that extracts graph - settings - image (indicated by #top)
 * and graph title (based on #subTop) from raw topic string.

 * @param raw           the raw topics string.
 * @param top           return parameter: overwritten with the topic
 *                      specification. See #Context.topic for details
 * @param subTop        return parameter: overwritten with the subTopic
 *                      index. See #Context.subTopic for details.
 */
inline void computeGraphicalTopicStructure(const std::string& raw,
                                           std::string& top, std::string& subTop) {
  size_t i, j, altJ(0);
  for (i = raw.find('[', 0), j = raw.find(']', i), subTop = "", top = "";
       i != std::string::npos && j != std::string::npos;
       subTop += raw.substr(i+1, j-i-1), top += raw.substr(altJ,i-altJ),
       altJ=j+1, i = raw.find('[', j), j = raw.find(']', i));
  top += raw.substr(altJ, raw.size()); //< add final piece from last ] to end
}


void slog::Topics::setBaseContext(const topic::Context& baseContext) {
  lock();
  volatile MutexReturn mr(this);
  this->baseContext = baseContext;
  for (auto c: launchedTopics) {
    recomputeSettings(c.first, c.second);
    assert(c.second);
  }
}

slog::topic::Context* slog::Topics::computeSettings(const std::string& topicRaw,
    unsigned char amountParams, unsigned char sizeType, char dataType) {

  // sub topic configuration
  std::string top(""), subTop("");
  computeGraphicalTopicStructure(topicRaw, top, subTop);

  auto ctx = new topic::Context(amountParams, sizeType, dataType, top, subTop);
  recomputeSettings(topicRaw, ctx);
  return ctx;
}

void slog::Topics::recomputeSettings(const std::string& topicRaw,
    topic::Context*& ctx, bool enabled) {

  // generate a sub-topic split for the passed identifier
  auto sub = std::vector<std::string>();
  computeSettingsTopicStructure(sub, topicRaw);

  // Initialize the context that is to be created with the base context.
  // Then go through the settings tree and apply the settings.
  // More specific settings have a higher priority than general settings.
  *ctx = this->baseContext;
  //enabled          //< true if some parent is enabled.
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
  if (!enabled) {
    delete(ctx);
    ctx= nullptr;
  }
}


void slog::Topics::enableTopic(std::string topic,
    std::shared_ptr<outputHandler::OutputHandler> out, uint memorySize,
    std::string topicPrefix, std::string plotStyle,
    unsigned char plotBufferSize) {

  lock();
  volatile MutexReturn mr(this);
  preprocessTopic(topic);

  // erase the implicitly disabled topics that are descendants of the
  // now-enabled topic #topic.
  for (auto d = disabledTopics.begin(); d != disabledTopics.end();
       d = SUB_EQ(topic, d->first) ? disabledTopics.erase(d) : ++d);

  // generate a sub-topic split for the passed identifier
  auto sub = std::vector<std::string>();
  computeSettingsTopicStructure(sub, topic);
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
  auto k = this->topics;

  // update the buffered context for all subjects that were enabled
  // before.
  for (auto c: this->launchedTopics) {
    if (SUB_EQ(topic, c.first)) {
      const auto memSizePrior = c.second->memorySize;
      recomputeSettings(c.first, c.second, true);
      const auto memSizeAfter = c.second->memorySize;

      if (memSizePrior != memSizeAfter) {
        if (c.second->nextFreeIndex) // if contains content
          c.second->out->logTopic(c.second,
                                  (const char*) c.second->els, (size_t) c.second->nextFreeIndex);
        if (c.second->els) free(c.second->els);
        c.second->els = (char* ) malloc(c.second->memorySize);
        c.second->nextFreeIndex = 0;
      }
      assert(!(memSizeAfter != memorySize && topic == c.first));
    }
  }
}


slog::topic::Context const & slog::Topics::getBaseContext() const {
  return this->baseContext;
}


void slog::Topics::lock() {
  mtx.lock();
}


void slog::Topics::unlock() {
  mtx.unlock();
}
