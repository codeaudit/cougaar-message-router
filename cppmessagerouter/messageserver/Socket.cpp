// Implementation of the Socket class.


#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream.h>



Socket::Socket() :
  m_sock ( -1 )
{

  memset ( &m_addr,
	   0,
	   sizeof ( m_addr ) );

}

Socket::~Socket()
{
  if ( is_valid() )
    ::close ( m_sock );
}

bool Socket::create()
{
  m_sock = socket ( AF_INET,
		    SOCK_STREAM,
		    0 );

  if ( ! is_valid() )
    return false;


  // TIME_WAIT - argh
  int on = 1;
  if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    return false;

  //set the receive buffer size
  /*int bufsize = 1024*1024 ;
  int r = setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
  printf("receive buffer size: %lu\n", bufsize);
  r = setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
  printf("send buffer size: %lu\n", bufsize); */
  
  return true;

}


int Socket::shutdown() {


  return ::shutdown(m_sock, SHUT_RDWR);
}


bool Socket::bind ( const int port )
{

  if ( ! is_valid() )
    {
      return false;
    }



  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons ( port );

  int bind_return = ::bind ( m_sock,
			     ( struct sockaddr * ) &m_addr,
			     sizeof ( m_addr ) );


  if ( bind_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::listen() const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( m_sock, MAXCONNECTIONS );


  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
  int addr_length = sizeof ( m_addr );
  new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( new_socket.m_sock <= 0 )
    return false;
  else
    return true;
}


bool Socket::send ( const std::string s ) const
{
  int status = ::send ( m_sock, s.c_str(), s.size(), /*MSG_NOSIGNAL*/0 );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}

bool Socket::send ( const char* c ) const
{
  int status = ::send ( m_sock, c, sizeof(c)*2, /*MSG_NOSIGNAL*/0 );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


int Socket::recv ( std::string& s ) const
{
  char buf [ MAXRECV + 1 ];

  s = "";

  memset ( buf, 0, MAXRECV + 1 );

  int status = ::recv ( m_sock, buf, MAXRECV, 0 );

  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      s = buf;
      return status;
    }
}

int Socket::recv ( char* c, int size) const {
  return recv(c, size, true);
}

int Socket::recv ( char* c, int size, bool wait ) const
{
    memset ( c, 0, size );

    int status=0;
    if (wait) {
      status = ::recv ( m_sock, c, size, MSG_WAITALL);
    }
    else {
      status = ::recv ( m_sock, c, size, 0 );
    }
    
    if ( status == -1 )
    {
      //std::cout << "status == -1   errno == " << errno << "  in Socket::recv(char*, size)\n";
      return 0;
    }
    else if ( status == 0 )
    {
      //std::cout << "status = 0" << endl;
      return 0;
    }
    /*else if (status != size) {
      printf("error: return size: %d - request size: %d\n", status, size);
      return 0;
    }*/
    else
    {
      return status;
    }
}

bool Socket::connect ( const std::string host, const int port )
{
  if ( ! is_valid() ) return false;

  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons ( port );

  int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

  if ( errno == EAFNOSUPPORT ) return false;

  status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

  if ( status == 0 )
    return true;
  else
    return false;
}


void Socket::set_non_blocking ( const bool b )
{

  int opts;

  opts = fcntl ( m_sock,
		 F_GETFL );

  if ( opts < 0 )
    {
      return;
    }

  if ( b )
    opts = ( opts | O_NONBLOCK );
  else
    opts = ( opts & ~O_NONBLOCK );

  fcntl ( m_sock,
	  F_SETFL,opts );

}

