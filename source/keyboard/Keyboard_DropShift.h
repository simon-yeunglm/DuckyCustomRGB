#pragma once

// by simon yeung, 12/12/2021
// all rights reserved

#include "Keyboard.h"

struct sRawp_command_t;
struct sRawp_response_t;
class Keyboard_DropShift : public Keyboard
{
private:
	void	addKey_1800_ANSI();

	bool	sendMessage(const sRawp_command_t& msg);
	bool	recvMessage(sRawp_response_t& msg, uint8_t cmd);

public:
	static unsigned short productID;

	Keyboard_DropShift(HIDDevice* device);
	virtual ~Keyboard_DropShift();
	
	virtual void	addKey(InputKey key, unsigned char colorPacketIdx, unsigned char packetOffset);
	virtual void	endAddKey();

	virtual void	setUpPacketHeaders();
	
	virtual void	commitKeyColor(RenderTarget* renderTarget) override;

	virtual void	connect()override;
	virtual void	disconnect()override;
};
