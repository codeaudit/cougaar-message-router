/***************************************************************************
                          connectionvalidator.h  -  description
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

#ifndef CONNECTIONVALIDATOR_H
#define CONNECTIONVALIDATOR_H

#include <qthread.h>
#include "clientconnection.h"

/**
  *@author David Craine
  */

typedef list<ClientConnection*> ValidationList;

class ConnectionValidator : public QThread  {
public: 
	ConnectionValidator();
	~ConnectionValidator();

  virtual void run();
  void validateConnection(ClientConnection *cc);

  private:
    ValidationList connectionList;

};



#endif
