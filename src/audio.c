#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "log.h"

BOOL SupportsLoopbackCapture(IMMDevice *device)
{
    IAudioClient *pAudioClient = NULL;
    HRESULT hr = device->lpVtbl->Activate(device, &IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    if (FAILED(hr)) {
        return FALSE; // Device does not support loopback
    }

    // Get the mix format for the audio client
    WAVEFORMATEX *pwfx = NULL;
    hr = pAudioClient->lpVtbl->GetMixFormat(pAudioClient, &pwfx);
    if (FAILED(hr)) {
        pAudioClient->lpVtbl->Release(pAudioClient);
        return FALSE; // Could not get mix format
    }

    // Check if the device is a render device
    if (pwfx->wFormatTag != WAVE_FORMAT_PCM && pwfx->wFormatTag != WAVE_FORMAT_EXTENSIBLE) {
        CoTaskMemFree(pwfx);
        pAudioClient->lpVtbl->Release(pAudioClient);
        return FALSE; // Device does not support PCM format
    }

    // Free resources
    CoTaskMemFree(pwfx);
    pAudioClient->lpVtbl->Release(pAudioClient);
    return TRUE; // Device supports loopback capture
}

void init_wasapi()
{
	IMMDeviceEnumerator *enumerator = NULL;
	HRESULT hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&enumerator);

	if(FAILED(hr))
	{
		lfatal("CoCreateInstance() failed: 0x%x", hr);
	}

	IMMDeviceCollection *devices = NULL;
	hr = enumerator->lpVtbl->EnumAudioEndpoints(enumerator, eRender, DEVICE_STATE_ACTIVE, &devices);
	if(FAILED(hr))
	{
		lfatal("enumerator->EnumAudioEndpoints() failed: 0x%x", hr);
	}

	IAudioClient
	uint count = 0;
	hr = devices->lpVtbl->GetCount(devices, &count);
	if(FAILED(hr))
	{
		lfatal("devices->GetCount() failed: 0x%x", hr);
	}

	for(int i = 0; i < count; ++i)
	{
		IMMDevice *device = NULL;
		hr = devices->lpVtbl->Item(devices, i, &device);
		if(FAILED(hr))
		{
			lfatal("devices->Item(%d) failed: 0x%x", i, hr);
		}

		IPropertyStore *props = NULL;
		hr = device->lpVtbl->OpenPropertyStore(device, STGM_READ, &props);
		if(FAILED(hr))
		{
			lfatal("devices[%d]->OpenPropertyStore() failed: 0x%x", i, hr);
		}
		PROPVARIANT info = {};
		hr = props->lpVtbl->GetValue(props, &PKEY_Device_FriendlyName, &info);
		if(FAILED(hr))
		{
			lfatal("devices[%d]->prop->GetValue() failed: 0x%x", i, hr);
		}

		if(SupportsLoopbackCapture(device))
		{
			linfo("device: %S(loopback)", info.pwszVal);
		}
		else
		{
			linfo("device: %S", info.pwszVal);
		}

	}

}


