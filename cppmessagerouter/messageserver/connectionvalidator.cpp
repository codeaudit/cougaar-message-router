/***************************************************************************
                          connectionvalidator.cpp  -  description
                             -------------------
    begin                : Mon Mar 29 2004
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

#include "connectionvalidator.h"
#include "context.h"
#include <iostream>

#define PING "command[help]"
#define VALIDATION_COUNT_LIMIT 60

ConnectionValidator::ConnectionValidator(){
}
ConnectionValidator::~ConnectionValidator(){
}

void ConnectionValidator::run() {
  while (true) {
    msleep(1000);
    //loop though the connectionList and remove items that don't have the validation flag set
    if (!connectionList.empty()) {
      ValidationList::iterator pos;
      pos = connectionList.begin();
       do {
        if (*pos == NULL) {
          Context::getInstance()->getLogger()->log("NULL value detected in connection list");
          pos = connectionList.erase(pos);
        }
        else if (((ClientConnection*)(*pos))->getValidationCount() == 0) {
          Context::getInstance()->getLogger()->log("Connection validated", ((ClientConnection*)(*pos))->name.c_str());
          pos = connectionList.erase(pos); //the connection's been validated so remove it from the list
        }
        else if (((ClientConnection*)(*pos))->getValidationCount() > VALIDATION_COUNT_LIMIT) {
          Context::getInstance()->getLogger()->log("Closing invalid connection", ((ClientConnection*)(*pos))->name.c_str());
          ((ClientConnection*)(*pos))->close();  //close the connection
          pos = connectionList.erase(pos);  //remove it from the list
        }
        else {
          Context::getInstance()->getLogger()->log("incrementing validation count", ((ClientConnection*)(*pos))->name.c_str());
          ((ClientConnection*)(*pos))->incrementValidationCount();  //
          pos++;
        }
      } while (pos != connectionList.end());
    }
  }
}

void ConnectionValidator::validateConnection(ClientConnection* cc) {
  if (cc != NULL) {
    connectionList.push_back(cc);  //add this connection to validation map
    cc->incrementValidationCount();  //set the increment the validation count
    Message *pingMsg = new Message();
    pingMsg->setbody(PING);
    pingMsg->setthread("ping");
    cc->sendMessage(*pingMsg);
  }
}
