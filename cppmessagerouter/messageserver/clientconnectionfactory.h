/***************************************************************************
                          clientconnectionfactory.h  -  description
                             -------------------
    begin                : Tue Apr 13 2004
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

#ifndef CLIENTCONNECTIONFACTORY_H
#define CLIENTCONNECTIONFACTORY_H

#include "clientconnection.h"
#include "clientconnectionmonitor.h"
#include "ServerSocket.h"

/**
  *@author David Craine
  */

class ClientConnectionFactory {
public: 
	ClientConnectionFactory();
	~ClientConnectionFactory();
  /** No descriptions */
  ClientConnection* createClientConnection(ServerSocket *ss, bool blockRead);
public: // Public attributes
  /**  */
  ClientConnectionMonitor * connectionMonitor;
};

#endif
