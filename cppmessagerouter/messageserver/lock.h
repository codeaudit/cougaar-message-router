/***************************************************************************
                          lock.h  -  description
                             -------------------
    begin                : Fri Jul 2 2004
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

#ifndef LOCK_H
#define LOCK_H

#include <qmutex.h>

/**
  *@author David Craine
  This is essentially a wrapper for the QMutex class.  The reason for this is to
  provide the gotLock() method.  In some QMutex implementations, if a thread gets a lock and then calls
  the isLocked() method, the behavior is undefined.   I watned to make sure that
  I could determine whether or not the current thread had obtained the lock.  This
  is mainly used in exception handling where a thread may dump out of a piece of code
  after obtaining a lock but before releasing it.  The Exception handler needs to
  be able to determine whether or not it should release the lock.
  */

class Lock {
public: 
	Lock();
	~Lock();
  void lock();
  void unlock();
  bool locked();
  bool tryLock();
  bool gotLock();

private:
  bool m_gotLock;
  QMutex m_lock;
};

#endif
