/***************************************************************************
                          message.h  -  description
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

#ifndef MESSAGE_H
#define MESSAGE_H

using namespace std;

#include <string>

/**
  *@author David Craine
  */

class Message {
public: // Public attributes
  /**  */
  string from;
  /**  */
  string to;
  /**  */
  string thread;
  /**  */
  string body;
  /**  */
  string subject;
  char msgHeader[8];
  string msgData;
public: 
	Message();
  Message(const Message& m);
  const Message& operator= (const Message& right);
  Message (const string& _from, const string& _to, const string& _thread, const string& _subject, const string& _body);
	~Message();
  /** Write property of string from. */
  void setfrom( const string& _newVal);
  /** Read property of string from. */
  string& getfrom();
  /** Write property of string  to. */
  void setto( const string & _newVal);
  /** Read property of string  to. */
  string & getto();
  /** Write property of subject . */
  void setsubject( const string& _newVal);
  /** Read property of subject . */
  string& getsubject();
  /** Write property of string thread. */
  void setthread( const string& _newVal);
  /** Read property of string thread. */
  string& getthread();
  /** Write property of string body. */
  void setbody( const string& _newVal);
  /** Read property of string body. */
  string& getbody();
  /** Write property of string rawData. */
  void setMessageHeader(char *);
  char * getMessageHeader();
  void setMessageData(const string &);
  string& getMessageData();
  /** No descriptions */
  void setMessageHeader(const string &);
  

private: // Private attributes
  /**  */
  bool msgHeaderSet;  //used to determine if the msg header has been explicitly set
};

#endif
