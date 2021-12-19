
// by simon yeung, 12/12/2021
// all rights reserved

#include "Keyboard_DropShift.h"
#include "usb/USBDevice.h"
#include "hid/HIDDevice.h"
#include "render/RenderTarget.h"

#define USE_CUSTOM_USB_PROTOCOL		(1)		// need to use custom compile firm ware

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Should be sync with Drop version of qmk_firmware\tmk_core\protocol\arm_atsam\raw_handler.c
// Note we have added 1 byte REPORT_ID at the start of message for the HID API
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define RAWP_PROTOCOL_VERSION		0x00
#define RAWP_CAT_MFGTEST			0x01

#define RAWP_CMD_SET_RGB_MODE		0x01
#define RAWP_CMD_SET_ALL_RGBS		0x02
#define RAWP_CMD_SET_SPEC_RGBS		0x03
#define RAWP_CMD_READ_ALL_ADCS		0x04
#define RAWP_CMD_RET_GCR_INFO		0x05
#define RAWP_CMD_RET_FW_INFO		0x06
#define RAWP_CMD_RET_QMK_INFO		0x07

// custom protocol for compact LED data
#define RAWP_CMD_SET_SPEC_RGBS_EX1	0x08	// the 1st packet which encoded cap/num/scroll lock state
#define RAWP_CMD_SET_SPEC_RGBS_EX2	0x09	// the 2nd to 5th packets

enum {
	RET_Success,
	RET_Fail,
	RET_Inactive,
	RET_UnknownCommand,
	RET_LEDNotFound
};

__pragma( pack(push, 1) )
struct sRawp_command_t{
	uint8_t		report_id;
	uint8_t		prot_ver;		// Protocol Version
	uint8_t		cat;			// Category
	uint8_t		cmd;			// Command
	uint8_t		data[61];		// Parameters
};

struct sRawp_response_t{
	uint8_t		report_id;
	uint8_t		prot_ver;		// Protocol Version
	uint8_t		cat;			// Category
	uint8_t		cmd;			// Command
	uint8_t		result;			// Result
	uint8_t		data[60];		// Parameters
};
__pragma( pack(pop))

///////////////////////////////////////////////////////////////////////////////////////////////////////

#define REPORT_ID	0x00
#define CMD(cmdType)	{ REPORT_ID, RAWP_PROTOCOL_VERSION, RAWP_CAT_MFGTEST, cmdType }

unsigned short Keyboard_DropShift::productID= 0xEE65;

struct EdgeLEDPacketData
{
	unsigned char	packetIdx;
	unsigned char	packetOffset;	// R offset
	int4			renderTargetPxRange;
};

static bool	validateResponse(const sRawp_response_t& response, uint8_t cmd)
{
	if (response.prot_ver	== RAWP_PROTOCOL_VERSION	&&
		response.cat		== RAWP_CAT_MFGTEST			&&
		response.cmd		== cmd						&&
		response.result		== RET_Success	)
	{
		return true;
	}
#if !DEBUG_DISABLE_HID_COMM
	else
	{
		printf("Unknown reply [%i] %i %i %i %i.\n", cmd, response.prot_ver, response.cat, response.cmd, response.result);
	}
#endif
	return false;
}

Keyboard_DropShift::Keyboard_DropShift(HIDDevice* device) : Keyboard(device)
{
#if USE_CUSTOM_USB_PROTOCOL
	m_isKeyColorReadPacket= false;
#endif
	beginAddKey();
	addKey_1800_ANSI();
	endAddKey();
}

Keyboard_DropShift::~Keyboard_DropShift()
{
}



void	Keyboard_DropShift::addKey(InputKey key, unsigned char colorPacketIdx, unsigned char packetOffset)
{
	m_keys[(int)key].packetIdx		= colorPacketIdx;
#if USE_CUSTOM_USB_PROTOCOL
	m_keys[(int)key].packetOffset	= packetOffset;
#else
	m_keys[(int)key].packetOffset	= packetOffset	+ 2;	// 1 byte for Report ID and 1 byte for ID
#endif
}

void	Keyboard_DropShift::endAddKey()
{
	// set up packets headers
#if USE_CUSTOM_USB_PROTOCOL
	m_packetNum= 6;
#else
	m_packetNum= 12;
#endif
	m_packets= new Packet[m_packetNum];
	setUpPacketHeaders();
}

void	Keyboard_DropShift::connect()
{
	// get firmware info
	{
		// send the message
		sRawp_command_t FW_INFO_cmd=  CMD(RAWP_CMD_RET_FW_INFO);
		bool isOk= sendMessage(FW_INFO_cmd);

		// read the reply
		sRawp_response_t FW_INFO_response;
		if (recvMessage(FW_INFO_response, RAWP_CMD_RET_FW_INFO))
		{
			uint32_t product_id= (FW_INFO_response.data[1] << 8) | (FW_INFO_response.data[0]);
			uint32_t device_ver= (FW_INFO_response.data[3] << 8) | (FW_INFO_response.data[2]);

			char serialName[16];
			strncpy(serialName, (char *)(&FW_INFO_response.data[4]), 16);
			serialName[15]= '\0';
				
			char compileVer[40];
			strncpy(compileVer, (char *)(&FW_INFO_response.data[20]), 40);
			compileVer[39]= '\0';

			printf("Firmware info: 0x%x 0x%x %s %s\n", product_id, device_ver, serialName, compileVer);
		}
	}
	
	// get QMK info
	{
		// send the message
		sRawp_command_t QMK_INFO_cmd=  CMD(RAWP_CMD_RET_QMK_INFO);
		bool isOk= sendMessage(QMK_INFO_cmd);

		// read the reply
		sRawp_response_t QMK_INFO_response;
		if (recvMessage(QMK_INFO_response, RAWP_CMD_RET_QMK_INFO))
		{
			char qmk_ver[40];
			strncpy(qmk_ver, (char *)(&QMK_INFO_response.data[0]), 40);
			qmk_ver[39]= '\0';
				
			char qmk_buildDate[20];
			strncpy(qmk_buildDate, (char *)(&QMK_INFO_response.data[40]), 20);
			qmk_buildDate[19]= '\0';

			printf("QMK info: %s %s\n", qmk_ver, qmk_buildDate);
		}
	}

	// change to LED_MFG_TEST_MODE_RAWP
	{
		// send the message
		sRawp_command_t RGB_mode_cmd=  CMD(RAWP_CMD_SET_RGB_MODE);
		RGB_mode_cmd.data[0]= 1;
		bool isOk= sendMessage(RGB_mode_cmd);

		// read the reply
		sRawp_response_t RGB_mode_response;
		recvMessage(RGB_mode_response, RAWP_CMD_SET_RGB_MODE);
	}
}

