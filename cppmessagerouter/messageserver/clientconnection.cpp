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
#define VERSION "MessageRouter 1.7.23"
 
#include "clientconnection.h"
#include <iostream.h>
#include <stdlib.h>
#include "ServerSocket.h"
#include "SocketException.h"
#include "message.h"
#include <string>
#include <vector>
#include "context.h"
#include "logger.h"
#include "serverstats.h"

void printbuffer(char *buf, int size) {
  for (int i=0; i<size; i++) {
    if (buf[i] < 32)
      printf("%x", buf[i]);
    else
      printf("%c", buf[i]);
  }
}

long msgcount=0;
ClientConnection::ClientConnection(ServerSocket* sock, bool blockread){
  ss = sock;
  name = "";
  use_block_read = blockread;
  sender = new MessageSender(sock);
  packetBufferPos = 0;
  packetBufferSize = 0;
  validationCount = 0;
  isClosed = false;
  isClosing = false;
}

ClientConnection::~ClientConnection(){
}

void ClientConnection::incrementValidationCount() {
  validationCount++;
}

int ClientConnection::getValidationCount() {
  return validationCount;
}

void ClientConnection::resetValidationCount() {
  validationCount = 0;
}

/** No descriptions */
void ClientConnection::run() {
  Message * msg;                                           
  sender->start();
  keepRunning = true;
  try
  {
    while ( keepRunning )
    {
      if (use_block_read) {    //determine whether to use block reading method or not
        MessageList& messages = getMessages();
        while (messages.size() > 0) {
          Message *msg = messages.front();
          messages.pop_front();
          keepRunning = processMessage(*msg);
          if (!keepRunning) {
            break;  //break out of the inner loop if we get a disconnect request
          }        
        }
      }
      else {
        msg = getMessage();
        if (msg != NULL) {
          keepRunning = processMessage(*msg);
          //processMessage(*msg);
        }
      }
    }
  }
  catch ( SocketException& ex) {
    keepRunning = FALSE;
    Logger *logger = Context::getInstance()->getLogger();
    logger->log(name.c_str(), ex.description().c_str(), Logger::LEVEL_DEBUG);
  }
  
  close();
}

bool ClientConnection::isRunning() {
  return keepRunning;
}

void ClientConnection::closeNow() {
  keepRunning = FALSE;
  if (ss != NULL) {
    ss->shutdown();
  }
  //delete ss;
  //ss = NULL;
  //close();
}

void ClientConnection::close() {
  isClosing = true;
  deregisterClient();
  Context::getInstance()->getLogger()->log(name.c_str(), "shutting down connection", Logger::LEVEL_WARN);
  /***** cleanup *****/
  //if (tmp_buffer != NULL) {
    //delete tmp_buffer;
  //}
  if (sender != NULL) {
    //Context::getInstance()->getLogger()->log(name.c_str(), "shutting down sender", Logger::LEVEL_WARN);
    sender->stop();
    //Context::getInstance()->getLogger()->log(name.c_str(), "closed sender", Logger::LEVEL_WARN);
    if (!sender->wait(2000)) {
      Context::getInstance()->getLogger()->log(name.c_str(), "unable to shutdown sender", Logger::LEVEL_WARN);
    }
    else {
      //Context::getInstance()->getLogger()->log(name.c_str(), "deleting sender", Logger::LEVEL_WARN);
      delete sender;
      sender = NULL;
      //Context::getInstance()->getLogger()->log(name.c_str(), "shutdown sender", Logger::LEVEL_WARN);
    }
  }

  //deregister any listeners for this client connection
  Context::getInstance()->getlistenerRegistry()->deregisterListener(this);
  //Context::getInstance()->getLogger()->log(name.c_str(), "deregistered Listeners", Logger::LEVEL_WARN);

  //deregister any eaves droppers for this connection
  //if (Context::getInstance()->isEavesDroppingEnabled()) {
    Context::getInstance()->getEavesDropRegistry()->deregisterAllEavesDroppers(this);
    Context::getInstance()->getEavesDropRegistry()->deregisterGlobalEavesDropper(this);
    Context::getInstance()->getEavesDropRegistry()->removeTarget(this->name);
    //Context::getInstance()->getLogger()->log(name.c_str(), "cleared out eavesdropped", Logger::LEVEL_WARN);
  //}
  
  //if (packetData != NULL) {
    //delete packetData;
  //}

  if (ss != NULL) {
    ss->shutdown();
    delete ss;
    ss = NULL;
    //Context::getInstance()->getLogger()->log(name.c_str(), "shutdown server socket", Logger::LEVEL_WARN);
  }

  Context::getInstance()->getLogger()->log(name.c_str(), "shutdown complete", Logger::LEVEL_WARN);
  isClosed = true;
  return;
}

