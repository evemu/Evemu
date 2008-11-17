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
	Author:		Captnoord
*/

#include "EvemuPCH.h"

#include "ByteBuffer.h"

// Dummy authorization handshake function
static const byte handshakeFunc[] = {
	0x74, 0x04, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x6E, 0x65	//marshaled Python string "None"
};

/* random research

0000   22 00 00 00 7e 00 00 00 00 14 03 01 10 02 56 4b
0010   10 14 89 6a 8a ea 92 cd fa 6c 48 ae 4f 46 10 fa
0020   18 c7 7f fe cf 2b 02 01 00 00 7e 00 00 00 00 2c
0030   10 14 d9 95 5f e0 72 cc 32 c3 87 8a 78 03 93 3b
0040   35 7a 3a a6 e6 de 16 05 10 03 53 48 41 10 12 73
0050   69 67 6e 69 6e 67 5f 68 61 73 68 6d 65 74 68 6f
0060   64 05 c0 00 10 19 63 72 79 70 74 69 6e 67 5f 73
0070   65 73 73 69 6f 6e 6b 65 79 6c 65 6e 67 74 68 10
0080   04 33 44 45 53 10 19 63 72 79 70 74 69 6e 67 5f
0090   73 65 73 73 69 6f 6e 6b 65 79 6d 65 74 68 6f 64
00a0   10 4c 01 02 00 00 03 66 00 00 00 a4 00 00 c1 60
00b0   60 dc a3 1d 2f 5f b3 ed 29 3f 23 79 6e 82 da 55
00c0   2c 7d 56 95 b6 6f a4 f4 db da 44 a4 df d7 aa ad
00d0   cd e5 d5 03 d3 bd 83 a9 c3 cb db 05 5d 50 b9 bc
00e0   36 93 b8 e8 93 4e 0c 11 93 92 27 e4 6f 28 10 13
00f0   63 72 79 70 74 69 6e 67 5f 73 65 73 73 69 6f 6e
0100   6b 65 79 10 08 52 53 41 5f 46 55 4c 4c 10 1d 63
0110   72 79 70 74 69 6e 67 5f 73 65 63 75 72 69 74 79
0120   70 72 6f 76 69 64 65 72 74 79 70 65        

22 00 00 00		- packet size
7e				- packet header id
00 00 00 00		- unknown
14				- Op_PyTuple
03				- tuple size
01				- Op_PyNone // first item from tuple
10				- Op_PyByteString2 // second item from tuple
02 56 4b		- data size + data
10				- Op_PyByteString2
14				- data size
89 6a 8a ea 92 cd fa 6c 48 ae 4f 46 10 fa 18 c7 7f fe cf 2b -  data
02				- pystring
01				- stringsize
00 00			- 1 char + 1 ending '\0'
7e
00 00 00 00
2c				- Op_PyTwoTuple
10				- Op_PyByteString2
14				- size
d9 95 5f e0 72 cc 32 c3 87 8a 78 03 93 3b 35 7a 3a a6 e6 de
16				- Op_PyDict
05				- dict size
10				- Op_PyByteString2
03				- data size
53 48 41		- data 'SHA'
10				- Op_PyByteString2
12				- data size
73 69 67 6e 69 6e 67 5f 68 61 73 68 6d 65 74 68 6f 64 - data: 'signing_hashmethod'
05				- Op_PySignedShort
c0 00
10				- Op_PyByteString2
19				- data size
63 72 79 70 74 69 6e 67 5f 73 65 73 73 69 6f 6e 6b 65 79 6c 65 6e 67 74 68 - data: 'crypting_sessionkeylength'
10				- Op_PyByteString2
04
33 44 45 53		- data '3DES'
10				- Op_PyByteString2
19
63 72 79 70 74 69 6e 67 5f 73 65 73 73 69 6f 6e 6b 65 79 6d 65 74 68 6f 64 - data: 'crypting_sessionkeymethod'  
10				- Op_PyByteString2
4c				- data size = 76 .... (4 x 16) + 12
01 02 00 00 03 66 00 00 00 a4 00 00 c1 60 60 dc
a3 1d 2f 5f b3 ed 29 3f 23 79 6e 82 da 55 2c 7d
56 95 b6 6f a4 f4 db da 44 a4 df d7 aa ad cd e5
d5 03 d3 bd 83 a9 c3 cb db 05 5d 50 b9 bc 36 93
b8 e8 93 4e 0c 11 93 92 27 e4 6f 28
10				- Op_PyByteString2
13				- data size
63 72 79 70 74 69 6e 67 5f 73 65 73 73 69 6f 6e 6b 65 79 - data:'crypting_sessionkey'

10
08 
52 53 41 5f 46 55 4c 4c - data: 'RSA_FULL'
10
1d
63 72 79 70 74 69 6e 67 5f 73 65 63 75 72 69 74 79 70 72 6f 76 69 64 65 72 74 79 70 65 - data: 'crypting_securityprovidertype'
            
*/

