/***************************************************************************
                          clientconnectionmonitor.h  -  description
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

#ifndef CLIENTCONNECTIONMONITOR_H
#define CLIENTCONNECTIONMONITOR_H

#include <qthread.h>
#include <qmutex.h>
#include <list>
#include "clientconnection.h"

/**
  *@author David Craine
  The point of this class is to monitor Client Connection objects to see when their thread
  has been terminated.  When a thread is terminated this class will then delete that Client
  Connection object.
  */

typedef list<ClientConnection *> MonitorList;  

class ClientConnectionMonitor : public QThread  {
public: 
	ClientConnectionMonitor();
	~ClientConnectionMonitor();
  /** No descriptions */
  void run();
  /**  */
  bool keepRunning;
  /** No descriptions */
  void addClientConnection(ClientConnection*);

  MonitorList monitorList;
  QMutex monitorListLock;
};

#endif
