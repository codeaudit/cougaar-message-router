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
#include "context.h"
#include <stdio.h>
#include <iostream.h>

ServerStats::ServerStats(){
  raw = avg1 = avg2 = avg3 = max = 0.0;
  incomingMsgCount = 0;
  outgoingMsgCount = 0;
  prevCount = 0;
  for (int i=0; i<MAX_INTERVALS; i++) { diffQueue[i] = 0; }
  cycleCount = 0;
  keepRunning = TRUE;
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
    currentInstance->start();
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

  int connectionCount = Context::getInstance()->getconnectionRegistry()->getConnectionCount();
  strftime(timestr, sizeof timestr, "%d-%b-%Y %H:%M:%S", l_time);  
  sprintf(tmpBuffer, "\nstart time: %s\nup time: %d days, %d hours, %d mins, %d secs\nincoming msg count: %u\noutgoing msg count: %u\nconnection count: %d\nraw: %g\n1min: %g\n5min: %g\n15min: %g\nmax: %g\n",
   timestr, days, hours, mins, secs, incomingMsgCount, outgoingMsgCount, connectionCount, raw, avg1, avg2, avg3, max);

  return *(new string(tmpBuffer));
}


void ServerStats::run() {
  unsigned long int sum = 0;
  unsigned long int intervalCount = 0;

  //cout << "server stats starting" << endl << flush;
  while (keepRunning) {
    msleep(SLEEP_INTERVAL);
    //cout << "Server stats waking up..." <<  endl << flush;

    diffQueue[cycleCount] = incomingMsgCount - prevCount;
    prevCount = incomingMsgCount;

    //calculate raw rate
    raw = (double)diffQueue[cycleCount]/(double)(SLEEP_INTERVAL/1000);
    max = raw>max?raw:max;
                
    //calculate 1 minute avg
    sum = 0;
    intervalCount = (unsigned long int) (60000/SLEEP_INTERVAL);
    for (int i=0; i<intervalCount; i++) {
      sum += diffQueue[(cycleCount + MAX_INTERVALS - i)%MAX_INTERVALS];
    }
    avg1 = (double)sum/60;  //msgs per sec over last minute

    //calculate 5 minute avg
    sum = 0;
    intervalCount = (unsigned long int) (300000/SLEEP_INTERVAL);
    for (int i=0; i<intervalCount; i++) {
      sum += diffQueue[(cycleCount + MAX_INTERVALS - i)%MAX_INTERVALS];
    }
    avg2 = (double)sum/300;  

    //calculate the total minute avg
    sum = 0;
    for (int i=0; i<MAX_INTERVALS; i++) { sum += diffQueue[i]; }
    avg3 = (double)sum/((SLEEP_INTERVAL*MAX_INTERVALS)/1000);

    //printf("cycle: %d  avg1: %f  avg2: %f  avg3: %f  max: %f\n", cycleCount, avg1, avg2, avg3, max);
    cycleCount = (cycleCount + 1) % MAX_INTERVALS;  //adjust the cycle count
    //cout << "Server stats going back to sleep..." << endl << flush;

    //check to see if it's time to dump the stats
    if (!cycleCount)
      logStats();

  }
}

void ServerStats::stop() {
  keepRunning = false;
}

void ServerStats::logStats() {
  //log the server stats
  string& str = getStatsStr();
  Context::getInstance()->getLogger()->log("Server Stats", str.c_str(), Logger::LEVEL_DEBUG);
  delete &str;
  //log the connection stats
  str = Context::getInstance()->getconnectionRegistry()->getConnectionStatsStr();
  Context::getInstance()->getLogger()->log("Connection Stats", str.c_str(), Logger::LEVEL_DEBUG);
  delete &str;
  //log the send queue stats
  str = Context::getInstance()->getconnectionRegistry()->getSendQueueStats();
  Context::getInstance()->getLogger()->log("Send Queue Stats", str.c_str(), Logger::LEVEL_DEBUG);
  delete &str;
  //log the connection profile stats
  str = Context::getInstance()->getconnectionRegistry()->getConnectionProfiles();
  Context::getInstance()->getLogger()->log("Connection Profiles", str.c_str(), Logger::LEVEL_DEBUG);
  delete &str;
}


