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
}

MessageSender::~MessageSender(){
}

/** No descriptions */
void MessageSender::run() {
  try {
    while(keepRunning) {
      while (this->stack.size() > 0) {
        sendMessage(*(this->stack.front()));
        this->stack.pop_front();
      }
      msleep(500);
    }
  }
  catch(SocketException& ex) {
    Context::getInstance()->getLogger()->log(ex.description().c_str(), Logger::LEVEL_DEBUG);
    cleanupMessages();  
  }
}

/** No descriptions */
void MessageSender::addMessage(Message& msg){
  stack.push_back(&msg);  
}

/** No descriptions */
void MessageSender::sendMessage(Message& msg){
  *ss << msg.getMessageHeader() << msg.getMessageData();
  delete &msg;
}

/** No descriptions */
void MessageSender::stop(){
  keepRunning = false;
  while (!this->finished()) {
    msleep(500);
  }
  cleanupMessages();
}

/** No descriptions */
void MessageSender::cleanupMessages(){
  cleanupLock.lock();
  while (this->stack.size() > 0) {
    Message *msg = this->stack.front();
    this->stack.pop_front();
    delete msg;
  }
  cleanupLock.unlock();
}
