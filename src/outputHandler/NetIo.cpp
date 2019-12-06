// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   10/29/18

#ifdef FOUND_ZMQ

#include <outputHandler/NetIo.h>

#include <sstream>
#include <iterator>


slog::outputHandler::NetIo::NetIo(bool useSocket,
    bool useCli, uint port) : context(1), socket(context, ZMQ_PUB), 
    connection(std::string("tcp://*:") + std::to_string(port)) {
  this->useNetwork = useSocket;
  this->useCli = useCli;
  socket.bind(connection.c_str());
}


void slog::outputHandler::NetIo::handle(
    std::vector<std::pair<const char*, size_t>> sts, 
    LogLevel msgLogLevel) {

  if (useCli) {
    auto stream = msgLogLevel >= LogLevel::WARNING ? &std::cerr : &std::cout;
    for (auto st:sts) std::copy(st.first,st.first+st.second,
        std::ostream_iterator<unsigned char>(*stream));
    *stream << std::endl;
  }

  if (useNetwork) {
    size_t sz = 0;
    for (auto st:sts) sz += st.second;
    // send via network.
    zmq::message_t logMsg(sz);
    uint32_t idx = 0;
    for (auto k = sts.begin();
        k != sts.end(); idx+= k->second, k++)
      memcpy((char*)logMsg.data() + idx, k->first, k->second);
    assert(idx==sz);
    socket.send(logMsg);
  }
}


#endif
