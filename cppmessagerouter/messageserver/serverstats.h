/***************************************************************************
                          serverstats.h  -  description
                             -------------------
    begin                : Tue May 25 2004
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

#ifndef SERVERSTATS_H
#define SERVERSTATS_H

using namespace std;

#include <qmutex.h>
#include <stdlib.h>
#include <string>
#include <time.h>

/**
  *@author David Craine
  */

class ServerStats {
public: 
	~ServerStats();
  
  void incrementIncomingMsgCount();
  void incrementOutgoingMsgCount();
  
  const unsigned long int& getIncomingMsgCount();
  /** Read property of unsigned long int outgoingMsgCount. */
  /** No descriptions */
  void resetStats();
  const unsigned long int& getOutgoingMsgCount();
  /** No descriptions */
  static ServerStats * getInstance();
  string& getStatsStr();

protected:
  ServerStats();
  
private: // Private attributes
  /**  */
  static ServerStats * currentInstance;
  
  unsigned long int incomingMsgCount;
  unsigned long int outgoingMsgCount;
  QMutex inLock;
  QMutex outLock;
  time_t startTime;
};

#endif
