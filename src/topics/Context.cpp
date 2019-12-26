// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18

#include <topics/Context.h>

slog::topic::Context::Context(unsigned char amount, unsigned char typeSize, char dataType,
                              std::string topic, std::string subTopic) noexcept
    : topic(std::move(topic)), subTopic(std::move(subTopic)), amount(amount), typeSize(typeSize), dataType(dataType) {}

slog::topic::Context::Context(std::shared_ptr<outputHandler::OutputHandler> out, uint memorySize,
                              std::string topicPrefix, std::string plotStyle,
                              uint64_t plotBufferSize) noexcept
    : out(std::move(out)), memorySize(memorySize), topicPrefix(std::move(topicPrefix)), plotStyle(std::move(plotStyle)),
      plotBufferSize(plotBufferSize) {}

slog::topic::Context& slog::topic::Context::operator=(Context const& d) {
  this->out = d.out;
  this->memorySize = d.memorySize;
  this->topicPrefix = d.topicPrefix;
  this->plotStyle = d.plotStyle;
  this->plotBufferSize= d.plotBufferSize;
  this->topicPrefix = d.topicPrefix;
  return *this;
}

slog::topic::Context::Context(const Context& d):
    els(nullptr), amount(0), typeSize(0), dataType(' ') {
  *this = d;
}

slog::topic::Context::~Context() {
  if (els) free(els);
}

/**
 *  Apply ancestor settings to a partially filled context item.
 */
void slog::topic::Context::apply(const Context& ancestor) {
  if (ancestor.out) this->out = ancestor.out;
  if (ancestor.memorySize) this->memorySize = ancestor.memorySize;
  if (!ancestor.topicPrefix.empty()) this->topicPrefix += ancestor.topicPrefix;
  if (!ancestor.plotStyle.empty()) this->plotStyle = ancestor.plotStyle;
  if (ancestor.plotBufferSize) this->plotBufferSize = ancestor.plotBufferSize;
}











