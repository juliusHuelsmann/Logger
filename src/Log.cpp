// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/15/18

#include <Log.hpp>
#include <outputHandler/StdIo.h>
#include <boost/algorithm/string.hpp>


void slog::Log::setExitLevel(LogLevel ll) {
  this->exitLevel = ll; //< is used in an atomic way; hence no mutex.
}


void slog::Log::setLogLevel(LogLevel ll) {
  this->logLevel = ll;  //< is used in an atomic way; hence no mutex.
}


slog::Log::Log(): logLevelCurrentMessage(LogLevel::INFO), out(new outputHandler::StdIo()),
  logLevel(LogLevel::INFO), exitLevel(LogLevel::ERROR), startString(""), sstream(), msg(0),
  flushed(true), mutexGuardCount(0), mtx() {
    this->msg = 0;
}


void slog::Log::lock() {
  mtx.lock();
}


void slog::Log::unlock() {
  mtx.unlock();
}


void slog::Log::start() {
  switch (this->logLevelCurrentMessage) {
    case LogLevel::DEBUG:
      sstream << "DEBUG";
      break;
    case LogLevel::INFO:
      sstream << "INFO ";
      break;
    case LogLevel::INFO_RARE:
      sstream << "INFO ";
      break;
    case LogLevel::WARNING:
      sstream << "WARN ";
      break;
    case LogLevel::ERROR:
      sstream << "ERROR";
      break;
    case LogLevel::FATAL:
      sstream << "FATAL";
      break;
    case LogLevel::NEVER:
    default:
      sstream << "Fatal error occurred while logging a message: "
        << " Log level of message corrupted : "
        << static_cast<unsigned int>(this->logLevelCurrentMessage)
        << "\n\nOriginal message:\n";

      // ensure to exit
      this->logLevelCurrentMessage  = LogLevel::NEVER;
  }
  //prepend current time.
  auto now = std::time(nullptr);
  auto local = std::localtime(&now);
  this->msg = (this->msg + 1) % 100;
  sstream <<  "["
    << local->tm_mday << "." << (local->tm_mon + 1) << ' '
    << local->tm_hour << ":" << local->tm_min << ":" << local->tm_sec
    << "." << (msg < 10 ? " " : "")  << msg << "] ";
  this->startString = std::string(sstream.str().size(),' ');
}


void slog::Log::end() {

  // Check if the message is to be logged.
  if (this->logLevelCurrentMessage >= this->logLevel) {

    auto s = sstream.str();
    boost::replace_all(s.erase(s.size()-1),"\n","\n" + startString);

    out->handle(s.c_str(), logLevelCurrentMessage);

    // Check if the program has to exit according to the log level
    if (this->logLevelCurrentMessage >= this->exitLevel) {

      std::stringstream error;
      error << "\n The logger-exit level is set to exit on messages of level "
        << static_cast<unsigned int >(this->exitLevel) << " or higher. \n Bye!\n";
      out->handle(error.str().c_str(), LogLevel::INFO);
      std::exit(-1);
    }
  }

  // reset the standard log level to INFO (in case no log level is explicitely
  // provided) and clear + empty the string stream
  sstream.str( std::string() );
  sstream.clear();
  this->logLevelCurrentMessage = LogLevel::INFO;
}
