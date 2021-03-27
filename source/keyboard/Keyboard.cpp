
// by simon yeung, 10/10/2020
// all rights reserved

#include <assert.h>
#include <stdio.h>
#include "Keyboard.h"
#include "math/Math.h"
#include "hid/HIDDevice.h"

#define REPORT_ID	0x00

Keyboard::Keyboard(HIDDevice* device) : USBDevice(device)
{
	m_packets			= nullptr;
	m_packetNum			= 0;
	m_layout			= KeyboardLayout::Unknown;
	m_isPressed.setAllOff();
	reset();
}

Keyboard::~Keyboard()
{
	if (m_packets)
		delete[] m_packets;
}


void	Keyboard::beginAddKey()
{
	reset();
}

void	Keyboard::reset()
{
	if (m_packets)
		delete[] m_packets;

	m_packets			= nullptr;
	m_packetNum			= 0;
	m_maxColorPacketIdx	= -1;
	memset(m_keys, 0, sizeof(m_keys));
}

void	Keyboard::addKey(KeyboardKey key, unsigned char colorPacketIdx, unsigned char packetOffset)
{
	m_keys[(int)key].packetIdx		= colorPacketIdx+ 1;	// 0 idx is header packet
	m_keys[(int)key].packetOffset	= packetOffset	+ 1;	// 1 byte for Report ID

	m_maxColorPacketIdx= Math::max(m_maxColorPacketIdx, (signed char)colorPacketIdx);
}

void	Keyboard::endAddKey()
{
	// set up packets headers
	m_packetNum= m_maxColorPacketIdx + 3;	// with header and footer packets
	m_packets= new Packet[m_packetNum];
	setUpPacketHeaders();
}

void	Keyboard::setUpPacketHeaders()
{
	assert(m_maxColorPacketIdx >= 0);

	// set up Report ID for all packets
	int numTotalPackets= m_packetNum;
	for(int i=0; i<numTotalPackets; ++i)
	{
		assert(m_packets != nullptr);
		memset(m_packets[i].data, 0, sizeof(Packet::data));
		m_packets[i].data[0]= REPORT_ID;
	}

	// header packet
	assert(m_packets != nullptr);
	m_packets[0].data[1+ 0]= 0x56;
	m_packets[0].data[1+ 1]= 0x81;
	m_packets[0].data[1+ 4]= 0x01;
	m_packets[0].data[1+ 8]= m_maxColorPacketIdx + 1;
	m_packets[0].data[1+12]= 0xaa;
	m_packets[0].data[1+13]= 0xaa;
	m_packets[0].data[1+14]= 0xaa;
	m_packets[0].data[1+15]= 0xaa;

	// color packets
	int lastPacketIdx= numTotalPackets - 1;
	for(int i=1; i<lastPacketIdx; ++i)
	{
		m_packets[i].data[1+ 0]= 0x56;
		m_packets[i].data[1+ 1]= 0x83;
		m_packets[i].data[1+ 2]= i-1;
	}

	// extra header in color packet [0]
	m_packets[1].data[1+ 4]= 0x01;
	m_packets[1].data[1+ 8]= 0x80;
	m_packets[1].data[1+ 9]= 0x01;
	m_packets[1].data[1+11]= 0xc1;
	m_packets[1].data[1+16]= 0xff;
	m_packets[1].data[1+17]= 0xff;
	m_packets[1].data[1+18]= 0xff;
	m_packets[1].data[1+19]= 0xff;

	// footer packet
	m_packets[lastPacketIdx].data[1+ 0]= 0x51;
	m_packets[lastPacketIdx].data[1+ 1]= 0x28;
	m_packets[lastPacketIdx].data[1+ 4]= 0xff;
}

void	Keyboard::setKeyColor(KeyboardKey key, unsigned char R, unsigned char G, unsigned char B)
{
	const KeyData& keyData= m_keys[(int)key];
	int packetIdxR= keyData.packetIdx;
	if (packetIdxR == 0)
		return;		// this key not exist in this keyboard

	int packetOffsetR	= keyData.packetOffset;
	bool isLastByte		= packetOffsetR == 64;
	
	// RGB color data may span across 2 packets
	int packetIdxG		= packetIdxR + isLastByte;
	int packetOffsetG	= isLastByte ? (1+4) : packetOffsetR + 1;
	
	int packetIdxB		= packetIdxG;
	int packetOffsetB	= packetOffsetG + 1;
	
	m_packets[packetIdxR].data[packetOffsetR]= R;
	m_packets[packetIdxG].data[packetOffsetG]= G;
	m_packets[packetIdxB].data[packetOffsetB]= B;
}

