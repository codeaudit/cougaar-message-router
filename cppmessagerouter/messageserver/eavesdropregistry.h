/***************************************************************************
                          eavesdropregistry.h  -  description
                             -------------------
    begin                : Wed May 12 2004
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

#ifndef EAVESDROPREGISTRY_H
#define EAVESDROPREGISTRY_H

#include <map.h>
#include "clientconnection.h"
#include <qmutex.h>
#include <list.h>
#include "message.h"

typedef list<ClientConnection*> ListEavesDroppers;
typedef map<string, ListEavesDroppers*> MapEavesDroppers; 



/**
  *@author David Craine
  */

class EavesDropRegistry {
public: 
	EavesDropRegistry();
	~EavesDropRegistry();
  /** No descriptions */
  void registerEavesDropper(string& target, ClientConnection* eavesdropper);
  void deregisterAllEavesDroppers(ClientConnection* eavesdropper);
  void registerGlobalEavesDropper(ClientConnection* eavesdropper);
  void deregisterEavesDropper(string& target, ClientConnection* eavesdropper);
  void removeTarget(string& target);
  void deregisterGlobalEavesDropper(ClientConnection* eavesdropper);
  /** No descriptions */
  void checkMessage(Message &msg);

 public:

 private:
  void sendMessage(string& to, Message& msg);

 private:
  QMutex mutex;
  MapEavesDroppers eavesDroppers;
  ListEavesDroppers globalEavesDroppers;
  
};

#endif
