/***************************************************************************
                          context.cpp  -  description
                             -------------------
    begin                : Wed Jan 28 2004
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

#include "context.h"
Context * Context::currentInstance;

Context::Context(){
  connectionRegistry = new ConnectionRegistry();
  listenerRegistry = new ListenerRegistry();
  logger = new Logger();
  connectionValidator = new ConnectionValidator();
  connectionValidator->start();
}

Context::~Context(){
}

/** Read property of ConnectionRegistry * connectionRegistry. */
ConnectionRegistry* Context::getconnectionRegistry(){
  return connectionRegistry;
}

/** No descriptions */
Context* Context::getInstance(){
  if (currentInstance == NULL) {
    currentInstance = new Context();
  }
  return currentInstance;
}
/** Read property of ListenerRegistry listenerRegistry. */
ListenerRegistry* Context::getlistenerRegistry(){
	return listenerRegistry;
}

Logger* Context::getLogger() {
  return logger;
}

ConnectionValidator* Context::getConnectionValidator() {
  return connectionValidator;
}
