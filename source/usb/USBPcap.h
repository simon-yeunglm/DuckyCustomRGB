#pragma once

// by simon yeung, 10/10/2020
// all rights reserved

#pragma pack(push)
#pragma pack(1)

#define	UCHAR		unsigned char
#define	USHORT		unsigned short
#define	UINT32		unsigned int
#define	UINT64		unsigned long long
#define	ULONG		unsigned long long
#define	USBD_STATUS unsigned int

typedef struct
{
    USHORT       headerLen; /* This header length */
    UINT64       irpId;     /* I/O Request packet ID */
    USBD_STATUS  status;    /* USB status code
                               (on return from host controller) */
    USHORT       function;  /* URB Function */
    UCHAR        info;      /* I/O Request info */

    USHORT       bus;       /* bus (RootHub) number */
    USHORT       device;    /* device address */
    UCHAR        endpoint;  /* endpoint number and transfer direction */
    UCHAR        transfer;  /* transfer type */

    UINT32       dataLength;/* Data length */
} USBPCAP_BUFFER_PACKET_HEADER, *PUSBPCAP_BUFFER_PACKET_HEADER;

#undef	UCHAR	
#undef	USHORT	
#undef	UINT32	
#undef	UINT64	
#undef	ULONG	
#undef	USBD_STATUS

#pragma pack(pop)

static_assert(sizeof(USBPCAP_BUFFER_PACKET_HEADER) == 27, "sizeof(USBPCAP_BUFFER_PACKET_HEADER) must == 27" );
