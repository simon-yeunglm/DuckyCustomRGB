
// by simon yeung, 27/03/2021
// all rights reserved

#include <assert.h>
#include <stdio.h>
#include "USBDevice.h"
#include "hid/HIDDevice.h"
#include "USBPcap.h"

#define REPORT_ID	0x00

USBDevice::USBDevice(HIDDevice* device)
{
	assert( device != nullptr );
	m_device			= device;
	m_handledPacketNum	= 0;
}

USBDevice::~USBDevice()
{
}

void	USBDevice::handlePacket(const void* packetBytes, int numBytes)
{
	++m_handledPacketNum;

	const int dataBufferSize= 65;	// first byte is Report ID
	static unsigned char data[dataBufferSize];

	assert( numBytes >= sizeof(USBPCAP_BUFFER_PACKET_HEADER) );
	bool hasLeftOverData= numBytes > sizeof(USBPCAP_BUFFER_PACKET_HEADER);
	if (!hasLeftOverData)
		return;	// no bytes to send

	USBPCAP_BUFFER_PACKET_HEADER*	header		= (USBPCAP_BUFFER_PACKET_HEADER*)packetBytes;
	unsigned char*					packetData	= ((unsigned char*)packetBytes) + sizeof(USBPCAP_BUFFER_PACKET_HEADER);

	bool isPDOtoFDO		= header->info == 0x01;
	bool isDirectionIn	= (header->endpoint & 0x80) != 0;
	if (isDirectionIn)
	{
		assert(isPDOtoFDO);

		// recv packets from keyboard
		int numRead= m_device->read(data, dataBufferSize);
#if !DEBUG_DISABLE_HID_COMM
#ifdef _DEBUG
		if (numRead > 0)
		{
			--numRead;	// remove Report ID
			int cmpResult= memcmp(data+1, packetData, numRead);
			if (cmpResult != 0)
			{
				printf("Unexpected recv packet %i    %i\n", m_handledPacketNum, numRead);

#if 0	// detail debug message
				for(int i=0; i<numRead; ++i)
					printf("0x%02x ", ((char*)packetData)[i]);
				printf("\n\n");
					
				for(int i=1; i<=numRead; ++i)
					printf("0x%02x ", ((char*)data)[i]);
				printf("\n\n");
#endif
			}
		}
#endif
#endif
	}
	else
	{
		assert(!isPDOtoFDO);

		// send packets to keyboard
		data[0]= REPORT_ID;	// Report ID for HID descriptor

		int packetSz= header->dataLength;
		assert(packetSz <= (dataBufferSize - 1));
		memcpy(data+1, packetData, packetSz);

		++packetSz;	// add Report ID length
		int bytes_written= m_device->write(data, packetSz);
#if !DEBUG_DISABLE_HID_COMM
		if (packetSz != bytes_written)
			printf("Unexpected end packet %i    %i\n", m_handledPacketNum, bytes_written);
#endif
	}	
}

void	USBDevice::wait(int time_ms)
{
#if !DEBUG_DISABLE_HID_COMM
	Sleep(time_ms);
#endif
}

#undef REPORT_ID