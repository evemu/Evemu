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
	Author:		Captnoord
*/

#include "EvemuPCH.h"
#include "ByteBuffer.h"
#include "PasswordModule.h"
#include "BinAsciiModule.h"

/** left over research

VIPKEY value ==
macho.CryptoHash(util.CaseFold((request['user_name']

*/

/*
def CaseFold(s):
s2 = s.upper().lower()
if (s2 != s):
return CaseFold(s2)
return s2

*/

static std::string CaseFold(std::string & str)
{
	std::string s2 = str;
	Utils::Strings::toUpperCase(s2);
	Utils::Strings::toLowerCase(s2);
	if (s2 != str)
	{
		return CaseFold(s2);
	}
	return s2;
}

static std::wstring CaseFold(std::wstring & str)
{
	std::wstring s2 = str;
	Utils::Strings::toUpperCase(s2);
	Utils::Strings::toLowerCase(s2);
	if (s2 != str)
	{
		return CaseFold(s2);
	}
	return s2;
}

// Dummy authorization handshake function
static const uint8 handshakeFunc[] = {
	0x74, 0x04, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x6E, 0x65	//marshaled Python string "None"
};
static const uint8 handshakeFuncSize = 9;

EveClientSocket::EveClientSocket(SOCKET fd) : Socket(fd, CLIENTSOCKET_SENDBUF_SIZE, CLIENTOCKET_RECVBUF_SIZE)
{
	mAuthed = false;
	mQueued = false;			// don't support authorization queue for now
	mNagleEanbled = false;		// don't enable 'nagle' for now

	mRemaining = 0;

	mSession = NULL;

	mCurrentStateMachine = &EveClientSocket::_authStateHandshake;
}

EveClientSocket::~EveClientSocket()
{
	/* delete the buffers in the send buffer queue */
	// disabled for now as we don't have a send queue yet
	//WorldPacket * pck;
	//while((pck = _queue.Pop()))
	//	delete pck;

	/* delete the 'auth' request packet if it still exists */
	//if (mRequest != NULL)
	//	delete mRequest;

	/* remove the link between the current socket and its client session */
	if (mSession)
	{
		mSession->SetSocket(NULL);
		mSession = NULL;
	}
}

void EveClientSocket::OnDisconnect()
{
	sSpace.OnClientDisconnect();
	
	if(mSession != NULL)
	{
		mSession->SetSocket(0);
		mSession = NULL;
	}
}

// sends initial handshake containing the version info
void EveClientSocket::_sendHandShake()
{
	uint32 authCount = (uint32)sSpace.GetAuthorizedCount();

	/* version response packet */
	PyTupleStream packet(45);
	packet << EveBirthday;
	packet << MachoNetVersion;
	packet << (authCount+10);
	packet << EveVersionNumber;
	packet << EveBuildVersion;
	packet << EveProjectVersion;

	SendSimpleStream(packet);
}

//send position in queue
void EveClientSocket::_sendQueuePos( int queuePos )
{
	// send a python integer back as a response on the queue query
	PyIntStream out(queuePos);
	SendSimpleStream(out);
}

// 'auth' commands
//OK = Reading Client 'Crypto' Context OR Queue Check
//OK CC = 'Crypto' Context Complete
//CC = 'Crypto' Context

/* send handshake accept */
void EveClientSocket::_sendAccept()
{
	PyStringStream out("OK CC"); // doesn't seem to matter what we send... (at this point)
	SendSimpleStream(out);
}

/* send a python integer as a response on the password type query */
void EveClientSocket::_sendRequirePasswordType(int passwordType)
{
	PyIntStream out(passwordType);
	SendSimpleStream(out);
}

void EveClientSocket::OnConnect()
{
	sSpace.OnClientConnect();
	//_latency = getMSTime();

	/* send initial 'auth' handshake */
	_sendHandShake();
}

void EveClientSocket::OnRead()
{
	for(;;)
	{
		//Check for the header if we don't have any bytes to wait for.
		if(mRemaining == 0)
		{
			if(GetReadBuffer().GetSize() < 4)
			{
				// No header in the packet, let's wait.
				return;
			}

			// copy the packet size from the buffer
			uint32 packetSize;
			GetReadBuffer().Read((void*)&packetSize, 4);
			mRemaining = packetSize;
		}

		if(mRemaining > 0)
		{
			if( GetReadBuffer().GetSize() < mRemaining )
			{
				// We have a fragmented packet. Wait for the complete one before proceeding.
				return;
			}
		}

		//ByteBuffer * packet = NULL;
		PyReadStream * packet = NULL;

		/* TODO we can skip this new ByteBuffer by feeding the circular buffer into the 'unmarshal' function
		 * requires to redesign the 'unmarshal' function
		 */
		if(mRemaining > 0)
		{
			packet = new PyReadStream(mRemaining);

			// Copy from packet buffer into our actual buffer.
			GetReadBuffer().Read(packet->content(), mRemaining);
			mRemaining = 0;
		}

		// packet log
		sFileLogger.logPacket(packet->content(), packet->size(), 0);

		// printf("\nrecv packet with opcode:%d and Type:%s and size:%d\n", ((PyPacket*)recvPyPacket)->type, recvPyPacket->TypeString(), packet->size());
		// packet->LogBuffer();

		// the state machine magic
		(this->*mCurrentStateMachine)(*packet);

		// this is the end of the road for the used read buffer
		if ( packet != NULL)
		{
			SafeDelete(packet);
		}
	}
}