void	Keyboard_DropShift::disconnect()
{
	// change to LED_MFG_TEST_MODE_OFF
	{
		// send the message
		wait(250);
		sRawp_command_t RGB_mode_cmd=  CMD(RAWP_CMD_SET_RGB_MODE);
		RGB_mode_cmd.data[0]= 0;
		bool isOk= sendMessage(RGB_mode_cmd);

		// read the reply
		wait(250);
		sRawp_response_t RGB_mode_response;
		recvMessage(RGB_mode_response, RAWP_CMD_SET_RGB_MODE);
	}
}

bool	Keyboard_DropShift::sendMessage(const sRawp_command_t& msg)
{
	int bytes_written= m_device->write((unsigned char*)&msg, sizeof(sRawp_command_t));
	bool isOk= sizeof(sRawp_command_t) == bytes_written;
#if !DEBUG_DISABLE_HID_COMM
	if (!isOk)
		printf("Send message writen size mis-match %i\n", bytes_written);
#endif
	return isOk;
}

bool	Keyboard_DropShift::recvMessage(sRawp_response_t& msg, uint8_t cmd)
{
	int numRead= m_device->read((unsigned char*)&msg, sizeof(sRawp_response_t));
	if (numRead != 0 && validateResponse(msg, cmd) )
		return true;
	else
		return false;
}

#if USE_CUSTOM_USB_PROTOCOL
void	Keyboard_DropShift::addKey_1800_ANSI()
{
	// packet 0
	addKey(InputKey::Escape			, 0, 5 + 0 * 3);
	addKey(InputKey::F1				, 0, 5 + 1 * 3);
	addKey(InputKey::F2				, 0, 5 + 2 * 3);
	addKey(InputKey::F3				, 0, 5 + 3 * 3);
	addKey(InputKey::F4				, 0, 5 + 4 * 3);
	addKey(InputKey::F5				, 0, 5 + 5 * 3);
	addKey(InputKey::F6				, 0, 5 + 6 * 3);
	addKey(InputKey::F7				, 0, 5 + 7 * 3);
	addKey(InputKey::F8				, 0, 5 + 8 * 3);
	addKey(InputKey::F9				, 0, 5 + 9 * 3);
	addKey(InputKey::F10			, 0, 5 +10 * 3);
	addKey(InputKey::F11			, 0, 5 +11 * 3);
	addKey(InputKey::F12			, 0, 5 +12 * 3);
	addKey(InputKey::PrintScreen	, 0, 5 +13 * 3);
	addKey(InputKey::NumPad_Extra1	, 0, 5 +14 * 3);
	addKey(InputKey::NumPad_Extra2	, 0, 5 +15 * 3);
	addKey(InputKey::NumPad_Extra3	, 0, 5 +16 * 3);
	addKey(InputKey::Pause			, 0, 5 +17 * 3);
	
	// packet 1
	addKey(InputKey::Tilde			, 1, 5 + 0 * 3);
	addKey(InputKey::Num_1			, 1, 5 + 1 * 3);
	addKey(InputKey::Num_2			, 1, 5 + 2 * 3);
	addKey(InputKey::Num_3			, 1, 5 + 3 * 3);
	addKey(InputKey::Num_4			, 1, 5 + 4 * 3);
	addKey(InputKey::Num_5			, 1, 5 + 5 * 3);
	addKey(InputKey::Num_6			, 1, 5 + 6 * 3);
	addKey(InputKey::Num_7			, 1, 5 + 7 * 3);
	addKey(InputKey::Num_8			, 1, 5 + 8 * 3);
	addKey(InputKey::Num_9			, 1, 5 + 9 * 3);
	addKey(InputKey::Num_0			, 1, 5 +10 * 3);
	addKey(InputKey::Minus			, 1, 5 +11 * 3);
	addKey(InputKey::Plus			, 1, 5 +12 * 3);
	addKey(InputKey::BackSpace		, 1, 5 +13 * 3);
	addKey(InputKey::NumLock		, 1, 5 +14 * 3);
	addKey(InputKey::NumPad_Divide	, 1, 5 +15 * 3);
	addKey(InputKey::NumPad_Multiply, 1, 5 +16 * 3);
	addKey(InputKey::Delete			, 1, 5 +17 * 3);
	
	// packet 2
	addKey(InputKey::Tab			, 2, 5 + 0 * 3);
	addKey(InputKey::Q				, 2, 5 + 1 * 3);
	addKey(InputKey::W				, 2, 5 + 2 * 3);
	addKey(InputKey::E				, 2, 5 + 3 * 3);
	addKey(InputKey::R				, 2, 5 + 4 * 3);
	addKey(InputKey::T				, 2, 5 + 5 * 3);
	addKey(InputKey::Y				, 2, 5 + 6 * 3);
	addKey(InputKey::U				, 2, 5 + 7 * 3);
	addKey(InputKey::I				, 2, 5 + 8 * 3);
	addKey(InputKey::O				, 2, 5 + 9 * 3);
	addKey(InputKey::P				, 2, 5 +10 * 3);
	addKey(InputKey::Bracket_Open	, 2, 5 +11 * 3);
	addKey(InputKey::Bracket_Close	, 2, 5 +12 * 3);
	addKey(InputKey::Backslash		, 2, 5 +13 * 3);
	addKey(InputKey::NumPad_7		, 2, 5 +14 * 3);
	addKey(InputKey::NumPad_8		, 2, 5 +15 * 3);
	addKey(InputKey::NumPad_9		, 2, 5 +16 * 3);
	addKey(InputKey::NumPad_Minus	, 2, 5 +17 * 3);
	
	// packet 3
	addKey(InputKey::CapsLock		, 3, 5 + 0 * 3);
	addKey(InputKey::A				, 3, 5 + 1 * 3);
	addKey(InputKey::S				, 3, 5 + 2 * 3);
	addKey(InputKey::D				, 3, 5 + 3 * 3);
	addKey(InputKey::F				, 3, 5 + 4 * 3);
	addKey(InputKey::G				, 3, 5 + 5 * 3);
	addKey(InputKey::H				, 3, 5 + 6 * 3);
	addKey(InputKey::J				, 3, 5 + 7 * 3);
	addKey(InputKey::K				, 3, 5 + 8 * 3);
	addKey(InputKey::L				, 3, 5 + 9 * 3);
	addKey(InputKey::Colon			, 3, 5 +10 * 3);
	addKey(InputKey::Apostrophe		, 3, 5 +11 * 3);
	addKey(InputKey::Enter			, 3, 5 +12 * 3);
	addKey(InputKey::NumPad_4		, 3, 5 +13 * 3);
	addKey(InputKey::NumPad_5		, 3, 5 +14 * 3);
	addKey(InputKey::NumPad_6		, 3, 5 +15 * 3);
	addKey(InputKey::NumPad_Plus	, 3, 5 +16 * 3);
	
	// packet 4
	addKey(InputKey::Shift_Left		, 4, 5 + 0 * 3);
	addKey(InputKey::Z				, 4, 5 + 1 * 3);
	addKey(InputKey::X				, 4, 5 + 2 * 3);
	addKey(InputKey::C				, 4, 5 + 3 * 3);
	addKey(InputKey::V				, 4, 5 + 4 * 3);
	addKey(InputKey::B				, 4, 5 + 5 * 3);
	addKey(InputKey::N				, 4, 5 + 6 * 3);
	addKey(InputKey::M				, 4, 5 + 7 * 3);
	addKey(InputKey::Comma			, 4, 5 + 8 * 3);
	addKey(InputKey::Dot			, 4, 5 + 9 * 3);
	addKey(InputKey::ForwardSlash	, 4, 5 +10 * 3);
	addKey(InputKey::Shift_Right	, 4, 5 +11 * 3);
	addKey(InputKey::NumPad_1		, 4, 5 +12 * 3);
	addKey(InputKey::NumPad_2		, 4, 5 +13 * 3);
	addKey(InputKey::NumPad_3		, 4, 5 +14 * 3);
	addKey(InputKey::Arrow_Up		, 4, 5 +15 * 3);
	addKey(InputKey::NumPad_Enter	, 4, 5 +16 * 3);
	
	// packet 5
	addKey(InputKey::Control_Left	, 5, 5 + 0 * 3);
	addKey(InputKey::Windows_Left	, 5, 5 + 1 * 3);
	addKey(InputKey::Alt_Left		, 5, 5 + 2 * 3);
	addKey(InputKey::SpaceBar		, 5, 5 + 3 * 3);
	addKey(InputKey::Alt_Right		, 5, 5 + 4 * 3);
	addKey(InputKey::Fn				, 5, 5 + 5 * 3);
	addKey(InputKey::NumPad_0		, 5, 5 + 6 * 3);
	addKey(InputKey::NumPad_Dot		, 5, 5 + 7 * 3);
	addKey(InputKey::Arrow_Left		, 5, 5 + 8 * 3);
	addKey(InputKey::Arrow_Down		, 5, 5 + 9 * 3);
	addKey(InputKey::Arrow_Right	, 5, 5 +10 * 3);

	m_layout= KeyboardLayout::_1800_ANSI;
}

