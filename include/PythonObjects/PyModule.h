/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2009 The EVEmu Team
    For the latest information visit http://evemu.mmoforge.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Captnoord
*/

#ifndef _PYMODULE_H
#define _PYMODULE_H

/**
 * \class PyModule
 *
 * @brief
 *
 * blaat I dono what I am doing.... part 2. :D
 *
 * @author Captnoord
 * @date March 2009
 */
class PyModule
{
public:
    uint8 gettype();
    void IncRef();
    void DecRef();
    uint32 hash();
private:
    uint8 mType;
    size_t mRefcnt;
    uint32 (PyModule::*mHash)();
public:
    PyModule();
    ~PyModule();
    PyString* mModuleName;
private:
    uint32 _hash();
};

#endif //_PYMODULE_H
