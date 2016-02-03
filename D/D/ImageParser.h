#ifndef _IMAGEPARSER_H_
#define _IMAGEPARSER_H_

#include <d2d1.h>
#include <wincodec.h>
class ImageParser
{
public:
	HRESULT LoadImageFile(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory,
							PCWSTR uri,
							UINT destinationWidth,
							UINT destinationHeight,
							ID2D1Bitmap **ppBitmap,
							IWICBitmap **ppWicBitma);
};
#endif