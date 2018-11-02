// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   10/29/18

#ifndef _NETWORK_LOGGER_
#define _NETWORK_LOGGER_
#ifdef FOUND_ZMQ

#include <outputHandler/OutputHandler.h>

#include <zmq.hpp>
#include <string>

namespace jlog {
  namespace outputHandler {
    class Netio : public OutputHandler {

    public:
      Netio(bool useNetwork=true, bool useCli=true, uint port=8000);
      Netio();

      /**
       * This function handles a way to deal with messages:
       * print to standard out / standard err.
       */
      virtual void handle(std::string st, LogLevel msgLogLevel) override;
      virtual ~Netio();

    private:
      zmq::context_t context;
      zmq::socket_t socket;
      const std::string connection;

      bool useNetwork;
      bool useCli;

    };
  }
}

#endif
#endif //_NETWORK_LOGGER_
