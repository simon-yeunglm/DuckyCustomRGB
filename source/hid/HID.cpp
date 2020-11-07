
// by simon yeung, 13/10/2020
// all rights reserved

#include "HID.h"
#include "HIDDef.h"
#include "../win.h"
#define HIDP_STATUS_SUCCESS (0x11 << 16)
typedef LONG NTSTATUS;  // NTSTATUS is a 32 bit value

#define LOAD_FUNC(func_name)    func_name= (func_##func_name)GetProcAddress(dll_handle, #func_name); if (func_name == nullptr) return false;

typedef BOOLEAN (__stdcall *func_HidD_GetAttributes      )(HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes);
typedef BOOLEAN (__stdcall *func_HidD_GetPreparsedData   )(HANDLE HidDeviceObject, PHIDP_PREPARSED_DATA* PreparsedData);
typedef BOOLEAN (__stdcall *func_HidD_FreePreparsedData  )(PHIDP_PREPARSED_DATA PreparsedData);
typedef NTSTATUS(__stdcall *func_HidP_GetCaps            )(PHIDP_PREPARSED_DATA PreparsedData, PHIDP_CAPS Capabilities);
typedef BOOLEAN (__stdcall *func_HidD_SetNumInputBuffers )(HANDLE HidDeviceObject, ULONG NumberBuffers);

static func_HidD_GetAttributes      HidD_GetAttributes      = nullptr;
static func_HidD_GetPreparsedData   HidD_GetPreparsedData   = nullptr;
static func_HidD_FreePreparsedData  HidD_FreePreparsedData  = nullptr;
static func_HidP_GetCaps            HidP_GetCaps            = nullptr;
static func_HidD_SetNumInputBuffers HidD_SetNumInputBuffers = nullptr;
static HMODULE					    dll_handle              = nullptr;

static bool loadFunc()
{
    LOAD_FUNC(HidD_GetAttributes        );
    LOAD_FUNC(HidD_GetPreparsedData     );
    LOAD_FUNC(HidD_FreePreparsedData    );
    LOAD_FUNC(HidP_GetCaps              );
    LOAD_FUNC(HidD_SetNumInputBuffers   );
    return true;
}

void HID::init()
{
	dll_handle = LoadLibraryA("hid.dll");
	if (dll_handle == nullptr)
		return;
    if (!loadFunc())
        release();
}

void HID::release()
{
	if (dll_handle)
		FreeLibrary(dll_handle);
	dll_handle = nullptr;
}

bool HID::getAttributes      (HANDLE HidDeviceObject, HIDD_ATTRIBUTES* Attributes)
{
    return HidD_GetAttributes(HidDeviceObject, Attributes);
}

bool HID::getPreparsedData   (HANDLE HidDeviceObject, PHIDP_PREPARSED_DATA* PreparsedData)
{
    return HidD_GetPreparsedData(HidDeviceObject, PreparsedData);
}

bool HID::freePreparsedData  (PHIDP_PREPARSED_DATA PreparsedData)
{
    return HidD_FreePreparsedData(PreparsedData);
}

bool HID::getCaps            (PHIDP_PREPARSED_DATA PreparsedData, HIDP_CAPS* Capabilities)
{
    return HidP_GetCaps(PreparsedData, Capabilities) == HIDP_STATUS_SUCCESS;
}

bool HID::setNumInputBuffers (HANDLE HidDeviceObject, ULONG NumberBuffers)
{
    return HidD_SetNumInputBuffers(HidDeviceObject, NumberBuffers);
}