void	Keyboard_DropShift::setUpPacketHeaders()
{
	// set up Report ID for all packets
	int numTotalPackets= m_packetNum;
	for(int i=0; i<numTotalPackets; ++i)
	{
		assert(m_packets != nullptr);
		memset(m_packets[i].data, 0, sizeof(Packet::data));
		Packet& pkt= m_packets[i];
		pkt.data[0]= REPORT_ID;
		pkt.data[1]= RAWP_PROTOCOL_VERSION;
		pkt.data[2]= RAWP_CAT_MFGTEST;
		pkt.data[3]= RAWP_CMD_SET_SPEC_RGBS_EX2;
		pkt.data[4]= i;
	}
	m_packets[0].data[3]= RAWP_CMD_SET_SPEC_RGBS_EX1;	// first packet hv extra info
}

void	Keyboard_DropShift::commitKeyColor(RenderTarget* renderTarget)
{
	// can be null at start up
	if (renderTarget == nullptr)
	{
		Keyboard::commitKeyColor(renderTarget);
		return;
	}

	// copy edge LED color
	{
		const int numEdgeLED= 21;
		static EdgeLEDPacketData edgeLEDPacketData[numEdgeLED]=
		{
			{0,		5 +18 * 3, {  0,  4,  1,  6 } },	// row 0, edge L
			{0,		5 +19 * 3, { 75,  4, 76,  6 } },	// row 0, edge R 
			
			{1,		5 +18 * 3, {  0,  6,  1, 10 } },	// row 1, edge L
			{1,		5 +19 * 3, { 75,  6, 76, 10 } },	// row 1, edge R
			
			{2,		5 +18 * 3, {  0, 10,  1, 12 } },	// row 2, edge L
			{2,		5 +19 * 3, { 75, 10, 76, 12 } },	// row 2, edge R
			
			{3,		5 +17 * 3, {  0, 12,  1, 16 } },	// row 3, edge L
			{3,		5 +18 * 3, { 75, 12, 76, 16 } },	// row 3, edge R
			{3,		5 +19 * 3, { 14, 23, 18, 24 } },	// row 5, edge B, space 1st
			
			{4,		5 +17 * 3, {  0, 16,  1, 18 } },	// row 4, edge L
			{4,		5 +18 * 3, { 75, 16, 76, 18 } },	// row 4, edge R
			{4,		5 +19 * 3, { 75, 23, 76, 24 } },	// row 5, edge B-R
			
			{5,		5 +11 * 3, {  0, 18,  1, 20 } },	// row 5, edge L
			{5,		5 +12 * 3, { 75, 18, 76, 20 } },	// row 5, edge R
			{5,		5 +13 * 3, { 18, 23, 21, 24 } },	// row 5 edge B, space 2md
			{5,		5 +14 * 3, { 21, 23, 24, 24 } },	// row 5 edge B, space 3rd
			{5,		5 +15 * 3, { 24, 23, 28, 24 } },	// row 5 edge B, space 4th
			{5,		5 +16 * 3, { 28, 23, 31, 24 } },	// row 5 edge B, space 5th
			{5,		5 +17 * 3, { 31, 23, 34, 24 } },	// row 5 edge B, space 6th
			{5,		5 +18 * 3, { 34, 23, 38, 24 } },	// row 5 edge B, space 7th
			{5,		5 +19 * 3, { 38, 23, 41, 24 } },	// row 5 edge B, space 8th
		};
		for(int i=0; i<numEdgeLED; ++i)
		{
			const EdgeLEDPacketData& data= edgeLEDPacketData[i];
			Packet&	pkt			= m_packets[data.packetIdx];
			int		pktOffsetR	= data.packetOffset;
			int		pktOffsetG	= pktOffsetR + 1;
			int		pktOffsetB	= pktOffsetR + 2;
			int4	pxRange		= data.renderTargetPxRange;
			renderTarget->getAverageColorInRange(pxRange, &pkt.data[pktOffsetR], &pkt.data[pktOffsetG], &pkt.data[pktOffsetB]);
		}
	}

	// copy lock states
	{
		bool isLockNum		= (GetKeyState(VK_NUMLOCK) & 0x01)!= 0;
		bool isLockCaps		= (GetKeyState(VK_CAPITAL) & 0x01)!= 0;
		bool isLockScroll	= (GetKeyState(VK_SCROLL ) & 0x01)!= 0;
		unsigned char states= (isLockScroll << 2) | (isLockCaps << 1) | (isLockNum << 0);
		m_packets[0].data[4]= states;
	}
	Keyboard::commitKeyColor(renderTarget);
}

