/***************************************************************************
                          logger.cpp  -  description
                             -------------------
    begin                : Tue Mar 9 2004
    copyright            : (C) 2004 by David Craine
    email                : dcraine@infotether.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "logger.h"
#include <time.h>
#include <string>
#include <iostream.h>
#include <stdlib.h>

Logger::Logger(){
  currentLevel = LEVEL_WARN;
  LEVEL_INFO_STR = "INFO";
  LEVEL_WARN_STR = "WARN";
  LEVEL_DEBUG_STR = "DEBUG";
  LEVEL_SHOUT_STR = "SHOUT";
}

Logger::~Logger(){
}

void Logger::setLevel(int level) {
  this->currentLevel = level;
}

/** No descriptions */
void Logger::log(string& msg, int level) const {
  log(msg.c_str(), level);
}

char * Logger::getCurrentTimeStr(char* str, unsigned int len) const {
  time_t now;
  struct tm *l_time;

  time(&now);
  l_time = localtime(&now);
  strftime(str, len, "%d-%b-%Y %H:%M:%S", l_time);
  return str;
}

string Logger::getLevelStr(int level) const {
  switch (level) {
    case LEVEL_INFO:
     return LEVEL_INFO_STR;
    case LEVEL_WARN:
      return LEVEL_WARN_STR;
    case LEVEL_DEBUG:
      return LEVEL_DEBUG_STR;
    case LEVEL_SHOUT:
      return LEVEL_SHOUT_STR;
    default:
      return "ERROR";
  }
}

//does not check the enabled flag
void Logger::forceLog(const char *msg) const {
  char str[30];
  printf("%s - %s\n", getCurrentTimeStr((char *)str, sizeof(str)), msg);
  cout << flush;
}
   
/** No descriptions */
void Logger::log(const char *msg, int level) const {
  if (!enabled) return;
  if (level < currentLevel) return;
  
  char str[30];

  printf("%s  - %s: %s\n", getCurrentTimeStr((char *)str, sizeof(str)), getLevelStr(level).c_str(), msg);
  cout << flush;
}

void Logger::log(const char  *subject, const char *msg, int level) const {
  if (!enabled) return;
  if (level < currentLevel) return;

  char str[30];
  printf("%s  - %s: %s : %s\n", getCurrentTimeStr((char *)str, sizeof(str)), getLevelStr(level).c_str(), subject, msg);
  cout << flush;
}

void Logger::log(const char *from, const char* to, const char *subject, const char* msg, int level) const {
  if (!enabled) return;
  if (level < currentLevel) return;

  char str[30];
  printf("%s  - %s: from: %s - to: %s - %s : %s\n", getCurrentTimeStr((char *)str, sizeof(str)), getLevelStr(level).c_str(), from, to, subject, msg);
  cout << flush;
}

const bool Logger::isenabled() {
	return enabled;
}

void Logger::enable() {
	enabled = true;
}

void Logger::disable() {
  enabled = false;
}
