
// by simon yeung, 10/10/2020
// all rights reserved

#include "Keyboard_One2RGB_TKL.h"
#include "Keyboard_One2RGB_TKL_connect.inl"
#include "Keyboard_One2RGB_TKL_disconnect.inl"

unsigned short Keyboard_One2RGB_TKL::productID= 0x356;

Keyboard_One2RGB_TKL::Keyboard_One2RGB_TKL(HIDDevice* device) : Keyboard(device)
{
	beginAddKey();
	addKeyTKL_ANSI();
	endAddKey();
}

Keyboard_One2RGB_TKL::~Keyboard_One2RGB_TKL()
{
}
	
void Keyboard_One2RGB_TKL::connect()
{
	// connect message
	for(int i= 0; i<16; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
	wait(200);
	for(int i= 16; i<48; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
	wait(1400);	
	for(int i= 48; i<1250; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
	wait(300);	
	for(int i= 1250; i<1548; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
	wait(300);
	for(int i= 1548; i<1844; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
	wait(400);	
	for(int i= 1844; i<1894; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);

	// CM 1
	wait(100);
	for(int i= 1894; i< 1934; ++i)
		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
	wait(500);

//	// assign color
//	for(int i= 1934; i<Keyboard_One2RGB_TKL_connect_num; ++i)
//		handlePacket(Keyboard_One2RGB_TKL_connect_pkts[i], (int)Keyboard_One2RGB_TKL_connect_size[i]);
}

void Keyboard_One2RGB_TKL::disconnect()
{
	// disconnect message
	for(int i= 0; i<10; ++i)
		handlePacket(Keyboard_One2RGB_TKL_disconnect_pkts[i], (int)Keyboard_One2RGB_TKL_disconnect_size[i]);
	wait(100);
	for(int i= 10; i<Keyboard_One2RGB_TKL_disconnect_num; ++i)
		handlePacket(Keyboard_One2RGB_TKL_disconnect_pkts[i], (int)Keyboard_One2RGB_TKL_disconnect_size[i]);
}
