/***************************************************************************
                          logentry.cpp  -  description
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

#include "logentry.h"

LogEntry::LogEntry(int logLevel, const char * msg) {
  init(logLevel, "", "", "", msg);
}

LogEntry::LogEntry(int logLevel, const char* subject, const char* msg) {
  init(logLevel, "", "", subject, msg);
}

LogEntry::LogEntry(int logLevel, const char *from, const char *to, const char * subject, const char* msg) {
  init(logLevel, from, to, subject, msg); 
}

LogEntry::~LogEntry() {
}

void LogEntry::init(int logLevel, const char *from, const char *to, const char * subject, const char* msg) {
  time_t now;
  struct tm* l_time;
  time(&now);

  l_time = localtime(&now);
  this->timestamp = *l_time;
  this->logLevel = logLevel;
  this->from =  from;
  this->to = to;
  this->subject  = subject;
  this->msg = msg;
} 
