/***************************************************************************
                          clientconnectionfactory.cpp  -  description
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

#include "clientconnectionfactory.h"


ClientConnectionFactory::ClientConnectionFactory(){
  connectionMonitor = new ClientConnectionMonitor();
  connectionMonitor->start();
}

ClientConnectionFactory::~ClientConnectionFactory(){
}

/** No descriptions */                                                          
ClientConnection* ClientConnectionFactory::createClientConnection(ServerSocket *ss, bool blockRead){
  ClientConnection *cc = new ClientConnection(ss, blockRead);
  connectionMonitor->addClientConnection(cc);
  return cc;
}
