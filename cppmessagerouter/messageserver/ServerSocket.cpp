// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream.h>


ServerSocket::ServerSocket ( int port )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }

}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

const int ServerSocket::recv (char* c, int size) const  {
  return this->recv(c, size, false);
}

const int ServerSocket::recv (char* c, int size, bool wait ) const
{
  int ret=0;
  ret = Socket::recv(c, size, wait);
  if ( !ret )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return ret;
}


const ServerSocket& ServerSocket::operator << ( const char * c ) const
{
  if ( ! Socket::send ( c ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw SocketException ( "Could not accept socket." );
    }

}


