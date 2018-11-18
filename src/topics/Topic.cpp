// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18


#include <topics/Topic.h>

slog::topic::Topic::Topic(): sub(), s(nullptr) { }
slog::topic::Topic::~Topic() {
  if (s) delete(s);
}

