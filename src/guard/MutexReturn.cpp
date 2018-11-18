// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   11/16/18


#include <Topics.hpp>


MutexReturn::MutexReturn(slog::Topics* t): t(t) { }
MutexReturn::~MutexReturn() {
  t->mtx.unlock();
}
