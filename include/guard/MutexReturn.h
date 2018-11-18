
// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18

#ifndef _MUTEXRETURN_H_
#define _MUTEXRETURN_H_

namespace slog { class Topics; }

struct MutexReturn {
public:
  explicit MutexReturn(slog::Topics* t);
  ~MutexReturn();
private:
  slog::Topics* t;
};

#endif //_MUTEXRETURN_H_