/* the client sends back its server info...
 * we should compare this with our own to make sure we can block unsupported clients.
 * I guess that would never happen unless you have a modified client. And even than its possible.
 */
void EveClientSocket::_authStateHandshake(PyReadStream& packet)
{
	Log.Debug(__FUNCTION__, "hand shaking...");

	PyTupleNetStream data(packet);

	uint16 MachoVersion;
	double VersionNr;
	uint32 BuildVersion;
	std::string Projectversion;
	uint32 Birthday;
	uint32 usercount; // what we send the client....

	data >> Birthday;
	data >> MachoVersion;
	data >> usercount;
	data >> VersionNr;
	data >> BuildVersion;
	data >> Projectversion;

	/* do version checking here */
	
	Log.Debug("AuthStateMachine","State changed into StateQueueCommand");
	mCurrentStateMachine = &EveClientSocket::_authStateQueueCommand;
}

void EveClientSocket::_authStateQueueCommand(PyReadStream& packet)
{
	PyTupleNetStream data(packet);

//	loginprogress::connecting
//	loginprogress::authenticating

	if (data.size() == 3)
	{
		Log.Debug("AuthStateMachine","State changed into StateStateNoCrypto");
		mCurrentStateMachine = &EveClientSocket::_authStateNoCrypto;
		return;
	}
	else if (data.size() == 2) //GetLogonQueuePosition
	{
		std::string unk;
		std::string command;

		data >> unk;		// in normal situations this should be a None object
		data >> command;	// in normal situations this should be a text command.

		ASCENT_ASSERT(command == "QC");

		// a check just to make sure
		if (command == "QC")
		{
			// Send the position 1 or 0  the queue, which is 1 for now until we implemented a real login queue
			// Send 100 and the client tells you that the queue is 100
			_sendQueuePos(1);

			//now act like client just connected
			//send out handshake again
			_sendHandShake();

			//and set proper state
			Log.Debug("AuthStateMachine","State changed into StateHandshake");
			mCurrentStateMachine = &EveClientSocket::_authStateHandshake;
		}
	}
	else 
	{
		//Log.Debug("EVE Socket","StateQueueCommand received a invalid packet");
		sLog.Debug("%s: Received invalid command packet:", GetRemoteIP().c_str());
		Disconnect();
	}
	/* If we get here.... it means that the authorization failed in some way, also we didn't handle the exceptions yet.
	 *
	 */
}

void EveClientSocket::_authStateNoCrypto(PyReadStream& packet)
{
	PyTupleNetStream data(packet);

	std::string keyVersion;
	data >> keyVersion;

	if(keyVersion == "placebo")
	{
		//sLog.Debug("%s: Received Placebo 'crypto' request, accepting.", GetRemoteIP().c_str());
		Log.Debug("AuthStateMachine","State changed into StateCryptoChallenge");
		mCurrentStateMachine = &EveClientSocket::_authStateCryptoChallenge;

		_sendAccept();
	}
	else
	{
		sLog.Debug("Received invalid 'crypto' request!");
		Disconnect();
	}
}



