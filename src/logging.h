#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <sstream>

// blatantly copied from
// http://stackoverflow.com/questions/6168107/how-to-implement-a-good-debug-logging-feature-in-a-project

enum loglevel_e
  {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4};

class logIt
{
public:
  logIt(loglevel_e _loglevel = logERROR) {
    _buffer << _loglevel << " :"
            << std::string(
                _loglevel > logDEBUG
                ? (_loglevel - logDEBUG) * 2
                : 1
                , ' ');
  }

  template <typename T>
  logIt & operator<<(T const & value)
  {
    _buffer << value;
    return *this;
  }

  ~logIt()
  {
    _buffer << std::endl;
    // This is atomic according to the POSIX standard
    // http://www.gnu.org/s/libc/manual/html_node/Streams-and-Threads.html
    std::cerr << _buffer.str();
  }

private:
  std::ostringstream _buffer;
};


#define log(level) \
  if (level > logINFO) ; \
  else logIt(level)

#endif
