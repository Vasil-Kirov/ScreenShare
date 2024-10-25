#pragma once
#include <vtypes.h>
#include <libavcodec/avcodec.h>

typedef struct ScreenFrame ScreenFrame;
typedef struct CaptureContext CaptureContext;

struct ScreenFrame {
	u8 *pixels;
	i32 width;
	i32 height;
	uint pitch;

	struct ID3D11Texture2D *_to_free;
};

struct CaptureContext {
	struct ID3D11Device *device;
	struct IDXGIOutputDuplication *duplicate;
	struct ID3D11DeviceContext *ctx;
	const AVCodec *encoder;
	const AVCodec *decoder;
	AVCodecContext *encoder_ctx;
	AVCodecContext *decoder_ctx;
	enum AVCodecID codec_type;
	AVPacket *pkt;
};