/** No descriptions
    This method gets the exact amount of data requested and places it in the buffer.
*/
void ClientConnection::getData(char *buffer, int size){
  tmp_buffer = new char[size];
  int index = 0;
  try {
    while (index < size) {
      int recv_size = ss->recv(tmp_buffer, (size-index));
      pack(tmp_buffer, 0, recv_size, buffer, index);
      index += recv_size;
    }
  }
  catch (SocketException &se) {
    delete tmp_buffer;
    tmp_buffer = NULL;
    throw se;
  }
 
  delete tmp_buffer;
  tmp_buffer = NULL;
}

/** No descriptions */
void ClientConnection::getHeaderData(unsigned char* buffer, int size){
  tmp_buffer = new char[size];
  int index = 0;
  try {
    while (index < size) {
      int recv_size = ss->recv(tmp_buffer, (size-index));
      pack(tmp_buffer, 0, recv_size, buffer, index);
      index += recv_size;
    }
  }
  catch (SocketException &se) {
    delete tmp_buffer;
    tmp_buffer = NULL;
    throw se;
  }
  
  delete tmp_buffer;
  tmp_buffer = NULL;
}


//Del by KDevelop: 
//Del by KDevelop: 
//Del by KDevelop: /** No descriptions */
Message* ClientConnection::getMessage(){
  unsigned char packet_header[PACKET_HEADER_SIZE];

  getHeaderData(packet_header, PACKET_HEADER_SIZE);
  
  //examine packet header to determine total message length
  unsigned char toLength = packet_header[0];
  unsigned char fromLength = packet_header[1];
  unsigned char threadLength = packet_header[2];
  unsigned char subjectLength = packet_header[3];
  long bodyLength =0;
  bodyLength |= packet_header[4];
  bodyLength <<= 8;
  bodyLength |= packet_header[5];
  bodyLength <<= 8;
  bodyLength |= packet_header[6];
  bodyLength <<= 8;
  bodyLength |= packet_header[7];
  int totalLength = toLength+fromLength+threadLength+subjectLength+bodyLength;
  packetData = new char[totalLength+1]; //add 1 for the end of string char
  memset(packetData, 0, sizeof(packetData));

  getData(packetData, totalLength);
  
  //now construct the Message object
  Message *msg = new Message();
  int index = 0;
  string messagedata = packetData;
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
  if (Context::getInstance()->isEavesDroppingEnabled()) {
    Context::getInstance()->getEavesDropRegistry()->checkMessage(msg);
  }
  ClientConnection *targetConnection = Context::getInstance()->getconnectionRegistry()->getConnection(msg.getto());
  if (targetConnection != NULL) {
    if (!targetConnection->isClosing) {
      targetConnection->sendMessage(msg);
    }
    else {
      delete &msg;
    }
  }
  else {
    Context::getInstance()->getLogger()->log(name.c_str(), msg.getto().c_str(), "ERROR", "unknown client", Logger::LEVEL_WARN);
    if (Context::getInstance()->errorMessagesEnabled()) {
      Message* reply = new Message();
      reply->setsubject("ERROR");
      reply->setto(msg.getfrom());
      reply->setbody("Unknown client: " + msg.getto());
      reply->setthread(msg.getthread());
      sendMessage(*reply);
    }
    delete &msg;  //delete the original message
  }
}

/** No descriptions */
bool ClientConnection::processMessage(Message& msg){
  ServerStats::getInstance()->incrementIncomingMsgCount();
  if (msg.getto() != "") {
    Context::getInstance()->getLogger()->log(msg.getfrom().c_str(), msg.getto().c_str(),
      msg.getsubject().c_str(), msg.getbody().c_str(), Logger::LEVEL_INFO);
    routeMessage(msg);
    return true;
  }
  else {
    bool ret = handleMessage(msg);
    delete &msg; //if we handle the message locally we need to delete it when we're done
    return ret;
  }
}

