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
#include "eavesdropregistry.h"
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

  EavesDropRegistry* getEavesDropRegistry();
  Logger* getLogger();
  ConnectionValidator *getConnectionValidator();
  void setAllowDuplicateConnections(bool);
  bool getAllowDuplicateConnections();
  void enableEavesDropping();
  bool isEavesDroppingEnabled();
  void disableEavesDropping();
  void enableErrorMessages();
  void disableErrorMessages();
  bool errorMessagesEnabled();
  void logToFile();
  void logToStdout();
  int getMaxSendQueueSize();
  void setMaxSendQueueSize(int size);
  void setSyncSend(bool);
  bool getSyncSend();
  void enableStatLogging();
  void disableStatLogging();
  bool getStatLogging();
  
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

  EavesDropRegistry* edr;
  
  Logger* logger;
  ConnectionValidator *connectionValidator;
  bool allowDuplicateConnections;
  bool eavesDroppingAllowed;
  bool errorMessagesAllowed;
  int maxSendQueueSize;
  bool syncSend;
  bool statLogging;
};

#endif
