#ifndef _LOG_CONST_H_
#define _LOG_CONST_H_


//
// Identifiers used in the front end for plotting.
#define LINE_PLOT "scatter"
#define HISTOGRAM "histogram"
#define SCATTER "points"
#define INTERVAL "interval"


namespace slog {

  /*
   * Log levels
   */
  enum LogLevel {
    DEBUG=0,
    INFO=1,
    WARN=2,
    ERROR=3,
    FATAL=4,
    NEVER=5
  };
}


#endif // _LOG_CONST_H_

