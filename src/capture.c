#include "capture.h"
#include "log.h"
// @NOTE: Windows Implementation
#include <d3d11.h>
#include <dxgi1_2.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")


void encode_frames(CaptureContext ctx, ScreenFrame frame[25])
{

	AVFrame *avf = av_frame_alloc();
	avf->format = ctx.encoder_ctx->pix_fmt;
	avf->width  = ctx.encoder_ctx->width;
	avf->height = ctx.encoder_ctx->height;

	make_re

	int res = av_frame_get_buffer(avf, 0);
	if(res < 0)
	{
		lerror("Failed to get frame buffer: %s", av_err2str(res));
		av_frame_free(&avf);
		return;
	}

	for(int i = 0; i < 25; ++i)
	{
		res = av_frame_make_writable(avf);
		if(res < 0)
		{
			lerror("Failed to make frame writable: %s", av_err2str(res));
			av_frame_free(&avf);
			return;
		}

		avf->pts = i;
		res = avcodec_send_frame(ctx.encoder_ctx, avf);
		if(res < 0)
		{
			lerror("Failed to send frame %d: %s", i, av_err2str(res));
			av_frame_free(&avf);
			return;
		}
		while(res >= 0)
		{
			res = avcodec_receive_packet(ctx.encoder_ctx, ctx.pkt);
			if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
				break;
			} else if (res < 0) {
				lerror("Error during encoding: %s", av_err2str(res));
				av_frame_free(&avf);
				return;
            }
		}
	}

	
	av_frame_free(&avf);
}

CaptureContext init_capture()
{
	CaptureContext res = {};

	enum AVCodecID codecs[] = {
		AV_CODEC_ID_AV1, AV_CODEC_ID_H265, AV_CODEC_ID_H264
	};

	const char *codec_names[] = {
		"AV1", "H265", "H264"
	};

	int found = 0;
	for(int i = 0; i < ARR_LEN(codecs); ++i)
	{
		res.encoder = avcodec_find_encoder(codecs[i]);
		res.decoder = avcodec_find_decoder(codecs[i]);
		if(res.encoder == NULL)
		{
			lerror("Couldn't get encoding: %s\n", codec_names[i]);
			continue;
		}
		if(res.decoder == NULL)
		{
			lerror("Couldn't get decoding: %s\n", codec_names[i]);
			continue;
		}
		res.codec_type = codecs[i];
		found = i;
		break;
	}
	
	if(res.encoder == NULL)
	{
		lfatal("Couldn't find a encoder");
	}
	if(res.decoder == NULL)
	{
		lfatal("Couldn't find a decoder");
	}
	linfo("Using codec: %s", codec_names[found]);

	res.encoder_ctx = avcodec_alloc_context3(res.encoder);
	res.decoder_ctx = avcodec_alloc_context3(res.decoder);

	if(res.encoder_ctx == NULL)
	{
		lfatal("Couldn't get codex for encoder");
	}
	if(res.decoder_ctx == NULL)
	{
		lfatal("Couldn't get codex for decoder");
	}

	res.encoder_ctx->bit_rate = KB(6000);
	res.encoder_ctx->width  = 1280;
	res.encoder_ctx->height = 720;
	res.encoder_ctx->time_base = (AVRational){1, 60};
	res.encoder_ctx->framerate = (AVRational){60, 1};
	res.encoder_ctx->gop_size  = 15;
	res.encoder_ctx->max_b_frames = 1;
	res.encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

	res.pkt = av_packet_alloc();
	if(res.pkt == NULL)
	{
		lfatal("Failed to allocate frame packet");
	}

	int open2_res = avcodec_open2(res.encoder_ctx, res.encoder, NULL);
	if(open2_res < 0)
	{
		lfatal("Failed to open encoder codec: %s", av_err2str(open2_res));
	}

	open2_res = avcodec_open2(res.decoder_ctx, res.decoder, NULL);
	if(open2_res < 0)
	{
		lfatal("Failed to open decoder codec: %s", av_err2str(open2_res));
	}

    IDXGIFactory1 *factory = NULL;
    IDXGIAdapter1 *adapter = NULL;
    IDXGIOutput *output = NULL;
    IDXGIOutput1 *output1 = NULL;
    ID3D11Device *device = NULL;
	ID3D11DeviceContext *context = NULL;
	IDXGIOutputDuplication *duplicate = NULL;

	CoInitialize(NULL);

	HRESULT hr;
	hr = CreateDXGIFactory1(&IID_IDXGIFactory1, (void **)&factory);
	if(FAILED(hr))
	{
		lfatal("CreateDXGIFactory1() failed: 0x%x", hr);
	}

	hr = D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			NULL, 0,
			D3D11_SDK_VERSION,
			&device,
			NULL,
			&context);

	if(FAILED(hr))
	{
		lfatal("D3D11CreateDevice() failed: 0x%x", hr);
	}

	hr = factory->lpVtbl->EnumAdapters1(factory, 0, &adapter);
	if(FAILED(hr))
	{
		lfatal("factory->EnumAdapters1(): 0x%x", hr);
	}

	hr = adapter->lpVtbl->EnumOutputs(adapter, 0, &output);
	if(FAILED(hr))
	{
		lfatal("adapter->EnumOptions() failed: 0x%x", hr);
	}

	hr = output->lpVtbl->QueryInterface(output, &IID_IDXGIOutput1, (void **)&output1);
	if(FAILED(hr))
	{
		lfatal("output->QueryInterface() failed: 0x%x", hr);
	}

	hr = output1->lpVtbl->DuplicateOutput(output1, (IUnknown *)device, &duplicate);
	if(FAILED(hr))
	{
		lfatal("output1->DuplicateOutput() failed: 0x%x", hr);
	}

	output1->lpVtbl->Release(output1);
	output->lpVtbl->Release(output);
	adapter->lpVtbl->Release(adapter);

	res.device = device;
	res.duplicate = duplicate;
	res.ctx = context;
	return res;
}

