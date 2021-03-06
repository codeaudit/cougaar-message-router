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
#define MAX_BUF_SIZE 5124
#define VERSION "MessageRouter 1.7.65"
#define ADMIN_ID "admin"
#define ADMIN_PWD "adminpwd"
 
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

static const int CMD_SIZE = 34;
static const string CMD_LIST[CMD_SIZE] = {"list - get a list of connected clients",
                    "register - register for online/offlien updates for all clients",
                    "deregister - deregister for online/offline updates",
                    "validateConnections - force valdiation of all connected clients",
                    "version - get the version of the server",
                    "enable eavesdropping - enable eavesdropping mode",
                    "disable eavesdropping - disable eavesdropping mode",
                    "eavesdrop - eavesdrop on a designated client (body of message must have client name)",
                    "uneavesdrop - uneavesdrop on designated client (body of message must have client name)",
                    "uneavesdropall - uneavesdrop on all clients currently being eavesdropped on",
                    "gloabaleavesdrop - eavesdrop on all clients",
                    "unglobaleavesdrop - uneavesdrop on all clients",
                    "enable error messages - enable error messages to be returned to clients",
                    "disable error messages - prevent error messages from being returned to clients",
                    "get stats - get current server statistics",
                    "get send queue stats - get stats on all sender queues",
                    "set max send queue size - sets the max size of send queues",
                    "reset stats - reset server statistics",
                    "kill sender - kill the send queue of a designated client (body of message must have client name)",
                    "kill connection - kill the designated client connection (body of message must have client name)",
                    "enable logging - enable the logging subsystem",
                    "disable logging - disable the logging subsystem",
                    "set log level <info|warn|debug|shout> - set the logging level",
                    "log to file - enable logging to file",
                    "log to stdout - enable logging to stdout",
                    "to: <broadcast> - send a message addressed to <broadcast> to send a message to all connected clients",
                    "get connection stats - get the stats for all current connections",
                    "get connection profiles - get the routing profile for all connections",
                    "enable stat logging - enables logging of stats to log file",
                    "disable stat logging - disables logging of stats to file",
                    "get stat logging - returns the current status of stat logging",
                    "enable sync send - enables sync sending mode",
                    "disable sync send - disables sync sending mode",
                    "help - get this message"
                    };

ClientConnection::ClientConnection(ServerSocket* sock, bool blockread){
  ss = sock;
  name = "";
  use_block_read = blockread;
  sender = new MessageSender(sock);
  packetBufferPos = 0;
  packetBufferSize = 0;
  packetBuffer = NULL;
  validationCount = 0;
  isClosed = false;
  isClosing = false;
  time(&startTime);
  incomingMsgCount = 0;
  outgoingMsgCount = 0;
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

void ClientConnection::cleanupMessages(MessageList * ml) {
  while (ml->size() > 0) {
    Message * msg = ml->front();
    ml->pop_front();
    delete msg;
  }
}

/** No descriptions */
void ClientConnection::run() {
  Message * msg = NULL;
  MessageList * messages = NULL;
                                           
  sender->start();
  keepRunning = true;
  try
  {
    while ( keepRunning )
    {
      if (use_block_read) {    //determine whether to use block reading method or not
        messages = getMessages();
        while (messages->size() > 0) {
          Message *msg = messages->front();
          messages->pop_front();
          keepRunning = processMessage(*msg);
          if (!keepRunning) {
            cleanupMessages(messages);
            break;  //break out of the inner loop if we get a disconnect request
          }        
        }
        delete messages;
        messages = NULL;
      }
      else {
        //cout << name << " waiting for msg..." << endl << flush;
        msg = getMessage();
        if (msg != NULL) {
          //cout << name << " processing msg..." << endl << flush;
          keepRunning = processMessage(*msg);
          //processMessage(*msg);
        }
        //cout << name << " processed msg." << endl << flush;
      }
    }
  }
  catch ( SocketException& ex) {
    keepRunning = FALSE;
    Logger *logger = Context::getInstance()->getLogger();
    logger->log(name.c_str(), ex.description().c_str(), Logger::LEVEL_WARN);
    if (messages != NULL) {
      cleanupMessages(messages);
      delete messages;
    }
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
  if (packetBuffer != NULL) {
    delete [] packetBuffer;
  }
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

  //clear the routingProfileMap
  routingProfileMap.clear();

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
    delete [] tmp_buffer;
    tmp_buffer = NULL;
    throw se;
  }
 
  delete [] tmp_buffer;
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
    delete [] tmp_buffer;
    tmp_buffer = NULL;
    throw se;
  }
  
  delete [] tmp_buffer;
  tmp_buffer = NULL;
}


