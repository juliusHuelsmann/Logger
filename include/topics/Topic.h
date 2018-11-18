
// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18
//

#ifndef LOGGER_TOPIC_H
#define LOGGER_TOPIC_H

#include <topics/Context.h>

namespace slog {
  namespace topic {
    struct Topic {
      Topic();
      std::unordered_map<std::string, Topic*> sub;
      Context* s;
    };
  }
}

#endif //LOGGER_TOPIC_H