#else
void	Keyboard_DropShift::addKey_1800_ANSI()
{
	addKey(InputKey::A, 3, 4 +10 * 4);
	addKey(InputKey::B, 5, 4 + 1 * 4);
	addKey(InputKey::C, 4, 4 +14 * 4);
	addKey(InputKey::D, 3, 4 +12 * 4);
	addKey(InputKey::E, 2, 4 + 9 * 4);
	addKey(InputKey::F, 3, 4 +13 * 4);
	addKey(InputKey::G, 3, 4 +14 * 4);
	addKey(InputKey::H, 4, 4 + 0 * 4);
	addKey(InputKey::I, 2, 4 +14 * 4);
	addKey(InputKey::J, 4, 4 + 1 * 4);
	addKey(InputKey::K, 4, 4 + 2 * 4);
	addKey(InputKey::L, 4, 4 + 3 * 4);
	addKey(InputKey::M, 5, 4 + 3 * 4);
	addKey(InputKey::N, 5, 4 + 2 * 4);
	addKey(InputKey::O, 3, 4 + 0 * 4);
	addKey(InputKey::P, 3, 4 + 1 * 4);
	addKey(InputKey::Q, 2, 4 + 7 * 4);
	addKey(InputKey::R, 2, 4 +10 * 4);
	addKey(InputKey::S, 3, 4 +11 * 4);
	addKey(InputKey::T, 2, 4 +11 * 4);
	addKey(InputKey::U, 2, 4 +13 * 4);
	addKey(InputKey::V, 5, 4 + 0 * 4);
	addKey(InputKey::W, 2, 4 + 8 * 4);
	addKey(InputKey::X, 4, 4 +13 * 4);
	addKey(InputKey::Y, 2, 4 +12 * 4);
	addKey(InputKey::Z, 4, 4 +12 * 4);

	addKey(InputKey::Num_0, 1, 4 +13 * 4);
	addKey(InputKey::Num_1, 1, 4 + 4 * 4);
	addKey(InputKey::Num_2, 1, 4 + 5 * 4);
	addKey(InputKey::Num_3, 1, 4 + 6 * 4);
	addKey(InputKey::Num_4, 1, 4 + 7 * 4);
	addKey(InputKey::Num_5, 1, 4 + 8 * 4);
	addKey(InputKey::Num_6, 1, 4 + 9 * 4);
	addKey(InputKey::Num_7, 1, 4 +10 * 4);
	addKey(InputKey::Num_8, 1, 4 +11 * 4);
	addKey(InputKey::Num_9, 1, 4 +12 * 4);

	addKey(InputKey::Escape, 0, 4 + 0 * 4);

	addKey(InputKey::Tilde			, 1, 4 + 3 * 4);
	addKey(InputKey::Plus			, 2, 4 + 0 * 4);
	addKey(InputKey::Minus			, 1, 4 +14 * 4);
	addKey(InputKey::BackSpace		, 2, 4 + 1 * 4);

	addKey(InputKey::Tab			, 2, 4 + 6 * 4);
	addKey(InputKey::Bracket_Open	, 3, 4 + 2 * 4);
	addKey(InputKey::Bracket_Close	, 3, 4 + 3 * 4);
	addKey(InputKey::Backslash		, 3, 4 + 4 * 4);

	addKey(InputKey::CapsLock		, 3, 4 + 9 * 4);
	addKey(InputKey::Colon			, 4, 4 + 4 * 4);
	addKey(InputKey::Apostrophe		, 4, 4 + 5 * 4);
	addKey(InputKey::Enter			, 4, 4 + 6 * 4);
	
	addKey(InputKey::Shift_Left		, 4, 4 +11 * 4);
	addKey(InputKey::Shift_Right	, 5, 4 + 7 * 4);
	addKey(InputKey::Comma			, 5, 4 + 4 * 4);
	addKey(InputKey::Dot			, 5, 4 + 5 * 4);
	addKey(InputKey::ForwardSlash	, 5, 4 + 6 * 4);

	addKey(InputKey::SpaceBar		, 6, 4 + 1 * 4);
	addKey(InputKey::Fn				, 6, 4 + 3 * 4);
	addKey(InputKey::Control_Left	, 5, 4 +13 * 4);
	addKey(InputKey::Windows_Left	, 5, 4 +14 * 4);
	addKey(InputKey::Alt_Left		, 6, 4 + 0 * 4);
	addKey(InputKey::Alt_Right		, 6, 4 + 2 * 4);

	addKey(InputKey::Arrow_Up		, 5, 4 +11 * 4);
	addKey(InputKey::Arrow_Down		, 6, 4 + 7 * 4);
	addKey(InputKey::Arrow_Left		, 6, 4 + 6 * 4);
	addKey(InputKey::Arrow_Right	, 6, 4 + 8 * 4);

	addKey(InputKey::Delete			, 2, 4 + 5 * 4);

	addKey(InputKey::PrintScreen	, 1, 4 + 0 * 4);
	addKey(InputKey::Pause			, 1, 4 + 2 * 4);

	addKey(InputKey::F1				, 0, 4 + 1 * 4);
	addKey(InputKey::F2				, 0, 4 + 2 * 4);
	addKey(InputKey::F3				, 0, 4 + 3 * 4);
	addKey(InputKey::F4				, 0, 4 + 4 * 4);
	addKey(InputKey::F5				, 0, 4 + 5 * 4);
	addKey(InputKey::F6				, 0, 4 + 6 * 4);
	addKey(InputKey::F7				, 0, 4 + 7 * 4);
	addKey(InputKey::F8				, 0, 4 + 8 * 4);
	addKey(InputKey::F9				, 0, 4 + 9 * 4);
	addKey(InputKey::F10			, 0, 4 +10 * 4);
	addKey(InputKey::F11			, 0, 4 +11 * 4);
	addKey(InputKey::F12			, 0, 4 +12 * 4);
	
	addKey(InputKey::NumLock		, 2, 4 + 2 * 4);

	addKey(InputKey::NumPad_0		, 6, 4 + 4 * 4);
	addKey(InputKey::NumPad_1		, 5, 4 + 8 * 4);
	addKey(InputKey::NumPad_2		, 5, 4 + 9 * 4);
	addKey(InputKey::NumPad_3		, 5, 4 +10 * 4);
	addKey(InputKey::NumPad_4		, 4, 4 + 7 * 4);
	addKey(InputKey::NumPad_5		, 4, 4 + 8 * 4);
	addKey(InputKey::NumPad_6		, 4, 4 + 9 * 4);
	addKey(InputKey::NumPad_7		, 3, 4 + 5 * 4);
	addKey(InputKey::NumPad_8		, 3, 4 + 6 * 4);
	addKey(InputKey::NumPad_9		, 3, 4 + 7 * 4);

	addKey(InputKey::NumPad_Plus	, 4, 4 +10 * 4);
	addKey(InputKey::NumPad_Minus	, 3, 4 + 8 * 4);
	addKey(InputKey::NumPad_Multiply, 2, 4 + 4 * 4);
	addKey(InputKey::NumPad_Divide	, 2, 4 + 3 * 4);
	addKey(InputKey::NumPad_Dot		, 6, 4 + 5 * 4);
	addKey(InputKey::NumPad_Enter	, 5, 4 +12 * 4);

	addKey(InputKey::NumPad_Extra1	, 0, 4 +13 * 4);
	addKey(InputKey::NumPad_Extra2	, 1, 4 + 1 * 4);
	addKey(InputKey::NumPad_Extra3	, 0, 4 +14 * 4);
//	addKey(InputKey::NumPad_Extra4	, 1, 4 + 2 * 4);

	m_layout= KeyboardLayout::_1800_ANSI;
}

