
// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   12/7/18


#include <topics/Context.h>
#include <outputHandler/FileLogger.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include <iostream>


typedef long long LogType;

slog::outputHandler::FileLogger::FileLogger(std::string rootDir): rootDir(rootDir), fileHandles{} {

    const boost::filesystem::path dir(rootDir);
    if(!(boost::filesystem::exists(dir)) && boost::filesystem::create_directories(dir)) {

        //std::cout << "created log output dir" << dir.generic_path().string() << std::end;
    }


}

slog::outputHandler::FileLogger::~FileLogger() {
  for (auto k = fileHandles.begin(); fileHandles.size(); k = fileHandles.erase(k))
      k->second.close();

}

void slog::outputHandler::FileLogger::handle(std::vector<std::pair<const char*, size_t>>,
                                             slog::LogLevel ) { }


void slog::outputHandler::FileLogger::logTopic(slog::topic::Context * topic,
                                                  const char* additionalData,
                                                  size_t additionalSize) {
  assert(topic->out.get() == this);
  assert(topic->typeSize == sizeof(LogType));
  if (!fileHandles.count(topic->topic)) {
    fileHandles.insert(std::make_pair(topic->topic, std::ofstream()));
    fileHandles[topic->topic].open(rootDir + topic->topic);
  }
  auto process = [&topic](const char* vector, size_t size, std::ofstream& handle) -> void {
      assert(size % topic->typeSize == 0);
      const auto amountOfEntries =  (uint64_t) (size / topic->typeSize);//rows*cols
      assert(amountOfEntries % topic->amount== 0);
      const auto amountLines = (uint64_t) amountOfEntries / topic->amount;
      uint64_t idx = 0;
      for (uint64_t  line = 0; line < amountLines; line++) {
        for (uint64_t col = 0; col < topic->amount; col++) {
          idx += topic->typeSize;
          handle << (col ? "\t" : "") <<   *((LogType*) &vector[idx]) << col;
        }
        handle << "\n";
      }
      assert(idx == size);
  };

  // if there is something in the buffer
  if (topic->nextFreeIndex) process(topic->els, topic->nextFreeIndex, fileHandles[topic->topic]);
  if (additionalSize) process(additionalData, additionalSize, fileHandles[topic->topic]);
  topic->nextFreeIndex = 0;

}



