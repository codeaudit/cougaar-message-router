/***************************************************************************
                          clientconnection.cpp  -  description
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
 using namespace std;
 
#define PACKET_HEADER_SIZE 8
#define MAX_BUF_SIZE 5096
 
#include "clientconnection.h"
#include <iostream.h>
#include <stdlib.h>
#include "ServerSocket.h"
#include "SocketException.h"
#include "message.h"
#include <string>
#include <vector>
#include "context.h"

void printbuffer(char *buf, int size) {
  for (int i=0; i<size; i++) {
    if (buf[i] < 32)
      printf("%x", buf[i]);
    else
      printf("%c", buf[i]);
  }
  cout << endl << flush;
}

long msgcount=0;
ClientConnection::ClientConnection(ServerSocket* sock, bool blockread){
  ss = sock;
  use_block_read = blockread;
  sender = new MessageSender(sock);
  packetBufferPos = 0;
  packetBufferSize = 0;
}

ClientConnection::~ClientConnection(){
}

/** No descriptions */
void ClientConnection::run() {
  Message * msg;                                           
  sender->start();
  try
  {
    while ( true )
    {
      if (use_block_read) {    //determine whether to use block reading method or not
        MessageList& messages = getMessages();
        while (messages.size() > 0) {
          Message *msg = messages.front();
          messages.pop_front();
          processMessage(*msg);        
        }
      }
      else {
        msg = getMessage();
        if (msg != NULL) {
          processMessage(*msg);
        }
      }
    }
  }
  catch ( SocketException& ex) {
    cout << "Socket exception" << endl;
    cout << ex.description() << endl;
  }

  //cleanup
  if (tmp_buffer != NULL) {
    delete tmp_buffer;
  }
  //if (packetData != NULL) {
    //delete packetData;
  //}
  deregisterClient();
  delete ss;
  if (sender != NULL) {
    sender->stop();
    delete sender;
  }
  delete this;
  
  return;
}

/** No descriptions
    This method gets the exact amount of data requested and places it in the buffer.
*/
void ClientConnection::getData(char *buffer, int size){
  tmp_buffer = new char[size];
  int index = 0;
  while (index < size) {
    int recv_size = ss->recv(tmp_buffer, (size-index));
    pack(tmp_buffer, 0, recv_size, buffer, index);
    index += recv_size;
  }
  delete tmp_buffer;
}


//Del by KDevelop: 
//Del by KDevelop: 
//Del by KDevelop: /** No descriptions */
Message* ClientConnection::getMessage(){
  char packet_header[PACKET_HEADER_SIZE];

  getData(packet_header, PACKET_HEADER_SIZE);
  
  //examine packet header to determine total message length
  char toLength = packet_header[0];
  char fromLength = packet_header[1];
  char threadLength = packet_header[2];
  char subjectLength = packet_header[3];
  long bodyLength =0;
  bodyLength |= packet_header[4];
  bodyLength <<= 8;
  bodyLength |= packet_header[5];
  bodyLength <<= 8;
  bodyLength |= packet_header[6];
  bodyLength <<= 8;
  bodyLength |= packet_header[7];
  /*printf("to length: %d\n", toLength);
  printf("from length: %d\n", fromLength);
  printf("thread length: %d\n", threadLength);
  printf("subject length: %d\n", subjectLength);
  printf("body length: %d\n", bodyLength);  */  
  int totalLength = toLength+fromLength+threadLength+subjectLength+bodyLength;
  //msgcount++;
  //printf("%d: received packet length: %d\n", msgcount, totalLength);
  packetData = new char[totalLength+1]; //add 1 for the end of string char
  memset(packetData, 0, sizeof(packetData));

  getData(packetData, totalLength);

  //printf("packet data: %s\n", packetData);
  
  //now construct the Message object
  Message *msg = new Message();
  int index = 0;
  string messagedata = packetData;
  //if (toLength == 0)   {  //if this message has no to address then it gets handled by the router
    if (toLength != 0) {
      msg->setto(messagedata.substr(0, toLength));
      index += toLength;
    }
    if (fromLength != 0) {
      msg->setfrom(messagedata.substr(index, fromLength));
      index += fromLength;
    }
    if (threadLength != 0) {
      msg->setthread(messagedata.substr(index, threadLength));
      index += threadLength;
    }
    if (subjectLength != 0) {
      msg->setsubject(messagedata.substr(index, subjectLength));
      index += subjectLength;
    }
    if (bodyLength != 0) {
      msg->setbody(messagedata.substr(index, bodyLength));
    }
  //}
  //else {  //otherwise, we're just going to route the message so all we need to know is the "to" address.  We set the header and
          //data directly since there's no need for the router to know the other contents of this message
    //msg->setto(messagedata.substr(0, toLength));
    //msg->setMessageHeader(packet_header);
    //msg->setMessageData(messagedata);
  //}
  
  delete packetData;
  
  return msg;
}

