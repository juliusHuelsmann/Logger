#ifndef _LOG_CONST_H_
#define _LOG_CONST_H_

#include <iostream>

//
// Identifiers used in the front end for plotting.
#define LINE_PLOT "scatter"
#define HISTOGRAM "histogram"
#define SCATTER "points"
#define INTERVAL "interval"

typedef std::ostream& (*ManipFn)(std::ostream&);
typedef std::ios_base& (*FlagsFn)(std::ios_base&);

namespace slog {


  /*
   * Log levels
   */
  enum class LogLevel : unsigned int {
    DEBUG,
    INFO,
    INFO_RARE,
    WARN,
    ERROR,
    FATAL,
    NEVER
  };
}


#endif // _LOG_CONST_H_

