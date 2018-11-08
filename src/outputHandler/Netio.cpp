// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   10/29/18

#ifdef FOUND_ZMQ

#include <outputHandler/Netio.h>

#include <iostream>
#include <sstream>

slog::outputHandler::Netio::Netio(bool useSocket, 
    bool useCli, uint port) : context(1), socket(context, ZMQ_PUB), 
    connection(std::string("tcp://*:") + std::to_string(port)) {
  this->useNetwork = useSocket;
  this->useCli = useCli;
  socket.bind(connection.c_str());
}


slog::outputHandler::Netio::~Netio() { 
  socket.close();
}


void slog::outputHandler::Netio::handle(
    const char* st, slog::LogLevel msgLogLevel, size_t length) {

  if (useCli) {
    auto stream = msgLogLevel >= slog::WARN ? &std::cerr : &std::cout;
    std::copy(st, st + length, std::ostream_iterator<unsigned char>(*stream));
    *stream << std::endl;
  }

  if (useNetwork) {
    // send via network.
    zmq::message_t logMsg(length);
    memcpy(logMsg.data(), st, length);
    socket.send(logMsg);
  }
}

#endif
