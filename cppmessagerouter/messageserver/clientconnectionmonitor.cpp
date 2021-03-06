/***************************************************************************
                          clientconnectionmonitor.cpp  -  description
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

#include "clientconnectionmonitor.h"
#include "context.h"

ClientConnectionMonitor::ClientConnectionMonitor(){
}
ClientConnectionMonitor::~ClientConnectionMonitor(){
}
/** No descriptions */
void ClientConnectionMonitor::addClientConnection(ClientConnection* cc){
  if (cc != NULL) {
    monitorListLock.lock();
    monitorList.push_back(cc);
    monitorListLock.unlock();
  }
}

/** No descriptions */
void ClientConnectionMonitor::run(){
  keepRunning = true;
  while (keepRunning) {
    msleep(2000);
    //cout << "Connection monitor waking up..." << endl << flush;
      //loop though the monitornList and delete connection objects whose threads are terminiated
    try {
      monitorListLock.lock();
      if (!monitorList.empty()) {
        MonitorList::iterator pos;
        pos = monitorList.begin();
         do {
          if (*pos == NULL) {
            Context::getInstance()->getLogger()->log("NULL value detected in monitor list", Logger::LEVEL_WARN);
            pos = monitorList.erase(pos);
          }
          else if (((ClientConnection*)(*pos))->isClosed == true) {
            Context::getInstance()->getLogger()->log("Deleting Connection Object", ((ClientConnection*)(*pos))->name.c_str(), Logger::LEVEL_WARN);
            ClientConnection *cc = (ClientConnection *)(*pos);
            cc->wait(1000);  //make sure the thread is terminated
            pos = monitorList.erase(pos); //first remove the reference to the connection from this monitor list
            delete cc;  //now delete the ClientConnection object itself        
          }
          else {
            pos++;
          }
        } while (pos != monitorList.end());
      }
      monitorListLock.unlock();
    }
    catch (...) {
      Context::getInstance()->getLogger()->log("Unknown exception in ClientConnectionMonitor", "", Logger::LEVEL_DEBUG);
      if (monitorListLock.gotLock()) {
        monitorListLock.unlock();
      }
    }
  }
}
