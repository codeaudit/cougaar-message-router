/***************************************************************************
                          logger.h  -  description
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

#ifndef LOGGER_H
#define LOGGER_H

using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <qthread.h>
#include <list>
#include <qmutex.h>
#include <logentry.h>
#include <time.h>

/**
  *@author David Craine
  */

class Logger : public QThread {
public: 
	Logger();
	~Logger();
  void setLevel(int level);
  /** No descriptions */
  void log(string& msg, int level);
  /** No descriptions */
  void log(const char *, int level);
  void forceLog(const char *);
  void log(const char *subject, const char *msg, int level);
  void log(const char *from, const char* to, const char *subject, const char* msg, int level);
  /** Write property of bool enabled. */
  void enable();
  void disable();
  /** Read property of bool enabled. */
  const bool isenabled();
  virtual void run();
  void stop();
  void setLogFilePath(const char *);
  
public:  // Public attributes
  static const int LEVEL_INFO =1;
  static const int LEVEL_WARN = 2;
  static const int LEVEL_DEBUG = 3;
  static const int LEVEL_SHOUT = 4;

private:  //private methods
  string getLevelStr(int level) const;
  char * getCurrentTimeStr(char*, unsigned int len) const;
  struct tm* getCurrentTime(struct tm*);
  char * convertTimeToStr(struct tm , char *, unsigned int) const;
  void addLogEntry(LogEntry *);
  void writeLogEntry(LogEntry *);
    
private: // Private attributes
  /**  */
  bool enabled;
  bool keepRunning;
  list<LogEntry *> stack;
  QMutex listLock;
  int currentLevel;
  string LEVEL_INFO_STR;
  string LEVEL_WARN_STR;
  string LEVEL_DEBUG_STR;
  string LEVEL_SHOUT_STR;
  char *pLogFileName;
};

#endif
