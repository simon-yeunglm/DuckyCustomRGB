#pragma once

// by simon yeung, 13/10/2020
// all rights reserved

// define struct to avoid DDK installation
//#include <ntstatus.h>
//#include <hidusage.h>
//#include <hidsdi.h>
//#include <hidpi.h>
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef USHORT USAGE, *PUSAGE;
typedef struct _HIDP_PREPARSED_DATA* PHIDP_PREPARSED_DATA;

#define HID_USAGE_PAGE_GENERIC          ((USAGE) 0x01)
#define HID_USAGE_GENERIC_POINTER       ((USAGE) 0x01)
#define HID_USAGE_GENERIC_MOUSE         ((USAGE) 0x02)
#define HID_USAGE_GENERIC_JOYSTICK      ((USAGE) 0x04)
#define HID_USAGE_GENERIC_GAMEPAD       ((USAGE) 0x05)
#define HID_USAGE_GENERIC_KEYBOARD      ((USAGE) 0x06)

typedef struct _HIDD_ATTRIBUTES
{
    ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)

    //
    // Vendor ids of this hid device
    //
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;

    //
    // Additional fields will be added to the end of this structure.
    //
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

typedef struct _HIDP_CAPS
{
    USAGE    Usage;
    USAGE    UsagePage;
    USHORT   InputReportByteLength;
    USHORT   OutputReportByteLength;
    USHORT   FeatureReportByteLength;
    USHORT   Reserved[17];

    USHORT   NumberLinkCollectionNodes;

    USHORT   NumberInputButtonCaps;
    USHORT   NumberInputValueCaps;
    USHORT   NumberInputDataIndices;

    USHORT   NumberOutputButtonCaps;
    USHORT   NumberOutputValueCaps;
    USHORT   NumberOutputDataIndices;

    USHORT   NumberFeatureButtonCaps;
    USHORT   NumberFeatureValueCaps;
    USHORT   NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;
