// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   10/29/18

#ifndef _NETWORK_LOGGER_
#define _NETWORK_LOGGER_
#ifdef FOUND_ZMQ
#define NET_IO_REALIZED

#include <outputHandler/OutputHandler.h>

#include <zmq.hpp>
#include <string>
#include <vector>

namespace slog {
  namespace outputHandler {
    class NetIo : public OutputHandler {

    public:

      NetIo(bool useNetwork=true, bool useCli=true, uint port=8000);

      /**
       * Socket is closed in the default socket destructor.
       */
      virtual ~NetIo() = default;

      /**
       * This function handles a way to deal with messages:
       * print to standard out / standard err.
       */
      virtual void handle( std::vector<std::pair<const char*, size_t>> sts,
                   LogLevel msgLogLevel) override;


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
