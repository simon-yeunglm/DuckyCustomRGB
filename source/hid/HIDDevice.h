#pragma once

// by simon yeung, 13/10/2020
// all rights reserved

#include "../win.h"
#include "collection/Array.h"

#define DEBUG_DISABLE_HID_COMM		(0)

typedef void*	HANDLE;

class HIDDevice
{
private:
	HANDLE				m_deviceHandle;
	unsigned short		m_vendorID;
	unsigned short		m_productID;
	int					m_interfaceID;
	unsigned short		m_inputReportLength;
	unsigned short		m_outputReportLength;
	OVERLAPPED			m_overlap;
	Array<OVERLAPPED>	m_overlaps;
	Array<HANDLE>		m_handles;

	HIDDevice()= default;
	HIDDevice(HANDLE deviceHandle, unsigned short vendorID, unsigned short productID, int interfaceIdx, unsigned short inputReportLength, unsigned short outputReportLength);
	~HIDDevice();

	static int	devicePathToInterfaceIdx(const char* path);

public:
	static HIDDevice*	createDevice(unsigned short vendorID, unsigned short productID= 0, int interfaceIdx= -1, unsigned short usagePage= 0xffff, unsigned short usage= 0xffff);
	static void			destroyDevice(HIDDevice* device);

	int	write(const unsigned char *data, int dataNumByte);
	int read(		unsigned char *data, int dataNumByte);
	
	// perform write/read operation simultaneously, assume write_data[] and read_data[] are of same size (i.e. numData) and same number of byte for each write/read operation (i.e.)dataNumByte
	// return if there are any errors
	bool write_read(	unsigned char* write_data[], unsigned char* read_data[], int numData, int dataNumByte);
	
	inline unsigned short	getProductID()const		{ return m_productID; }
};
