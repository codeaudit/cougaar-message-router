/***************************************************************************
                          eavesdropregistry.cpp  -  description
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

#include "eavesdropregistry.h"

EavesDropRegistry::EavesDropRegistry(){
}
EavesDropRegistry::~EavesDropRegistry() {
}
/** No descriptions */
void EavesDropRegistry::registerEavesDropper(string& target, ClientConnection* eavesdropper){
  mutex.lock();
  ListEavesDroppers *targetEavesDroppers = NULL;
  if (eavesDroppers.count(target) == 0) {  //if a list currently does not exist
    targetEavesDroppers = new ListEavesDroppers();  //create the new list
    eavesDroppers[target] = targetEavesDroppers;    //add the list to the map
  }
  else {
    targetEavesDroppers = eavesDroppers[target];
  }
  targetEavesDroppers->push_back(eavesdropper);
  mutex.unlock();
}

void EavesDropRegistry::removeTarget(string& target) {
  mutex.lock();
  if (eavesDroppers.count(target) > 0) {
    ListEavesDroppers *targetEavesDroppers = eavesDroppers[target];
    eavesDroppers.erase(target);
    delete targetEavesDroppers;
  }
  mutex.unlock();
}

void EavesDropRegistry::deregisterAllEavesDroppers(ClientConnection* eavesdropper) {
  //attempt to remove the designated eaves dropper from every list since we don't
  //know how many lists it is in
  mutex.lock();
  if (!eavesDroppers.empty()) {
    MapEavesDroppers::iterator pos;
    pos = eavesDroppers.begin();
    while (pos != eavesDroppers.end()) {  //iterate over all the lists of eaves droppers
      if (eavesDroppers[pos->first] != NULL) {
        eavesDroppers[pos->first]->remove(eavesdropper);   //remove the eavesdropper from the list     
      }
      pos++;
    }
  }
  mutex.unlock();
}

void EavesDropRegistry::deregisterEavesDropper(string& target, ClientConnection* eavesdropper) {
  mutex.lock();
  if (eavesDroppers.count(target) > 0) {
    ListEavesDroppers* led = eavesDroppers[target];
    if (led != NULL) {  
      led->remove(eavesdropper);   //remove the eavesdropper from the list
    }
  }
  mutex.unlock();
}

void EavesDropRegistry::registerGlobalEavesDropper(ClientConnection* eavesdropper) {
  mutex.lock();

  globalEavesDroppers.push_back(eavesdropper);

  mutex.unlock();
}

void EavesDropRegistry::deregisterGlobalEavesDropper(ClientConnection* eavesdropper) {
  mutex.lock();

  globalEavesDroppers.remove(eavesdropper);

  mutex.unlock();
}

void EavesDropRegistry::sendLocalMessage(string& to, Message &msg) {
  //check the local eaves droppers
  if (eavesDroppers.count(to) > 0) {
    ListEavesDroppers *ed = eavesDroppers[to];
    ListEavesDroppers::iterator pos;
    pos = ed->begin();
    while (pos != ed->end()) {
      if (*pos != NULL) {
        Message *outmsg = new Message(msg);
        ((ClientConnection *)(*pos))->sendMessage(*outmsg);
      }
      pos++;
    }
  }
}

void EavesDropRegistry::sendGlobalMessage(Message &msg) {
  //check the global eaves droppers
  if (!globalEavesDroppers.empty()) {
    ListEavesDroppers::iterator pos;
    pos = globalEavesDroppers.begin();
    while (pos != globalEavesDroppers.end()) {
      if (*pos != NULL) {
        Message *outmsg = new Message(msg);
        ((ClientConnection *)(*pos))->sendMessage(*outmsg);
      }
      pos++;
    }
  }
}

/** No descriptions */
void EavesDropRegistry::checkMessage(Message &msg) {
  mutex.lock();
  //check local eaves droppers for the from address
  sendLocalMessage(msg.getfrom(), msg);
  //check local eaves droppers for the to address
  sendLocalMessage(msg.getto(), msg);
  //check global eaves droppers
  sendGlobalMessage(msg);

  mutex.unlock();
}
