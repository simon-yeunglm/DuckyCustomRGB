
// by simon yeung, 10/10/2020
// all rights reserved

#if _DEBUG
#define MEM_LEAK_DETECT	(1)
#else
#define MEM_LEAK_DETECT	(0)
#endif
#if MEM_LEAK_DETECT
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "hid/HID.h"
#include "hid/HIDDevice.h"
#include "keyboard/Keyboard_One2RGB_TKL.h"
#include "util/Timer.h"
#include "math/Math.h"
#include "audio/Audio.h"
#include "macro/MacroPlayer.h"
#include "render/RenderGraph.h"

#include "render/pass/RenderPass_ColorBreath.h"
#include "render/pass/RenderPass_ColorScroll.h"
#include "render/pass/RenderPass_ReactiveKey.h"
#include "render/pass/RenderPass_ReactiveRipple.h"
#include "render/pass/RenderPass_KeyStateBlend.h"
#include "render/pass/RenderPass_KeyLockBlink.h"
#include "render/pass/RenderPass_AudioVolumeBar.h"
#include "render/pass/RenderPass_Idle.h"
#include "render/pass/RenderPass_ReactivePulse.h"

#include "macro/action/MacroAction_KeyPress.h"
#include "macro/action/MacroAction_KeyDown.h"
#include "macro/action/MacroAction_KeyUp.h"
#include "macro/action/MacroAction_Delay.h"
#include "macro/action/MacroAction_VolumeUp.h"
#include "macro/action/MacroAction_VolumeDown.h"
#include "macro/action/MacroAction_MediaPlayPause.h"
#include "macro/action/MacroAction_OpenExe.h"


#if _DEBUG
#define QUIT_HOLD_ESC_TIME		(0.01f)	// seconds
#else
#define QUIT_HOLD_ESC_TIME		(5.0f)	// seconds
#endif

static void*				s_rawInputData		= nullptr;
static UINT					s_rawInputDataSz	= 0;

// for code gen
static void printPacketArrayString(const char* packetStr, int pkt_num)
{
	printf("static const int %snum= %i;\n", packetStr, pkt_num);
	printf("static const unsigned char* %spkts[]=\n{\n", packetStr);
	for(int i=0;i < pkt_num; ++i)
	{
		printf("\t%spkt%i,\n", packetStr, i+1);
	}
	printf("};\n");
	printf("static const size_t %ssize[]=\n{\n", packetStr);
	for(int i=0;i < pkt_num; ++i)
	{
		printf("\tsizeof(%spkt%i),\n", packetStr, i+1);
	}
	printf("};\n\n");
}

static void Macrolayer_keyPress(Keyboard* keyboard, KeyboardKey key, bool isPressed, void* userPtr)
{
	MacroPlayer* macroPlayer= (MacroPlayer*)userPtr;
	macroPlayer->keyStateChanged(keyboard, key, isPressed);
}

// WndProc is called when a window message is sent to the handle of the window
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
			Keyboard* keyboard= (Keyboard*)pCreateStruct->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)keyboard);

			RAWINPUTDEVICE  rid;
			rid.dwFlags=RIDEV_NOLEGACY|RIDEV_INPUTSINK;    
			rid.usUsagePage=1;
			rid.usUsage=6;
			rid.hwndTarget=hWnd;
			RegisterRawInputDevices(&rid,1,sizeof(rid));
			break;
		}
		case WM_INPUT:
		{
			Keyboard* keyboard= (Keyboard*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (keyboard== nullptr)
				break;

			UINT dataSize= 0;
			if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT,NULL,&dataSize,sizeof(RAWINPUTHEADER)) == -1)
				break;

			// allocate input data memory
			if (dataSize > s_rawInputDataSz)
			{
				if (s_rawInputData)
					free(s_rawInputData);
				s_rawInputData	= malloc(dataSize);
				s_rawInputDataSz= dataSize;
			}

			// get raw input
			if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, s_rawInputData, &dataSize, sizeof(RAWINPUTHEADER)) != dataSize)
				break;
			
			// skip program generated scan code
			PRAWINPUT	raw			=(PRAWINPUT)s_rawInputData;
			if (raw->header.hDevice == nullptr)
				break;

			// skip not used key code
			bool		hasPrefixE1	= (raw->data.keyboard.Flags & RI_KEY_E1) != 0;
			if (hasPrefixE1)
				break;

			// key keyboard state and convert to KeyboardKey
			bool		hasPrefixE0		= (raw->data.keyboard.Flags & RI_KEY_E0) != 0;
			bool		isPressed		= (raw->data.keyboard.Flags & 0x01) == 0;
			KeyboardKey	key				= KeyboardKeyFromScanCode(raw->data.keyboard.MakeCode, hasPrefixE0);
			if (key != KeyboardKey::Num)	// skip error key code/redundant print screen key code
				keyboard->keyStateChanged(key, isPressed);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
};

