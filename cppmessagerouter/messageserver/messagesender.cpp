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
  this->name = name;
  this->keepRunning = true;
  this->isStopped = false;
}

MessageSender::~MessageSender(){
}

/** No descriptions */
void MessageSender::run() {
  try {
    while(keepRunning) {
      stackLock.lock();
      while (this->stack.size() > 0) {
        sendMessage(*(this->stack.front()));
        this->stack.pop_front();
      }
      stackLock.unlock();
      msleep(500);
    }
  }
  catch(SocketException& ex) {
    keepRunning = false;
    Context::getInstance()->getLogger()->log(ex.description().c_str(), Logger::LEVEL_DEBUG);
    stackLock.unlock();
    //set this sender in the stopped state
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
  stackLock.lock();
  stack.push_back(&msg);
  stackLock.unlock();  
}

/** No descriptions */
void MessageSender::sendMessage(Message& msg){
  sendLock.lock();
  try {
    *ss << msg.getMessageHeader() << msg.getMessageData();
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
  if (!isStopped) {
    keepRunning = false;
    while (!this->finished()) {
      msleep(500);
    }
    cleanupMessages();
    isStopped = TRUE;
  }
  stopLock.unlock();
}

/** No descriptions */
void MessageSender::cleanupMessages(){
  cleanupLock.lock();
  stackLock.lock();
  try {
    while (this->stack.size() > 0) {
      Message *msg = this->stack.front();
      this->stack.pop_front();
      delete msg;
    }
  }
  catch (...)  {
    Context::getInstance()->getLogger()->log("Error in cleanupMessages()", Logger::LEVEL_WARN);
  }
  stackLock.unlock();
  cleanupLock.unlock();

}