void	Keyboard::commitKeyColor()
{
	static unsigned char recvData[sizeof(Packet::data)];

	for(int i=0; i<m_packetNum; ++i)
	{
		// send packet
		const Packet& pkt= m_packets[i];
		int bytes_written= m_device->write(pkt.data, sizeof(Packet::data));
#if !DEBUG_DISABLE_HID_COMM
		if (sizeof(Packet::data) != bytes_written)
			printf("Commit send color error [%i]    %i\n", i, bytes_written);

		// recv ack packet
		int numRead= m_device->read(recvData, sizeof(recvData));
		if (!(numRead == 65 && recvData[1] == pkt.data[1] && recvData[2] == pkt.data[2]) )
		{
			printf("Commit recv color error [%i]    %i\n", i, numRead);

#if 0	// detail debug message
			for(int i=0; i<numRead; ++i)
				printf("0x%02x ", ((char*)recvData)[i]);
			printf("\n\n");
#endif
		}
#endif
	}
}

void	Keyboard::addKeyNumPad()
{
	// code not tested, reference from DuckyAPI
	addKey(KeyboardKey::NumLock			, 5, 0x33);

	addKey(KeyboardKey::NumPad_0		, 5, 0x3f);
	addKey(KeyboardKey::NumPad_1		, 5, 0x3c);
	addKey(KeyboardKey::NumPad_2		, 6, 0x12);
	addKey(KeyboardKey::NumPad_3		, 6, 0x24);
	addKey(KeyboardKey::NumPad_4		, 5, 0x39);
	addKey(KeyboardKey::NumPad_5		, 6, 0x0f);
	addKey(KeyboardKey::NumPad_6		, 6, 0x21);
	addKey(KeyboardKey::NumPad_7		, 5, 0x36);
	addKey(KeyboardKey::NumPad_8		, 6, 0x0c);
	addKey(KeyboardKey::NumPad_9		, 6, 0x1e);

	addKey(KeyboardKey::NumPad_Plus		, 6, 0x30);
	addKey(KeyboardKey::NumPad_Minus	, 6, 0x2d);
	addKey(KeyboardKey::NumPad_Multiply	, 6, 0x1b);
	addKey(KeyboardKey::NumPad_Divide	, 6, 0x09);
	addKey(KeyboardKey::NumPad_Dot		, 6, 0x27);
	addKey(KeyboardKey::NumPad_Enter	, 6, 0x39);

	addKey(KeyboardKey::NumPad_Extra1	, 5, 0x30);
	addKey(KeyboardKey::NumPad_Extra2	, 6, 0x06);
	addKey(KeyboardKey::NumPad_Extra3	, 6, 0x18);
	addKey(KeyboardKey::NumPad_Extra4	, 6, 0x2a);
}

