// D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "D.h"

#define MAX_LOADSTRING 100

ID2D1Factory	*gD2dFactory = nullptr;
IWICImagingFactory	*gWICFactory = nullptr;
IWICBitmap	*gWicBitmap  = nullptr;
ID2D1RenderTarget *FRt = nullptr;
ID2D1GdiInteropRenderTarget *FInteropTarget = nullptr;
ID2D1RadialGradientBrush * FBrush = nullptr;
POINT FSourcePosition;
POINT FWindowPosition;
SIZE FSize;
BLENDFUNCTION FBlend;
int Width = 600;
int Height = 600;
ID2D1DCRenderTarget *dcRenderTarget = nullptr;
ID2D1HwndRenderTarget *hwndRenderTarget = nullptr;
ID2D1HwndRenderTarget *hwndRenderTarget1 = nullptr;
ID2D1Bitmap *gBitmap = nullptr;
ID2D1SolidColorBrush *gBrush = nullptr;
HDC gDC = nullptr;
// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

CDCHandle		hDcOffScreen = NULL;
CBitmap			hBmpOffScreen = NULL;
LPVOID			pBmpOffScreenBits = NULL;
// 窗体信息
int				nClientWidth = 0;
int				nClientHeight = 0;

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
ULONG_PTR gdiplusStartupToken;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	Gdiplus::GdiplusStartupInput gdiInput;
	Gdiplus::GdiplusStartup(&gdiplusStartupToken, &gdiInput, NULL);
	CoInitialize(nullptr);
 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &gD2dFactory);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Create D2D factory failed£¡", L"ÌáÊ¾", MB_OK);
		return 0;
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory, (LPVOID*)&gWICFactory);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Create WIC factory failed£¡", L"ÌáÊ¾", MB_OK);
		return 0;
	}

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CoUninitialize();
	Gdiplus::GdiplusShutdown(gdiplusStartupToken);
	return (int) msg.wParam;
}

HBITMAP CreateGDIBitmap(int nWid, int nHei, void ** ppBits)
{
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nWid;
	bmi.bmiHeader.biHeight = -nHei; // top-down image 
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;

	HDC hdc = ::GetDC(NULL);
	LPVOID pBits = NULL;
	HBITMAP hBmp = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, ppBits, 0, 0);
	::ReleaseDC(NULL, hdc);
	return hBmp;
}

HRESULT LoadImageFile(IWICImagingFactory *pIWICFactory,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight)
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
		hRet = dcRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, &gBitmap);
	}

	if (SUCCEEDED(hRet))
	{
		hRet = pIWICFactory->CreateBitmapFromSource(pConverter, WICBitmapCacheOnLoad, &gWicBitmap);
	}
	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hRet;
}

void CreateDeviceResources(HWND hWnd)
{
	FSourcePosition.x = FSourcePosition.y = 0;
	FWindowPosition.x = FWindowPosition.y = 0;
	FSize.cx = Width;      //600;
	FSize.cy = Height;     //400;

	FBlend.BlendOp = AC_SRC_OVER;
	FBlend.BlendFlags = 0;
	FBlend.SourceConstantAlpha = 255;
	FBlend.AlphaFormat = AC_SRC_ALPHA;

	D2D1_PIXEL_FORMAT pf = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		pf,
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

	HRESULT hr = gD2dFactory->CreateHwndRenderTarget(rtp,
		D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(600, 600)),
		&hwndRenderTarget);

	hwndRenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)(&FInteropTarget));

	

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE_PREMULTIPLIED),
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
		);

	hr = gD2dFactory->CreateDCRenderTarget(&props, &dcRenderTarget);
	LoadImageFile(gWICFactory, L"2.png", 68, 68);
	dcRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &gBrush);
}

//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;// AKEINTRESOURCE(IDC_D);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

void Render(HWND hwnd)
{ 
	PAINTSTRUCT ps;
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	CPaintDC dc(hwnd);
	
	if ((nClientWidth != rcClient.right - rcClient.left) ||
		(nClientHeight != rcClient.bottom - rcClient.top) ||
		hBmpOffScreen.IsNull()|| hDcOffScreen.IsNull())
	{
		nClientWidth = rcClient.right - rcClient.left;
		nClientHeight = rcClient.bottom - rcClient.top;

		if (!hBmpOffScreen.IsNull())
			hBmpOffScreen.DeleteObject();

		if (!hDcOffScreen.IsNull())
			hDcOffScreen.DeleteDC();
		hDcOffScreen.CreateCompatibleDC(dc);
		HBITMAP hBitmap = CreateGDIBitmap(nClientWidth, nClientHeight, &pBmpOffScreenBits);
		hBmpOffScreen.Attach(hBitmap);
		
		hDcOffScreen.SelectBitmap(hBmpOffScreen);
		hDcOffScreen.SetBkMode(TRANSPARENT);
	}

	HRESULT hr = dcRenderTarget->BindDC(hDcOffScreen, &rcClient);
	dcRenderTarget->BeginDraw();
	dcRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	D2D1_SIZE_F size = gBitmap->GetSize();
	D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);
	dcRenderTarget->DrawBitmap(
		gBitmap,
		D2D1::RectF(
		(rcClient.left),
		(rcClient.top),
		(size.width),
		(size.height)),
		1.0f);
	dcRenderTarget->EndDraw();