/** No descriptions */
void ClientConnection::registerClient(string& name){
  Context::getInstance()->getLogger()->log(name.c_str(), "registering connection", Logger::LEVEL_WARN);
  this->name = name;
  if (sender != NULL) {
    sender->setName(name);
  }
  Context::getInstance()->getconnectionRegistry()->registerConnection(this, name);
  //notify any listeners of this registration
  Message* msg = new Message();
  msg->setsubject("online");
  msg->setbody(name);
  Context::getInstance()->getlistenerRegistry()->notifyListeners(*msg);
}

/** Adds a message to the message sender queue */
void ClientConnection::sendMessage(Message& msg){
  ServerStats::getInstance()->incrementOutgoingMsgCount();
  sender->addMessage(msg);
}

/** Bypasses the message sender queue and sends the message directly**/
void ClientConnection::sendMessageNow(Message& msg) {
  sender->sendMessage(msg);
  ServerStats::getInstance()->incrementOutgoingMsgCount();
}

/** No descriptions */
void ClientConnection::deregisterClient(){
  Context::getInstance()->getconnectionRegistry()->deregisterConnection(this->name);
  //notify listeners
  Message* msg = new Message();
  msg->setsubject("offline");
  msg->setbody(name);
  Context::getInstance()->getlistenerRegistry()->notifyListeners(*msg);  
}

