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
}

MessageSender::~MessageSender(){
}

void MessageSender::setName(string& name) {
  this->name = name;
}

/** No descriptions */
void MessageSender::run() {
  bool gotLock;
  int tryCount;
  
  try {
    while(keepRunning) {
      gotLock = FALSE;
      tryCount = 0;

      while (!(gotLock = stackLock.tryLock()) && (tryCount++ < 10)) {  //if we can't get a lock on the stack
        msleep(250);  //sleep and try again
      }
      if (!gotLock) {  //log the fact that we couldn't get the lock
        Context::getInstance()->getLogger()->log("run() unable to get lock on sender", name.c_str(), Logger::LEVEL_DEBUG);
      }
      while (keepRunning && (stack.size() > 0)) {
        Message *msg = stack.front();
        stack.pop_front();
        sendMessage(*msg);
      }
      if (gotLock) {
        stackLock.unlock();
      }
      msleep(200);
    }
  }
  catch(SocketException& ex) {
    keepRunning = FALSE;
    if (gotLock) {
      stackLock.unlock();
    }
    Context::getInstance()->getLogger()->log(name.c_str(), ex.description().c_str(), Logger::LEVEL_DEBUG);  
    stopLock.lock();
    if (!isStopped) {
      cleanupMessages();
      isStopped = TRUE;
    }
  }
  catch(...) {
     keepRunning = FALSE;
     if (gotLock) {
       stackLock.unlock();
     }
     Context::getInstance()->getLogger()->log("Unknown error in MessageSender::run()", name.c_str(), Logger::LEVEL_DEBUG);
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
  int tryCount = 0;
  bool gotLock = FALSE;
  
  if (!keepRunning) {  //if the message router has stopped running
    delete &msg;  //delete the message
    return;
  }
  while (!(gotLock = stackLock.tryLock()) && (tryCount++ < 10)) {  //if we can't get a lock on the stack
    msleep(250);  //sleep and try again  
  }

  if (!gotLock) {  //log the fact that we couldn't get the lock
    Context::getInstance()->getLogger()->log("addMessage() unable to get lock on sender", name.c_str(), Logger::LEVEL_DEBUG);
  }

  stack.push_back(&msg);  //push the message o the stack even if we didn't get the lock

  if (gotLock) {    //only release the lock if we actually acquired it
    stackLock.unlock();
  }
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

  delete &msg;
  sendLock.unlock();
}

/** No descriptions */
void MessageSender::stop(){
  stopLock.lock();
  //Context::getInstance()->getLogger()->log("close: got stop lock", Logger::LEVEL_WARN);
  if (!isStopped) {
    //Context::getInstance()->getLogger()->log("close:stopping", Logger::LEVEL_WARN);
    keepRunning = FALSE;
    while (!this->finished()) {
      msleep(500);
    }
    //Context::getInstance()->getLogger()->log("close: finished", Logger::LEVEL_WARN);
    cleanupMessages();
    //Context::getInstance()->getLogger()->log("close: cleaned up messages", Logger::LEVEL_WARN);
    isStopped = TRUE;
  }
  stopLock.unlock();
}

/** No descriptions */
void MessageSender::cleanupMessages(){
  bool gotLock = FALSE;
  int tryCount =0;
  
  cleanupLock.lock();

  while (!(gotLock = stackLock.tryLock()) && (tryCount++ < 10)) {  //if we can't get a lock on the stack
    msleep(250);  //sleep and try again
  }

  if (!gotLock) {  //log the fact that we couldn't get the lock
    Context::getInstance()->getLogger()->log("cleanupMessages() unable to get lock on sender", name.c_str(), Logger::LEVEL_DEBUG);
  }

  try {
    while (!stack.empty()) {
      Message *msg = stack.front();
      stack.pop_front();

      if (msg != NULL) {
        delete msg;
      }      
    }
  }
  catch (...)  {
    Context::getInstance()->getLogger()->log("Error in cleanupMessages()", Logger::LEVEL_WARN);
  }
  if (gotLock) {
    stackLock.unlock();
  }
  
  cleanupLock.unlock();
  //Context::getInstance()->getLogger()->log("cleanupmessages: complete", Logger::LEVEL_WARN);

}

string& MessageSender::getStats() {
  char buffer[256];

  sprintf(buffer, "queue length: %d",  stack.size());
  return *(new string(buffer));
}
