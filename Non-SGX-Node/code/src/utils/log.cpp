#include "utils/log.hpp"

#ifdef NATIVE
#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>
#endif

std::mutex logMutex;

void logMessage (bool log, const char* file, int line, const char* message) {
#ifdef NATIVE
  if (log) {
    using namespace std::chrono;

    logMutex.lock();
    steady_clock::time_point p = steady_clock::now();
    milliseconds ms = duration_cast<milliseconds>(p.time_since_epoch());
    time_t t =  duration_cast<seconds>(ms).count();
    char tStr[25];
    strftime(tStr, 25, "%Y-%m-%d %H:%M:%S.", localtime(&t));

    std::cout << "[" << tStr << (ms.count() % 1000) << " " << 
      file << ":" << line << "] " << message << std::endl;
    logMutex.unlock();
  }
#endif
}