/***************************************************************************
                          connectionregistry.h  -  description
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
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONNECTIONREGISTRY_H
#define CONNECTIONREGISTRY_H

#include "clientconnection.h"

/**
  *@author David Craine
  */

typedef map<string, ClientConnection*> ConnectionMap;

class ConnectionRegistry {
public: 
	ConnectionRegistry();
	~ConnectionRegistry();

  void registerConnection(ClientConnection *cc, string& name);
  bool checkForExistingConnection(const string& name);
  ClientConnection * getConnection(const string& name);
  /** No descriptions */
  void deregisterConnection(string &name);
  /** No descriptions */
  string& listConnections();
  void validateConnections();

private:
  ConnectionMap clientMap;
  QMutex registryLock;

};

#endif
