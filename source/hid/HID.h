#pragma once

// by simon yeung, 13/10/2020
// all rights reserved

typedef unsigned long ULONG;
typedef void*	HANDLE;
typedef struct _HIDP_CAPS HIDP_CAPS;
typedef struct _HIDD_ATTRIBUTES HIDD_ATTRIBUTES;
typedef struct _HIDP_PREPARSED_DATA* PHIDP_PREPARSED_DATA;

class HID
{
public:
	static void init();
	static void release();

private:
	
	static bool getAttributes      (HANDLE HidDeviceObject, HIDD_ATTRIBUTES* Attributes);
	static bool getPreparsedData   (HANDLE HidDeviceObject, PHIDP_PREPARSED_DATA* PreparsedData);
	static bool freePreparsedData  (PHIDP_PREPARSED_DATA PreparsedData);
	static bool getCaps            (PHIDP_PREPARSED_DATA PreparsedData, HIDP_CAPS* Capabilities);
	static bool setNumInputBuffers (HANDLE HidDeviceObject, ULONG NumberBuffers);

	friend class HIDDevice;
};
