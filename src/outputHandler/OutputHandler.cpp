
#include <topics/Context.h>
#include <outputHandler/OutputHandler.h>
#include <cassert>
#include <sstream>


void slog::outputHandler::OutputHandler::handle(const char* st, LogLevel msgLogLevel) {
  handle(st,msgLogLevel, strlen(st));
}

void slog::outputHandler::OutputHandler::handle(const char* sts, LogLevel msgLogLevel, size_t len) {
  auto d = std::vector<std::pair<const char*,size_t>>();
  d.push_back(std::make_pair(sts, len));
  handle(d, msgLogLevel);
}

void slog::outputHandler::OutputHandler::logTopic(
      slog::topic::Context * topic, const char* additionalData, size_t additionalSize) {

  assert(topic->out.get() == this);
  if (additionalSize || topic->nextFreeIndex) {

    // Generate settings prefix.
    auto settings = std::stringstream ();
    settings << (unsigned char) 2
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

    // generate vector that conatains the stuff to be sent over the network.
    auto str = settings.str();
    auto vals = std::vector<std::pair<const char*, size_t>>();
    vals.push_back(std::make_pair(str.c_str(), str.size()));

    // If there is content that resides inside the buffer, add it to the vector
    if (topic->nextFreeIndex) vals.push_back(std::make_pair(
            topic->els, (size_t) topic->nextFreeIndex));

    if (additionalSize)
      vals.push_back(std::make_pair(additionalData, additionalSize));

    // write out and set the #nextFreeIndex to zero as all the values inside
    // the buffer have been written.
    handle(vals, INFO);
    topic->nextFreeIndex = 0;
  }
}
