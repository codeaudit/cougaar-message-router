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
}
Logger::~Logger(){
}
/** No descriptions */
void Logger::log(string& msg) const {
  log(msg.c_str());
}


//does not check the enabled flag
void Logger::forceLog(const char *msg) const {
  time_t now;
  struct tm *l_time;
  char str[30];

  time(&now);
  l_time = localtime(&now);
  strftime(str, sizeof str, "%d-%b-%Y %H:%M:%S", l_time);
  printf("%s - %s\n", str, msg);
  cout << flush;
}
   
/** No descriptions */
void Logger::log(const char *msg) const {
  if (!enabled) return;
  
  time_t now;
  struct tm *l_time;
  char str[30];

  time(&now);
  l_time = localtime(&now);
  strftime(str, sizeof str, "%d-%b-%Y %H:%M:%S", l_time);
  printf("%s  - %s\n", str, msg);
  cout << flush;
}

void Logger::log(const char  *subject, const char *msg) const {
  if (!enabled) return;

  time_t now;
  struct tm *l_time;
  char str[30];

  time(&now);
  l_time = localtime(&now);
  strftime(str, sizeof str, "%d-%b-%Y %H:%M:%S", l_time);
  printf("%s  - %s : %s\n", str, subject, msg);
  cout << flush;
}

void Logger::log(const char *from, const char* to, const char *subject, const char* msg) const {
  if (!enabled) return;

  time_t now;
  struct tm *l_time;
  char str[30];

  time(&now);
  l_time = localtime(&now);
  strftime(str, sizeof str, "%d-%b-%Y %H:%M:%S", l_time);
  printf("%s  - from: %s - to: %s - %s : %s\n", str, from, to, subject, msg);
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