Keyboard*	createKeyboard(HIDDevice* device, unsigned short keyboardID)
{
	if (device == nullptr)
		return nullptr;

	// TODO: use a hash table instead if have too many keyboard type
	//       and load all the keyboard data from file instead of hard code Keyboard class
	if (keyboardID == 0x0356)	// One 2 RGB TKL ANSI
		return new Keyboard_One2RGB_TKL(device);
	if (keyboardID == 0x0348)	// Shine 7 ISO
		return nullptr;
	else
		return nullptr;
}

void		destroyKeyboard(Keyboard* keyboard)
{
	if (keyboard)
		delete keyboard;
}

HWND		createWindow(HINSTANCE hInstance, Keyboard* keyboard)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.hInstance     = hInstance;
	wc.lpszClassName = L"KeyboardWindowClass";
	wc.lpfnWndProc   = WndProc;
	RegisterClass(&wc);
	return CreateWindow(wc.lpszClassName,NULL,0,0,0,0,0,HWND_MESSAGE,NULL,hInstance, keyboard);
}

void		destroyWindow(HWND windowHandle)
{
	if (windowHandle)
		DestroyWindow(windowHandle);
}

bool		pollWindowInput(HWND windowHandle, bool isBlocking)
{
	MSG msg;
	if (isBlocking && GetMessage(&msg, windowHandle, WM_INPUT , WM_INPUT ))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	while(PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return true;
			
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return false;
}

#if _DEBUG	// it is more convenient to have a console window for debugging
int main(HINSTANCE hInstance)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
#if DEBUG_DISABLE_HID_COMM
	printf("\n========== DEBUG_DISABLE_HID_COMM ==========\n\n");
#endif
// // example debug code gen
//	printPacketArrayString("Keyboard_One2RGB_TKL_connect_", 1970);
//	printPacketArrayString("Keyboard_One2RGB_TKL_disconnect_", 298);
	
	// lower program priority
	HANDLE processHandle= GetCurrentProcess();
	bool ok= SetPriorityClass(processHandle, IDLE_PRIORITY_CLASS);
	HANDLE threadHandle= GetCurrentThread();
	ok= SetThreadPriority(threadHandle, THREAD_PRIORITY_IDLE);

	// create a new scope to call ~MacroPlayer() before _CrtDumpMemoryLeaks()
	{
		// init
		const unsigned short	DuckyVID	= 0x04d9;
		Audio::init();
		HID::init();
		HIDDevice*	deivce	= HIDDevice::createDevice(DuckyVID, 0, 1, 0xff00, 0x01);
		Keyboard*	keyboard= createKeyboard(deivce, deivce->getProductID());

		Timer		timer;
		MacroPlayer	macroPlayer;
		RenderGraph*renderGraph= new RenderGraph(keyboard);
		
		KeyboardKey		ctrlShiftKeys[]	= {KeyboardKey::Shift_Right	, KeyboardKey::Control_Right};
		KeyboardState	ctrlShiftState	= KeyboardState(ctrlShiftKeys, _countof(ctrlShiftKeys));
		macroPlayer.addMacro(ctrlShiftState, 0.0f, 0.0f, 1, new MacroAction_OpenExe("notepad"));

		KeyboardKey		ctrlEnterKeys[]	= {KeyboardKey::Enter	, KeyboardKey::Control_Right};
		KeyboardState	ctrlEnterState	= KeyboardState(ctrlEnterKeys, _countof(ctrlEnterKeys));
		macroPlayer.addMacro(ctrlEnterState, 0.0f, 0.0f, 4, 
			// undo the enter key
			new MacroAction_KeyDown(KeyboardKey::Control_Left), 
			new MacroAction_KeyPress(KeyboardKey::Z), 
			new MacroAction_KeyUp(KeyboardKey::Control_Left), 
			// start calculator
			new MacroAction_OpenExe("calc"));
		
		const float		volumeRepeatTime	= 0.03f;
		const float		volumeRepeatDelay	= 0.2f;
		KeyboardKey		ctrlUpKeys[]		= {KeyboardKey::Arrow_Up	, KeyboardKey::Control_Right};
		KeyboardState	ctrlUpState			= KeyboardState(ctrlUpKeys, _countof(ctrlUpKeys));
		macroPlayer.addMacro(ctrlUpState	, volumeRepeatTime, volumeRepeatDelay, 1, new MacroAction_VolumeUp());
		
		KeyboardKey		ctrlInsKeys[]		= {KeyboardKey::Insert		, KeyboardKey::Control_Right};
		KeyboardState	ctrlInsState		= KeyboardState(ctrlInsKeys, _countof(ctrlInsKeys));
		macroPlayer.addMacro(ctrlInsState	, volumeRepeatTime, volumeRepeatDelay, 1, new MacroAction_VolumeUp());
		
		KeyboardKey		ctrlDownKeys[]		= {KeyboardKey::Arrow_Down	, KeyboardKey::Control_Right};
		KeyboardState	ctrlDownState		= KeyboardState(ctrlDownKeys, _countof(ctrlDownKeys));
		macroPlayer.addMacro(ctrlDownState	, volumeRepeatTime, volumeRepeatDelay, 1, new MacroAction_VolumeDown());
		
		KeyboardKey		ctrlDelKeys[]		= {KeyboardKey::Delete	, KeyboardKey::Control_Right};
		KeyboardState	ctrlDelState		= KeyboardState(ctrlDelKeys, _countof(ctrlDelKeys));
		macroPlayer.addMacro(ctrlDelState	, volumeRepeatTime, volumeRepeatDelay, 1, new MacroAction_VolumeDown());
		
		KeyboardKey		ctrlHomeKeys[]		= {KeyboardKey::Home	, KeyboardKey::Control_Right};
		KeyboardState	ctrlHomeState		= KeyboardState(ctrlHomeKeys, _countof(ctrlHomeKeys));
		macroPlayer.addMacro(ctrlHomeState	, 0.0f, 0.0f, 1, new MacroAction_MediaPlayPause());
		
		// set keyboard LED
		float3 holdControlColor[]	= { {1.0f, 0.1f, 0.1f}, {0.65f, 0.05f, 0.05f},  };
		float3 holdShiftColor[]		= { {0.1f, 0.1f, 1.0f}, {0.05f, 0.05f, 0.65f},  };
		float3 holdAltColor[]		= { {1.0f, 0.9f, 0.1f}, {0.65f, 0.5f, 0.05f},  };
		float3 scrollColor[]= { 
			{1.0f , 0.0f , 0.0f }, // red
			{1.0f , 0.5f , 0.0f }, // orange
			{1.0f , 1.0f , 0.0f }, // yellow
			{0.5f , 0.75f, 1.0f }, // cyan 1
			{0.25f, 0.5f , 1.0f }, // cyan 2
			{0.0f , 0.0f , 1.0f }, // blue
			{0.5f , 0.0f , 0.75f}, // purple
			{1.0f , 0.0f , 0.75f}, // pink
		};
		
		KeyboardKey		controlKeys[]	= {KeyboardKey::Control_Left, KeyboardKey::Control_Right};
		KeyboardKey		shiftKeys[]		= {KeyboardKey::Shift_Left	, KeyboardKey::Shift_Right	};
		KeyboardKey		altKeys[]		= {KeyboardKey::Alt_Left	, KeyboardKey::Alt_Right	};
		KeyboardState	controlState	= KeyboardState(controlKeys	, _countof(controlKeys	));
		KeyboardState	shiftState		= KeyboardState(shiftKeys	, _countof(shiftKeys	));
		KeyboardState	altState		= KeyboardState(altKeys		, _countof(shiftKeys	));

		KeyboardKey ignorePressKeys[]= { KeyboardKey::F5, KeyboardKey::F7, KeyboardKey::SpaceBar, KeyboardKey::Enter };
		KeyboardKey shortRippleKeys[]= { KeyboardKey::F5, KeyboardKey::F7, KeyboardKey::SpaceBar };
		renderGraph->setGraph( 
			new RenderPass_Idle(14.0f * 60.0f,
				new RenderPass_AudioVolumeBar(0.25f, 1.75f, 0.5f, 
					new RenderPass_KeyLockBlink(2.0f,
						new RenderPass_ReactiveRipple(			{ 1.0f, 0.5f, 0.1f }, { 1.0f, 0.85f, 0.2f }, 6.0f, 2.5f, 0.3f, KeyboardState(KeyboardKey::Enter		),	
							new RenderPass_ReactiveRipple(		{ 1.0f, 0.85f, 0.05f}, { 1.0f, 1.0f, 1.0f }, 1.5f, 2.5f, 0.2f, KeyboardState(KeyboardKey::SpaceBar	),	
								new RenderPass_ReactiveRipple(	{ 1.0f, 1.0f, 0.2f }, { 1.0f, 0.5f, 0.1f }, 1.5f, 2.5f, 0.2f, KeyboardState(shortRippleKeys, _countof(shortRippleKeys)),	
									new RenderPass_ReactivePulse({0.95f,1.0f, 1.0f }, { 1.0f,1.0f,1.0f} , -18.0f, 0.67f	, KeyboardState(KeyboardKey::BackSpace), 
										new RenderPass_ReactiveKey( { 0.95f,1.0f,1.0f}, 0.5f, KeyboardState(ignorePressKeys		, _countof(ignorePressKeys	), false),	
										new RenderPass_KeyStateBlend(0.125f		,	new RenderPass_ColorScroll(scrollColor		, _countof(scrollColor		), 10.0f, 1.5f	, nullptr),
																	controlState,	new RenderPass_ColorBreath(holdControlColor	, _countof(holdControlColor	), 0.4f			, nullptr),	
																	shiftState	,	new RenderPass_ColorBreath(holdShiftColor	, _countof(holdShiftColor	), 0.4f			, nullptr),	
																	altState	,	new RenderPass_ColorBreath(holdAltColor		, _countof(holdAltColor		), 0.4f			, nullptr)	)
										)
									)
								)
							)
						)
					)
				)
			)
		);

		if (keyboard)
		{
			// bind keyboard macro
			keyboard->registerKeyPressCallback(&Macrolayer_keyPress, &macroPlayer);
		
			// try connect to the keyboard
			keyboard->connect();

			// set all color to black
			keyboard->commitKeyColor();		
			Sleep(1000);
		}

		// create hidden window to listen key press
		HWND windowHandle= createWindow(hInstance, keyboard);

		// start listening key press and update LED animation
		timer.restart();
		double lastFrameTime= timer.getTotalTime();
		Sleep(1);
		bool isBlockingInput= false;
		float quitTimer= 0.0f;	// quit the program by holding the escape key for a while
		while (quitTimer < QUIT_HOLD_ESC_TIME)
		{
			// poll input
			if (pollWindowInput(windowHandle, isBlockingInput))
				break;

			// tick timer
			timer.tick();
			double	frameStartTime	= timer.getTotalTime();
			float	elapsedTime		= (float)(frameStartTime - lastFrameTime);

			// update quit timer
			if (keyboard && keyboard->isKeyDown(KeyboardKey::Escape))
				quitTimer+= Math::min(elapsedTime, 0.5f);	// min to avoid large delta time when wake up from blocking input
			else
				quitTimer= 0.0f;

			// update keyboard macro and LED
			float	desireFrameRate	= macroPlayer.update(elapsedTime);
			desireFrameRate			= Math::max(renderGraph->update(elapsedTime), desireFrameRate);
			isBlockingInput			= desireFrameRate <= 0.0f;
			renderGraph->render();

			// sleep a while to achieve target framerate
			float updateRenderTime= timer.tick();
			if (desireFrameRate > 0)
			{
				const float bufferTime	= 0.004f;	// sleep a feww ms less to make the thread wake up ealier to have elapsedTime better match with desireFrameRate due to Windows task scheduling
				float desireFrameTime= 1.0f / desireFrameRate;
				float sleepTime	= Math::max(desireFrameTime - updateRenderTime - bufferTime, 0.001f);	// sleep at least 1 ms to avoid draining all CPU resources
				Sleep((DWORD)(sleepTime * 1000));
			}
			lastFrameTime= frameStartTime;
		}

		// release
		if (keyboard)
		{
			// unbind keyboard macro
			keyboard->deregisterKeyPressCallback(&Macrolayer_keyPress, &macroPlayer);

			// disconnect keyboard
			Sleep(1000);
			keyboard->disconnect();
			destroyKeyboard(keyboard);
		}
		SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)nullptr);
		destroyWindow(windowHandle);
		delete renderGraph;

		if (deivce)
			HIDDevice::destroyDevice(deivce);
		HID::release();
		Audio::relrease();
		if (s_rawInputData)
			free(s_rawInputData);
	}
#if MEM_LEAK_DETECT
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