/** No descriptions */
bool ClientConnection::handleMessage(Message& msg){
  if (Context::getInstance()->isEavesDroppingEnabled()) {
    Context::getInstance()->getEavesDropRegistry()->checkMessage(msg);
  }
  string& subject = msg.getsubject();
  Context::getInstance()->getLogger()->log(msg.getfrom().c_str(), (const char *)"server",
    msg.getsubject().c_str(), msg.getbody().c_str(), Logger::LEVEL_INFO);
  try {  
    Message* reply = new Message();
    reply->setthread(msg.getthread());
    if (subject == "connect") {  //handle the connect request
      //check if a connection with this uer name already exists
      if (Context::getInstance()->getconnectionRegistry()->checkForExistingConnection(msg.getbody())) {
        //if duplicate connections aren't allowed then refuse this current request
        if (!Context::getInstance()->getAllowDuplicateConnections()) {
          reply->setsubject("ERROR");
          reply->setbody("duplicate reigstrations not allowed");
          Context::getInstance()->getLogger()->log("Attempt to login under duplicate user name", msg.getbody().c_str(), Logger::LEVEL_WARN);
          sendMessageNow(*reply);
          return false;
        }
        else {  //othewrwise disconnect the existing connection and allow this request
          ClientConnection *cc = Context::getInstance()->getconnectionRegistry()->getConnection(msg.getbody());
          if (cc != NULL) {
            Context::getInstance()->getLogger()->log("Duplicate connection request...closing original connection.", msg.getbody().c_str(), Logger::LEVEL_WARN);
            cc->closeNow();
            cc->wait(2000);
          }
          reply->setsubject("connected");
          reply->setto(msg.getbody());
          registerClient(msg.getbody());
        }
      }
      else {
        reply->setsubject("connected");
        reply->setto(msg.getbody());
        registerClient(msg.getbody());
      }
    }
    else if (subject == "disconnect") { //handle the disconnect request
      delete reply;
      return false;
    }
    else if (subject == "list") {  //handle the list request
      reply->setto(msg.getfrom());
      reply->setsubject("list");
      string& list = Context::getInstance()->getconnectionRegistry()->listConnections();
      Context::getInstance()->getLogger()->log((const char *)"Current List", list.c_str(), Logger::LEVEL_INFO);
      reply->setbody(list);
      delete &list;
    }
    else if (subject == "register") {
      Context::getInstance()->getlistenerRegistry()->registerListener(this);
      reply->setto(msg.getfrom());
      reply->setsubject("registered");
    }
    else if (subject == "deregister") {
      Context::getInstance()->getlistenerRegistry()->deregisterListener(this);
      reply->setto(msg.getfrom());
      reply->setsubject("deregistered");
    }
    else if (subject == "validateConnections") {
      Context::getInstance()->getconnectionRegistry()->validateConnections();
      delete reply;
      return true;
    }
    else if (subject == "version") {
      reply->setto(msg.getfrom());
      reply->setsubject("version");
      reply->setbody(VERSION);
    }
    else if (subject == "eavesdrop") {
      if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->registerEavesDropper(msg.getbody(), this);
        reply->setto(msg.getfrom());
        reply->setsubject("eavesdrop enabled");
        reply->setbody(msg.getbody());
      }
    }
    else if (subject == "globaleavesdrop") {
       if (Context::getInstance()->isEavesDroppingEnabled()) {
          Context::getInstance()->getEavesDropRegistry()->registerGlobalEavesDropper(this);
          reply->setto(msg.getfrom());
          reply->setsubject("globaleavesdrop enabled");
       }
    }
    else if (subject == "uneavesdrop") {
     if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->deregisterEavesDropper(msg.getbody(), this);
        reply->setto(msg.getfrom());
        reply->setsubject("eavesdrop disabled");
        reply->setbody(msg.getbody());
      }
    }
    else if (subject == "uneavesdropall") {
     if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->deregisterAllEavesDroppers(this);
        reply->setto(msg.getfrom());
        reply->setsubject("globaleavesdrop disabled");
      }
    }
    else if (subject == "unglobaleavesdrop") {
      if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->deregisterGlobalEavesDropper(this);
        reply->setto(msg.getfrom());
        reply->setsubject("globaleavesdrop disnabled");
      }
    }
    else if (subject == "enable eavesdropping") {
      Context::getInstance()->enableEavesDropping();
      reply->setto(msg.getfrom());
      reply->setsubject("eavesdropping enabled");
    }
    else if (subject == "disable eavesdropping") {
      Context::getInstance()->disableEavesDropping();
      reply->setto(msg.getfrom());
      reply->setsubject("eavesdropping disabled");
    }
    else if (subject == "enable error messages") {
      Context::getInstance()->enableErrorMessages();
      reply->setto(msg.getfrom());
      reply->setsubject("error messages enabled");
    }
    else if (subject == "disable error messages") {
      Context::getInstance()->disableErrorMessages();
      reply->setto(msg.getfrom());
      reply->setsubject("error message disabled");
    }
    else if (subject == "get stats") {
      reply->setto(msg.getfrom());
      reply->setsubject("stats");
      string& stats = ServerStats::getInstance()->getStatsStr();
      reply->setbody(stats);
      delete &stats;
    }
    else if (subject == "reset stats") {
      ServerStats::getInstance()->resetStats();
      reply->setto(msg.getfrom());
      reply->setsubject("stats reset");
    }      
    else if (msg.getthread() == "ping") { //a ping response causes validation count to reset
      delete reply;
      resetValidationCount();
      return true;
    }
    else if (subject == "kill sender") {  //used for testing
      reply->setto(msg.getfrom());
      reply->setbody(msg.getbody());
      Context::getInstance()->getLogger()->log(msg.getfrom().c_str(), "SERVER", "Request to kill sender", msg.getbody().c_str(), Logger::LEVEL_DEBUG);
      ClientConnection *cc = Context::getInstance()->getconnectionRegistry()->getConnection(msg.getbody());
      if (cc != NULL) {
        cc->sender->stop();
        reply->setsubject("sender killed");
      }
      else {
        reply->setsubject("unable to kill sender");
      }
    }
    else if (subject == "kill connection") {  //used for testing
      reply->setto(msg.getfrom());
      reply->setbody(msg.getbody());
      Context::getInstance()->getLogger()->log(msg.getfrom().c_str(), "SERVER", "Request to kill connection", msg.getbody().c_str(), Logger::LEVEL_DEBUG);
      ClientConnection *cc = Context::getInstance()->getconnectionRegistry()->getConnection(msg.getbody());
      if (cc != NULL) {
        cc->closeNow();
        reply->setsubject("connection killed");
      }
      else {
        reply->setsubject("unable to kill connection");
      }
    }
    else if (subject.find("set log level", -1) == 0) {
      if (subject.find("info", -1) != -1) {
        Context::getInstance()->getLogger()->setLevel(Logger::LEVEL_INFO);
        reply->setsubject("log level set to INFO");
      }
      else if (subject.find("warn", -1) != -1) {
        Context::getInstance()->getLogger()->setLevel(Logger::LEVEL_WARN);
        reply->setsubject("log level set to WARN");
      }
      else if (subject.find("debug", -1) != -1) {
        Context::getInstance()->getLogger()->setLevel(Logger::LEVEL_DEBUG);
        reply->setsubject("log level set to DEBUG");
      }
      else if (subject.find("shout", -1) != -1) {
        Context::getInstance()->getLogger()->setLevel(Logger::LEVEL_SHOUT);
        reply->setsubject("log level set to SHOUT");
      }
      else {
        reply->setsubject("unknown log level");
      }
      reply->setto(msg.getfrom());
    }
    else if (subject == "enable logging") {
      Context::getInstance()->getLogger()->enable();
      reply->setto(msg.getfrom());
      reply->setsubject("logging enabled");
    }
    else if (subject == "disable logging") {
      Context::getInstance()->getLogger()->disable();
      reply->setto(msg.getfrom());
      reply->setsubject("logging disabled");
    }
    else { //send an error reply
      if (Context::getInstance()->errorMessagesEnabled()) { //if error messaging is allowed
        reply->setto(msg.getfrom());
        reply->setsubject("ERROR");
        reply->setbody("Unknown command");
      }
      else {   //just delete the reply of error messages are disabled
        delete reply;
        return true;
      }
    }
     
    sendMessage(*reply);
  }
  catch (std::exception& ex) {
    Context::getInstance()->getLogger()->log(ex.what(), Logger::LEVEL_DEBUG);
  }
  return true;
  
}

