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
  keepRunning = true;
  pLogFileName = "/var/log/messagerouter.log";
}

Logger::~Logger(){
}

void Logger::setLevel(int level) {
  this->currentLevel = level;
}

void Logger::writeLogEntry(LogEntry *entry) {
  char timebuffer[30];
  
  FILE *pFile = fopen(pLogFileName, "a");
  if (entry->subject == "") { //msg only format
    fprintf(pFile, "%s - %s: %s\n", convertTimeToStr(entry->timestamp, (char *)timebuffer, sizeof(timebuffer)),
           getLevelStr(entry->logLevel).c_str(),
           entry->msg.c_str());
  }
  else if (entry->to == "") { //subject and message format
     fprintf(pFile, "%s  - %s: %s : %s\n", convertTimeToStr(entry->timestamp, (char *)timebuffer, sizeof(timebuffer)),
           getLevelStr(entry->logLevel).c_str(),
           entry->subject.c_str(),
           entry->msg.c_str());
  }
  else {
     fprintf(pFile, "%s  - %s: from: %s - to: %s - %s : %s\n", convertTimeToStr(entry->timestamp, (char *)timebuffer, sizeof(timebuffer)),
           getLevelStr(entry->logLevel).c_str(),
           entry->from.c_str(),
           entry->to.c_str(),
           entry->subject.c_str(),
           entry->msg.c_str());
  }
  fclose(pFile);
}

void Logger::run() {
  while (keepRunning) {
    msleep(5000);
    listLock.lock();
    while (keepRunning && (stack.size() > 0)) {
      LogEntry *entry = stack.front();
      stack.pop_front();
      writeLogEntry(entry);
      delete entry;
    }
    listLock.unlock();
    
  }
}

void Logger::stop() {
  keepRunning = false;
}


/** No descriptions */
void Logger::log(string& msg, int level)  {
  log(msg.c_str(), level);
}

char * Logger::convertTimeToStr(struct tm time, char *str, unsigned int len) const {
  strftime(str, len, "%d-%b-%Y %H:%M:%S", &time);
  return str;
}

char * Logger::getCurrentTimeStr(char* str, unsigned int len) const {
  time_t now;
  struct tm *l_time;

  time(&now);
  l_time = localtime(&now);
  strftime(str, len, "%d-%b-%Y %H:%M:%S", l_time);
  return str;
}

struct tm* getCurrentTime(struct tm* l_time)  {
  time_t now;

  time(&now);
  l_time = localtime(&now);
  return l_time;
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
void Logger::forceLog(const char *msg) {
  //char str[30];
  //printf("%s - %s\n", getCurrentTimeStr((char *)str, sizeof(str)), msg);
  //cout << flush;
  addLogEntry(new LogEntry(LEVEL_DEBUG, msg));
}

/** No descriptions */
void Logger::log(const char *msg, int level) {
  if (!enabled) return;
  if (level < currentLevel) return;
  
  //char str[30];
  //printf("%s  - %s: %s\n", getCurrentTimeStr((char *)str, sizeof(str)), getLevelStr(level).c_str(), msg);
  //cout << flush;
  addLogEntry(new LogEntry(level, msg));
}

void Logger::log(const char  *subject, const char *msg, int level) {
  if (!enabled) return;
  if (level < currentLevel) return;

  //char str[30];
  //printf("%s  - %s: %s : %s\n", getCurrentTimeStr((char *)str, sizeof(str)), getLevelStr(level).c_str(), subject, msg);
  //cout << flush;
  addLogEntry(new LogEntry(level, subject, msg));
}

void Logger::log(const char *from, const char* to, const char *subject, const char* msg, int level) {
  if (!enabled) return;
  if (level < currentLevel) return;

  //char str[30];
  //printf("%s  - %s: from: %s - to: %s - %s : %s\n", getCurrentTimeStr((char *)str, sizeof(str)), getLevelStr(level).c_str(), from, to, subject, msg);
  //cout << flush;
  addLogEntry(new LogEntry(level, from, to, subject, msg));
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

void Logger::addLogEntry(LogEntry* entry) {
  listLock.lock();
  stack.push_back((LogEntry *)entry);
  listLock.unlock();
}
