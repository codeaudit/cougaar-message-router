/***************************************************************************
                          messagesender.cpp  -  description
                             -------------------
    begin                : Wed Feb 4 2004
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

#include "messagesender.h"
#include <list>
#include "message.h"
#include <iostream.h>
#include "SocketException.h"
#include "context.h"


int tmpcount=0;
MessageSender::MessageSender(ServerSocket* sock){
  this->ss = sock;
  this->name = "";;
  this->keepRunning = true;
  this->isStopped = false;
  maxSendQueueSize = Context::getInstance()->getMaxSendQueueSize();
}

MessageSender::~MessageSender(){
}

void MessageSender::setName(string& name) {
  this->name = name;
}

/** No descriptions */
void MessageSender::run() {
  try {
    while(keepRunning) {
      //cout << name << " message sender waking up..." << endl << flush;
      //copy items from incomingStack to sendStack
      sendStackLock.lock();  //lock the send stack so we can write to it
      incomingStackLock.lock();  //lock the incomingStack so messages can't be added
      while (incomingStack.size() > 0) {
        Message *msg = incomingStack.front();
        incomingStack.pop_front();
        if (msg != NULL) {
          sendStack.push_back(msg);
        }
      }
      incomingStackLock.unlock();  //release the incomingStack so messages can be added again
      
      while (keepRunning && (sendStack.size() > 0)) {
        Message *msg = sendStack.front();
        sendStack.pop_front();
        sendMessage(*msg);
      }
      sendStackLock.unlock();  //now we can release the sendStack
      //cout << name << " message sender going to sleep..." << endl << flush;
      
      msleep(500);
    }
  }
  catch(SocketException& ex) {
    keepRunning = FALSE;
    Context::getInstance()->getLogger()->log(name.c_str(), ex.description().c_str(), Logger::LEVEL_DEBUG);  
    if (sendStackLock.gotLock()) sendStackLock.unlock();
    if (incomingStackLock.gotLock()) incomingStackLock.unlock();
    stopLock.lock();
    if (!isStopped) {
      cleanupMessages();
      isStopped = TRUE;
    }
    stopLock.unlock();
  }
  catch(...) {
     keepRunning = FALSE;
     Context::getInstance()->getLogger()->log("Unknown error in MessageSender::run()", name.c_str(), Logger::LEVEL_DEBUG);
     if (sendStackLock.gotLock()) sendStackLock.unlock();
     if (incomingStackLock.gotLock()) incomingStackLock.unlock();
     stopLock.lock();
     if (!isStopped) {
       cleanupMessages();
       isStopped = TRUE;
     }
     stopLock.unlock();
  }
}

/** No descriptions */
void MessageSender::addMessage(Message& msg){
  if (!keepRunning) {  //if the message router has stopped running
    delete &msg;  //delete the message
    return;
  }

  incomingStackLock.lock();  //lock the incomging stack
  if (incomingStack.size() >= maxSendQueueSize) {
    incomingStackLock.unlock();
    Context::getInstance()->getLogger()->log("Max Send Queue size exceeded", name.c_str(), Logger::LEVEL_DEBUG);
    delete &msg;
    return;
  }

  incomingStack.push_back(&msg);  //push the message onto the stack
  incomingStackLock.unlock(); //unlock the incoming stack
}

/** No descriptions */
void MessageSender::sendMessage(Message& msg){
  sendLock.lock();
  try {
    *ss << msg.getMessageHeader() << msg.getMessageData();
    Context::getInstance()->getLogger()->log(msg.getfrom().c_str(), msg.getto().c_str(), "Sent Message", msg.getbody().c_str(), Logger::LEVEL_INFO);
  }
  catch (SocketException &se) {
    Context::getInstance()->getLogger()->log("ERROR", se.description().c_str(), Logger::LEVEL_WARN);
    delete &msg;
    sendLock.unlock();
    throw se;
  }
  catch (...) {
    Context::getInstance()->getLogger()->log("Unknown exception in sendMessage()", Logger::LEVEL_WARN);    
  }
  
  //cout << "deleting message" << endl << flush;
  delete &msg;
  //cout << "message deleted" << endl << flush;
  
  sendLock.unlock();
  
}

/** No descriptions */
void MessageSender::stop(){
  stopLock.lock();
  if (!isStopped) {
    keepRunning = FALSE;
    int tryCount = 0;    
    while (!this->finished() && (tryCount++ <= 100)) {
      msleep(200);
    }
    if (!this->finished()) {
      Context::getInstance()->getLogger()->log("Could not stop Message Sender", name.c_str(), Logger::LEVEL_DEBUG);
    }
    cleanupMessages();
    isStopped = TRUE;
  }
  stopLock.unlock();
}

/** No descriptions */
void MessageSender::cleanupMessages(){
  cleanupLock.lock();
  //cleanup the incoming stack
  try {
    incomingStackLock.lock();
    while (!incomingStack.empty()) {
       Message *msg = incomingStack.front();
       incomingStack.pop_front();
       if (msg != NULL) {
          delete msg;
      }
    }
    incomingStackLock.unlock();  //release the incomingStack
  
    //cleanup the sendStack
    sendStackLock.lock();
    while (!sendStack.empty()) {
      Message *msg = sendStack.front();
      sendStack.pop_front();
      if (msg != NULL) {
        delete msg;
      }      
    }
    sendStackLock.unlock();
  }
  catch (...)  {
    Context::getInstance()->getLogger()->log("Error in cleanupMessages()", Logger::LEVEL_WARN);
    if (incomingStackLock.gotLock()) incomingStackLock.unlock();
    if (sendStackLock.gotLock()) sendStackLock.unlock();
  }
  
  cleanupLock.unlock();

}

string& MessageSender::getStats() {
  char buffer[256];

  sprintf(buffer, "incoming queue length: %d\nsend queue length: %d",  incomingStack.size(), sendStack.size());
  return *(new string(buffer));
}
