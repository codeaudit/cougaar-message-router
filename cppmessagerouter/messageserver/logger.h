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
  /** No descriptions */
  void log(string& msg) const;
  /** No descriptions */
  void log(const char *) const;
  void log(const char *subject, const char *msg) const;
  void log(const char *from, const char* to, const char *subject, const char* msg) const;
  /** Write property of bool enabled. */
  void enable();
  void disable();
  /** Read property of bool enabled. */
  const bool isenabled();
private: // Private attributes
  /**  */
  bool enabled;
};

#endif
