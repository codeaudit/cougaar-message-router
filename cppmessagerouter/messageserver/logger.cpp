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
  pLogFileName = "/var/log/messagerouter.log";  //default path
  use_output_file = true;
}

Logger::~Logger(){
}

void Logger::setLevel(int level) {
  this->currentLevel = level;
}

void Logger::writeLogEntry(FILE *pFile, LogEntry *entry) {
  char timebuffer[30];
    
  if (entry == NULL) return;
  
  try {
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
  }
  catch (...) {
    cout << "Unknown error in writeLogEntry()" << endl << flush;
  }
  
  delete entry;
}

void Logger::run() {
  while (keepRunning) {
    try {
      msleep(5000);
      //cout << "Logger wakking up..." << endl << flush;
      //first copy any entries from the incoming stack to the outgoing stack
      //this way we can free the incoming stack so additional log entries can be
      //posted while this thread is busing writing the current postings to the
      //log file.  The copy should be quick since we're just moving pointers between
      //stacks

      //cout << "checking for logs entries" << endl << flush;
      incomingStackLock.lock(); //lock the incoming stack so new entries can't be added
      while (keepRunning && (incomingStack.size() > 0)) {
        LogEntry *entry = incomingStack.front();
        incomingStack.pop_front();
        outgoingStack.push_back(entry);
      }
      incomingStackLock.unlock(); //unlock the incoming stack so new entries can be added again

      if (outgoingStack.size() > 0) {
        FILE *pFile;
        if ((pFile = fopen(pLogFileName, "a")) != NULL) {
          while (keepRunning && (outgoingStack.size() > 0)) {
            LogEntry *entry = outgoingStack.front();
            outgoingStack.pop_front();
            writeLogEntry(pFile, entry);
          }
          fclose(pFile);
        }
        else {
          cout << "ALERT: Unable to open log file" << endl << flush;
        }
      }
      //cout << "Logger going back to sleep..." << endl << flush;
    }
    catch (...) {
      cout << "Unknown exception in Logger" << endl << flush;
      if (incomingStackLock.gotLock()) {
        incomingStackLock.unlock();
      }
    }
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
  if (use_output_file) {
    addLogEntry(new LogEntry(LEVEL_DEBUG, msg));
  }
  else {
    writeLogEntry(stdout, new LogEntry(LEVEL_DEBUG, msg));
    cout << flush;
  }
}

/** No descriptions */
void Logger::log(const char *msg, int level) {
  if (!enabled) return;
  if (level < currentLevel) return;
  
  if (use_output_file) {
    addLogEntry(new LogEntry(level, msg));
  }
  else {
    writeLogEntry(stdout, new LogEntry(level, msg));
    cout << flush;
  }
}

void Logger::log(const char  *subject, const char *msg, int level) {
  if (!enabled) return;
  if (level < currentLevel) return;

  if (use_output_file) {
    addLogEntry(new LogEntry(level, subject, msg));
  }
  else {
    writeLogEntry(stdout, new LogEntry(level, subject, msg));
    cout << flush;
  }
}

void Logger::log(const char *from, const char* to, const char *subject, const char* msg, int level) {
  if (!enabled) return;
  if (level < currentLevel) return;

  if (use_output_file) {
    addLogEntry(new LogEntry(level, from, to, subject, msg));
  }
  else {
    writeLogEntry(stdout, new LogEntry(level, from, to, subject, msg));
    cout << flush;
  }
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
  incomingStackLock.lock();
  incomingStack.push_back((LogEntry *)entry);
  incomingStackLock.unlock();
}

void Logger::setLogFilePath(const char *path) {
  pLogFileName = (char *)path;
}
