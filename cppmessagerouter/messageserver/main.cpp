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
 #include <string>
 #include <map.h>

 
 int main(int argc, char *argv[])
 {

 bool block_read = false;
 if (argc > 1) {
   if (!strcmp(argv[1], "-b")) {
     block_read = true;
     cout << "block read enabled" << endl << flush;
   }
 }

  try
    {
      // Create the socket
      ServerSocket server ( 6667 );
      
      while ( true )
      {
        //cout << "waiting for socket connection" << endl;
        ServerSocket *new_sock = new ServerSocket();
        server.accept ( *new_sock );
        //cout << "accepted new socket connection" << endl;
        ClientConnection* cc = new ClientConnection(new_sock, false);
        //cout << "created client connection object" << endl;
        cc->start();
        //cout << "started client connection" << endl;
      }
    }
    catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n" << endl;
    }
    
    return EXIT_SUCCESS;
 }

 