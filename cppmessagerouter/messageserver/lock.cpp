/***************************************************************************
                          lock.cpp  -  description
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

#include "lock.h"

Lock::Lock(){
  m_gotLock = false;
}

Lock::~Lock(){
}

void Lock::lock() {
  m_lock.lock();
  m_gotLock = true;
}

void Lock::unlock() {
  m_lock.unlock();
  m_gotLock = false;
}

bool Lock::locked() {
  return m_lock.locked();
}

bool Lock::tryLock() {
  return m_lock.tryLock();
}

bool Lock::gotLock() {
  return m_gotLock;
}