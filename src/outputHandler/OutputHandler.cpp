
#include <misc/Parser.hpp>
#include <outputHandler/OutputHandler.h>
#include <topics/Context.h>

#include <cassert>
#include <sstream>

void slog::outputHandler::OutputHandler::handle(const char *st, LogLevel msgLogLevel) {
  handle(st,msgLogLevel, strlen(st));
}

void slog::outputHandler::OutputHandler::handle(const char *sts, LogLevel msgLogLevel, size_t len) {
  auto d = std::vector<std::pair<const char*,size_t>>();
  d.push_back(std::make_pair(sts, len));
  handle(d, msgLogLevel);
}

void slog::outputHandler::OutputHandler::logTopic(slog::topic::Context *topic,
                                                  const char *additionalData,
                                                  size_t additionalSize) {

  //YYY: don't recompute, store in context.
  assert(topic->out.get() == this);
  if (additionalSize || topic->nextFreeIndex) {

    //auto const size = 2 + topic->topic.size() + 1 + topic->subTopic.size() + 5 + 8;
    auto const size = sizeof(unsigned char) + 3 * sizeof(unsigned char)
        + topic->topic.size() + topic->subTopic.size() + topic->plotStyle.size()
        + sizeof(topic->amount) + sizeof(topic->typeSize)
        + sizeof(topic->dataType) + sizeof(topic->plotBufferSize);
    std::vector<char> vec (size);

    std::size_t index = 0;
    parser::set(&vec[0], static_cast<unsigned char>(2), index);

    parser::set(&vec[0], static_cast<unsigned char>(topic->topic.size()), index);
    parser::copy(&vec[0], topic->topic.c_str(), topic->topic.size(), index);

    parser::set(&vec[0], static_cast<unsigned char>(topic->subTopic.size()), index);
    parser::copy(&vec[0], topic->subTopic.c_str(), topic->subTopic.size(), index);
    
    parser::set(&vec[0], static_cast<unsigned char>(topic->plotStyle.size()), index);
    parser::copy(&vec[0], topic->plotStyle.c_str(), topic->plotStyle.size(), index);
    
    parser::set(&vec[0], topic->amount, index);
    parser::set(&vec[0], topic->typeSize, index);
    parser::set(&vec[0], topic->dataType, index);
    parser::set(&vec[0], topic->plotBufferSize, index);
    assert(size == index);

    // generate vector that conatains the stuff to be sent over the network.
    std::vector<std::pair<char const *, size_t>> vals{{&vec[0], size}};

    // If there is content that resides inside the buffer, add it to the vector
    if (topic->nextFreeIndex) { vals.emplace_back(topic->els, (size_t) topic->nextFreeIndex); }

    if (additionalSize) { vals.emplace_back(additionalData, additionalSize); }

    // write out and set the #nextFreeIndex to zero as all the values inside
    // the buffer have been written.
    handle(vals, LogLevel::INFO);
    topic->nextFreeIndex = 0;
  }
}
