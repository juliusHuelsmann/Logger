#include <Logger.hpp>


slog::Logger::Logger(): Log(), Topics() {
  this->setBaseContext(this->out);
}


void slog::Logger::setStreamMethod(
    std::shared_ptr<outputHandler::OutputHandler> nOut) {
  // The pointer to the outputHandler has to be defined.
  // For disabling all logging use #setLogLevel.
  assert(nOut.get());

  Log::lock();
  Topics::lock();
  if (this->getBaseContext().out == this->out) this->setBaseContext(nOut);
  this->out = nOut;

  Topics::unlock();
  Log::unlock();
}


slog::Logger& slog::Logger::getInstance() {
  return getInstance(slog::LogLevel::INFO);
}


slog::Logger& slog::Logger::getInstance(slog::LogLevel e) {
  static slog::Logger instance;
  instance.logLevelCurrentMessage = e;
  return instance;
}
