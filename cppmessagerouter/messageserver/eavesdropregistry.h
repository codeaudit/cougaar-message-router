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

#include <multimap.h>
#include "clientconnection.h"
#include <qmutex.h>

typedef multimap<string, ClientConnection*> MapEavesDroppers;


/**
  *@author David Craine
  */

class EavesDropRegistry {
public: 
	EavesDropRegistry();
	~EavesDropRegistry();
  /** No descriptions */
  void registerEavesDropper(string& target, ClientConnection& eavesdropper);
  void deregisterEavesDropper(ClientConnection& eavesdropper);
  void registerGlobalEavesDropper(ClientConnection& eavesdropper);
  void deregisterGlobalEavesDropper(ClientConnection& eavesdropper);

 public:
  MapEavesDroppers eavesDroppers;

 private:
  QMutex mutex;
};

#endif
