/***************************************************************************
                          listenerregistry.cpp  -  description
                             -------------------
    begin                : Mon Feb 23 2004
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

#include "listenerregistry.h"
#include <list>

ListenerRegistry::ListenerRegistry(){
}

ListenerRegistry::~ListenerRegistry(){
}

void  ListenerRegistry::registerListener(ClientConnection* cc )  {
  listenerListLock.lock();
  listeners.push_back(cc);
  listenerListLock.unlock();
}

void ListenerRegistry::deregisterListener(ClientConnection* cc)  {
  listenerListLock.lock();
  listeners.remove(cc);
  listenerListLock.unlock();
}

/** No descriptions */
void ListenerRegistry::notifyListeners(Message &msg)  {
  list<ClientConnection *>::iterator iter;

  listenerListLock.lock();
  iter = listeners.begin();
  for (iter = listeners.begin(); iter != listeners.end(); iter++) { 
    ClientConnection *cc = (ClientConnection *)(*iter);
    if (cc != NULL) {
      //since a call to sendMessage will end up deleting the message
      //we need to make a copy of it before we send it to each recipient
      Message *tmpMsg = new Message(msg);
      cc->sendMessage(*tmpMsg);
    }
  }
  listenerListLock.unlock();
  //now we can delete the original message
  delete &msg;
}
