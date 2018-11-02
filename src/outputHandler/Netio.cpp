// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   10/29/18

#ifdef FOUND_ZMQ

#include <outputHandler/Netio.h>

#include <iostream>
#include <sstream>
#include <cstring>

jlog::outputHandler::Netio::Netio(bool useSocket, 
    bool useCli, uint port) : context(1), socket(context, ZMQ_PUB), 
    connection(std::string("tcp://*:") + std::to_string(port)) {
  this->useNetwork = useSocket;
  this->useCli = useCli;
  socket.bind(connection.c_str());
}


jlog::outputHandler::Netio::~Netio() { 
  socket.close();
}


void jlog::outputHandler::Netio::handle(
    std::string st, jlog::LogLevel msgLogLevel) {

  if (useCli) {
    if (msgLogLevel >= jlog::WARN) std::cerr << st;
    else std::cout << st;
    std::cout << std::flush;
    std::cerr << std::flush;
  }

  if (useNetwork) {
    // send via network.
    auto sz = st.size();
    zmq::message_t logMsg(sz);
    memcpy(logMsg.data(), st.data(), sz);
    socket.send(logMsg);
  }
}

#endif
