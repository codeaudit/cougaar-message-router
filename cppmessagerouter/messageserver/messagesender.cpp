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
  try {
    while(keepRunning) {
      stackLock.lock();
      while (keepRunning && (stack.size() > 0)) {
        Message *msg = stack.front();
        stack.pop_front();
        sendMessage(*msg);
      }
      stackLock.unlock();
      msleep(500);
    }
  }
  catch(SocketException& ex) {
    keepRunning = FALSE;
    stackLock.unlock();
    Context::getInstance()->getLogger()->log(name.c_str(), ex.description().c_str(), Logger::LEVEL_DEBUG);  
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
  cleanupLock.lock();
  //Context::getInstance()->getLogger()->log("cleanupmessages: got cleanup lock", Logger::LEVEL_WARN);
  stackLock.lock();
  //Context::getInstance()->getLogger()->log("cleanupmessages: got stack lock", Logger::LEVEL_WARN);

  try {
    while (!stack.empty()) {
      //Context::getInstance()->getLogger()->log("cleanupmessages: stack contains items", Logger::LEVEL_WARN);
      Message *msg = stack.front();
      //Context::getInstance()->getLogger()->log("cleanupmessages: found message", msg->getsubject().c_str(), Logger::LEVEL_WARN);
      stack.pop_front();
      //Context::getInstance()->getLogger()->log("cleanupmessages: popped message from stack", Logger::LEVEL_WARN);

      if (msg != NULL) {
        delete msg;
        //Context::getInstance()->getLogger()->log("cleanupmessages: deleted msg", Logger::LEVEL_WARN);
      }      
    }
  }
  catch (...)  {
    Context::getInstance()->getLogger()->log("Error in cleanupMessages()", Logger::LEVEL_WARN);
  }
  stackLock.unlock();
  cleanupLock.unlock();
  //Context::getInstance()->getLogger()->log("cleanupmessages: complete", Logger::LEVEL_WARN);

}

string& MessageSender::getStats() {
  char buffer[256];

  sprintf(buffer, "queue length: %d",  stack.size());
  return *(new string(buffer));
}
