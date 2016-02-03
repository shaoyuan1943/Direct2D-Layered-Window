// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO:  在此处引用程序需要的其他头文件
#include <d2d1.h>
#include <wincodec.h>
#include "ImageParser.h"
#include <atlbase.h>
#include "atlapp.h"
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
#define SAFE_RELEASE(o)	if(0){o->Release(); o = nullptr;}