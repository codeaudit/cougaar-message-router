/***************************************************************************
                          listenerregistry.h  -  description
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

#ifndef LISTENERREGISTRY_H
#define LISTENERREGISTRY_H

#include "clientconnection.h"

/**
  *@author David Craine
  */
typedef list<ClientConnection *> ListenerList;
  
class ListenerRegistry {
public: 
	ListenerRegistry();
	~ListenerRegistry();
  /** No descriptions */
  //void registerListener(ClientConnection *);
  void registerListener(ClientConnection *);
  /** No descriptions */
  //void deregisterListener(ClientConnection *);
  void deregisterListener(ClientConnection *);
  /** No descriptions */
  void notifyListeners(Message &);

private:
  ListenerList listeners;
};

#endif
