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



bool keepRunning = true;

MessageSender::MessageSender(ServerSocket* sock){
  this->ss = sock;
  this->name = name;
}

MessageSender::~MessageSender(){
}

/** No descriptions */
void MessageSender::run() {
  while(keepRunning) {
    while (this->stack.size() > 0) {
      sendMessage(*(this->stack.front()));
      this->stack.pop_front();
    }
    usleep(1000);

  }   
}

/** No descriptions */
void MessageSender::addMessage(Message& msg){
  stack.push_back(&msg);  
}

/** No descriptions */
void MessageSender::sendMessage(Message& msg){
  //cout << "message sender for: " << name << " sending to: " << msg.getto() << endl;

  //we need to strip out the ending '\0' from the data string
  /*const char *s =msg.getMessageData().c_str();
  int len = strlen(s);
  char *dataBuffer  = new char[len];
  for (int i=0; i<len; i++) {
    dataBuffer[i]=s[i];
  } */
  *ss << msg.getMessageHeader() << msg.getMessageData();
  delete &msg;
}
