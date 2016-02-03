
#include "stdafx.h"
#include "ImageParser.h"

HRESULT ImageParser::LoadImageFile(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory,
									PCWSTR uri,
									UINT destinationWidth,
									UINT destinationHeight,
									ID2D1Bitmap **ppBitmap,
									IWICBitmap **ppWicBitmap)
{
	HRESULT hRet = S_OK;

	IWICBitmapDecoder		*pDecoder = nullptr;
	IWICBitmapFrameDecode	*pSource = nullptr;
	IWICStream				*pStream = nullptr;
	IWICFormatConverter		*pConverter = nullptr;
	IWICBitmapScaler		*pScaler = nullptr;

	hRet = pIWICFactory->CreateDecoderFromFilename(uri, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
	if (SUCCEEDED(hRet))
	{
		hRet = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hRet))
	{
		hRet = pIWICFactory->CreateFormatConverter(&pConverter);
	}


	UINT originalWidth, originalHeight;
	hRet = pSource->GetSize(&originalWidth, &originalHeight);
	if (SUCCEEDED(hRet))
	{
		if (destinationWidth != 0 && destinationHeight != 0)
		{
			originalWidth = destinationWidth;
			originalHeight = destinationHeight;
		}

		hRet = pIWICFactory->CreateBitmapScaler(&pScaler);
		if (SUCCEEDED(hRet))
		{
			hRet = pScaler->Initialize(pSource, originalWidth, originalHeight, WICBitmapInterpolationModeCubic);
		}

		if (SUCCEEDED(hRet))
		{
			hRet = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.f,
				WICBitmapPaletteTypeMedianCut);
		}
	}

	if (SUCCEEDED(hRet))
	{
		hRet = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, ppBitmap);
	}

	if (SUCCEEDED(hRet))
	{
		hRet = pIWICFactory->CreateBitmapFromSource(pConverter, WICBitmapCacheOnLoad, ppWicBitmap);
	}
	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hRet;
}