/*  SHA-1 example

#ifndef _WIN32_WINNT
#define _WIN32_WINNT
0x0400 #endif
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <wincrypt.h>
void crypt();
void MyHandleError(char *s);

int main()
{
	crypt();
	return 0;
}

void crypt()
{
	HCRYPTPROV hCryptProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;
	HCRYPTHASH hHashSession;
	CHAR szPassword[14] = "test password";
	CHAR szData[25] = "The book is on the table";
	DWORD dwPassLen = strlen(szPassword);
	DWORD dwDataLen = strlen(szData);
	DWORD dwSessionHashLen;
	DWORD dwPassHashLen;
	if(!CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{         
		MyHandleError("Error during CryptAcquireContext!");
	}
	if(!CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hHash))
	{
		MyHandleError("Error during CryptCreateHash!");
	}
	if(!CryptHashData(hHash, (BYTE *)szPassword, dwPassLen, 0))
	{
		MyHandleError("Error during CryptHashData!");
	}
	if(!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwPassHashLen, 0))
	{
		MyHandleError("Error during CryptGetHashParam!");
	}
	BYTE * szPassHash = new BYTE[dwPassHashLen + 1];
	memset(szPassHash, 0, dwPassHashLen + 1);
	if(!CryptGetHashParam(hHash, HP_HASHVAL, szPassHash, &dwPassHashLen, 0))
	{
		MyHandleError("Error during CryptGetHashParam!");
	}
	if(!CryptDeriveKey(hCryptProv, CALG_3DES, hHash, CRYPT_EXPORTABLE, &hKey))
	{
		MyHandleError("Error during CryptDeriveKey!");
	}
	if(!CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hHashSession))
	{
		MyHandleError("Error during CryptCreateHash for session key!");
	}
	if(!CryptHashSessionKey(hHashSession, hKey, 0))	
	{
		MyHandleError("Error during CryptHashSessionKey for session key!");
	}
	DWORD dwCount = sizeof(DWORD);
	if(!CryptGetHashParam(hHashSession, HP_HASHSIZE, (BYTE *)&dwSessionHashLen, &dwCount, 0))
	{
		MyHandleError("Error during CryptGetHashParam for session key - Getting size!");
	}
	BYTE * szSessionHash = new BYTE[dwSessionHashLen + 1];
	memset(szSessionHash, 0, dwSessionHashLen + 1);
	if(!CryptGetHashParam(hHashSession, HP_HASHVAL, szSessionHash, &dwSessionHashLen, 0))
	{
		MyHandleError("Error during CryptGetHashParam!");
	}
	if(hHashSession)
	{
		if(!(CryptDestroyHash(hHashSession)))
			MyHandleError("Error during session CryptDestroyHash");
	}
	if(hHash)
	{
		if(!(CryptDestroyHash(hHash)))
			MyHandleError("Error during CryptDestroyHash");
	}
	if(hKey)
	{
		if(!(CryptDestroyKey(hKey)))
			MyHandleError("Error during CryptDestroyKey");
	}
	if(hCryptProv)
	{
		if(!(CryptReleaseContext(hCryptProv, 0)))
			MyHandleError("Error during CryptReleaseContext");
	}
	printf("The program to derive a key completed without error. \n");
}

void MyHandleError(char *s)
{
	printf("An error occurred in running the program.\n");
	printf("%s\n",s);
	printf("Error number %x\n.",GetLastError());
	printf("Program terminating.\n");
	exit(1);
} 
*/

