/***************************************************************************
                          serverstats.cpp  -  description
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

#include "serverstats.h"
#include <stdio.h>

ServerStats::ServerStats(){
  incomingMsgCount = 0;
  outgoingMsgCount = 0;
}

ServerStats::~ServerStats(){
}
/** Read property of unsigned long int incomingMsgCount. */
const unsigned long int& ServerStats::getIncomingMsgCount(){
	return incomingMsgCount;
}
/** Write property of unsigned long int incomingMsgCount. */
void ServerStats::incrementIncomingMsgCount(){
  inLock.lock();
	incomingMsgCount++;
  inLock.unlock();
}

/** Read property of unsigned long int outgoingMsgCount. */
const unsigned long int& ServerStats::getOutgoingMsgCount(){
	return outgoingMsgCount;
}

void ServerStats::incrementOutgoingMsgCount() {
  outLock.lock();
  outgoingMsgCount++;
  outLock.unlock();
}

/** No descriptions */
void ServerStats::resetStats(){
  outgoingMsgCount = 0;
  incomingMsgCount = 0;
}

/** No descriptions */
ServerStats * ServerStats::currentInstance = NULL;
ServerStats * ServerStats::getInstance(){
  if (currentInstance == NULL) {
    currentInstance = new ServerStats();
  }
  return currentInstance;
}

string& ServerStats::getStatsStr() {
  char tmpBuffer[128];
  
  sprintf(tmpBuffer, "incoming: %u\noutgoing: %u\n", incomingMsgCount, outgoingMsgCount);
  return *(new string(tmpBuffer));
}