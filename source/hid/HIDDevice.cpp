
// by simon yeung, 13/10/2020
// all rights reserved

#include "HIDDevice.h"
#include "HID.h"
#include "HIDDef.h"
#include <SetupAPI.h>
#include <guiddef.h>
#include <assert.h>
#include <stdlib.h>

static const GUID GUID_DEVINTERFACE_HID= { 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };

HIDDevice*		HIDDevice::createDevice(unsigned short vendorID, unsigned short productID, int interfaceIdx, unsigned short usagePage, unsigned short usage)
{
	HIDDevice*	device= nullptr;

	SP_DEVINFO_DATA						devInfoData;
	SP_DEVICE_INTERFACE_DATA			devInterfaceData;
	
	memset(&devInfoData, 0x0, sizeof(devInfoData));
	devInfoData.cbSize		= sizeof(SP_DEVINFO_DATA);
	devInterfaceData.cbSize	= sizeof(SP_DEVICE_INTERFACE_DATA);
	
	// get all HID class info
	HDEVINFO devInfoSet = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_HID, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (devInfoSet == nullptr)
		return nullptr;
	
	// iterate devices
	BOOL isStillHvDevice= true;
	for(int devIdx = 0; isStillHvDevice; ++devIdx)
	{
		// get the device
		isStillHvDevice= SetupDiEnumDeviceInterfaces(devInfoSet, nullptr, &GUID_DEVINTERFACE_HID, devIdx, &devInterfaceData);
		if (!isStillHvDevice)
			break;
		
		// get requiredSize of detail data
		DWORD requiredSize = 0;
		SetupDiGetDeviceInterfaceDetailA(devInfoSet, &devInterfaceData, nullptr, 0, &requiredSize, nullptr);
		
		// create device detail data
		SP_DEVICE_INTERFACE_DETAIL_DATA_A*	devInterfaceDetailData	= (SP_DEVICE_INTERFACE_DETAIL_DATA_A*) malloc(requiredSize);
		assert(devInterfaceDetailData);
		devInterfaceDetailData->cbSize								= sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

		// get device path
		bool isOk= SetupDiGetDeviceInterfaceDetailA(devInfoSet, &devInterfaceData, devInterfaceDetailData, requiredSize, nullptr, nullptr);
		if (!isOk)
		{
			free(devInterfaceDetailData);
			continue;
		}

		// find device that is HIDClass and have driver
		bool hasDriver= false;
		for(int infoIdx= 0; isOk && !hasDriver; ++infoIdx)
		{
			char propertyName[256];

			// get device class name
			isOk= SetupDiEnumDeviceInfo(devInfoSet, infoIdx, &devInfoData);
			if (isOk)
				isOk= SetupDiGetDeviceRegistryPropertyA(devInfoSet, &devInfoData, SPDRP_CLASS, nullptr, (PBYTE)propertyName, sizeof(propertyName), nullptr);
			
			// get device driver
			if (isOk && (	(strcmp(propertyName, "HIDClass") == 0) ||
							(strcmp(propertyName, "Mouse"	) == 0) ||
							(strcmp(propertyName, "Keyboard") == 0)	)	)
				hasDriver= SetupDiGetDeviceRegistryPropertyA(devInfoSet, &devInfoData, SPDRP_DRIVER, nullptr, (PBYTE)propertyName, sizeof(propertyName), nullptr);
		}

		// open the device to get the vendor ID and product ID
		if (hasDriver)
		{
			// check is interface idx ok
			if (interfaceIdx != -1)
				isOk= interfaceIdx == devicePathToInterfaceIdx(devInterfaceDetailData->DevicePath);

			// open the device
			HANDLE devHandle = INVALID_HANDLE_VALUE;
			if (isOk)
				devHandle = CreateFileA(devInterfaceDetailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

			if (devHandle != INVALID_HANDLE_VALUE)
			{
				// get vendor ID and product ID
				HIDD_ATTRIBUTES attrib;
				attrib.Size = sizeof(HIDD_ATTRIBUTES);
				HID::getAttributes(devHandle, &attrib);
				isOk=	 vendorID == attrib.VendorID					&&
						(productID== attrib.ProductID || productID == 0);
				
				// get preparsed data
				PHIDP_PREPARSED_DATA	pp_data = nullptr;
				HIDP_CAPS				caps;
				if (isOk)
					isOk = HID::getPreparsedData(devHandle, &pp_data);
				
				// get usage page and usage
				if (isOk && (isOk= HID::getCaps(pp_data, &caps)))
				{
					isOk=	(usagePage	== 0xffff || usagePage	== caps.UsagePage	) &&
							(usage		== 0xffff || usage		== caps.Usage		);
				}
				if (pp_data)
					HID::freePreparsedData(pp_data);

				// create HIDDevice if all conditions are ok
				if (isOk)
				{
					// set size of input buffer
					HID::setNumInputBuffers(devHandle, 64);

					// get interface index if user doesnt specify
					if (interfaceIdx == -1)	
						interfaceIdx= devicePathToInterfaceIdx(devInterfaceDetailData->DevicePath);

					// create device
					device= new HIDDevice(devHandle, attrib.VendorID, attrib.ProductID, interfaceIdx, caps.InputReportByteLength, caps.OutputReportByteLength);

					// already found suitable device, stop iterate
					isStillHvDevice= false;
				}
				else
					CloseHandle(devHandle);
			}
		}
		free(devInterfaceDetailData);
	}
	
	// free all HID class info
	SetupDiDestroyDeviceInfoList(devInfoSet);
	return device;
}

int	HIDDevice::devicePathToInterfaceIdx(const char* path)
{
	// find interface string
	const char* interfaceStr= strstr(path, "&mi_");
	if (interfaceStr == nullptr)
		return -1;	// fail to find

	// skip &mi_ text
	interfaceStr+= 4;

	// parse str
	char* endPtr= nullptr;
	int idx= strtol(interfaceStr, &endPtr, 16);
	if (interfaceStr == endPtr)
		return -1;	// failed to parse

	return idx;
}

void				HIDDevice::destroyDevice(HIDDevice* device)
{
	delete device;
}

HIDDevice::HIDDevice(HANDLE deviceHandle, unsigned short vendorID, unsigned short productID, int interfaceIdx, unsigned short inputReportLength, unsigned short outputReportLength)
{
	m_deviceHandle		= deviceHandle		;
	m_vendorID			= vendorID			;
	m_productID			= productID			;
	m_interfaceID		= interfaceIdx		;
	m_inputReportLength	= inputReportLength	;
	m_outputReportLength= outputReportLength;
		
	memset(&m_overlap, 0, sizeof(m_overlap));
	m_overlap.hEvent = CreateEvent(nullptr, false, false, nullptr);
}

HIDDevice::~HIDDevice()
{
	CloseHandle(m_overlap.hEvent);
	CloseHandle(m_deviceHandle);
	int numOverlaps= m_overlaps.num();
	for(int i=0; i<numOverlaps; ++i)
		CloseHandle(m_overlaps[i].hEvent);
	m_overlaps.clear();
	m_handles.clear();
}

int	HIDDevice::write(const unsigned char *data, int dataNumByte)
{
#if DEBUG_DISABLE_HID_COMM
	return 0;
#else
	
	assert(dataNumByte <= m_outputReportLength);
	bool isOk= WriteFile(m_deviceHandle, data, dataNumByte, nullptr, &m_overlap);
	if (!isOk)
	{
		if (GetLastError() != ERROR_IO_PENDING) 
			return -1;
	}
	
	DWORD numByteWritten= 0;
	isOk= GetOverlappedResult(m_deviceHandle, &m_overlap, &numByteWritten, true);
	if (!isOk)
		return -1;
	return numByteWritten;
#endif
}

int HIDDevice::read(unsigned char *data, int dataNumByte)
{
#if DEBUG_DISABLE_HID_COMM
	return 0;
#else
	assert(dataNumByte >= m_inputReportLength);
	DWORD numByteRead= 0;
	bool isOk= ReadFile(m_deviceHandle, data, dataNumByte, &numByteRead, &m_overlap);
	
	if (!isOk && GetLastError() != ERROR_IO_PENDING)
	{
		// return fail
		CancelIo(m_deviceHandle);
		return -1;
	}
	
	isOk = GetOverlappedResult(m_deviceHandle, &m_overlap, &numByteRead, true);
	if (!isOk)
		return -1;

	return numByteRead;
#endif
}

bool HIDDevice::write_read(	unsigned char* write_data[], unsigned char* read_data[], int numData, int dataNumByte)
{
	assert(dataNumByte <= m_outputReportLength);

	// create OVERLAPPED struct
	int numWriteReadOp= numData * 2;
	m_overlaps.reserve(numWriteReadOp);
	int numOverlaps= m_overlaps.num();
	for(int i=numOverlaps; i<numWriteReadOp; ++i)
	{
		OVERLAPPED ov;
		memset(&ov, 0, sizeof(ov));
		ov.hEvent = CreateEvent(nullptr, false, false, nullptr);
		m_overlaps.add(ov);
	}
	
	// create handles array for wait operation complete
	m_handles.reserve( numWriteReadOp);
	m_handles.clear(true);
	for(int i=0; i<numWriteReadOp; ++i)
		m_handles.add(m_overlaps[i].hEvent);

	// perform all write/read request
	for(int i=0; i<numData; ++i)
	{
		// write
		bool isOk= WriteFile(m_deviceHandle, write_data[i], dataNumByte, nullptr, &m_overlaps[i]);
		if (!isOk)
		{
			if (GetLastError() != ERROR_IO_PENDING) 
				return false;
		}

		// read
		DWORD numByteRead= 0;
		isOk= ReadFile(m_deviceHandle, read_data[i], dataNumByte, &numByteRead, &m_overlaps[numData + i]);
		DWORD err;
		if (!isOk && (err= GetLastError()) != ERROR_IO_PENDING)
		{
			// return fail
			CancelIo(m_deviceHandle);
			return false;
		}

//		HANDLE h[2];
//		h[0]= m_overlaps[i].hEvent;
//		h[1]= m_overlaps[numData + i].hEvent;
//		WaitForMultipleObjects(2, h, true, INFINITE);
	}
	
	// wait for all operation to be completed
	WaitForMultipleObjects(numWriteReadOp, m_handles.getData(), true, INFINITE);


	
//	for(int i=0; i<numData; ++i)
//	{
//		// write
//		bool isOk= WriteFile(m_deviceHandle, write_data[i], dataNumByte, nullptr, &m_overlaps[i]);
//		if (!isOk)
//		{
//			if (GetLastError() != ERROR_IO_PENDING) 
//				return false;
//		}
//	}
//	
//	WaitForMultipleObjects(numData, m_handles.getData(), true, INFINITE);
//
//	for(int i=0; i<numData; ++i)
//	{
//		// read
//		DWORD numByteRead= 0;
//		bool isOk= ReadFile(m_deviceHandle, read_data[i], dataNumByte, &numByteRead, &m_overlaps[numData + i]);
//		DWORD err;
//		if (!isOk && (err= GetLastError()) != ERROR_IO_PENDING)
//		{
//			// return fail
//			CancelIo(m_deviceHandle);
//			return false;
//		}
//	}
//
//	WaitForMultipleObjects(numData, m_handles.getData() + numData, true, INFINITE);

	

	// get read result
	for(int i=0; i<numData; ++i)
	{
		DWORD numByteRead= 0;
		GetOverlappedResult(m_deviceHandle, &m_overlaps[numData + i], &numByteRead, true);
	}

	return true;
}
