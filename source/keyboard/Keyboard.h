#pragma once

// by simon yeung, 10/10/2020
// all rights reserved

#include "KeyboardState.h"
#include "usb/USBDevice.h"
#include "collection/Array.h"

class Keyboard;
struct RenderTarget;

typedef void (*KeyBoardKeyPressCallback)(Keyboard* keyboard, InputKey key, bool isPressed, void* userPtr);

enum class KeyboardLayout
{
	Unknown,
	TKL_ANSI,
	TKL_ISO,
	TKL_JIS,
	Full_ANSI,
	Full_ISO,
	Full_JIS,
	_1800_ANSI
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
		unsigned	char	packetIdx;		// including the header packet, 255 => not used key
		unsigned	char	packetOffset;	// byte offset from the packet start, including the Report ID
	};

	struct Packet
	{
		unsigned char	data[65];	// [0] is Report ID
	};

	KeyData			m_keys[(int)InputKey::Num];
	signed char		m_maxColorPacketIdx;
	Packet*			m_packets;	// including header and footer packets
	int				m_packetNum;
	int				m_packetIdxOffset;
	KeyboardLayout	m_layout;
	KeyboardState	m_isPressed;
	
	Keyboard();

	virtual void	beginAddKey();
	virtual void	addKey(InputKey key, unsigned char colorPacketIdx, unsigned char packetOffset);
	virtual void	endAddKey();

	void			reset();
	virtual void	setUpPacketHeaders();
	
	void	addKeyNumPad();
	void	addKeyTKL_ANSI();
	void	addKeyFull_ANSI();
	
public:
	Keyboard(HIDDevice* device);
	virtual ~Keyboard();
	
	// set key color, but not send to keyboard yet
	void	setKeyColor(InputKey key, unsigned char R, unsigned char G, unsigned char B);

	// send all key color to keyboard
	virtual void commitKeyColor(RenderTarget* renderTarget);
	
	void	registerKeyPressCallback(  KeyBoardKeyPressCallback callback, void* userPtr);
	void	deregisterKeyPressCallback(KeyBoardKeyPressCallback callback, void* userPtr);
	
	void	keyStateChanged(InputKey key, bool isPressed);
	inline KeyboardLayout	getLayout() { return m_layout; }
	inline bool				isKeyDown(InputKey key)const	{ return m_isPressed.isKeyOn(key); }
	bool					isAnyKeyDown(const KeyboardState& key)const;
	bool					isExactKeyDown(const KeyboardState& key)const;
};