void ClientConnection::pack(char *src, int srcStartPos, int srcLength, char *dest, int destStartPos) {
  int index = destStartPos;
  for (int i=srcStartPos; i<srcLength; i++) {
    dest[index] = src[i];
    index++;
  }
}

void ClientConnection::pack(char *src, int srcStartPos, int srcLength, unsigned char *dest, int destStartPos) {
  int index = destStartPos;
  for (int i=srcStartPos; i<srcLength; i++) {
    dest[index] = (unsigned char)src[i];
    index++;
  }
}

/** No descriptions */
MessageList& ClientConnection::getMessages(){
  char* readBuffer = new char[MAX_BUF_SIZE+1];
  memset(readBuffer, 0, sizeof(readBuffer));
  MessageList *messages = new MessageList();
  //we want to fill the packetBuffer with as much data as we can get
  int actualSize = ss->recv(readBuffer, MAX_BUF_SIZE, false);
  int residualPacketLength = packetBufferSize-packetBufferPos;
  char* tmpBuf = new char[actualSize + residualPacketLength];  //create a temporary buffer to hold the data;
  //since the tmpBuf will become the packetBuffer, set the size now
  packetBufferSize = actualSize + residualPacketLength;  
  //pack the residual data from the packet buffer into the tmp buffer
  pack(packetBuffer, packetBufferPos, residualPacketLength, tmpBuf, 0);
  //pack the newly read data into the tmp buffer 
  pack(readBuffer, 0, actualSize, tmpBuf, residualPacketLength);
  if (packetBuffer != NULL) {
    delete packetBuffer;  //delete the old packet buffer
  }
  packetBuffer = tmpBuf;  //set the packet buffer to the tmp buffer

  packetBufferPos = 0;  //set the packetBufferPos to the beginning of the buffer
  //if there aren't even 8 bytes in the buffer there's not even enough for a header so
  //there's no use in parsing it yet
  if (packetBufferSize < PACKET_HEADER_SIZE) {
    return *messages;
  }
  
  //now parse through the packet buffer to pull out as many messages as possible
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
    //check to make sure the remainder of the packetBuffer fully contains this packet
    if ((packetBufferSize-packetBufferPos) < totalLength) {
      packetBufferPos -= PACKET_HEADER_SIZE; //step back to tbe beginning of this packet
      return *messages;  //return the current list of messages
    }
    //otherwise, we need to constuct the Message
    Message *msg = new Message();
    char *datastr = createSubStr(packetBuffer, packetBufferPos, totalLength);
    string packetBufferStr = datastr;
    int pos = 0;
    if (toLength != 0) {
      msg->setto(packetBufferStr.substr(pos, toLength));
      pos += toLength;
    }
    if (fromLength != 0) {
      msg->setfrom(packetBufferStr.substr(pos, fromLength));
      pos += fromLength;
    }
    if (threadLength != 0) {
      msg->setthread(packetBufferStr.substr(pos, threadLength));
      pos += threadLength;
    }
    if (subjectLength != 0) {
      msg->setsubject(packetBufferStr.substr(pos, subjectLength));
      pos += subjectLength;
    }
    if (bodyLength != 0) {
      msg->setbody(packetBufferStr.substr(pos, bodyLength));
      pos += bodyLength;
    }
    delete datastr;
    messages->push_back(msg);
    packetBufferPos += totalLength;  //update the packet buffer pos
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

const bool ClientConnection::operator== (const ClientConnection& right) {
  return (this->name == right.name);
}


