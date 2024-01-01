#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>

#define LOG_MODE 1

#if LOG_MODE == 1
  #define print_log_ln(X) Serial.println(X)
  #define print_log(X) Serial.print(X)
#else
  #define print_log_ln(X) 
  #define print_log(X)
#endif

#endif 