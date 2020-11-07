#pragma once

// by simon yeung, 26/10/2020
// all rights reserved

#include <endpointvolume.h>
#include "AudioVolumeCallback.h"

class AudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback 
{
private:
	LONG				m_refCnt;
	AudioVolumeCallback	m_callback;
	void*				m_userPtr;

public:
	AudioEndpointVolumeCallback()
	{
		m_refCnt	= 1;
		m_callback	= nullptr;
		m_userPtr	= nullptr;
	}
	virtual ~AudioEndpointVolumeCallback()
	{
	}

	virtual STDMETHODIMP_(ULONG)    AddRef()override
	{
        return InterlockedIncrement(&m_refCnt);
	}

	virtual STDMETHODIMP_(ULONG)    Release()override
	{
		LONG ref = InterlockedDecrement(&m_refCnt);  
		if (ref == 0) 
			delete this; 
		return ref; 
	}

	virtual STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)override
	{
		if (riid == IID_IUnknown || riid== __uuidof(IAudioEndpointVolumeCallback))  
		{ 
			*ppvObject = static_cast<IUnknown*>(this); 
			AddRef(); 
			return S_OK; 
		} 
		else
		{
			*ppvObject= nullptr; 
			return E_NOINTERFACE; 
		}
	} 

	virtual STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)override
	{ 
		if (m_callback)
			m_callback(pNotify->fMasterVolume * !pNotify->bMuted, m_userPtr);
		return S_OK;
	} 
	
	inline void registerVolumeCallback(AudioVolumeCallback callback, void* userPtr)
	{
		m_callback	= callback;
		m_userPtr	= userPtr;
	}
	
	inline void deregisterVolumeCallback(AudioVolumeCallback callback, void* userPtr)
	{
		if (m_callback	== callback &&
			m_userPtr	== userPtr )
		{
			m_callback	= nullptr;
			m_userPtr	= nullptr;
		}
	}
};
