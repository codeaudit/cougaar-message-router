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
   clientMap[name]=cc;
}
 
ClientConnection * ConnectionRegistry::findConnection(const string& name) {
  ClientConnection* cc = clientMap[name];
  return cc;
  
}

/** No descriptions */
void ConnectionRegistry::deregisterConnection(string &name){
  clientMap.erase(name);
}

/** No descriptions */
string& ConnectionRegistry::listConnections() {
  string* ret = new string();
  
  ConnectionMap::iterator pos;
  for (pos = clientMap.begin(); pos != clientMap.end(); ++pos) {
    if (clientMap[pos->first] != NULL) {
      *ret += pos->first + "\n";
    }
    else {
      Context::getInstance()->getLogger()->log("Found NULL connection", (pos->first).c_str());
      clientMap.erase(pos);
    }
  }
   
  return *ret;
}

void ConnectionRegistry::validateConnections() {
  ConnectionMap::iterator pos;
  for (pos = clientMap.begin(); pos != clientMap.end(); ++pos) {
    printf("scheduling validation for host: %s\n", (pos->first).c_str());
    cout<<flush;
    ClientConnection* cc = clientMap[pos->first];
    Context::getInstance()->getConnectionValidator()->validateConnection(cc);
  }
}