EveClientSocket::EveClientSocket(SOCKET fd) : Socket(fd, CLIENTSOCKET_SENDBUF_SIZE, CLIENTOCKET_RECVBUF_SIZE)
{
	mAuthed = false;
	mQueued = false;			// don't support authorization queue for now
	mNagleEanbled = false;		// don't enable 'nagle' for now

	mRemaining = 0;

	mRequest = NULL;
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
	if (mRequest != NULL)
		delete mRequest;

	/* remove the link between the current socket and its client session */
	if (mSession)
	{
		mSession->SetSocket(NULL);
		mSession = NULL;
	}
}

void EveClientSocket::OnDisconnect()
{
	sSpace.mAcceptedConnections--;
	if(mSession)
	{
		mSession->SetSocket(0);
		mSession = NULL;
	}
}

// sends initial handshake containing the version info
void EveClientSocket::_sendHandShake()
{
	uint32 authCount = (uint32)sSpace.GetAuthorizedCount();

	VersionExchange version;
	version.birthday = EVEBirthday;
	version.macho_version = MachoNetVersion;
	version.user_count = authCount;
	version.version_number = EVEVersionNumber;
	version.build_version = EVEBuildVersion;
	version.project_version = EVEProjectVersion;

	PyRep *rep = version.Encode();
	OutPacket(rep);
}

//send position in queue
void EveClientSocket::_sendQueuePos( int queuePos )
{
	// send a python integer back as a response on the queue query
	PyRepInteger packet(queuePos);
	OutPacket(&packet);
}

// 'auth' commands
//OK = Reading Client 'Crypto' Context OR Queue Check
//OK CC = 'Crypto' Context Complete
//CC = 'Crypto' Context

/* send handshake accept */
void EveClientSocket::_sendAccept()
{
	PyRepString packet("OK CC"); // doesn't seem to matter what we send... (at this point)
	OutPacket(&packet);
}

/* send a python integer as a response on the password type query */
void EveClientSocket::_sendRequirePasswordType(int passwordType)
{
	PyRepInteger packet(passwordType);
	OutPacket(&packet);
}

/* low level packet sending, uses burst */
OUTPACKET_RESULT EveClientSocket::_outPacket(EVENetPacket * packet)
{
	uint32 len = packet->length;

	//ByteBuffer packetcheck;
	//packetcheck.append(packet->data, packet->length);

	//printf("\nsend:\n");
	//packetcheck.LogBuffer();

	if(IsConnected() == false)
		return OUTPACKET_RESULT_NOT_CONNECTED;

	BurstBegin();
	if( GetWriteBuffer().GetSpace() < (len+4) )
	{
		BurstEnd();
		return OUTPACKET_RESULT_NO_ROOM_IN_BUFFER;
	}

	// Packet logger :)
	//sSpaceLog.LogBuffer((uint32)len, opcode, (const uint8*)data, 1);

	// Pass the size of the packet to our send buffer
	bool rv = BurstSend((const uint8*)&len, 4);

	// Pass the rest of the packet to our send buffer (if there is any)
	if(len > 0 && rv == true)
	{
		rv = BurstSend((const uint8*)packet->data, (uint32)len);
	}

	if(rv == true)
	{
		BurstPush();
	}
	BurstEnd();
	return rv ? OUTPACKET_RESULT_SUCCESS : OUTPACKET_RESULT_SOCKET_ERROR;
}

// TODO loose the new packet and delete packet stuff..
void EveClientSocket::OutPacket(PyRep * packet )
{
	EVENetPacket * netData = new EVENetPacket;
	netData->data = Marshal(packet, netData->length);
	_outPacket(netData);
	SafeDelete(netData);
}

