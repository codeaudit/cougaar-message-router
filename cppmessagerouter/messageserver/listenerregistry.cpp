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

typedef list<ClientConnection*> ListenerList;
ListenerList listeners;

ListenerRegistry::ListenerRegistry(){
}

ListenerRegistry::~ListenerRegistry(){
}

/** No descriptions */
/*void ListenerRegistry::registerListener(ClientConnection *cc ){
  listeners.push_back(cc);
} */

void  ListenerRegistry::registerListener(ClientConnection* const cc ) const {
  listeners.push_back(cc);
}

/** No descriptions */
/*void ListenerRegistry::deregisterListener(ClientConnection *cc){
  listeners.remove(cc);
}*/

void ListenerRegistry::deregisterListener(ClientConnection * const cc) const {
  listeners.remove(cc);
}

/** No descriptions */
void ListenerRegistry::notifyListeners(Message &msg) const {
  list<ClientConnection *>::iterator iter;

  iter = listeners.begin();
  for (iter = listeners.begin(); iter != listeners.end(); iter++) {
    //since a call to sendMessage will end up deleting the message
    //we need to make a copy of it before we send it to each recipient
    Message *tmpMsg = new Message(msg);
    ((ClientConnection *)*iter)->sendMessage(*tmpMsg);
  }
  //now we can delete the original message
  delete &msg;
}
