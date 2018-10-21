# Logger
Simple extensible logging library in c++.


## Log data
Logging data is possble for any kind of data in a stream syntax.
```c++
using namespace log;

vector<const char*> vc = {"hello", "world"};
char* k = malloc(sizeof(*k)*10);

// log with Info message
Logger::getInstance() <<vc << "everything" << 3 << std::endl;
```


## LOG_LEVELS 
Different log_levels can be defined and used
```C++
Logger::getInstance(WARNING) << k << std::endl;
```

## Flexibility
The way the data is handled can be changed simply by inheriting the `OutputHanlder`, e.g. for sending the data over the network.
```cpp
namespace log {
  namespace outputHandler {
    class Stdio : public OutputHandler {

      public: 
        Stdio();

        /**
         * This function handles a way to deal with messages:
         * print to standard out / standard err.
         */
        virtual void handle(std::string st, LogLevel msgLogLevel) override;
        virtual ~Stdio();

    };
  }
}
#endif //_STDIO_H_
```