/** No descriptions */
void ClientConnection::routeMessage(Message& msg){
  ClientConnection *targetConnection = Context::getInstance()->getconnectionRegistry()->findConnection(msg.getto());
  if (targetConnection != NULL) {
    targetConnection->sendMessage(msg);
  }
  else {
    Message* reply = new Message();
    reply->setsubject("ERROR");
    reply->setto(msg.getfrom());
    reply->setbody("Unknown client: " + msg.getto());
    sendMessage(*reply);
  }
}

/** No descriptions */
void ClientConnection::processMessage(Message& msg){
  if (msg.getto() != "") {
    //cout << "routing msg from: " << name << " to: " << msg.getto() << endl << flush;
    routeMessage(msg);
  }
  else {
    //cout << "handling msg from: " << msg.getfrom() << " to: " << msg.getto() << " body: " << msg.getbody() << endl;
    handleMessage(msg);
  }
}

/** No descriptions */
void ClientConnection::registerClient(string& name){
  this->name = name;
  sender->name = name;
  Context::getInstance()->getconnectionRegistry()->registerConnection(this, name);
}

/** No descriptions */
void ClientConnection::sendMessage(Message& msg){
  char* header = msg.getMessageHeader();
  char toLength = header[0];
  char fromLength = header[1];
  char threadLength = header[2];
  char subjectLength = header[3];
  long bodyLength =0;
  bodyLength |= header[4];
  bodyLength <<= 8;
  bodyLength |= header[5];
  bodyLength <<= 8;
  bodyLength |= header[6];
  bodyLength <<= 8;
  bodyLength |= header[7];
  //printf("sending message length: %d\n",toLength+fromLength+threadLength+subjectLength+bodyLength);
  /*msgcount++;
  char tmpc[30];
  sprintf(tmpc, "%d : %d",msgcount, (toLength+fromLength+threadLength+subjectLength+bodyLength));
  cout << "Sent message: " << tmpc << endl << flush;*/

  //cout << "client connection for: " << name << " sending to: " << msg.getto() << ":" << msg.getbody() << endl;
  sender->addMessage(msg);
  
}

/** No descriptions */
void ClientConnection::deregisterClient(){
  Context::getInstance()->getconnectionRegistry()->deregisterConnection(this->name);
}

/** No descriptions */
void ClientConnection::handleMessage(Message& msg){
  string& subject = msg.getsubject();
  try {  
    Message* reply = new Message();
    reply->setthread(msg.getthread());
    if (subject == "connect") {  //handle the connect request
      //cout << "connected: " << msg.getbody() << endl;
      reply->setsubject("connected");
      reply->setto(msg.getbody());
      registerClient(msg.getbody());
    }
    else if (subject == "list") {  //handle the list request
      reply->setto(msg.getfrom());
      reply->setsubject("list");
      string& list = Context::getInstance()->getconnectionRegistry()->listConnections();
      reply->setbody(list);
      delete &list;
    }
    else { //send an error reply
      reply->setto(msg.getfrom());
      reply->setsubject("ERROR");
      reply->setbody("Unknown command");
    }
    sendMessage(*reply);
  }
  catch (std::exception& ex) {
    cout << "Exception: " << ex.what() << endl;
  }
  
}

void ClientConnection::pack(char *src, int srcStartPos, int srcLength, char *dest, int destStartPos) {
  int index = destStartPos;
  for (int i=srcStartPos; i<srcLength; i++) {
    dest[index] = src[i];
    index++;
  }
}

