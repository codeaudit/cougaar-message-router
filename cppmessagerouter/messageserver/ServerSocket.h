// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"


class ServerSocket : public Socket
{
 public:

  ServerSocket ( int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;
  const ServerSocket& operator << ( const char * ) const;
  const int recv ( char *, int ) const;
  const int recv ( char *, int, bool ) const;

  void accept ( ServerSocket& );

};


#endif

