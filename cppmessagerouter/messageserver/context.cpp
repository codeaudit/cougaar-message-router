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
#include "serverstats.h"

Context * Context::currentInstance;

Context::Context(){
  allowDuplicateConnections = TRUE;  //default value
  connectionRegistry = new ConnectionRegistry();
  listenerRegistry = new ListenerRegistry();
  edr = new EavesDropRegistry();
  eavesDroppingAllowed = FALSE;
  errorMessagesAllowed = FALSE;
  logger = new Logger();
  logger->start();
  connectionValidator = new ConnectionValidator();
  connectionValidator->start();
  ServerStats::getInstance();
  maxSendQueueSize=5000;      //default
  syncSend = false;  //default
  statLogging = false;
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

void Context::setAllowDuplicateConnections(bool b) {
  allowDuplicateConnections = b;
}

bool Context::getAllowDuplicateConnections() {
  return allowDuplicateConnections;
}

void Context::enableEavesDropping() {
   eavesDroppingAllowed = TRUE;
}

void Context::disableEavesDropping() {
  eavesDroppingAllowed = FALSE;
}

bool Context::isEavesDroppingEnabled() {
  return eavesDroppingAllowed;
}

EavesDropRegistry* Context::getEavesDropRegistry() {
  return edr;
}

void Context::enableErrorMessages() {
  errorMessagesAllowed = TRUE;
}

void Context::disableErrorMessages() {
  errorMessagesAllowed = FALSE;
}

bool Context::errorMessagesEnabled() {
  return errorMessagesAllowed;
}

void Context::logToFile() {
  logger->use_output_file = true;
}

void Context::logToStdout() {
  logger->use_output_file = false;
}

int Context::getMaxSendQueueSize() {
  return maxSendQueueSize;
}

void Context::setMaxSendQueueSize(int size) {
  maxSendQueueSize = size;
}

void Context::setSyncSend(bool b) {
  syncSend = b;
}

bool Context::getSyncSend() {
  return syncSend;
}

void Context::enableStatLogging() {
  statLogging = true;
}

void Context::disableStatLogging() {
  statLogging = false;
}

bool Context::getStatLogging() {
  return statLogging;
}