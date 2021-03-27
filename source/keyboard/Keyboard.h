#pragma once

// by simon yeung, 10/10/2020
// all rights reserved

#include "KeyboardState.h"
#include "usb/USBDevice.h"
#include "collection/Array.h"

class Keyboard;

typedef void (*KeyBoardKeyPressCallback)(Keyboard* keyboard, KeyboardKey key, bool isPressed, void* userPtr);

enum class KeyboardLayout
{
	Unknown,
	TKL_ANSI,
	TKL_ISO,
	TKL_JIS,
	Full_ANSI,
	Full_ISO,
	Full_JIS,
};

class Keyboard : public USBDevice
{
private:
	struct KeyPressCallback
	{
		KeyBoardKeyPressCallback	callback;
		void*						userPtr;
		
		inline bool	operator== (const KeyPressCallback& RHS)
		{
			return	callback== RHS.callback &&
					userPtr	== RHS.userPtr;
		}
	};

	Array<KeyPressCallback>	m_keyPressCallback;

protected:
	struct KeyData
	{
		unsigned	char	packetIdx;		// including the header packet, 0 => not used key
		unsigned	char	packetOffset;	// byte offset from the packet start, including the Report ID
	};

	struct Packet
	{
		unsigned char	data[65];	// [0] is Report ID
	};

	KeyData			m_keys[(int)KeyboardKey::Num];
	signed char		m_maxColorPacketIdx;
	Packet*			m_packets;	// including header and footer packets
	int				m_packetNum;
	KeyboardLayout	m_layout;
	KeyboardState	m_isPressed;

	void	beginAddKey();
	void	addKey(KeyboardKey key, unsigned char colorPacketIdx, unsigned char packetOffset);
	void	endAddKey();

	void	reset();
	void	setUpPacketHeaders();
	
	void	addKeyNumPad();
	void	addKeyTKL_ANSI();
	void	addKeyFull_ANSI();
	
public:
	Keyboard(HIDDevice* device);
	virtual ~Keyboard();
	
	// set key color, but not send to keyboard yet
	void	setKeyColor(KeyboardKey key, unsigned char R, unsigned char G, unsigned char B);

	// send all key color to keyboard
	void	commitKeyColor();
	
	void	registerKeyPressCallback(  KeyBoardKeyPressCallback callback, void* userPtr);
	void	deregisterKeyPressCallback(KeyBoardKeyPressCallback callback, void* userPtr);
	
	void	keyStateChanged(KeyboardKey key, bool isPressed);
	inline KeyboardLayout	getLayout() { return m_layout; }
	inline bool				isKeyDown(KeyboardKey key)const	{ return m_isPressed.isKeyOn(key); }
	bool					isAnyKeyDown(const KeyboardState& key)const;
	bool					isExactKeyDown(const KeyboardState& key)const;
};