void	Keyboard_DropShift::setUpPacketHeaders()
{
	// set up Report ID for all packets
	int numTotalPackets= m_packetNum;
	for(int i=0; i<numTotalPackets; ++i)
	{
		assert(m_packets != nullptr);
		memset(m_packets[i].data, 0, sizeof(Packet::data));
		Packet& pkt= m_packets[i];
		pkt.data[0]= REPORT_ID;
		pkt.data[1]= RAWP_PROTOCOL_VERSION;
		pkt.data[2]= RAWP_CAT_MFGTEST;
		pkt.data[3]= RAWP_CMD_SET_SPEC_RGBS;
	}

	// LED idx from qmk_firmware\keyboards\masssdrop\shift\config_led.h
	// set up packet
	{
		const int	pktIdx	= 0;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 1 ;
		pkt.data[4 + 1 * 4 + 1	]= 2 ;
		pkt.data[4 + 2 * 4 + 1	]= 3 ;
		pkt.data[4 + 3 * 4 + 1	]= 4 ;
		pkt.data[4 + 4 * 4 + 1	]= 5 ;
		pkt.data[4 + 5 * 4 + 1	]= 6 ;
		pkt.data[4 + 6 * 4 + 1	]= 7 ;
		pkt.data[4 + 7 * 4 + 1	]= 8 ;
		pkt.data[4 + 8 * 4 + 1	]= 9 ;
		pkt.data[4 + 9 * 4 + 1	]= 10;
		pkt.data[4 +10 * 4 + 1	]= 11;
		pkt.data[4 +11 * 4 + 1	]= 12;
		pkt.data[4 +12 * 4 + 1	]= 13;
		pkt.data[4 +13 * 4 + 1	]= 15;
		pkt.data[4 +14 * 4 + 1	]= 16;
	}
	{
		const int	pktIdx	= 1;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 18;
		pkt.data[4 + 1 * 4 + 1	]= 19;
		pkt.data[4 + 2 * 4 + 1	]= 20;
		pkt.data[4 + 3 * 4 + 1	]= 22;
		pkt.data[4 + 4 * 4 + 1	]= 23;
		pkt.data[4 + 5 * 4 + 1	]= 24;
		pkt.data[4 + 6 * 4 + 1	]= 25;
		pkt.data[4 + 7 * 4 + 1	]= 26;
		pkt.data[4 + 8 * 4 + 1	]= 27;
		pkt.data[4 + 9 * 4 + 1	]= 28;
		pkt.data[4 +10 * 4 + 1	]= 29;
		pkt.data[4 +11 * 4 + 1	]= 30;
		pkt.data[4 +12 * 4 + 1	]= 31;
		pkt.data[4 +13 * 4 + 1	]= 32;
		pkt.data[4 +14 * 4 + 1	]= 33;
	}
	{
		const int	pktIdx	= 2;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 34;
		pkt.data[4 + 1 * 4 + 1	]= 35;
		pkt.data[4 + 2 * 4 + 1	]= 36;
		pkt.data[4 + 3 * 4 + 1	]= 37;
		pkt.data[4 + 4 * 4 + 1	]= 38;
		pkt.data[4 + 5 * 4 + 1	]= 39;
		pkt.data[4 + 6 * 4 + 1	]= 40;
		pkt.data[4 + 7 * 4 + 1	]= 41;
		pkt.data[4 + 8 * 4 + 1	]= 42;
		pkt.data[4 + 9 * 4 + 1	]= 43;
		pkt.data[4 +10 * 4 + 1	]= 44;
		pkt.data[4 +11 * 4 + 1	]= 45;
		pkt.data[4 +12 * 4 + 1	]= 46;
		pkt.data[4 +13 * 4 + 1	]= 47;
		pkt.data[4 +14 * 4 + 1	]= 48;
	}
	{
		const int	pktIdx	= 3;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 49;
		pkt.data[4 + 1 * 4 + 1	]= 50;
		pkt.data[4 + 2 * 4 + 1	]= 51;
		pkt.data[4 + 3 * 4 + 1	]= 52;
		pkt.data[4 + 4 * 4 + 1	]= 53;
		pkt.data[4 + 5 * 4 + 1	]= 54;
		pkt.data[4 + 6 * 4 + 1	]= 55;
		pkt.data[4 + 7 * 4 + 1	]= 56;
		pkt.data[4 + 8 * 4 + 1	]= 57;
		pkt.data[4 + 9 * 4 + 1	]= 58;
		pkt.data[4 +10 * 4 + 1	]= 59;
		pkt.data[4 +11 * 4 + 1	]= 60;
		pkt.data[4 +12 * 4 + 1	]= 61;
		pkt.data[4 +13 * 4 + 1	]= 62;
		pkt.data[4 +14 * 4 + 1	]= 63;
	}
	{
		const int	pktIdx	= 4;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 64;
		pkt.data[4 + 1 * 4 + 1	]= 65;
		pkt.data[4 + 2 * 4 + 1	]= 66;
		pkt.data[4 + 3 * 4 + 1	]= 67;
		pkt.data[4 + 4 * 4 + 1	]= 68;
		pkt.data[4 + 5 * 4 + 1	]= 69;
		pkt.data[4 + 6 * 4 + 1	]= 70;
		pkt.data[4 + 7 * 4 + 1	]= 71;
		pkt.data[4 + 8 * 4 + 1	]= 72;
		pkt.data[4 + 9 * 4 + 1	]= 73;
		pkt.data[4 +10 * 4 + 1	]= 74;
		pkt.data[4 +11 * 4 + 1	]= 75;
		pkt.data[4 +12 * 4 + 1	]= 76;
		pkt.data[4 +13 * 4 + 1	]= 77;
		pkt.data[4 +14 * 4 + 1	]= 78;
	}
	{
		const int	pktIdx	= 5;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 79;
		pkt.data[4 + 1 * 4 + 1	]= 80;
		pkt.data[4 + 2 * 4 + 1	]= 81;
		pkt.data[4 + 3 * 4 + 1	]= 82;
		pkt.data[4 + 4 * 4 + 1	]= 83;
		pkt.data[4 + 5 * 4 + 1	]= 84;
		pkt.data[4 + 6 * 4 + 1	]= 85;
		pkt.data[4 + 7 * 4 + 1	]= 86;
		pkt.data[4 + 8 * 4 + 1	]= 87;
		pkt.data[4 + 9 * 4 + 1	]= 88;
		pkt.data[4 +10 * 4 + 1	]= 89;
		pkt.data[4 +11 * 4 + 1	]= 90;
		pkt.data[4 +12 * 4 + 1	]= 91;
		pkt.data[4 +13 * 4 + 1	]= 92;
		pkt.data[4 +14 * 4 + 1	]= 93;
	}
	{
		const int	pktIdx	= 6;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 94 ;
		pkt.data[4 + 1 * 4 + 1	]= 95 ;
		pkt.data[4 + 2 * 4 + 1	]= 96 ;
		pkt.data[4 + 3 * 4 + 1	]= 97 ;
		pkt.data[4 + 4 * 4 + 1	]= 98 ;
		pkt.data[4 + 5 * 4 + 1	]= 99 ;
		pkt.data[4 + 6 * 4 + 1	]= 100;
		pkt.data[4 + 7 * 4 + 1	]= 101;
		pkt.data[4 + 8 * 4 + 1	]= 102;

		// under glow
		pkt.data[4 + 9 * 4 + 1	]= 103;
		pkt.data[4 +10 * 4 + 1	]= 104;
		pkt.data[4 +11 * 4 + 1	]= 105;
		pkt.data[4 +12 * 4 + 1	]= 106;
		pkt.data[4 +13 * 4 + 1	]= 107;
		pkt.data[4 +14 * 4 + 1	]= 108;
	}
	{
		const int	pktIdx	= 7;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 109;
		pkt.data[4 + 1 * 4 + 1	]= 110;
		pkt.data[4 + 2 * 4 + 1	]= 111;
		pkt.data[4 + 3 * 4 + 1	]= 112;
		pkt.data[4 + 4 * 4 + 1	]= 113;
		pkt.data[4 + 5 * 4 + 1	]= 114;
		pkt.data[4 + 6 * 4 + 1	]= 115;
		pkt.data[4 + 7 * 4 + 1	]= 116;
		pkt.data[4 + 8 * 4 + 1	]= 117;
		pkt.data[4 + 9 * 4 + 1	]= 118;
		pkt.data[4 +10 * 4 + 1	]= 119;
		pkt.data[4 +11 * 4 + 1	]= 120;
		pkt.data[4 +12 * 4 + 1	]= 121;
		pkt.data[4 +13 * 4 + 1	]= 122;
		pkt.data[4 +14 * 4 + 1	]= 123;
	}
	{
		const int	pktIdx	= 8;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 124;
		pkt.data[4 + 1 * 4 + 1	]= 125;
		pkt.data[4 + 2 * 4 + 1	]= 126;
		pkt.data[4 + 3 * 4 + 1	]= 127;
		pkt.data[4 + 4 * 4 + 1	]= 128;
		pkt.data[4 + 5 * 4 + 1	]= 129;
		pkt.data[4 + 6 * 4 + 1	]= 130;
		pkt.data[4 + 7 * 4 + 1	]= 131;
		pkt.data[4 + 8 * 4 + 1	]= 132;
		pkt.data[4 + 9 * 4 + 1	]= 133;
		pkt.data[4 +10 * 4 + 1	]= 134;
		pkt.data[4 +11 * 4 + 1	]= 135;
		pkt.data[4 +12 * 4 + 1	]= 136;
		pkt.data[4 +13 * 4 + 1	]= 137;
		pkt.data[4 +14 * 4 + 1	]= 138;
	}
	{
		const int	pktIdx	= 9;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 139;
		pkt.data[4 + 1 * 4 + 1	]= 140;
		pkt.data[4 + 2 * 4 + 1	]= 141;
		pkt.data[4 + 3 * 4 + 1	]= 142;
		pkt.data[4 + 4 * 4 + 1	]= 143;
		pkt.data[4 + 5 * 4 + 1	]= 144;
		pkt.data[4 + 6 * 4 + 1	]= 145;
		pkt.data[4 + 7 * 4 + 1	]= 146;
		pkt.data[4 + 8 * 4 + 1	]= 147;
		pkt.data[4 + 9 * 4 + 1	]= 148;
		pkt.data[4 +10 * 4 + 1	]= 149;
		pkt.data[4 +11 * 4 + 1	]= 150;
		pkt.data[4 +12 * 4 + 1	]= 151;
		pkt.data[4 +13 * 4 + 1	]= 152;
		pkt.data[4 +14 * 4 + 1	]= 153;
	}
	{
		const int	pktIdx	= 10;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 15;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 154;
		pkt.data[4 + 1 * 4 + 1	]= 155;
		pkt.data[4 + 2 * 4 + 1	]= 156;
		pkt.data[4 + 3 * 4 + 1	]= 157;
		pkt.data[4 + 4 * 4 + 1	]= 158;
		pkt.data[4 + 5 * 4 + 1	]= 159;
		pkt.data[4 + 6 * 4 + 1	]= 160;
		pkt.data[4 + 7 * 4 + 1	]= 161;
		pkt.data[4 + 8 * 4 + 1	]= 162;
		pkt.data[4 + 9 * 4 + 1	]= 163;
		pkt.data[4 +10 * 4 + 1	]= 164;
		pkt.data[4 +11 * 4 + 1	]= 165;
		pkt.data[4 +12 * 4 + 1	]= 166;
		pkt.data[4 +13 * 4 + 1	]= 167;		// indicator top
		pkt.data[4 +14 * 4 + 1	]= 168;		// indicator middle
	}
	{
		const int	pktIdx	= 11;
		Packet&		pkt		= m_packets[pktIdx];
		// num LED
		pkt.data[4			]= 1;
		// LED idx
		pkt.data[4 + 0 * 4 + 1	]= 169;		// indicator bottom
	}
}

