
// by simon yeung, 10/10/2020
// all rights reserved

#include <assert.h>
#include <stdio.h>
#include "Keyboard.h"
#include "math/Math.h"
#include "hid/HIDDevice.h"

#define REPORT_ID	0x00

Keyboard::Keyboard() : USBDevice()
{
	m_packets				= nullptr;
	m_packetNum				= 0;
	m_layout				= KeyboardLayout::Unknown;
	m_isKeyColorReadPacket	= true;
	m_isPressed.setAllOff();
	reset();
}

Keyboard::Keyboard(HIDDevice* device) : USBDevice(device)
{
	m_packets				= nullptr;
	m_packetNum				= 0;
	m_layout				= KeyboardLayout::Unknown;
	m_isKeyColorReadPacket	= true;
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
	for(int i=0; i<(int)InputKey::Num; ++i)
		m_keys[i].packetIdx= 255;
}

void	Keyboard::addKey(InputKey key, unsigned char colorPacketIdx, unsigned char packetOffset)
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

void	Keyboard::setKeyColor(InputKey key, unsigned char R, unsigned char G, unsigned char B)
{
	const KeyData& keyData= m_keys[(int)key];
	int packetIdxR= keyData.packetIdx;
	if (packetIdxR == 255)
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

void	Keyboard::commitKeyColor(RenderTarget* renderTarget)
{
#if 1
	static unsigned char recvData[sizeof(Packet::data)];

	for(int i=0; i<m_packetNum; ++i)
	{
		// send packet
		const Packet& pkt= m_packets[i];
		int bytes_written= m_device->write(pkt.data, sizeof(Packet::data));

		if (m_isKeyColorReadPacket)
		{
#if !DEBUG_DISABLE_HID_COMM
			if (sizeof(Packet::data) != bytes_written)
				printf("Commit send color error [%i]    %i\n", i, bytes_written);
			else
			{
				// recv ack packet
				int numRead= m_device->read(recvData, sizeof(recvData));
				if (!(numRead == 65 && recvData[1] == pkt.data[1] && recvData[2] == pkt.data[2]) )
				{
					printf("Commit recv color error [%i]    %i\n", i, numRead);

#if 0				// detail debug message
					for(int i=0; i<numRead; ++i)
						printf("0x%02x ", ((char*)recvData)[i]);
					printf("\n\n");
#endif
				}
#endif
			}
		}
	}


#else
	static_assert(false, "handle m_isKeyColorReadPacket");
	// experimental code, it runs faster on Ducky One2, but
	// unfortunately, Drop Shift will hang...
	const int maxPackets= 13;
	assert(m_packetNum < maxPackets);
	static unsigned char* recvData[maxPackets];
	static unsigned char recvDataArray[sizeof(Packet::data) * maxPackets];
	for(int i=0; i<m_packetNum; ++i)
		recvData[i]= recvDataArray + i * sizeof(Packet::data);
	
	static unsigned char* sendData[maxPackets];
	for(int i=0; i<m_packetNum; ++i)
		sendData[i]= m_packets[i].data;

	m_device->write_read( sendData, recvData, m_packetNum, sizeof(Packet::data));
#endif
}

void	Keyboard::addKeyNumPad()
{
	// code not tested, reference from DuckyAPI
	addKey(InputKey::NumLock		, 5, 0x33);

	addKey(InputKey::NumPad_0		, 5, 0x3f);
	addKey(InputKey::NumPad_1		, 5, 0x3c);
	addKey(InputKey::NumPad_2		, 6, 0x12);
	addKey(InputKey::NumPad_3		, 6, 0x24);
	addKey(InputKey::NumPad_4		, 5, 0x39);
	addKey(InputKey::NumPad_5		, 6, 0x0f);
	addKey(InputKey::NumPad_6		, 6, 0x21);
	addKey(InputKey::NumPad_7		, 5, 0x36);
	addKey(InputKey::NumPad_8		, 6, 0x0c);
	addKey(InputKey::NumPad_9		, 6, 0x1e);

	addKey(InputKey::NumPad_Plus	, 6, 0x30);
	addKey(InputKey::NumPad_Minus	, 6, 0x2d);
	addKey(InputKey::NumPad_Multiply, 6, 0x1b);
	addKey(InputKey::NumPad_Divide	, 6, 0x09);
	addKey(InputKey::NumPad_Dot		, 6, 0x27);
	addKey(InputKey::NumPad_Enter	, 6, 0x39);

	addKey(InputKey::NumPad_Extra1	, 5, 0x30);
	addKey(InputKey::NumPad_Extra2	, 6, 0x06);
	addKey(InputKey::NumPad_Extra3	, 6, 0x18);
	addKey(InputKey::NumPad_Extra4	, 6, 0x2a);
}

void	Keyboard::addKeyTKL_ANSI()
{
	addKey(InputKey::A, 0, 51);
	addKey(InputKey::B, 2, 24);
	addKey(InputKey::C, 1, 48);
	addKey(InputKey::D, 1, 27);
	addKey(InputKey::E, 1, 24);
	addKey(InputKey::F, 1, 45);
	addKey(InputKey::G, 1, 63);
	addKey(InputKey::H, 2, 21);
	addKey(InputKey::I, 2, 54);
	addKey(InputKey::J, 2, 39);
	addKey(InputKey::K, 2, 57);
	addKey(InputKey::L, 3, 15);
	addKey(InputKey::M, 2, 60);
	addKey(InputKey::N, 2, 42);
	addKey(InputKey::O, 3, 12);
	addKey(InputKey::P, 3, 30);
	addKey(InputKey::Q, 0, 48);
	addKey(InputKey::R, 1, 42);
	addKey(InputKey::S, 1,  9);
	addKey(InputKey::T, 1, 60);
	addKey(InputKey::U, 2, 36);
	addKey(InputKey::V, 2,  6);
	addKey(InputKey::W, 1,  6);
	addKey(InputKey::X, 1, 30);
	addKey(InputKey::Y, 2, 18);
	addKey(InputKey::Z, 1, 12);

	addKey(InputKey::Num_0, 3, 27);
	addKey(InputKey::Num_1, 0, 45);
	addKey(InputKey::Num_2, 0, 63);
	addKey(InputKey::Num_3, 1, 21);
	addKey(InputKey::Num_4, 1, 39);
	addKey(InputKey::Num_5, 1, 57);
	addKey(InputKey::Num_6, 2, 15);
	addKey(InputKey::Num_7, 2, 33);
	addKey(InputKey::Num_8, 2, 51);
	addKey(InputKey::Num_9, 3,  9);

	addKey(InputKey::Escape, 0, 24);

	addKey(InputKey::Tilde			, 0, 27);
	addKey(InputKey::Plus			, 3, 63);
	addKey(InputKey::Minus			, 3, 45);
	addKey(InputKey::BackSpace		, 4, 39);

	addKey(InputKey::Tab			, 0, 30);
	addKey(InputKey::Bracket_Open	, 3, 48);
	addKey(InputKey::Bracket_Close	, 4,  6);
	addKey(InputKey::Backslash		, 4, 42);

	addKey(InputKey::CapsLock		, 0, 33);
	addKey(InputKey::Colon			, 3, 33);
	addKey(InputKey::Apostrophe		, 3, 51);
	addKey(InputKey::Enter			, 4, 45);
	
	addKey(InputKey::Shift_Left		, 0, 36);
	addKey(InputKey::Shift_Right	, 4, 30);
	addKey(InputKey::Comma			, 3, 18);
	addKey(InputKey::Dot			, 3, 36);
	addKey(InputKey::ForwardSlash	, 3, 54);

	addKey(InputKey::SpaceBar		, 2, 27);
	addKey(InputKey::Fn				, 4, 33);
	addKey(InputKey::Control_Left	, 0, 39);
	addKey(InputKey::Control_Right	, 4, 51);
	addKey(InputKey::Windows_Left	, 0, 57);
	addKey(InputKey::Windows_Right	, 4, 15);
	addKey(InputKey::Alt_Left		, 1, 15);
	addKey(InputKey::Alt_Right		, 3, 39);

	addKey(InputKey::Arrow_Up		, 5, 24);
	addKey(InputKey::Arrow_Down		, 5, 27);
	addKey(InputKey::Arrow_Left		, 5,  9);
	addKey(InputKey::Arrow_Right	, 5, 45);

	addKey(InputKey::Insert			, 4, 57);
	addKey(InputKey::Delete			, 4, 60);
	addKey(InputKey::Home			, 5, 15);
	addKey(InputKey::End			, 5, 18);
	addKey(InputKey::PageUp			, 5, 33);
	addKey(InputKey::PageDown		, 5, 36);

	addKey(InputKey::PrintScreen	, 4, 54);
	addKey(InputKey::ScrollLock		, 5, 12);
	addKey(InputKey::Pause			, 5, 30);

	addKey(InputKey::F1				, 0, 60);
	addKey(InputKey::F2				, 1, 18);
	addKey(InputKey::F3				, 1, 36);
	addKey(InputKey::F4				, 1, 54);
	addKey(InputKey::F5				, 2, 30);
	addKey(InputKey::F6				, 2, 48);
	addKey(InputKey::F7				, 3,  6);
	addKey(InputKey::F8				, 3, 24);
	addKey(InputKey::F9				, 3, 42);
	addKey(InputKey::F10			, 3, 60);
	addKey(InputKey::F11			, 4, 18);
	addKey(InputKey::F12			, 4, 36);
	
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

void	Keyboard::keyStateChanged(InputKey key, bool isPressed)
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
