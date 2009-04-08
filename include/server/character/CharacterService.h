/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2008 The EVEmu Team
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
	Author:		Zhur
*/


#ifndef __CHARACTERSERVICE_H_INCL__
#define __CHARACTERSERVICE_H_INCL__

#include "../PyService.h"

#include "CharacterDB.h"

class CharacterService : public PyService {
public:
	CharacterService(PyServiceMgr *mgr, DBcore *dbc);
	virtual ~CharacterService();
	
protected:
	class Dispatcher;
	Dispatcher *const m_dispatch;

	CharacterDB m_db;
	
	PyCallable_DECL_CALL(GetCharacterToSelect)
	PyCallable_DECL_CALL(GetCharactersToSelect)
	PyCallable_DECL_CALL(SelectCharacterID)
	PyCallable_DECL_CALL(GetOwnerNoteLabels)
	PyCallable_DECL_CALL(GetCharCreationInfo)
	PyCallable_DECL_CALL(GetCharNewExtraCreationInfo)
	PyCallable_DECL_CALL(GetAppearanceInfo)
	PyCallable_DECL_CALL(ValidateName)
	PyCallable_DECL_CALL(ValidateNameEx)
	PyCallable_DECL_CALL(CreateCharacter)
	PyCallable_DECL_CALL(CreateCharacter2)
	PyCallable_DECL_CALL(Ping)
	PyCallable_DECL_CALL(PrepareCharacterForDelete)
	PyCallable_DECL_CALL(CancelCharacterDeletePrepare)
	PyCallable_DECL_CALL(AddOwnerNote)
	PyCallable_DECL_CALL(EditOwnerNote)
	PyCallable_DECL_CALL(GetOwnerNote)
	PyCallable_DECL_CALL(GetHomeStation)
	PyCallable_DECL_CALL(GetCloneTypeID)
	PyCallable_DECL_CALL(GetCharacterAppearanceList)
	PyCallable_DECL_CALL(GetRecentShipKillsAndLosses)

	PyCallable_DECL_CALL(GetCharacterDescription)//mandela
	PyCallable_DECL_CALL(SetCharacterDescription)//mandela

	PyCallable_DECL_CALL(GetNote) // LSMoura
	PyCallable_DECL_CALL(SetNote) // LSMoura

	uint32 GetSkillPointsForSkillLevel(InventoryItem *i, uint8 level); // Johnsus
};


#endif // __CHARACTERSERVICE_H_INCL__