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
 
ClientConnection * ConnectionRegistry::findConnection(const string& name) {
  if (clientMap.count(name) > 0) {  //make sure a connection exists
    ClientConnection* cc = clientMap[name];
    return cc;
  }
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