void	Keyboard::addKeyTKL_ANSI()
{
	addKey(KeyboardKey::A, 0, 51);
	addKey(KeyboardKey::B, 2, 24);
	addKey(KeyboardKey::C, 1, 48);
	addKey(KeyboardKey::D, 1, 27);
	addKey(KeyboardKey::E, 1, 24);
	addKey(KeyboardKey::F, 1, 45);
	addKey(KeyboardKey::G, 1, 63);
	addKey(KeyboardKey::H, 2, 21);
	addKey(KeyboardKey::I, 2, 54);
	addKey(KeyboardKey::J, 2, 39);
	addKey(KeyboardKey::K, 2, 57);
	addKey(KeyboardKey::L, 3, 15);
	addKey(KeyboardKey::M, 2, 60);
	addKey(KeyboardKey::N, 2, 42);
	addKey(KeyboardKey::O, 3, 12);
	addKey(KeyboardKey::P, 3, 30);
	addKey(KeyboardKey::Q, 0, 48);
	addKey(KeyboardKey::R, 1, 42);
	addKey(KeyboardKey::S, 1,  9);
	addKey(KeyboardKey::T, 1, 60);
	addKey(KeyboardKey::U, 2, 36);
	addKey(KeyboardKey::V, 2,  6);
	addKey(KeyboardKey::W, 1,  6);
	addKey(KeyboardKey::X, 1, 30);
	addKey(KeyboardKey::Y, 2, 18);
	addKey(KeyboardKey::Z, 1, 12);

	addKey(KeyboardKey::Num_0, 3, 27);
	addKey(KeyboardKey::Num_1, 0, 45);
	addKey(KeyboardKey::Num_2, 0, 63);
	addKey(KeyboardKey::Num_3, 1, 21);
	addKey(KeyboardKey::Num_4, 1, 39);
	addKey(KeyboardKey::Num_5, 1, 57);
	addKey(KeyboardKey::Num_6, 2, 15);
	addKey(KeyboardKey::Num_7, 2, 33);
	addKey(KeyboardKey::Num_8, 2, 51);
	addKey(KeyboardKey::Num_9, 3,  9);

	addKey(KeyboardKey::Escape, 0, 24);

	addKey(KeyboardKey::Tilde		, 0, 27);
	addKey(KeyboardKey::Plus		, 3, 63);
	addKey(KeyboardKey::Minus		, 3, 45);
	addKey(KeyboardKey::BackSpace	, 4, 39);

	addKey(KeyboardKey::Tab			 , 0, 30);
	addKey(KeyboardKey::Bracket_Open , 3, 48);
	addKey(KeyboardKey::Bracket_Close, 4,  6);
	addKey(KeyboardKey::Backslash	 , 4, 42);

	addKey(KeyboardKey::CapsLock	, 0, 33);
	addKey(KeyboardKey::Colon		, 3, 33);
	addKey(KeyboardKey::Apostrophe	, 3, 51);
	addKey(KeyboardKey::Enter		, 4, 45);
	
	addKey(KeyboardKey::Shift_Left	, 0, 36);
	addKey(KeyboardKey::Shift_Right	, 4, 30);
	addKey(KeyboardKey::Comma		, 3, 18);
	addKey(KeyboardKey::Dot			, 3, 36);
	addKey(KeyboardKey::ForwardSlash, 3, 54);

	addKey(KeyboardKey::SpaceBar	 , 2, 27);
	addKey(KeyboardKey::Fn			 , 4, 33);
	addKey(KeyboardKey::Control_Left , 0, 39);
	addKey(KeyboardKey::Control_Right, 4, 51);
	addKey(KeyboardKey::Windows_Left , 0, 57);
	addKey(KeyboardKey::Windows_Right, 4, 15);
	addKey(KeyboardKey::Alt_Left	 , 1, 15);
	addKey(KeyboardKey::Alt_Right	 , 3, 39);

	addKey(KeyboardKey::Arrow_Up	, 5, 24);
	addKey(KeyboardKey::Arrow_Down	, 5, 27);
	addKey(KeyboardKey::Arrow_Left	, 5,  9);
	addKey(KeyboardKey::Arrow_Right	, 5, 45);

	addKey(KeyboardKey::Insert		, 4, 57);
	addKey(KeyboardKey::Delete		, 4, 60);
	addKey(KeyboardKey::Home		, 5, 15);
	addKey(KeyboardKey::End			, 5, 18);
	addKey(KeyboardKey::PageUp		, 5, 33);
	addKey(KeyboardKey::PageDown	, 5, 36);

	addKey(KeyboardKey::PrintScreen	, 4, 54);
	addKey(KeyboardKey::ScrollLock	, 5, 12);
	addKey(KeyboardKey::Pause		, 5, 30);

	addKey(KeyboardKey::F1			, 0, 60);
	addKey(KeyboardKey::F2			, 1, 18);
	addKey(KeyboardKey::F3			, 1, 36);
	addKey(KeyboardKey::F4			, 1, 54);
	addKey(KeyboardKey::F5			, 2, 30);
	addKey(KeyboardKey::F6			, 2, 48);
	addKey(KeyboardKey::F7			, 3,  6);
	addKey(KeyboardKey::F8			, 3, 24);
	addKey(KeyboardKey::F9			, 3, 42);
	addKey(KeyboardKey::F10			, 3, 60);
	addKey(KeyboardKey::F11			, 4, 18);
	addKey(KeyboardKey::F12			, 4, 36);
	
	m_layout= KeyboardLayout::TKL_ANSI;
}

void	Keyboard::addKeyFull_ANSI()
{
	addKeyTKL_ANSI();
	addKeyNumPad();
	m_layout= KeyboardLayout::Full_ANSI;
}

void	Keyboard::registerKeyPressCallback(  KeyBoardKeyPressCallback callback, void* userPtr)
{
	m_keyPressCallback.add({callback, userPtr});
}

void	Keyboard::deregisterKeyPressCallback(KeyBoardKeyPressCallback callback, void* userPtr)
{
	m_keyPressCallback.remove({callback, userPtr});
}

void	Keyboard::keyStateChanged(KeyboardKey key, bool isPressed)
{
	if (m_isPressed.setKeyIsOn(key, isPressed))
	{
		int numCB= m_keyPressCallback.num();
		for(int i=0; i<numCB; ++i)
		{
			KeyPressCallback& CB= m_keyPressCallback[i];
			CB.callback(this, key, isPressed, CB.userPtr);
		}
	}
}

bool	Keyboard::isAnyKeyDown(const KeyboardState& key)const
{
	return m_isPressed.isAnyKeyOn(key);
}

bool	Keyboard::isExactKeyDown(const KeyboardState& key)const
{
	return m_isPressed == key;
}

#undef REPORT_ID
