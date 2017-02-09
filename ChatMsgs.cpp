
#include "StdAfx.h"
#include "PhysicsObj.h"
#include "Monster.h"
#include "Player.h"

//Network access.
#include "Client.h"
#include "BinaryWriter.h"
#include "ChatMsgs.h"

#define MESSAGE_BEGIN(x) BinaryWriter *x = new BinaryWriter
#define MESSAGE_END(x) return x

BinaryWriter *LocalChat(const char *szText, const char *szName, DWORD dwSourceID, long lColor)
{
	MESSAGE_BEGIN(LocalChat);

	LocalChat->WriteDWORD(0x02BB);
	LocalChat->WriteString(szText);
	LocalChat->WriteString(szName);
	LocalChat->WriteDWORD(dwSourceID);
	LocalChat->WriteLong(lColor);

	MESSAGE_END(LocalChat);
}

BinaryWriter *SetTurbineChatChannels(DWORD dwSourceID)
{
	//Enable all chat channels, this ID comes back with each send
	MESSAGE_BEGIN(SetTurbineChatChannels);
	SetTurbineChatChannels->WriteDWORD(0x295);
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF0); // Allegiance channel ID
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF1); // General channel ID
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF2); // Trade channel ID
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF3); // LFG channel ID 
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF4); // Roleplay channel ID
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF5); // Olthoi ?
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF6); // Socety
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF7);
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF8);
	SetTurbineChatChannels->WriteDWORD(0xDEDBEEF9);
	MESSAGE_END(SetTurbineChatChannels);
}

//0x01, inbound, 0x03, outbound
BinaryWriter *TurbineChat(DWORD sender)
{
	wchar_t* text = L"Hello.";

	BinaryWriter* payload = new BinaryWriter();
	payload->WriteDWORD(0x01); //Inbound Message


	payload->WriteDWORD(0x000BEEF0); //Channel Number
	payload->WriteStringW(text); // Text
	payload->WriteDWORD(0); // Unknown
	payload->WriteDWORD(sender);
	payload->WriteDWORD(0);
	payload->WriteDWORD(0);
		
	

	BinaryWriter* msg = new BinaryWriter();

	msg->WriteDWORD(0xF7DE);
	msg->WriteDWORD(0);
	msg->WriteDWORD(0);
	msg->WriteDWORD(0);
	msg->WriteDWORD(0);
	msg->WriteDWORD(0);
	msg->WriteDWORD(0);
	msg->WriteDWORD(payload->GetSize());
	msg->WriteData(payload->GetData(),payload->GetSize());
	
	MESSAGE_END(msg);
}
BinaryWriter *EmoteChat(const char* szText, const char* szName, DWORD dwSourceID)
{
	MESSAGE_BEGIN(EmoteChat);

	EmoteChat->WriteDWORD(0x1E0);
	EmoteChat->WriteDWORD(dwSourceID);
	EmoteChat->WriteString(szName);
	EmoteChat->WriteString(szText);

	MESSAGE_END(EmoteChat);
}

//0x5719F5DB

BinaryWriter *DirectChat(const char* szText, const char* szName, DWORD dwSourceID, DWORD dwDestID, long lColor)
{
	//Fake-tells: bit 10 must be set.
	//Envoy: bit 21 must be set.

	//Example:
	//Step 1. Take a DWORD of any value. (entirely random if you wish, it doesn't matter.)
	//Step 2. Remove bits 10 and 21 from that DWORD.
	//Step 3. If FAKE-tell: set bit 10 (other don't.) These tells will never be drawn to the chat.
	//Step 4. If Envoy: set bit 21 (otherwise don't.) These tells will be prefixed with '+Envoy'.
	//Step 5. Now take the DWORD and XOR it against the source player's GUID.
	//The final result will be the magic number.

#define RANDOM_LONG() ((rand() << 30) | (rand()<<15) | rand()) // RAND_MAX=7fff

	BOOL bFakeTell = FALSE;
	BOOL bEnvoy = FALSE;

	DWORD dwMagicValue;
	dwMagicValue = RANDOM_LONG();				//Step 1
	dwMagicValue &= ~((1 << 10) | (1 << 21));	//Step 2

	if (bFakeTell)							//Step 3
		dwMagicValue |= 1 << 10;
	if (bEnvoy)								//Step 4
		dwMagicValue |= 1 << 21;

	dwMagicValue ^= dwSourceID;					//Step 5

	MESSAGE_BEGIN(DirectChat);
	DirectChat->WriteDWORD(0x2BD);
	DirectChat->WriteString(szText);
	DirectChat->WriteString(szName);
	DirectChat->WriteDWORD(dwSourceID);
	DirectChat->WriteDWORD(dwDestID);
	DirectChat->WriteLong(lColor);
	DirectChat->WriteDWORD(dwMagicValue);
	MESSAGE_END(DirectChat);
}

BinaryWriter *ActionChat(const char* szText, const char* szName, DWORD dwSourceID)
{
	MESSAGE_BEGIN(EmoteChat);

	EmoteChat->WriteDWORD(0x1E2);
	EmoteChat->WriteDWORD(dwSourceID);
	EmoteChat->WriteString(szName);
	EmoteChat->WriteString(szText);

	MESSAGE_END(EmoteChat);
}

BinaryWriter *ServerText(const char *szText, long lColor)
{
	MESSAGE_BEGIN(ServerText);

	ServerText->WriteDWORD(0xF7E0);
	ServerText->WriteString(szText);
	ServerText->WriteLong(lColor);

	MESSAGE_END(ServerText);
}

BinaryWriter *ServerBroadcast(const char *szSource, const char *szText, long lColor)
{
	// Using string class to prevent from static buffer collisions.
	std::string strBroadcast;
	strBroadcast = "Broadcast from ";
	strBroadcast += szSource;
	strBroadcast += "> ";
	strBroadcast += szText;

	MESSAGE_END(ServerText(strBroadcast.c_str(), lColor));
}

BinaryWriter *ChannelChat(DWORD dwChannel, const char* szName, const char* szText)
{
	MESSAGE_BEGIN(ChannelChat);

	ChannelChat->WriteDWORD(0x147);
	ChannelChat->WriteDWORD(dwChannel);
	ChannelChat->WriteString(szName);
	ChannelChat->WriteString(szText);

	MESSAGE_END(ChannelChat);
}