ScreenFrame capture_screen(CaptureContext ctx)
{
	HRESULT hr;

	DXGI_OUTDUPL_FRAME_INFO frame_info;
	IDXGIResource *resource = NULL;
	hr = ctx.duplicate->lpVtbl->AcquireNextFrame(ctx.duplicate, 0, &frame_info, &resource);
	if(hr == S_OK)
	{
		DXGI_OUTDUPL_DESC desc = {};
		ctx.duplicate->lpVtbl->GetDesc(ctx.duplicate, &desc);
		ID3D11Texture2D *texture;
		ID3D11Texture2D *desktop_image;
		D3D11_TEXTURE2D_DESC tex_desc = {};

		hr = resource->lpVtbl->QueryInterface(resource, &IID_ID3D11Texture2D, (void **)&desktop_image);
		if(FAILED(hr))
		{
			lerror("resouce->QueryInterface() failed: 0x%x", hr);
			ctx.duplicate->lpVtbl->ReleaseFrame(ctx.duplicate);
			return (ScreenFrame){};
		}

		// tex_desc.Width = desc.ModeDesc.Width;
		// tex_desc.Height = desc.ModeDesc.Height;
		// tex_desc.Format = desc.ModeDesc.Format;
		// tex_desc.ArraySize = 1;
		// tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		// tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
		// tex_desc.SampleDesc.Count = 1;
		// tex_desc.SampleDesc.Quality = 0;
		// tex_desc.MipLevels = 1;
		// tex_desc.Usage = D3D11_USAGE_DEFAULT;
		// tex_desc.CPUAccessFlags = 0;

		tex_desc.Width = desc.ModeDesc.Width;
		tex_desc.Height = desc.ModeDesc.Height;
		tex_desc.Format = desc.ModeDesc.Format;
		tex_desc.ArraySize = 1;
		tex_desc.BindFlags = 0;
		tex_desc.MiscFlags = 0;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.MipLevels = 1;
		tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		tex_desc.Usage = D3D11_USAGE_STAGING;

		hr = ctx.device->lpVtbl->CreateTexture2D(ctx.device, &tex_desc, NULL, &texture);
		if(FAILED(hr))
		{
			lerror("device->CreateTexture2D() failed: 0x%x", hr);
			ctx.duplicate->lpVtbl->ReleaseFrame(ctx.duplicate);
			return (ScreenFrame){};
		}

		ctx.ctx->lpVtbl->CopyResource(ctx.ctx, (ID3D11Resource *)texture, (ID3D11Resource *)desktop_image);
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		ctx.ctx->lpVtbl->Map(ctx.ctx, (ID3D11Resource *)texture, 0, D3D11_MAP_READ_WRITE, 0, &mapped);
		desktop_image->lpVtbl->Release(desktop_image);
		resource->lpVtbl->Release(resource);

		ScreenFrame result = {};
		result.pixels = mapped.pData;
		result.width = tex_desc.Width;
		result.height = tex_desc.Height;
		result.pitch = mapped.RowPitch;
		result._to_free = texture;
		return result;
	}

	return (ScreenFrame){};
}

void capture_free_frame(CaptureContext ctx, ScreenFrame frame)
{
	if(frame._to_free == NULL)
		return;
	frame._to_free->lpVtbl->Release(frame._to_free);
	ctx.duplicate->lpVtbl->ReleaseFrame(ctx.duplicate);
}



