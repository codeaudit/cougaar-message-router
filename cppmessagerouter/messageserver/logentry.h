/***************************************************************************
                          logentry.h  -  description
                             -------------------
    begin                : Tue Jun 15 2004
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

#ifndef LOGENTRY_H
#define LOGENTRY_H

using namespace std;

#include <string>
#include <time.h>

/**
  *@author David Craine
  */

class LogEntry {
public: 
	LogEntry(int logLevel, const char* msg);
  LogEntry(int logLevel, const char * subject, const char* msg);
  LogEntry(int logLevel, const char *from, const char *to, const char * subject, const char* msg);
	~LogEntry();

private:
  void init(int logLevel, const char *from, const char *to, const char * subject, const char* msg);
  
public:
  struct tm *timestamp;
  int logLevel;
  string from;
  string to;
  string subject;
  string msg;
};

#endif
