/***************************************************************************
                          eavesdropregistry.cpp  -  description
                             -------------------
    begin                : Wed May 12 2004
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

#include "eavesdropregistry.h"

EavesDropRegistry::EavesDropRegistry(){
}
EavesDropRegistry::~EavesDropRegistry(){
}
/** No descriptions */
void EavesDropRegistry::registerEavesDropper(string& target, ClientConnection& eavesdropper){
}

void EavesDropRegistry::deregisterEavesDropper(ClientConnection& eavesdropper) {
}

void EavesDropRegistry::registerGlobalEavesDropper(ClientConnection& eavesdropper) {
}

void EavesDropRegistry::deregisterGlobalEavesDropper(ClientConnection& eavesdropper) {
}
