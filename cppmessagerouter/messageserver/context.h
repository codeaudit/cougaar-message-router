/***************************************************************************
                          context.h  -  description
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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "connectionregistry.h"
#include "listenerregistry.h"
#include "connectionvalidator.h"
#include "logger.h"

/**
  *@author David Craine
  */

class Context {
public: 
	~Context();
  /** Read property of ConnectionRegistry * connectionRegistry. */
  ConnectionRegistry* getconnectionRegistry();
  /** No descriptions */
  static Context* getInstance();
  /** Read property of ListenerRegistry listenerRegistry. */
  ListenerRegistry* getlistenerRegistry();
  Logger* getLogger();
  ConnectionValidator *getConnectionValidator();
  void setAllowDuplicateConnections(bool);
  bool getAllowDuplicateConnections();
  
private:
	Context();
 
protected: // Protected attributes
private: // Private attributes
  /**  */
  static Context * currentInstance;
  /**  */
  ConnectionRegistry * connectionRegistry;
  /**  */
  ListenerRegistry* listenerRegistry;

  Logger* logger;
  ConnectionValidator *connectionValidator;
  bool allowDuplicateConnections;
};

#endif
