#ifndef _SLOG_CONST_H_
#define _SLOG_CONST_H_

#include <iostream>

//
// Identifiers used in the front end for plotting.
#define LINE_PLOT "scatter"
#define HISTOGRAM "histogram"
#define SCATTER "points"
#define INTERVAL "interval"

typedef std::ostream& (*ManipFn)(std::ostream&);
typedef std::ios_base& (*FlagsFn)(std::ios_base&);

//#define SLOG(a) GLOG((unsigned int)static_cast<unsigned int>(a)/2)


/*
 * Log levels
 */
enum class LogLevel : unsigned int {
  DEBUG,
  INFO,
  INFO_RARE,
  WARNING,
  ERROR,
  SEVERE,
  FATAL,
  NEVER
};


#endif // _SLOG_CONST_H_

