/***************************************************************************
                          messagesender.h  -  description
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

#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <qthread.h>
#include <qmutex.h>
#include "ServerSocket.h"
#include "message.h"
#include <list>

/**
  *@author David Craine
  */


class MessageSender : public QThread  {
public: 
	MessageSender(ServerSocket* sock);
	~MessageSender();
  /** No descriptions */
  void addMessage(Message &);
  /** No descriptions */
  void run();
  /** No descriptions */
  void stop();
  void sendMessage(Message &);
  void setName(string& name);
  
public: // Public attributes
  /**  */
  ServerSocket* ss;
  string name;
  list<Message *> stack;
  
private: // Private methods
  /** No descriptions */
  void cleanupMessages();

private: // Private attributes
  bool keepRunning;
  bool isStopped;
  QMutex cleanupLock;
  QMutex sendLock;
  QMutex stackLock;
  QMutex stopLock;

};

#endif