void EveClientSocket::OnConnect()
{
	sSpace.mAcceptedConnections++;
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

		ByteBuffer * packet = NULL;

		/* TODO we can skip this new ByteBuffer by feeding the circular buffer into the 'unmarshal' function
		 * requires to redesign the 'unmarshal' function
		 */
		if(mRemaining > 0)
		{
			packet = new ByteBuffer;
			packet->resize(mRemaining);

			// Copy from packet buffer into our actual buffer.
			GetReadBuffer().Read(packet->data(), mRemaining);
			mRemaining = 0;
		}

		// packet log
		//sWorldLog.LogBuffer(mSize, mOpcode, mSize ? Packet->contents() : NULL, 0);

		/* IMPORTANT TODO, InflateAndUnmarshal should not be in the socket, as when receiving big packets this could block the
		 * network core. So move it somehow to the session class, also as its recursive it can run forever.
		 */
		PyRep *recvPyPacket = InflateAndUnmarshal(packet->contents(), (uint32)packet->size());

		// printf("\nrecv packet with opcode:%d and Type:%s and size:%d\n", ((PyPacket*)recvPyPacket)->type, recvPyPacket->TypeString(), packet->size());
		// packet->LogBuffer();

		// the state machine magic
		(this->*mCurrentStateMachine)(recvPyPacket);

		// this is the end of the road for the used read buffer
		if ( packet != NULL)
		{
			SafeDelete(packet);
		}
	}
}


/* the client sends back its server info...
 * we should compare this with our own to make sure we can block unsupported clients.
 * I gues that would never happen unless you have a modified client. And even than its possible.
 */
