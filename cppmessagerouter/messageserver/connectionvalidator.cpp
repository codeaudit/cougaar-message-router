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
    ValidationList::iterator pos;
    for (pos = connectionList.begin(); pos != connectionList.end(); ++pos) {
      if (((ClientConnection*)(*pos))->getValidationCount() == 0) {
        connectionList.erase(pos); //the connection's been validated so remove it from the list
      }
      else if (((ClientConnection*)(*pos))->getValidationCount() > VALIDATION_COUNT_LIMIT) {
        ((ClientConnection*)(*pos))->close();  //close the connection
        connectionList.erase(pos);  //remove it from the list
      }
      else {
        ((ClientConnection*)(*pos))->incrementValidationCount();  //
      }
    }
  }
}

void ConnectionValidator::validateConnection(ClientConnection* cc) {
  connectionList.push_back(cc);  //add this connection to validation map
  cc->incrementValidationCount();  //set the increment the validation count
  Message *pingMsg = new Message();
  pingMsg->setbody(PING);
  pingMsg->setthread("ping");
  cc->sendMessage(*pingMsg);
}
