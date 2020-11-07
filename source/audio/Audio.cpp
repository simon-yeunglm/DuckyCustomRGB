
// by simon yeung, 26/10/2020
// all rights reserved

#include "win.h"
#include <objbase.h>
#include <mmdeviceapi.h>
#include "Audio.h"
#include "AudioEndpointVolumeCallback.h"

AudioEndpointVolumeCallback*	s_volumeCallback= nullptr;
IAudioEndpointVolume*			s_endpointVolume= nullptr;

void Audio::init()
{
	HRESULT hr= CoInitialize(nullptr);

	IMMDeviceEnumerator*	deviceEnumerator= nullptr;
	hr= CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
	if (hr != S_OK)
		return;
	
	IMMDevice*	defaultDevice	= nullptr;
	hr= deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	if (hr != S_OK)
		return;

	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID*)&s_endpointVolume);
	if (hr == S_OK)
	{
		s_volumeCallback= new AudioEndpointVolumeCallback(); 
		s_endpointVolume->RegisterControlChangeNotify(s_volumeCallback); 
	}
	defaultDevice->Release();
}

void Audio::relrease()
{
	if (s_endpointVolume)
	{
		if (s_volumeCallback)
		{
			s_endpointVolume->UnregisterControlChangeNotify(s_volumeCallback);
			s_volumeCallback->Release(); 
		}
		s_endpointVolume->Release(); 
	}
	CoUninitialize();
}

void Audio::registerVolumeCallback(AudioVolumeCallback callback, void* userPtr)
{
	if (s_volumeCallback)
		s_volumeCallback->registerVolumeCallback(callback, userPtr);
}

void Audio::deregisterVolumeCallback(AudioVolumeCallback callback, void* userPtr)
{
	if (s_volumeCallback)
		s_volumeCallback->deregisterVolumeCallback(callback, userPtr);
}