/** No descriptions */
MessageList& ClientConnection::getMessages(){
  //cout << "in getMessages()" << endl << flush;
  char* readBuffer = new char[MAX_BUF_SIZE+1];
  memset(readBuffer, 0, sizeof(readBuffer));
  MessageList *messages = new MessageList();
  //cout << "created message list" << endl << flush;
  //we want to fill the packetBuffer with as much data as we can get
  int actualSize = ss->recv(readBuffer, MAX_BUF_SIZE, false);
  //printbuffer(readBuffer, actualSize);
  //cout << "read data" << endl << flush;
  //printf("message size: %d\n", actualSize);
  int residualPacketLength = packetBufferSize-packetBufferPos;
  //printf("residual packet length %d\n", residualPacketLength);
  //cout << "got residual packet lentgh" << endl << flush;
  char* tmpBuf = new char[actualSize + residualPacketLength];  //create a temporary buffer to hold the data;
  //since the tmpBuf will become the packetBuffer, set the size now
  packetBufferSize = actualSize + residualPacketLength;  
  //cout << "created tmp buffer" << endl << flush;
  //pack the residual data from the packet buffer into the tmp buffer
  pack(packetBuffer, packetBufferPos, residualPacketLength, tmpBuf, 0);
  //cout << "packed residual" << endl << flush;
  //pack the newly read data into the tmp buffer 
  pack(readBuffer, 0, actualSize, tmpBuf, residualPacketLength);
  //cout << "packed tmp buffer" << endl << flush;
  if (packetBuffer != NULL) {
    delete packetBuffer;  //delete the old packet buffer
    //cout << "deleted packetBuffer" << endl << flush;
  }
  packetBuffer = tmpBuf;  //set the packet buffer to the tmp buffer
  //cout << "set packetbuffer to tmpBuf" << endl << flush;  

  packetBufferPos = 0;  //set the packetBufferPos to the beginning of the buffer
  //if there aren't even 8 bytes in the buffer there's not even enough for a header so
  //there's no use in parsing it yet
  if (packetBufferSize < PACKET_HEADER_SIZE) {
    return *messages;
  }
  
  //now parse through the packet buffer to pull out as many messages as possible
  int currentPacketLength = 0;
  while ((packetBufferPos+PACKET_HEADER_SIZE) < packetBufferSize) { //make sure there's enough room for the next packet header
    //process the current packet header
    char toLength = packetBuffer[packetBufferPos]; packetBufferPos++;
    char fromLength = packetBuffer[packetBufferPos]; packetBufferPos++;
    char threadLength = packetBuffer[packetBufferPos]; packetBufferPos++;
    char subjectLength = packetBuffer[packetBufferPos]; packetBufferPos++;
    long bodyLength =0;
    bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
    bodyLength <<= 8;
    bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
    bodyLength <<= 8;
    bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
    bodyLength <<= 8;
    bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
    int totalLength = toLength+fromLength+threadLength+subjectLength+bodyLength;
    //printf("total length: %d\n", totalLength);
    //printf("packetBufferPos: %d, packetBufferSize: %d\n", packetBufferPos, packetBufferSize);
    cout << flush;
    //check to make sure the remainder of the packetBuffer fully contains this packet
    if ((packetBufferSize-packetBufferPos) < totalLength) {
      packetBufferPos -= PACKET_HEADER_SIZE; //step back to tbe beginning of this packet
      return *messages;  //return the current list of messages
    }
    //otherwise, we need to constuct the Message
    Message *msg = new Message();
    //cout << "CREATED NEW MESSAGE" << endl << flush;
    char *datastr = createSubStr(packetBuffer, packetBufferPos, totalLength);
    string packetBufferStr = datastr;
    //cout << "packetBufferStr" << endl << packetBufferStr << endl << flush;
    int pos = 0;
    if (toLength != 0) {
      //cout << "check to" << endl << flush;
      msg->setto(packetBufferStr.substr(pos, toLength));
      //cout << "checked to" << endl << flush;
      pos += toLength;
    }
    if (fromLength != 0) {
      //cout << "check from" << endl << flush;
      msg->setfrom(packetBufferStr.substr(pos, fromLength));
      //cout << "checked from" << endl << flush;
      pos += fromLength;
    }
    if (threadLength != 0) {
      //cout << "check thread" << endl << flush;
      msg->setthread(packetBufferStr.substr(pos, threadLength));
      //cout << "checked thread" << endl << flush;
      pos += threadLength;
    }
    if (subjectLength != 0) {
      //cout << "check subject" << endl << flush;      
      msg->setsubject(packetBufferStr.substr(pos, subjectLength));
      //cout << "checked subject" << endl << flush; 
      pos += subjectLength;
    }
    if (bodyLength != 0) {
      //printf("body lengthL %d\n", bodyLength);
      //cout << "check body" << endl << flush;      
      msg->setbody(packetBufferStr.substr(pos, bodyLength));
      //cout << "checked body" << endl << flush; 
      pos += bodyLength;
    }
    delete datastr;
    messages->push_back(msg);
    packetBufferPos += totalLength;  //update the packet buffer pos
    //cout << "ADDED MESSAGE TO LIST" << endl << flush;        
  }
  return *messages;


} 
/** No descriptions */
char * ClientConnection::createSubStr(char *src, int start, int length){
  char *ret = new char[length+1];  //add one for the end of string char
  memset(ret, 0, length+1);
  for (int i=0; i<length; i++) {
    ret[i] = src[i+start];
  }

  return ret;
}
