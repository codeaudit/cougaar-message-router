 /***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Wed Nov 19 09:45:05 EST 2003
    copyright            : (C) 2003 by David Craine
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

 #ifdef HAVE_CONFIG_H
 #include <config.h>
 #endif

 #include <iostream.h>
 #include <stdlib.h>
 #include "ServerSocket.h"
 #include "SocketException.h"
 #include "clientconnection.h"
 #include "clientconnectionfactory.h"
 #include <string>
 #include <map.h>
 #include "context.h"

 
 int main(int argc, char *argv[])
 {

 Context::getInstance()->getLogger()->forceLog("Message Router started");
 bool block_read = false;
 if (argc > 1) {
   for (int i=1; i<argc; i++) {
     if (!strcmp(argv[i], "-b")) {
        block_read = true;
        cout << "block read enabled" << endl << flush;
        
      }
      if (!strcmp(argv[i], "-i")) {
        Logger* logger = Context::getInstance()->getLogger();
        logger->setLevel(Logger::LEVEL_INFO);
        logger->enable();
        cout << "debugging enabled at level INFO" << endl << flush;
      }
      if (!strcmp(argv[i], "-w")) {
        Logger* logger = Context::getInstance()->getLogger();
        logger->setLevel(Logger::LEVEL_WARN);
        logger->enable();
        cout << "debugging enabled at level WARN" << endl << flush;
      }
      if (!strcmp(argv[i], "-d")) {
        Logger* logger = Context::getInstance()->getLogger();
        logger->setLevel(Logger::LEVEL_DEBUG);
        logger->enable();
        cout << "debugging enabled at LEVEL DEBUG" << endl << flush;
      }
      if (!strcmp(argv[i], "-s")) {
        Logger* logger = Context::getInstance()->getLogger();
        logger->setLevel(Logger::LEVEL_SHOUT);
        logger->enable();
        cout << "debugging enabled at LEVEL SHOUT" << endl << flush;
      }      
   }
 }

  try
    {
      // Create the socket
      ServerSocket server ( 6667 );
      //create the connection factory
      ClientConnectionFactory *ccf = new ClientConnectionFactory();
      while ( true )
      {
        ServerSocket *new_sock = new ServerSocket();
        server.accept ( *new_sock );
        //ClientConnection* cc = new ClientConnection(new_sock, false);
        ClientConnection *cc = ccf->createClientConnection(new_sock, false);
        cc->start();
      }
    }
    catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n" << endl;
    }
    
    return EXIT_SUCCESS;
 }
 