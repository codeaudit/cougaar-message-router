####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = messageserver

## INCLUDES were found outside kdevelop specific part

messageserver_SOURCES = logger.cpp listenerregistry.cpp messagesender.cpp connectionregistry.cpp context.cpp message.cpp main.cpp Socket.cpp ServerSocket.cpp ClientSocket.cpp clientconnection.cpp 
SOURCES = logger.cpp listenerregistry.cpp messagesender.cpp connectionregistry.cpp context.cpp message.cpp main.cpp Socket.cpp ServerSocket.cpp ClientSocket.cpp clientconnection.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
messageserver_LDADD   = $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = clientconnection.cpp clientconnection.h ClientSocket.cpp ClientSocket.h ServerSocket.cpp ServerSocket.h Socket.cpp SocketException.h Socket.h main.cpp message.cpp message.h context.cpp context.h connectionregistry.cpp connectionregistry.h messagesender.cpp messagesender.h listenerregistry.cpp listenerregistry.h logger.cpp logger.h 

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

messageserver_METASOURCES = AUTO

# the library search path. 
messageserver_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(messageserver_METASOURCES)

