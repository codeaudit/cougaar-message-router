/***************************************************************************
                          connectionregistry.cpp  -  description
                             -------------------
    begin                : Wed Jan 28 2004
    copyright            : (C) 2004 by David Craine
    email                : dcraine@infotether.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *                                                                         *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "connectionregistry.h"
#include "context.h"
#include <iostream.h>


ConnectionRegistry::ConnectionRegistry(){
}
ConnectionRegistry::~ConnectionRegistry(){
}

void ConnectionRegistry::registerConnection(ClientConnection *cc, string& name) {
   registryLock.lock();
   
   clientMap[name]=cc;
   
   registryLock.unlock();
}
 
ClientConnection * ConnectionRegistry::getConnection(const string& name) {
  registryLock.lock();
  if (clientMap.count(name) > 0) {  //make sure a connection exists
    ClientConnection* cc = clientMap[name];
    registryLock.unlock();
    return cc;
  }
  registryLock.unlock();
  return NULL;  
}

/** No descriptions */
void ConnectionRegistry::deregisterConnection(string &name){
  registryLock.lock();
  
  clientMap.erase(name);
  
  registryLock.unlock();
}

/** No descriptions */
string& ConnectionRegistry::listConnections() {
  string* ret = new string();

  if (!clientMap.empty()) {
    ConnectionMap::iterator pos;
    pos = clientMap.begin();
    while (pos != clientMap.end()) {
      if (clientMap[pos->first] != NULL) {
        *ret += pos->first + "\n";
        pos++;
      }
      else {
        Context::getInstance()->getLogger()->log("Found NULL connection", (pos->first).c_str(), Logger::LEVEL_WARN);
        clientMap.erase(pos++);
      }
    }
  }
   
  return *ret;
}

void ConnectionRegistry::validateConnections() {
  ConnectionMap::iterator pos;
  for (pos = clientMap.begin(); pos != clientMap.end(); ++pos) {
    //printf("scheduling validation for host: %s\n", (pos->first).c_str());
    //cout<<flush;
    ClientConnection* cc = clientMap[pos->first];
    Context::getInstance()->getConnectionValidator()->validateConnection(cc);
  }
}

bool ConnectionRegistry::checkForExistingConnection(const string& name) {
   if (clientMap.count(name) == 0) {
     return FALSE;
   }
   else {
     return TRUE;
   }
}

string& ConnectionRegistry::getSendQueueStats() {
  string* ret = new string();

  if (!clientMap.empty()) {
    ConnectionMap::iterator pos;
    pos = clientMap.begin();
    while (pos != clientMap.end()) {
      if (clientMap[pos->first] != NULL) {
        *ret += pos->first + " : ";
        string& stats = clientMap[pos->first]->getSendQueueStats();
        *ret += stats + "\n";
        delete &stats;
        pos++;
      }
      else {
        Context::getInstance()->getLogger()->log("Found NULL connection", (pos->first).c_str(), Logger::LEVEL_WARN);
        clientMap.erase(pos++);
      }
    }
  }

  return *ret;
}

void ConnectionRegistry::broadcastMessage(Message& msg) {
  if (!clientMap.empty()) {
    ConnectionMap::iterator pos;
    pos = clientMap.begin();
    while (pos != clientMap.end()) {
      if (clientMap[pos->first] != NULL) {
        Message *copy = new Message(msg);
        clientMap[pos->first]->sendMessage(*copy);
        pos++;
      }
      else {
        clientMap.erase(pos++);
      }
    }
  }
}

int ConnectionRegistry::getConnectionCount() {
  return clientMap.size();
}

string& ConnectionRegistry::getConnectionStatsStr() {
   string *ret = new string("\n");
   struct tm *l_time;
   char tmpBuffer[256];
   char timestr[30];
   unsigned long int uptime;
   unsigned long int days, hours, mins, secs;
   time_t now;

   time(&now);
 
   if (!clientMap.empty()) {
    ConnectionMap::iterator pos;
    pos = clientMap.begin();
    while (pos != clientMap.end()) {
      if (clientMap[pos->first] != NULL) {
         time_t startTime = clientMap[pos->first]->getStartTime();
         uptime = (unsigned long int) difftime(now, startTime);
         days = uptime/86400;
         hours = (uptime%86400)/3600;
         mins = (uptime%3600)/60;
         secs = uptime % 60;
         l_time = localtime(&startTime);
         strftime(timestr, sizeof timestr, "%d-%b-%Y %H:%M:%S", l_time);
         unsigned int incomingMsgCount = clientMap[pos->first]->getIncomingMsgCount();
         unsigned int outgoingMsgCount = clientMap[pos->first]->getOutgoingMsgCount();
         sprintf(tmpBuffer, "\nconnect time: %s\nduration: %d days, %d hours, %d mins, %d secs\nincoming msg count: %u\noutgoing msg count: %u",
          timestr, days, hours, mins, secs, incomingMsgCount, outgoingMsgCount);


         *ret += pos->first + " : ";
         *ret += tmpBuffer;
         *ret += "\n";
         pos++;
      }
      else {
        Context::getInstance()->getLogger()->log("Found NULL connection", (pos->first).c_str(), Logger::LEVEL_WARN);
        clientMap.erase(pos++);
      }
    }
  }
  return *ret;
}

string& ConnectionRegistry::getConnectionProfiles() {
  string *ret = new string("\n");

  if (!clientMap.empty()) {
    ConnectionMap::iterator pos;
    pos = clientMap.begin();
    while (pos != clientMap.end()) {
      if (clientMap[pos->first] != NULL) {
        *ret += pos->first + " : ";
        string& profile = clientMap[pos->first]->getRoutingProfileStr();
        *ret += profile;
        delete &profile; 
      }
      pos++;
    }
  }

  return *ret;
}
