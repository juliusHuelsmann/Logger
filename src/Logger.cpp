
#include <Logger.hpp>


slog::topic::Topic slog::Logger::topics = slog::topic::Topic();
slog::topic::Context slog::Logger::baseContext = slog::topic::Context();
std::unordered_map<std::string, slog::topic::Context*> slog::Logger::launchedTopics =
std::unordered_map<std::string, slog::topic::Context*>();
std::unordered_map<std::string, bool> slog::Logger::disabledTopics =
std::unordered_map<std::string, bool>();

uint slog::Logger::Mut::cntr = 0;
std::unordered_map<std::string, char> slog::Logger::types = {
  {"char",'c'}, {"signed char", 'b'}, {"unsigned char", 'B'}, {"bool", '?'},
  {"short", 'h'}, {"unsigned short", 'H'},
  {"int", 'i'}, {"unsigned int", 'I'},
  {"long", 'l'}, {"unsigned long", 'L'},
  {"long long", '1'}, {"unsigned long long", 'Q'},
  {"float", 'f'}, {"double", 'd'}};