void EveClientSocket::_authStateCryptoChallenge(PyReadStream& packet)
{
	PyTupleNetStream data(packet);
	PyDictNetStream testDict;

	std::string clientResponceHash;
	data >> clientResponceHash;
	data >> testDict;

	std::wstring UserName;
	std::wstring UserPass;
	std::string UserPasswordHash;
	int UserAffiliateId;
	uint16 machoTest;

	testDict["macho_version"] >> machoTest;
	testDict["user_name"] >> UserName;
	testDict["user_password"] >> UserPass;
	testDict["user_password_hash"] >> UserPasswordHash;
	testDict["user_affiliateid"] >> UserAffiliateId;

	// send password version. I think hehe..
	PyIntStream out(2);
	SendSimpleStream(out);

	mAccountInfo = sAccountMgr.lookupAccount(UserName);

	// check if the password matches the saved password.
	if(memcmp(UserPasswordHash.c_str(), mAccountInfo->AccountShaHash, UserPasswordHash.size()) == TRUE)
	{
		// pass doesn't seem to match... send exception...
		Disconnect();
		return;
	}
	else
	{
		PyTupleStream outPacket;

		outPacket << "hi";			// this is crap... makes me think we can send like everything...
			PyTupleStream tTuple;
			tTuple << PyBufferStream(handshakeFunc, handshakeFuncSize); // marshaled PyNone object function taking no parameters
			tTuple << false;			// 
			tTuple << PyDictStream();	// empty dict
		outPacket << tTuple;

		PyDictStream tDict;
		outPacket << tDict; // add empty dictionary.

			tDict["boot_version"] << EveVersionNumber;
			tDict["boot_region"] << EveProjectRegion;
			tDict["cluster_usercount"] << 10;
			tDict["user_logonqueueposition"] << 1;
			tDict["challenge_responsehash"] << PyStringStream("654", 3);//PyBufferStream(64);
			tDict["macho_version"] << MachoNetVersion;
			tDict["boot_codename"] << EveProjectCodename;
			tDict["boot_build"] << EveBuildVersion;
			tDict["proxy_nodeid"] << 0xFFAA;
			tDict.finish();
		outPacket << tDict; // dict with content

		SendSimpleStream(outPacket);
	}
	
	Log.Debug("AuthStateMachine","State changed into HandservershakeSend");
	mCurrentStateMachine = &EveClientSocket::_authStateHandshakeSend;
}

void EveClientSocket::_authStateHandshakeSend(PyReadStream& packet)
{
	uint32 userid = sSpace.GenerateUserId();
	uint32 userClientId = mAccountInfo->AccountId;

	/* Server challenge response ack */
	PyDictStream outDict;
	outDict["live_updates"] << PyListStream(); // empty list

		PyDictStream outSessionInit;

		std::string remoteip = GetRemoteIP();
		remoteip += ":";
		remoteip += Utils::Strings::toString(GetRemotePort());

		outSessionInit["address"] << remoteip;
		outSessionInit["role"] << 2; // 2 is player
		outSessionInit["maxSessionTime"] << PyStreamNone;
		outSessionInit["userType"] << 1; /* 1: normal user, 2: rooky, 23: trial account. */
		outSessionInit["jit"] << "EN"; // should be harvested from handshake.
		outSessionInit["inDetention"] << PyStreamNone;//false; // banned person true?
		outSessionInit["userid"] << userid;
		outSessionInit.finish();

	outDict["session_init"] << outSessionInit;
	outDict["client_hashes"] << PyListStream(); // empty list
	outDict["user_clientid"] << userClientId;//mUserClientId;
	outDict.finish();

	SendSimpleStream(outDict);

	/* here we assume that the authorization chain has been successful */
	if (mSession == NULL)
		mSession = new EveClientSession(userid, this);
	else
		mSession->Init(userid);

	sSpace.AddSession(mSession);
	sSpace.AddGlobalSession(mSession);

	Log.Debug("AuthStateMachine","State changed into StateDone");
	mCurrentStateMachine = &EveClientSocket::_authStateDone;
	mAuthed = true;
}

/* 'ingame' packet dispatcher */

/* Packet fact list
   - in game packet aren't 'PackedObject1' as it seems ( assumption based on the old code )

*/
void EveClientSocket::_authStateDone(PyReadStream& packet)
{
	Log.Debug("ClientSocket","received packet 'whooo' we passed authorization");

	// small hack to manage to handle the unexpected stuff..
	// and of course is this not the correct way 'todo' this
	/*if ( packet->CheckType(PyRep::PackedObject1) == true )
	{
		//Log.Debug("AuthStateMachine","Exception debug string:%s", ((PyRepPackedObject1*)packet)->type.c_str());
		

		//Log.Debug("AuthStateMachine","State changed into StateException");
		//mCurrentStateMachine = &EveClientSocket::_authStateException;
		//(this->*mCurrentStateMachine)(recvPyPacket);

		//Log.Debug("AuthStateMachine","Exception noticed");
		//mCurrentStateMachine(packet);
		_authStateException(packet);
		return;		
	}

	//take the PyRep and turn it into a PyPacket
	PyPacket *pyPacket = new PyPacket;
	if(pyPacket->Decode(packet) == false) 	//packet is consumed here, as in deleted....hehehehe kinda inefficient.. but its ok for now
	{
		Log.Error("ClientSocket","%s: Failed to decode packet rep", GetRemoteIP().c_str());
		return;
	}

	mSession->QueuePacket(pyPacket);*/
}

void EveClientSocket::_authStateException(PyReadStream& packet)
{
	Log.Debug("AuthStateMachine","Processing Exception");

	/*PyRepPackedObject1* obj = (PyRepPackedObject1*)packet;
	//obj->

	//ByteBuffer data;
	//data.append(packet->)

	// whoo delete if for now
	//SafeDelete(packet);
	SafeDelete(obj);
	obj = NULL;*/
}