// 	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
// 	dc.AlphaBlend(0, 0, nClientWidth, nClientWidth
// 		, hDcOffScreen, 0, 0, nClientWidth, nClientWidth, bf);


// 
// 	POINT ptDest = { rcClient.left, rcClient.top };
// 	POINT ptSrc = { 0, 0 };
// 	SIZE szLayered = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
// 	BLENDFUNCTION bf;
// 	bf.AlphaFormat = AC_SRC_ALPHA;
// 	bf.BlendFlags = 0;
// 	bf.BlendOp = AC_SRC_OVER;
// 	bf.SourceConstantAlpha = 255;
// 	::UpdateLayeredWindow(hwnd, hDcOffScreen, &ptDest, &szLayered, hdc, &ptSrc, (COLORREF)0, &bf, ULW_ALPHA);
// 

}

void Render1(HWND hwnd)
{
	RECT rc;
	ZeroMemory(&rc, sizeof(rc));
	RECT rcClient;
	::GetWindowRect(hwnd, &rcClient);
	SIZE wndSize = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	HDC myhdc = NULL;

	hwndRenderTarget->BeginDraw();

	hwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Yellow));
	hwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	FInteropTarget->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &myhdc);

	gDC = ::CreateCompatibleDC(myhdc);
	HBITMAP memBitmap = ::CreateCompatibleBitmap(myhdc, wndSize.cx, wndSize.cy);
	::SelectObject(gDC, memBitmap);

	// 用gdi+来读取图片文件
	Gdiplus::Image image(L"bk1.png");
	Gdiplus::Graphics graphics(gDC);
	graphics.DrawImage(&image, 0, 0, wndSize.cx, wndSize.cy);
	// 
	// 		Gdiplus::Image image2(L"2.png");
	// 		Gdiplus::Graphics graphics2(gDC);
	// 		graphics2.DrawImage(&image2, 0, 0, 68, 68);

	POINT ptDest = { rcClient.left, rcClient.top };
	POINT ptSrc = { 0, 0 };
	SIZE szLayered = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	BLENDFUNCTION bf;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = 255;
	FInteropTarget->ReleaseDC(&rcClient);
	hwndRenderTarget->EndDraw();
	::UpdateLayeredWindow(hwnd, NULL, &ptDest, &szLayered, gDC, &ptSrc, RGB(0, 0, 0), &bf, ULW_ALPHA);
}

void Render2(HWND hwnd)
{
	RECT rc;
	ZeroMemory(&rc, sizeof(rc));
	RECT rcClient;
	::GetWindowRect(hwnd, &rcClient);
	SIZE wndSize = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	HDC hwndDC = GetDC(hwnd);

	gDC = ::CreateCompatibleDC(hwndDC);
	HBITMAP memBitmap = ::CreateCompatibleBitmap(hwndDC, wndSize.cx, wndSize.cy);
	::SelectObject(gDC, memBitmap);
	dcRenderTarget->BindDC(gDC, &rcClient);
	dcRenderTarget->BeginDraw();
	dcRenderTarget->DrawLine(D2D1::Point2F(200, 200), D2D1::Point2F(300, 300), gBrush, 50);
	dcRenderTarget->DrawLine(D2D1::Point2F(100, 100), D2D1::Point2F(100, 300), gBrush, 50);
	dcRenderTarget->DrawLine(D2D1::Point2F(0, 0), D2D1::Point2F(100, 600), gBrush);
	dcRenderTarget->DrawBitmap(gBitmap, D2D1::RectF(0, 0, 68, 68), 1.0f);
	dcRenderTarget->EndDraw();
	POINT ptDest = { rcClient.left, rcClient.top };
	POINT ptSrc = { 0, 0 };
	SIZE szLayered = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	BLENDFUNCTION bf;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = 255;
	::UpdateLayeredWindow(hwnd, hwndDC, &ptDest, &szLayered, gDC, &ptSrc, RGB(0, 0, 0), &bf, ULW_ALPHA);
	::ReleaseDC(nullptr, gDC);
	::ReleaseDC(hwnd, hwndDC);
}
//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  200, 100, Width, Height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   CreateDeviceResources(hWnd);
   SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
// 
   RECT rc;
   GetClientRect(hWnd, &rc);
   ::InvalidateRect(hWnd, &rc, FALSE);
   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	RECT rc;
	HDC hdc;
	ZeroMemory(&rc, sizeof(rc));
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		if (hwndRenderTarget)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			hwndRenderTarget->Resize(D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));
		}
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_LBUTTONDOWN:
		//SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, true, nullptr, 0);
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
// 	case WM_MOUSEMOVE:
// 		//SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, false, nullptr, 0);
// 		break;
	case WM_PAINT:
	{
		//Render(hWnd);
		//Render1(hWnd);
		Render2(hWnd);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
