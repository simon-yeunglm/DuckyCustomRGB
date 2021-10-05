#pragma once

// by simon yeung, 27/03/2021
// all rights reserved

class HIDDevice;

class USBDevice
{
protected:
	HIDDevice*				m_device;
	int						m_handledPacketNum;
	
	USBDevice();

	void	handlePacket(const void* packetBytes, int numBytes);
	void	wait(int time_ms);

public:
	USBDevice(HIDDevice* device);
	virtual ~USBDevice();
	
	virtual void	connect()= 0;
	virtual void	disconnect()= 0;
};
