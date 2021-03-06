/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

//! clanNetwork="NetObjects"
//! header=network.h

#ifndef header_netobject_client
#define header_netobject_client

#ifdef CL_API_DLL
#ifdef CL_NETWORK_EXPORT
#define CL_API_NETWORK __declspec(dllexport)
#else
#define CL_API_NETWORK __declspec(dllimport)
#endif
#else
#define CL_API_NETWORK
#endif

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <string>
#include "../../signals.h"

class CL_NetObject_Client_Generic;
class CL_NetPacket;
class CL_NetObject_Controller_Generic;
class CL_NetComputer;

//: Network replication object, client version.
//- !group=Network/NetObjects!
//- !header=network.h!
//- <p>The netobject is a message dispatcher for objects being replicated over the network.</p>
//- <p>It provides an easy system for a server object to send messages to replicated clients
//- on remote computers, as well as getting answers back.</p>
class CL_API_NETWORK CL_NetObject_Client
{
//! Construction:
public:
	//: NetObject client constructor.
	CL_NetObject_Client(int obj_id, const CL_NetComputer &server, CL_NetObject_Controller_Generic *controller);

	//: Copy constructor.
	CL_NetObject_Client(const CL_NetObject_Client &copy);

	//: NetObject destructor.
	virtual ~CL_NetObject_Client();

//! Attributes:
public:
	//: Returns the netobject handle that identifies it over the network.
	int get_obj_id() const;

	//: Returns the netcomputer handle to the server of this object.
	CL_NetComputer &get_server() const;

//! Operations:
public:
	//: Send a message to the server object.
	void send(int msg_type, const CL_NetPacket &message, bool reliable = true);

//! Signals:
public:
	//: Signal emitted when receiving messages of the specified type from the server.
	CL_Signal_v1<CL_NetPacket &> &sig_received_message(int msg_type);

//! Implementation:
private:
	CL_NetObject_Client_Generic *impl;
};

#endif
