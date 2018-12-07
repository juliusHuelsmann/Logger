
// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   12/7/18
//

#ifndef SENSENODE_FILELOGGER_H
#define SENSENODE_FILELOGGER_H

#include <outputHandler/OutputHandler.h>

#include <zmq.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

namespace slog {
  namespace outputHandler {
    class FileLogger : public OutputHandler {

    public:

      FileLogger(std::string rootDir="../../local/results/");
      virtual ~FileLogger();

      virtual void handle(
            std::vector<std::pair<const char*, size_t>> sts,
            slog::LogLevel msgLogLevel) override;


        virtual void logTopic(topic::Context * context, const char* additionalData=nullptr,
                              size_t additionalSize=0);

    private:
        /**
         * Root directory in which files with topic name will be placed.
         */
        const std::string rootDir;

        std::map<std::string, std::ofstream> fileHandles;

    };
  }
}

#endif //SENSENODE_FILELOGGER_H
