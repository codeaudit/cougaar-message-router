/***************************************************************************
                          message.cpp  -  description
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

#include "message.h"

Message::Message(){
  this->msgHeaderSet = false;
}

Message::Message(const Message& m) {
  this->msgHeaderSet = m.msgHeaderSet;
  this->from = m.from;
  this->to = m.to;
  this->thread = m.thread;
  this->subject = m.subject;
  this->body = m.body;
}

const Message& Message::operator= (const Message& right) {
  this->msgHeaderSet = right.msgHeaderSet;
  this->from = right.from;
  this->to = right.to;
  this->thread = right.thread;
  this->subject = right.subject;
  this->body = right.body;
}

Message::Message (const string& _from, const string& _to, const string& _thread, const string& _subject, const string& _body) {
  this->msgHeaderSet = false;
  this->from = _from;
  this->to = _to;
  this->thread = _thread;
  this->subject = _subject;
  this->body = _body;
}

Message::~Message(){
}
/** Read property of string from. */
string& Message::getfrom(){
	return from;
}
/** Write property of string from. */
void Message::setfrom( const string& _newVal){
	from = _newVal;
}
/** Read property of string  to. */
string & Message::getto(){
	return to;
}
/** Write property of string  to. */
void Message::setto( const string & _newVal){
	to = _newVal;
}
/** Read property of subject . */
string& Message::getsubject(){
	return subject;
}
/** Write property of subject . */
void Message::setsubject( const string& _newVal){
	 subject = _newVal;
}
/** Read property of string thread. */
string& Message::getthread(){
	return thread;
}
/** Write property of string thread. */
void Message::setthread( const string& _newVal){
	thread = _newVal;
}
/** Read property of string body. */
string& Message::getbody(){
	return body;
}
/** Write property of string body. */
void Message::setbody( const string& _newVal){
	body = _newVal;
}

void Message::setMessageHeader(char *c) {
  msgHeaderSet = true;
  for (int i=0; i<sizeof(msgHeader); i++) {
    msgHeader[i] = c[i];
  }
}


/** No descriptions */
void Message::setMessageHeader(const string &str){
  this->setMessageHeader(str.c_str());
}

char * Message::getMessageHeader() {
  if (!msgHeaderSet) {  //if the message header has not been explicity set then we need to calculate it
    msgHeader[0] = to.length();     //set "to" size
    msgHeader[1] = from.length();
    msgHeader[2] = thread.length();
    msgHeader[3] = subject.length();
    long bodyLength = body.length();
    msgHeader[4] = bodyLength & 4294967295L;
    msgHeader[5] = bodyLength & 16777215L;
    msgHeader[6] = bodyLength & 65535L;
    msgHeader[7] = bodyLength & 255L;
  }
  return msgHeader;

}

void Message::setMessageData(const string &data) {
  msgData =  data;
}

string& Message::getMessageData() {
  if (msgData == "") {  //if the message data has not been explicitly set, then we need to construct it
    msgData = to+from+thread+subject+body;
  }
  return msgData;  
}
  