void EveClientSocket::_authStateHandshake(PyRep* packet)
{
	VersionExchange data;
	if(data.Decode(&packet) == false)
	{
		//Log.Debug("%s: Received invalid version exchange!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

	if(data.birthday != EVEBirthday)
	{
		//sLog.Debug("%s: Client's birthday does not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}

	if(data.macho_version != MachoNetVersion)
	{
		//sLog.Debug("%s: Client's macho_version not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}

	if(data.version_number != EVEVersionNumber)
	{
		//sLog.Debug("%s: Client's version_number not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}

	if(data.build_version != EVEBuildVersion)
	{
		//sLog.Debug("%s: Client's build_version not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}

	if(data.project_version != EVEProjectVersion)
	{
		//sLog.Debug("%s: Client's project_version not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}

	Log.Debug("AuthStateMachine","State changed into StateQueueCommand");
	mCurrentStateMachine = &EveClientSocket::_authStateQueueCommand;
}

void EveClientSocket::_authStateQueueCommand(PyRep* packet)
{
	//check if it actually is tuple
	if(packet->CheckType(PyRep::Tuple) == false)
	{
		//sLog.Debug("%s: Invalid packet during waiting for command (tuple expected).", GetRemoteIP().c_str());
		Disconnect();
		return;
	}
	PyRepTuple *tuple = (PyRepTuple *)packet;
	
	//decode
	if(tuple->items.size() == 2)			//GetLogonQueuePosition
	{
		//QC = Queue Check
		NetCommand_QC cmd;
		if(cmd.Decode(&tuple) == false)
		{
			//sLog.Debug("%s: Failed to decode 2-arg command.", GetRemoteIP().c_str());
			Disconnect();
			return;
		}

		if(cmd.command != "QC")
		{
			//sLog.Debug("%s: Unknown 2-arg command '%s'.", GetRemoteIP().c_str(), cmd.command.c_str());
			Disconnect();
			return;
		}
		sLog.Debug("%s: Got Queue Check command.", GetRemoteIP().c_str());
		

		/* Send the position 1 or 0  the queue, which is 1 for now until we implemented a real login queue
		 * Send 100 and the client tells you that the queue is 100
		 */
		_sendQueuePos(1);

		//now act like client just connected
		//send out handshake again
		_sendHandShake();

		//and set proper state
		Log.Debug("AuthStateMachine","State changed into StateHandshake");
		mCurrentStateMachine = &EveClientSocket::_authStateHandshake;
		return;
	} 
	else if(tuple->items.size() == 3)
	{
		//this is sent when client is logging in
		NetCommand_VK cmd;
		if(cmd.Decode(&tuple) == false)
		{
			//sLog.Debug("%s: Failed to decode 3-arg command.", GetRemoteIP().c_str());
			Disconnect();
			return;
		}
		// VK == VIP mode
		if(cmd.command != "VK")
		{
			//sLog.Debug("%s: Unknown 3-arg command '%s'.", GetRemoteIP().c_str(), cmd.command.c_str());
			Disconnect();
			return;
		}
		//sLog.Debug("%s: Got VK command, vipKey=%s.", GetRemoteIP().c_str(), cmd.vipKey.c_str());
		Log.Debug("AuthStateMachine","State changed into StateStateNoCrypto");
		mCurrentStateMachine = &EveClientSocket::_authStateNoCrypto;
		return;
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

void EveClientSocket::_authStateNoCrypto(PyRep* packet)
{
	CryptoRequestPacket cryptoRequest;

	PyRep * data = packet;

	if(cryptoRequest.Decode(&data) == false)
	{
		//sLog.Debug("%S: Received invalid 'crypto' request!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

	if(cryptoRequest.keyVersion == "placebo")
	{
		//sLog.Debug("%s: Received Placebo 'crypto' request, accepting.", GetRemoteIP().c_str());
		Log.Debug("AuthStateMachine","State changed into StateCryptoChallenge");
		mCurrentStateMachine = &EveClientSocket::_authStateCryptoChallenge;

		_sendAccept();
	}
	else
	{
		//i'm sure cryptoRequest.keyVersion can specify either CryptoAPI or PyCrypto, but its all binary so im not sure how.
		PyRep *params = cryptoRequest.keyParams.Clone();
		CryptoAPIRequestParams car;
		if(car.Decode(&params) == false)
		{
			//sLog.Debug("%s: Received invalid CryptoAPI request!", GetRemoteIP().c_str());
			Disconnect();
			return;
		}

		sLog.Debug("%s: 'Unhandled' CryptoAPI request: hashmethod=%s sessionkeylength=%d provider=%s sessionkeymethod=%s", GetRemoteIP().c_str(), car.hashmethod.c_str(), car.sessionkeylength, car.provider.c_str(), car.sessionkeymethod.c_str());
		sLog.Debug("%s: You must change your client to use Placebo 'crypto' in common.ini to talk to this server!\n", GetRemoteIP().c_str());
	}
}

void EveClientSocket::_authStateCryptoChallenge(PyRep* packet)
{
	// just to be sure
	if(mRequest != NULL)
		delete mRequest;

	mRequest = new CryptoChallengePacket;

	if(mRequest->Decode(&packet) == false)
	{
		//sLog.Debug("%s: Received invalid 'crypto' challenge!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

	sLog.Debug("%s: Received Client Challenge.", GetRemoteIP().c_str(), mRequest->user_name.c_str());

	if(mRequest->user_password->CheckType(PyRep::None) == true)
	{
		//this is little wrong because on live they send password version always, but only once,
		//but we send password version when we get request with hashed password ...
		sLog.Debug("%s: Got hashed password, requesting plain.", GetRemoteIP().c_str());
		delete mRequest;
		mRequest = NULL;

		//send passwordVersion required: 1=plain, 2=hashed
		_sendRequirePasswordType(1);
		return;
	}

	sLog.Debug("%s:		username='%s'", GetRemoteIP().c_str(), mRequest->user_name.c_str());

	//send our handshake
	CryptoServerHandshake server_shake;
	server_shake.serverChallenge = "hi";
	server_shake.func_marshaled_code = new PyRepBuffer(handshakeFunc, sizeof(handshakeFunc));
	server_shake.macho_version = MachoNetVersion;
	server_shake.boot_version = EVEVersionNumber;
	server_shake.boot_build = EVEBuildVersion;
	server_shake.boot_codename = EVEProjectCodename;
	server_shake.boot_region = EVEProjectRegion;
	server_shake.cluster_usercount = 100;//m_userCount;
	server_shake.proxy_nodeid = 0xFFAA;
	server_shake.user_logonqueueposition = 1;
	server_shake.challenge_responsehash = "654"; // binascii.crc_hqx of 64 zero bytes in a string, in a single element tuple, marshaled

	// whoo recycle the incomming pointer
	packet = server_shake.Encode();
	OutPacket(packet);
	SafeDelete(packet);
	
	Log.Debug("AuthStateMachine","State changed into HandservershakeSend");
	mCurrentStateMachine = &EveClientSocket::_authStateHandshakeSend;
}

void EveClientSocket::_authStateHandshakeSend(PyRep* packet)
{
	CryptoHandshakeResult handshakeResult;
	PyRep * data = packet;
	if(handshakeResult.Decode(&data) == false)
	{
		sLog.Debug("%s: Received invalid 'crypto' handshake result!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

	//mSession = new EveClientSession(mUserid, mUserName, this);
	mSession = new EveClientSession(0, "", this);

	mSession->_client->Login(mRequest);

	

	mSession->Init(mSession->_client->GetAccountID(), mSession->_client->GetName());


	//this is a bit crappy ...
	//client->Login(m_request);

	/* now comes code that should be in the client class, but as we are hacking anyway why care at all */
	/*sLog.Debug("Login with %s", mRequest->user_name.c_str());

	if(mRequest->user_password->CheckType(PyRep::PackedObject2) == false)
	{
		//sLog.Debug("Failed to get password: user password is not PackedObject2.");
		Disconnect();
		return;
	}
	PyRepPackedObject2 *obj = (PyRepPackedObject2 *)mRequest->user_password;
	//we can check type, should be "util.PasswordString"

	Call_SingleStringArg passString;
	if(!passString.Decode(&obj->args1))
	{
		Disconnect();
		//sLog.Debug("Failed to decode password.");
		return;
	}*/

	// db check stuff
	/*if(!sPyServiceMgr.GetServiceDB()->DoLogin(mRequest->user_name.c_str(), pass.arg.c_str(), m_accountID, m_role)) {
		_log(CLIENT__MESSAGE, "%s: Login rejected by DB", mRequest->user_name.c_str());

		PyRepPackedObject1 *e = new PyRepPackedObject1("exceptions.GPSMACHONETMSG_TYPE_TRANSPORTCLOSED");
		e->args = new PyRepTuple(1);
		e->args->items[0] = new PyRepString("LoginAuthFailed");

		//throw(PyException(e));
	}*/

	// this is needed so if we exit before selecting a character, the account 'online' flag would switch back to 0
	//m_char.charid = 0;
	//sPyServiceMgr.GetServiceDB()->SetAccountOnlineStatus(m_accountID, true);

	/*mUserName =  mRequest->user_name;
	mUserid = 42;


	//send this before session change
	CryptoHandshakeAck cryptoHandshakeAck;
	cryptoHandshakeAck.connectionLogID = 1;	//TODO: what is this??
	cryptoHandshakeAck.jit = mRequest->user_languageid;
	cryptoHandshakeAck.userid = mUserid;//m_accountID;
	cryptoHandshakeAck.maxSessionTime = new PyRepNone;
	cryptoHandshakeAck.userType = 1;	//TODO: what is this??*/

	/*
	1 = I think normal user
	2 = rooky
	23 = I think trial account (unable to verify atm)
	*/

	/*cryptoHandshakeAck.role = 1;//m_role;
	cryptoHandshakeAck.address = GetRemoteIP();
	cryptoHandshakeAck.inDetention = new PyRepNone;
	cryptoHandshakeAck.user_clientid = 42;//m_accountID;
	
	PyRep * ectpack = cryptoHandshakeAck.Encode();
	OutPacket(ectpack);
	delete ectpack;*/

	/* end of the evil hacking */

	delete mRequest;
	mRequest = NULL;

	/* assumed that authorization is completed */
/*	if (mSession == NULL)
	{
		mSession = new EveClientSession(mUserid, mUserName, this);
	}*/

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
void EveClientSocket::_authStateDone(PyRep* packet)
{
	//Log.Debug("ClientSocket","received packet 'whooo' we passed authorization");

	// small hack to manage to handle the unexpected stuff..
	// and of course is this not the correct way 'todo' this
	if ( packet->CheckType(PyRep::PackedObject1) == true )
	{
		Log.Debug("AuthStateMachine","Exception debug string:%s", ((PyRepPackedObject1*)packet)->type.c_str());
		

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

	mSession->QueuePacket(pyPacket);
}

void EveClientSocket::_authStateException(PyRep* packet)
{
	Log.Debug("AuthStateMachine","Processing Exception");

	PyRepPackedObject1* obj = (PyRepPackedObject1*)packet;
	//obj->

	//ByteBuffer data;
	//data.append(packet->)

	// whoo delete if for now
	//SafeDelete(packet);
	SafeDelete(obj);
	obj = NULL;
}