void	Keyboard_DropShift::commitKeyColor(RenderTarget* renderTarget)
{
	// can be null at start up
	if (renderTarget == nullptr)
	{
		Keyboard::commitKeyColor(renderTarget);
		return;
	}

	// compute edge LED color
	{
		const int numEdgeLED= 64;
		static EdgeLEDPacketData edgeLEDPacketData[numEdgeLED]=
		{
			// top
			{6,		4 + 9 * 4 + 1	+ 1, {  0,  0,  1,  1 } },
			{6,		4 +10 * 4 + 1	+ 1, {  1,  0,  4,  1 } },
			{6,		4 +11 * 4 + 1	+ 1, {  6,  0, 10,  1 } },
			{6,		4 +12 * 4 + 1	+ 1, { 10,  0, 14,  1 } },
			{6,		4 +13 * 4 + 1	+ 1, { 14,  0, 18,  1 } },
			{6,		4 +14 * 4 + 1	+ 1, { 18,  0, 21,  1 } },

			{7,		4 + 0 * 4 + 1	+ 1, { 21,  0, 24,  1 } },
			{7,		4 + 1 * 4 + 1	+ 1, { 24,  0, 28,  1 } },
			{7,		4 + 2 * 4 + 1	+ 1, { 28,  0, 31,  1 } },
			{7,		4 + 3 * 4 + 1	+ 1, { 31,  0, 34,  1 } },
			{7,		4 + 4 * 4 + 1	+ 1, { 34,  0, 38,  1 } },
			{7,		4 + 5 * 4 + 1	+ 1, { 38,  0, 41,  1 } },
			{7,		4 + 6 * 4 + 1	+ 1, { 41,  0, 44,  1 } },
			{7,		4 + 7 * 4 + 1	+ 1, { 44,  0, 48,  1 } },
			{7,		4 + 8 * 4 + 1	+ 1, { 48,  0, 51,  1 } },
			{7,		4 + 9 * 4 + 1	+ 1, { 51,  0, 54,  1 } },
			{7,		4 +10 * 4 + 1	+ 1, { 54,  0, 57,  1 } },
			{7,		4 +11 * 4 + 1	+ 1, { 57,  0, 60,  1 } },
			{7,		4 +12 * 4 + 1	+ 1, { 60,  0, 62,  1 } },
			{7,		4 +13 * 4 + 1	+ 1, { 62,  0, 66,  1 } },
			{7,		4 +14 * 4 + 1	+ 1, { 66,  0, 70,  1 } },

			{8,		4 + 0 * 4 + 1	+ 1, { 74,  0, 75,  1 } },
			{8,		4 + 1 * 4 + 1	+ 1, { 75,  0, 76,  1 } },
			
			// right
			{8,		4 + 2 * 4 + 1	+ 1, { 75,  1, 76,  4 } },
			{8,		4 + 3 * 4 + 1	+ 1, { 75,  4, 76,  6 } },
			{8,		4 + 4 * 4 + 1	+ 1, { 75,  6, 76, 10 } },
			{8,		4 + 5 * 4 + 1	+ 1, { 75, 10, 76, 12 } },
			{8,		4 + 6 * 4 + 1	+ 1, { 75, 12, 76, 16 } },
			{8,		4 + 7 * 4 + 1	+ 1, { 75, 16, 76, 18 } },
			{8,		4 + 8 * 4 + 1	+ 1, { 75, 18, 76, 20 } },
			{8,		4 + 9 * 4 + 1	+ 1, { 75, 20, 76, 23 } },

			// bottom
			{8,		4 +10 * 4 + 1	+ 1, { 75, 23, 76, 24 } },
			{8,		4 +11 * 4 + 1	+ 1, { 74, 23, 75, 24 } },
			{8,		4 +12 * 4 + 1	+ 1, { 70, 23, 74, 24 } },
			{8,		4 +13 * 4 + 1	+ 1, { 66, 23, 70, 24 } },
			{8,		4 +14 * 4 + 1	+ 1, { 62, 23, 66, 24 } },

			{9,		4 + 0 * 4 + 1	+ 1, { 60, 23, 62, 24 } },
			{9,		4 + 1 * 4 + 1	+ 1, { 57, 23, 60, 24 } },
			{9,		4 + 2 * 4 + 1	+ 1, { 54, 23, 57, 24 } },
			{9,		4 + 3 * 4 + 1	+ 1, { 51, 23, 54, 24 } },
			{9,		4 + 4 * 4 + 1	+ 1, { 48, 23, 51, 24 } },
			{9,		4 + 5 * 4 + 1	+ 1, { 44, 23, 48, 24 } },
			{9,		4 + 6 * 4 + 1	+ 1, { 41, 23, 44, 24 } },
			{9,		4 + 7 * 4 + 1	+ 1, { 38, 23, 41, 24 } },
			{9,		4 + 8 * 4 + 1	+ 1, { 34, 23, 38, 24 } },
			{9,		4 + 9 * 4 + 1	+ 1, { 31, 23, 34, 24 } },
			{9,		4 +10 * 4 + 1	+ 1, { 28, 23, 31, 24 } },
			{9,		4 +11 * 4 + 1	+ 1, { 24, 23, 28, 24 } },
			{9,		4 +12 * 4 + 1	+ 1, { 21, 23, 24, 24 } },
			{9,		4 +13 * 4 + 1	+ 1, { 18, 23, 21, 24 } },
			{9,		4 +14 * 4 + 1	+ 1, { 14, 23, 18, 24 } },

			{10,	4 + 0 * 4 + 1	+ 1, { 10, 23, 14, 24 } },
			{10,	4 + 1 * 4 + 1	+ 1, {  6, 23, 10, 24 } },
			{10,	4 + 2 * 4 + 1	+ 1, {  4, 23,  6, 24 } },
			{10,	4 + 3 * 4 + 1	+ 1, {  1, 23,  4, 24 } },
			{10,	4 + 4 * 4 + 1	+ 1, {  0, 23,  1, 24 } },

			// left
			{10,	4 + 5 * 4 + 1	+ 1, { 0, 20, 1, 23 } },
			{10,	4 + 6 * 4 + 1	+ 1, { 0, 18, 1, 20 } },
			{10,	4 + 7 * 4 + 1	+ 1, { 0, 16, 1, 18 } },
			{10,	4 + 8 * 4 + 1	+ 1, { 0, 12, 1, 16 } },
			{10,	4 + 9 * 4 + 1	+ 1, { 0, 10, 1, 12 } },
			{10,	4 +10 * 4 + 1	+ 1, { 0,  6, 1, 10 } },
			{10,	4 +11 * 4 + 1	+ 1, { 0,  4, 1,  6 } },
			{10,	4 +12 * 4 + 1	+ 1, { 0,  1, 1,  4 } },
		};

		for(int i=0; i<numEdgeLED; ++i)
		{
			const EdgeLEDPacketData& data= edgeLEDPacketData[i];
			Packet&	pkt			= m_packets[data.packetIdx];
			int		pktOffsetR	= data.packetOffset;
			int		pktOffsetG	= pktOffsetR + 1;
			int		pktOffsetB	= pktOffsetR + 2;
			int4	pxRange		= data.renderTargetPxRange;
			renderTarget->getAverageColorInRange(pxRange, &pkt.data[pktOffsetR], &pkt.data[pktOffsetG], &pkt.data[pktOffsetB]);
		}
	}

	// compute indicator color
	{
		// get color
		unsigned char indicatorKeyLockNumR;
		unsigned char indicatorKeyLockNumG;
		unsigned char indicatorKeyLockNumB;

		unsigned char indicatorKeyLockCapR;
		unsigned char indicatorKeyLockCapG;
		unsigned char indicatorKeyLockCapB;

		unsigned char indicatorKeyLockScrollR;
		unsigned char indicatorKeyLockScrollG;
		unsigned char indicatorKeyLockScrollB;
		
		renderTarget->getAverageColorInRange({59, 0, 61, 1}, &indicatorKeyLockNumR		, &indicatorKeyLockNumG		, &indicatorKeyLockNumB);
		renderTarget->getAverageColorInRange({59, 1, 61, 3}, &indicatorKeyLockCapR		, &indicatorKeyLockCapG		, &indicatorKeyLockCapB);
		renderTarget->getAverageColorInRange({59, 3, 61, 4}, &indicatorKeyLockScrollR	, &indicatorKeyLockScrollG	, &indicatorKeyLockScrollB);

		// lock state
		bool isLockNum		= (GetKeyState(VK_NUMLOCK) & 0x01)!= 0;
		bool isLockCaps		= (GetKeyState(VK_CAPITAL) & 0x01)!= 0;
		bool isLockScroll	= (GetKeyState(VK_SCROLL ) & 0x01)!= 0;
	
		// set num lock color
		unsigned char lockNumPktIdx		= 10;
		unsigned char lockNumPktOffset	= 4 +13 * 4 + 1 + 1;
		m_packets[lockNumPktIdx].data[lockNumPktOffset  ]= indicatorKeyLockNumR * isLockNum;
		m_packets[lockNumPktIdx].data[lockNumPktOffset+1]= indicatorKeyLockNumG * isLockNum;
		m_packets[lockNumPktIdx].data[lockNumPktOffset+2]= indicatorKeyLockNumB * isLockNum;
	
		// set num cap color
		unsigned char lockCapsPktIdx		= 10;
		unsigned char lockCapsPktOffset		= 4 +14 * 4 + 1 + 1;
		m_packets[lockCapsPktIdx].data[lockCapsPktOffset  ]= indicatorKeyLockCapR * isLockCaps;
		m_packets[lockCapsPktIdx].data[lockCapsPktOffset+1]= indicatorKeyLockCapG * isLockCaps;
		m_packets[lockCapsPktIdx].data[lockCapsPktOffset+2]= indicatorKeyLockCapB * isLockCaps;
	
		// set num scroll color
		unsigned char lockScrollPktIdx		= 11;
		unsigned char lockScrollPktOffset	= 4 + 0 * 4 + 1 + 1;
		m_packets[lockScrollPktIdx].data[lockScrollPktOffset  ]= indicatorKeyLockScrollR * isLockScroll;
		m_packets[lockScrollPktIdx].data[lockScrollPktOffset+1]= indicatorKeyLockScrollG * isLockScroll;
		m_packets[lockScrollPktIdx].data[lockScrollPktOffset+2]= indicatorKeyLockScrollB * isLockScroll;
	}

	Keyboard::commitKeyColor(renderTarget);
}
#endif

#undef REPORT_ID
