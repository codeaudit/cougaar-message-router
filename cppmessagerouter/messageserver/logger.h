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
#include <string>

/**
  *@author David Craine
  */

class Logger {
public: 
	Logger();
	~Logger();
  void setLevel(int level);
  /** No descriptions */
  void log(string& msg, int level) const;
  /** No descriptions */
  void log(const char *, int level) const;
  void forceLog(const char *) const;
  void log(const char *subject, const char *msg, int level) const;
  void log(const char *from, const char* to, const char *subject, const char* msg, int level) const;
  /** Write property of bool enabled. */
  void enable();
  void disable();
  /** Read property of bool enabled. */
  const bool isenabled();
  
public:  // Public attributes
  static const int LEVEL_INFO =1;
  static const int LEVEL_WARN = 2;
  static const int LEVEL_DEBUG = 3;
  static const int LEVEL_SHOUT = 4;

private:  //private methods
  string getLevelStr(int level) const;
    
private: // Private attributes
  /**  */
  bool enabled;
  int currentLevel;
  string LEVEL_INFO_STR;
  string LEVEL_WARN_STR;
  string LEVEL_DEBUG_STR;
  string LEVEL_SHOUT_STR;
};

#endif