//Del by KDevelop: 
//Del by KDevelop: 
//Del by KDevelop: /** No descriptions */
Message* ClientConnection::getMessage(){
  //cout << name << " in getMessage()..." << endl << flush;
  unsigned char packet_header[PACKET_HEADER_SIZE];

  getHeaderData(packet_header, PACKET_HEADER_SIZE);
  //cout << name << " got header data..." << endl << flush;
  
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
  //cout << name << " got data..." << endl << flush;
  
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
  
    delete [] packetData;
  
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
      routingProfileMap[msg.getto()]++;
      if (Context::getInstance()->getSyncSend()) {
        targetConnection->sendMessageNow(msg);
      }
      else {
        targetConnection->sendMessage(msg);
      }
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


bool ClientConnection::broadcastMessage(Message& msg) {
  Context::getInstance()->getconnectionRegistry()->broadcastMessage(msg);
  delete &msg;
}

/** No descriptions */
bool ClientConnection::processMessage(Message& msg){
  ServerStats::getInstance()->incrementIncomingMsgCount();
  incomingMsgCount++;
  
  if (msg.getto() == "broadcast") {
    broadcastMessage(msg);
    return true;
  }
  if (msg.getto() != "") {
    Context::getInstance()->getLogger()->log(msg.getfrom().c_str(), msg.getto().c_str(),
      "Routing Message", (msg.getthread() + ":" + msg.getsubject() + ":"+msg.getbody()).c_str(), Logger::LEVEL_INFO);
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
  outgoingMsgCount++;
  
  sender->addMessage(msg);
}

/** Bypasses the message sender queue and sends the message directly**/
void ClientConnection::sendMessageNow(Message& msg) {
  sender->sendMessage(msg);
  ServerStats::getInstance()->incrementOutgoingMsgCount();
  outgoingMsgCount++;
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

void ClientConnection::checkForAdmin(string data,  bool& isAdmin, bool& validPwd) {
  isAdmin = false;
  validPwd = false;
  
  string::size_type pos = data.find(ADMIN_ID, 0);
  if (pos == 0) {  //logging in as admin user
    if (data.length()-strlen(ADMIN_ID)<=0) { //no pwd specified
      isAdmin = true;
      return;
    }
    else {
      int pwdpos = strlen(ADMIN_ID);
      if (data[pwdpos] != ':') return; //this isn't the admin userid
      isAdmin = true;
      if (data.length()-pwdpos+1 <= 0) return;  //no pwd specified
      if (data.substr(pwdpos+1, data.length()-pwdpos+1) == ADMIN_PWD) {
        validPwd = true;
      }
    }
  }
  return;
}

/** No descriptions */
bool ClientConnection::handleMessage(Message& msg){
  if (Context::getInstance()->isEavesDroppingEnabled()) {
    Context::getInstance()->getEavesDropRegistry()->checkMessage(msg);
  }
  string& subject = msg.getsubject();
  Context::getInstance()->getLogger()->log(name.c_str(), (const char *)"server",
    msg.getsubject().c_str(), msg.getbody().c_str(), Logger::LEVEL_INFO);
  try {  
    Message* reply = new Message();
    reply->setto(msg.getfrom());
    reply->setthread(msg.getthread());
    if (subject == "connect") {  //handle the connect request
       bool isAdmin;
       bool validPwd;

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
          //check to see if this is an admin login attempt
          checkForAdmin(msg.body, isAdmin, validPwd);
          if (isAdmin) {
            if (validPwd) {
              reply->setsubject("connected");
              Context::getInstance()->getLogger()->log("Admin login successful", Logger::LEVEL_SHOUT);
              string adminid = ADMIN_ID;
              reply->setto(adminid);
              registerClient(adminid);
            }
            else {
              reply->setsubject("ERROR");
              reply->setbody("invalid admin pwd");
              Context::getInstance()->getLogger()->log("Attempt to login as admin user with invalid password", Logger::LEVEL_SHOUT);
              sendMessageNow(*reply);
              return false;
            }
          }
          else {
            reply->setsubject("connected");
            reply->setto(msg.getbody());
            registerClient(msg.getbody());
          }
        }
      }
      else {
          //check to see if this is an admin login attempt
          bool isAdmin, validPwd;
          checkForAdmin(msg.body, isAdmin, validPwd);
          if (isAdmin) {
            if (validPwd) {
              reply->setsubject("connected");
              Context::getInstance()->getLogger()->log("Admin login successful", Logger::LEVEL_SHOUT);
              string adminid = ADMIN_ID;
              reply->setto(adminid);
              registerClient(adminid);
            }
            else {
              reply->setsubject("ERROR");
              reply->setbody("invalid admin pwd");
              Context::getInstance()->getLogger()->log("Attempt to login as admin user with invalid password", Logger::LEVEL_SHOUT);
              sendMessageNow(*reply);
              return false;
            }
          }
          else {
            reply->setsubject("connected");
            reply->setto(msg.getbody());
            registerClient(msg.getbody());
          }
      }
    }
    else if (subject == "disconnect") { //handle the disconnect request
      delete reply;
      return false;
    }
    else if (subject == "list") {  //handle the list request
      reply->setsubject("list");
      string& list = Context::getInstance()->getconnectionRegistry()->listConnections();
      //Context::getInstance()->getLogger()->log((const char *)"Current List", list.c_str(), Logger::LEVEL_INFO);
      reply->setbody(list);
      delete &list;
    }
    else if (subject == "get send queue stats") {
      reply->setsubject("send queue stats");
      string& stats = Context::getInstance()->getconnectionRegistry()->getSendQueueStats();
      reply->setbody(stats);
      delete &stats;
    }
    else if (subject == "get max send queue size") {
      char buffer[128];
      sprintf(buffer, "Current max send queue size: %d", Context::getInstance()->getMaxSendQueueSize());
      reply->setsubject(buffer);
    }
    else if (subject == "register") {
      Context::getInstance()->getlistenerRegistry()->registerListener(this);
      reply->setsubject("registered");
    }
    else if (subject == "deregister") {
      Context::getInstance()->getlistenerRegistry()->deregisterListener(this);
      reply->setsubject("deregistered");
    }    
    else if (subject == "version") {
      reply->setsubject("version");
      reply->setbody(VERSION);
    }
    else if (subject == "eavesdrop") {
      if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->registerEavesDropper(msg.getbody(), this);
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
        reply->setsubject("eavesdrop disabled");
        reply->setbody(msg.getbody());
      }
    }
    else if (subject == "uneavesdropall") {
     if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->deregisterAllEavesDroppers(this);
        reply->setsubject("all eavesdroppers disabled");
      }
    }
    else if (subject == "unglobaleavesdrop") {
      if (Context::getInstance()->isEavesDroppingEnabled()) {
        Context::getInstance()->getEavesDropRegistry()->deregisterGlobalEavesDropper(this);
        reply->setsubject("globaleavesdrop disnabled");
      }
    }   
    else if (subject == "get stats") {
      reply->setsubject("stats");
      string& stats = ServerStats::getInstance()->getStatsStr();
      reply->setbody(stats);
      delete &stats;
    }   
    else if (msg.getthread() == "ping") { //a ping response causes validation count to reset
      delete reply;
      resetValidationCount();
      return true;
    }
    else if (subject == "log to file") {
      Context::getInstance()->logToFile();
      reply->setsubject("logging to file");
    }
    else if (subject == "log to stdout") {
      Context::getInstance()->logToStdout();
      reply->setsubject("logging to stdout");
    }
    else if (subject == "get connection stats") {
       reply->setsubject("connection stats");
       string& stats = Context::getInstance()->getconnectionRegistry()->getConnectionStatsStr();
       reply->setbody(stats);
       delete &stats;
    }
    else if (subject == "get connection profiles") {
       reply->setsubject("connection profiles");
       string& profiles = Context::getInstance()->getconnectionRegistry()->getConnectionProfiles();
       reply->setbody(profiles);
       delete &profiles;
    }
    else if (subject == "get sync send status") {
      reply->setsubject("sync send status");
      if (Context::getInstance()->getSyncSend()) {
        reply->setbody("enabled");
      }
      else {
        reply->setbody("disabled");
      }
    }
    else if (subject == "get stat logging") {
      reply->setsubject("stat logging status");
      if (Context::getInstance()->getStatLogging()) {
        reply->setbody("enabled");
      }
      else {
        reply->setbody("disabled");
      }
    }
    else if (subject == "help") {
      reply->setsubject("COMMAND LIST");
      string body = "\n";
      for (int i=0; i<CMD_SIZE; i++) {
         body +=CMD_LIST[i];
         body += "\n";
      }
      reply->setbody(body);
    }
    else if (name == "admin") {   //handle the admin commands
      if (subject == "validateConnections") {
        Context::getInstance()->getconnectionRegistry()->validateConnections();
        delete reply;
        return true;
      }
      else if (subject == "enable eavesdropping") {
        Context::getInstance()->enableEavesDropping();
        reply->setsubject("eavesdropping enabled");
      }
      else if (subject == "disable eavesdropping") {
        Context::getInstance()->disableEavesDropping();
        reply->setsubject("eavesdropping disabled");
      }
      else if (subject == "set max send queue size") {
        int val = atoi(msg.getbody().c_str());
        if (val > 0) {
          char buffer[128];
          sprintf(buffer, "Max send queue size changed from %d to %d",
              Context::getInstance()->getMaxSendQueueSize(), val);
          Context::getInstance()->setMaxSendQueueSize(val);
          Context::getInstance()->getLogger()->log(buffer, Logger::LEVEL_DEBUG);
          reply->setsubject(buffer);
        }
        else {
          reply->setsubject("set max send queue size");
          reply->setbody("invalid value specified");
        }
      }
      else if (subject == "enable error messages") {
        Context::getInstance()->enableErrorMessages();
        reply->setsubject("error messages enabled");
      }
      else if (subject == "disable error messages") {
        Context::getInstance()->disableErrorMessages();
        reply->setsubject("error messages disabled");
      }
      else if (subject == "reset stats") {
        ServerStats::getInstance()->resetStats();
        reply->setsubject("stats reset");
      }
      else if (subject == "kill sender") {  //used for testing
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
      else if (subject == "enable logging") {
        Context::getInstance()->getLogger()->enable();
        reply->setsubject("logging enabled");
      }
      else if (subject == "disable logging") {
        Context::getInstance()->getLogger()->disable();
        reply->setsubject("logging disabled");
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
      else if (subject == "enable sync send") {
        Context::getInstance()->setSyncSend(true);
        reply->setsubject("sync send enabled");
      }
      else if (subject == "disable sync send") {
        Context::getInstance()->setSyncSend(false);
        reply->setsubject("sync send disabled");
      }
      else if (subject == "enable stat logging") {
        Context::getInstance()->enableStatLogging();
        reply->setsubject("stat logging enabled");
      }
      else if (subject == "disable stat logging") {
        Context::getInstance()->disableStatLogging();
        reply->setsubject("stat logging disabled");
      }
      else { //send an error reply
        if (Context::getInstance()->errorMessagesEnabled()) { //if error messaging is allowed
          reply->setsubject("ERROR");
          reply->setbody("Unknown command");
        }
        else {   //just delete the reply if error messages are disabled
          delete reply;
          return true;
        }
      } 
    }
    else { //send an error reply
      if (Context::getInstance()->errorMessagesEnabled()) { //if error messaging is allowed
        reply->setsubject("ERROR");
        reply->setbody("Unknown command");
      }
      else {   //just delete the reply if error messages are disabled
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
  for (int i=srcStartPos; i<(srcLength+srcStartPos); i++) {
    dest[index] = src[i];
    index++;
  }
}

void ClientConnection::pack(char *src, int srcStartPos, int srcLength, unsigned char *dest, int destStartPos) {
  int index = destStartPos;
  for (int i=srcStartPos; i<(srcLength+srcStartPos); i++) {
    dest[index] = (unsigned char)src[i];
    index++;
  }
}

/** THIS METHOD HAS NOT BEEN TESTED IN A LONG TIME.  IT NEEDS TO BE DEBUGGED.*/
MessageList * ClientConnection::getMessages(){
  char* readBuffer = new char[MAX_BUF_SIZE+1];
  MessageList *messages = NULL;
  try {
    memset((void *)readBuffer, 0, sizeof(readBuffer));
    messages = new MessageList();
    //we want to fill the packetBuffer with as much data as we can get
    int actualSize = ss->recv(readBuffer, MAX_BUF_SIZE, false);
    //cout << name << " actual size: " << actualSize << endl << flush;
    int residualPacketLength = packetBufferSize-packetBufferPos;
    //cout << name << " residual packet length: " << residualPacketLength << endl << flush;
    //cout << name << " Unpacked Buffer" << endl << flush;
    //dumpPacket(packetBuffer, 0, packetBufferSize);
    //since the tmpBuf will become the packetBuffer, set the size now
    packetBufferSize = actualSize + residualPacketLength;
    char* tmpBuf = new char[packetBufferSize];  //create a temporary buffer to hold the data;
    //cout << name << " packetBufferPos: " << packetBufferPos << endl << flush;
    //cout << name << " packetBufferSize: " << packetBufferSize << endl << flush;
    //cout << name << "Residual data in Packet Buffer" << endl << flush;
    //dumpPacket(packetBuffer, packetBufferPos, residualPacketLength);
    //pack the residual data from the packet buffer into the tmp buffer
    pack(packetBuffer, packetBufferPos, residualPacketLength, tmpBuf, 0);
    //cout << name << " Residual Data in tmp Buffer" << endl << flush;
    //dumpPacket(tmpBuf, 0, residualPacketLength);
    //pack the newly read data into the tmp buffer 
    pack(readBuffer, 0, actualSize, tmpBuf, residualPacketLength);
    delete [] readBuffer;
    readBuffer = NULL;
    //if (packetBuffer != NULL) {
    delete [] packetBuffer;  //delete the old packet buffer
    packetBuffer = NULL;
    //}
    packetBuffer = tmpBuf;  //set the packet buffer to the tmp buffer
    //cout << name << " Fully Packet Buffer" << endl << flush;
    //dumpPacket(packetBuffer, 0, packetBufferSize);

    packetBufferPos = 0;  //set the packetBufferPos to the beginning of the buffer
    //if there aren't even 8 bytes in the buffer there's not even enough for a header so
    //there's no use in parsing it yet
    if (packetBufferSize < PACKET_HEADER_SIZE) {
      return messages;
    }
  }
  catch (SocketException& se) {
    if (messages != NULL) delete messages;
    if (readBuffer != NULL) delete [] readBuffer;
    throw se;
  }
  
  //now parse through the packet buffer to pull out as many messages as possible
  while ((packetBufferPos+PACKET_HEADER_SIZE) < packetBufferSize) { //make sure there's enough room for the next packet header
    //cout << name << " while loop packetBufferPos " << packetBufferPos << endl << flush;
    //process the current packet header
    unsigned char toLength = (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    unsigned char fromLength = (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    unsigned char threadLength = (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    unsigned char subjectLength = (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    long bodyLength =0;
    bodyLength |= (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    bodyLength <<= 8;
    bodyLength |= (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    bodyLength <<= 8;
    bodyLength |= (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    bodyLength <<= 8;
    bodyLength |= (unsigned char)packetBuffer[packetBufferPos]; packetBufferPos++;
    int totalLength = toLength+fromLength+threadLength+subjectLength+bodyLength;
    //cout << name << " toLength: " << (int)toLength << endl << flush;
    //cout << name << " fromLength: " << (int)fromLength << endl << flush;
    //cout << name << " threadLength: " << (int)threadLength << endl << flush;
    //cout << name << " subjectLength: " << (int)subjectLength << endl << flush;
    //cout << name << " bodyLength: " << (int)bodyLength << endl << flush;
    //cout << name << " totalLength: " << (int)totalLength << endl << flush;
        
    //check to make sure the remainder of the packetBuffer fully contains this packet
    if ((packetBufferSize-packetBufferPos) < totalLength) {
      packetBufferPos -= PACKET_HEADER_SIZE; //step back to tbe beginning of this packet
      //cout << name <<  " reset packetBufferPos to " << packetBufferPos << endl << flush;
      return messages;  //return the current list of messages
    }
    //otherwise, we need to constuct the Message
    Message *msg = new Message();
    char *datastr = createSubStr(packetBuffer, packetBufferPos, totalLength);
    //cout << name << " datastr: " << datastr << endl << flush;
    string packetBufferStr = datastr;
    delete [] datastr;
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
    messages->push_back(msg);
    packetBufferPos += totalLength;  //update the packet buffer pos
  }
  return messages;


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

string& ClientConnection::getSendQueueStats() {
  return sender->getStats();  
}

time_t ClientConnection::getStartTime() {
  return startTime;
}

unsigned int ClientConnection::getIncomingMsgCount() {
  return incomingMsgCount;
}

unsigned int ClientConnection::getOutgoingMsgCount() {
  return outgoingMsgCount;
}

string& ClientConnection::getRoutingProfileStr() {
  string* ret = new string("\n");
  char tmpbuf[10];
  
  if (!routingProfileMap.empty()) {
    ConnectionProfileMap::iterator pos;
    pos = routingProfileMap.begin();
    while (pos != routingProfileMap.end()) {
      *ret += "  ->  ";
      *ret += pos->first+": ";
      sprintf(tmpbuf, "%d", routingProfileMap[pos->first]);
      *ret += tmpbuf;
      *ret += "\n";
      pos++;
    }
  }
  return *ret;
}

void ClientConnection::dumpPacket(char * packet, int pos, int packetLength) {
  for (int i=pos; i<(pos+packetLength); i++) {
    cout << (unsigned int)packet[i] << " ";
  }
  cout << endl << flush;
}

