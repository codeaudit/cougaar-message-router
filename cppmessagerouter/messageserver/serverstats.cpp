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
#include <iostream.h>

ServerStats::ServerStats(){
  incomingMsgCount = 0;
  outgoingMsgCount = 0;
  time(&startTime);
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
  char tmpBuffer[256];
  struct tm *l_time;
  char timestr[30];
  time_t now;
  unsigned long int uptime;
  unsigned long int days, hours, mins, secs;
  
  l_time = localtime(&startTime);
  time(&now);
  uptime = (unsigned long int) difftime(now, startTime);

  days = uptime/86400;
  hours = (uptime%86400)/3600;
  mins = (uptime%3600)/60;
  secs = uptime % 60;
  
  strftime(timestr, sizeof timestr, "%d-%b-%Y %H:%M:%S", l_time);  
  sprintf(tmpBuffer, "\nstart time: %s\nup time: %d days, %d hours, %d mins, %d secs\nincoming msg count: %u\noutgoing msg count: %u\n",
   timestr, days, hours, mins, secs, incomingMsgCount, outgoingMsgCount);

  return *(new string(tmpBuffer));
}