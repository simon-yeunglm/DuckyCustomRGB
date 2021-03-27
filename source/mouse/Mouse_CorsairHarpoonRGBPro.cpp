
// by simon yeung, 27/03/2021
// all rights reserved

#include "hid/HIDDevice.h"
#include "Mouse_CorsairHarpoonRGBPro.h"
#include "Mouse_CorsairHarpoonRGBPro_connect.inl"
#include "Mouse_CorsairHarpoonRGBPro_disconnect.inl"
#include <memory>

#define REPORT_ID	0x00

unsigned short Mouse_CorsairHarpoonRGBPro::productID= 0x1b75;

// mouse input USB packet not listened, but decoded as below format
// 10 bytes packets received in end-point 1
// 
// 01 XX 00 00 00 YY YY ZZ ZZ WW
//
// XX = 1 byte bit mask 0x01 = left click
//						0x02 = right click
//						0x04 = middle click
//						0x08 = back
//						0x10 = forward
//						0x20 = DPI button
//
// YY YY = 2 bytes indicate mouse movement
// ZZ ZZ = 2 bytes indicate mouse movement
// WW = 1 byte scroll wheel 0x01 = scroll up
//							0xff = scroll down
//

Mouse_CorsairHarpoonRGBPro::Mouse_CorsairHarpoonRGBPro(HIDDevice* device) : Mouse(device)
{
}

Mouse_CorsairHarpoonRGBPro::~Mouse_CorsairHarpoonRGBPro()
{
}
	
void Mouse_CorsairHarpoonRGBPro::connect()
{
	// connect message
	for(int i= 0; i<Mouse_CorsairHarpoonRGBPro_connect_num; ++i)
		handlePacket(Mouse_CorsairHarpoonRGBPro_connect_pkts[i], (int)Mouse_CorsairHarpoonRGBPro_connect_size[i]);

	// turn off LED
	setColor(0, 0, 0);
}

void Mouse_CorsairHarpoonRGBPro::disconnect()
{
	// disconnect message
	for(int i= 0; i<Mouse_CorsairHarpoonRGBPro_disconnect_num; ++i)
		handlePacket(Mouse_CorsairHarpoonRGBPro_disconnect_pkts[i], (int)Mouse_CorsairHarpoonRGBPro_disconnect_size[i]);
}

void Mouse_CorsairHarpoonRGBPro::setColor(float R, float G, float B)
{
	// apply gamma to the color to make it better match the color of other devices
	R*= R * R;
	G*= G * G;
	B*= B * B;
	
	unsigned char Rc= (unsigned char)(R * 255.0f);
	unsigned char Gc= (unsigned char)(G * 255.0f);
	unsigned char Bc= (unsigned char)(B * 255.0f);

	static unsigned char sendData[65];
	memset(&sendData, 0, sizeof(sendData));
	sendData[0]= 0;
	sendData[1]= 0x07;
	sendData[2]= 0x22;
	sendData[3]= 0x01;
	sendData[4]= 0x01;
	sendData[5]= 0x03;
	sendData[6]= Rc;
	sendData[7]= Gc;
	sendData[8]= Bc;

	// send packet
	int bytes_written= m_device->write(sendData, sizeof(sendData));
}

#undef REPORT_ID
