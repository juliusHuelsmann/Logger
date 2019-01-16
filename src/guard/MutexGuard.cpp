// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18


#include <Log.hpp>


slog::Log::MutexGuard::MutexGuard(slog::Log * l): l(l) {
  // the first MutexGuard has to lock one times more
  assert(l->mutexGuardCount
      < std::numeric_limits<decltype(l->mutexGuardCount)>::max());
  if (!l->mutexGuardCount++) {
    l->lock();
  }
}


slog::Log::MutexGuard::~MutexGuard() {
  l->unlock();
  assert(l->mutexGuardCount
      > std::numeric_limits<decltype(l->mutexGuardCount)>::min());
  if (!--l->mutexGuardCount) {
    if (!l->flushed) *this << std::endl;
    l->unlock();
  }
}



