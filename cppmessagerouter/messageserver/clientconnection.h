/***************************************************************************
                          clientconnection.h  -  description
                             -------------------
    begin                : Wed Dec 10 2003
    copyright            : (C) 2003 by David Craine
    email                : dcraine@infoether.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <qthread.h>
#include <qmutex.h>
#include "ServerSocket.h"
#include <map>
#include <list>
#include "message.h"
#include "messagesender.h"

/**
  *@author David Craine
  */

typedef list<Message *> MessageList;

class ClientConnection : public QThread {
public: 
	ClientConnection(ServerSocket* ss, bool blockread);
	~ClientConnection();

/** No descriptions */
  virtual void run();

  /** No descriptions */
  void sendMessage(Message& msg);
  void sendMessageNow(Message& msg);
  /** No descriptions */
  void deregisterClient();
  /** No descriptions */
  MessageList& getMessages();
  void pack(char *src, int srcStartPos, int srcLength, char *dest, int destStartPos);
  void pack(char *src, int srcStartPos, int srcLength, unsigned char *dest, int destStartPos);

  const bool operator == (const ClientConnection& right);
  void incrementValidationCount();
  int getValidationCount();
  void resetValidationCount();
  void close();
  void closeNow();
  bool isRunning();

public:
  /**  */
  string name;
  bool keepRunning;
  bool isClosed;
  bool isClosing;

private: // Private attributes
  /**  */
  ServerSocket* ss; 
  /**  */
  MessageSender* sender;
  /**  */
  char* packetBuffer;
  /**  */
  int packetBufferPos;
  /**  */
  char * tmp_buffer;
  /**  */
  char * packetData;
  /**  */
  int packetBufferSize;
  /**  */
  bool use_block_read;
  /**  */
  bool validationCount;  //used to track outstanding validation requests
  QMutex closeLock;
  
private: // Private methods
  /** No descriptions */
  Message* getMessage();
  /** No descriptions */
  void routeMessage(Message& msg);
  /** No descriptions */
  bool processMessage(Message& msg);
  /** No descriptions */
  void registerClient(string &);
  /** No descriptions */
  bool handleMessage(Message& msg);
  /** No descriptions */
  void getData(char *buffer, int size);
  /** No descriptions */
  char * createSubStr(char *src, int start, int end);
  /** No descriptions */
  void getHeaderData(unsigned char *buffer, int size);
};



#endif
