/*
 * Copyright 2011 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: fc_comunication.h
 */

#ifndef FC_COMUNICATION_H
#define FC_COMUNICATION_H
#define _SOF_4CPC_SRC_
#include "inc/stdInc.h"
#include "inc.vis/visDef.h"
#include "inc.vis/visMain.h"
#include "inc.vis/visShared.h"

class FC_comunication
{
public:
    FC_comunication();
   ~FC_comunication();
    bool connect(char * TargetAddress, int TargetPort);
    bool disconnect(void);
    bool readValue(char * varname, void * value);
    bool writeValue(char * varname, void * value);
#if 0
    bool getVariableList(VIS_DATA **ppDBIVar, VIS_UINT * uDBIVar);
    bool getNextVariable(VIS_DATA **ppDBIVar, VIS_UINT uDBIVar, XVisuVar **pxVisu);
#endif
private:
#if 0
    bool updateStatus();
#endif
    bool findVariable(char * varname, VIS_DATA **ppDBIVar);
    VIS_UINT g_uDBIVar;
    VIS_DATA *g_pDBIVar;
    bool isConnected;
    VIS_UINT VariableListuCount;
};

#endif // FC_COMUNICATION_H

