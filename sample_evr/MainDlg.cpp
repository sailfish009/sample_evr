//========================================================================
// Mediafoundation EVR sample 
//------------------------------------------------------------------------
// Copyright (c) 2018 Ji Wong Park <sailfish009@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "stdafx.h"
#include "MainDlg.h"
#include <atldlgs.h>
#include <atomic>
#include <io.h>

///////////////////////////////////////////////////////////////////////////

#define  VIDEO_MEMORY_SIZE (4096*1080)

#include <d3d9.h>
#include <dshow.h>
#include <strmif.h>
#include <evr.h>
#include <atlcom.h>
#include <Mfapi.h>
#include <Mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")

#define SR(x) { if (x) x.Release(); }        //SAFE_RELEASE
#define SC    SUCCEEDED

///////////////////////////////////////////////////////////////////////////

BOOL video_toggle = FALSE;

// Source
CComPtr<IStream> pStream;
CComPtr<IMFByteStream> pByteStream;
CHAR *temp = (CHAR*)GlobalAlloc(GMEM_FIXED, VIDEO_MEMORY_SIZE);

// Sink
CComPtr<IMFMediaSink> pMediaSink;
CComPtr<IMFStreamSink> pStreamSink;
CComPtr<IMFActivate> pActivate;
CComPtr<IMFVideoRenderer> pEVR;
CComPtr<IEVRFilterConfig> pConfig;
CComPtr<IMFGetService> pGetService;
CComPtr<IMFVideoDisplayControl> pVDC;
HRESULT hr = S_OK;

///////////////////////////////////////////////////////////////////////////

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  switch (video_toggle)
  {
  case FALSE:
    video_toggle = TRUE;
    //run
    init();
    break;

  default:
    video_toggle = FALSE;
    //stop
    break;
  }

  return 0;
}


FILE *fp = nullptr;

void CMainDlg::init()
{
  // MF init
  ///////////////////////////////////////////////////////////////////////////
  hr = MFStartup(MF_VERSION);
  ///////////////////////////////////////////////////////////////////////////

  // File open
  ///////////////////////////////////////////////////////////////////////////
#if 1
  if (fopen_s(&fp, "../../nv12.yuv", "rb"))
  {
    printf("file open error\n");
    return;
  }
#endif

  // Media Source
  ///////////////////////////////////////////////////////////////////////////
  hr = ::CreateStreamOnHGlobal(temp, TRUE, &pStream);
  hr = MFCreateMFByteStreamOnStream(pStream, &pByteStream);
  // fread(temp, 1, VIDEO_MEMORY_SIZE, fp);

  
  ///////////////////////////////////////////////////////////////////////////

  // EVR sink
  ///////////////////////////////////////////////////////////////////////////
  hr = MFCreateVideoRendererActivate(view_hwnd, &pActivate);
  hr = pActivate->ActivateObject(IID_IMFMediaSink, (void**)&pMediaSink);
  hr = pMediaSink->QueryInterface(IID_IMFVideoRenderer, (void**)&pEVR);
  hr = pEVR->InitializeRenderer(NULL, NULL);
  hr = pEVR->QueryInterface(IID_IEVRFilterConfig, (void**)&pConfig);
  hr = pConfig->SetNumberOfStreams(16);
  hr = pEVR->QueryInterface(IID_IMFGetService, reinterpret_cast<LPVOID*>(&pGetService));
  hr = pGetService->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl, reinterpret_cast<LPVOID*>(&pVDC));

  if (SC(hr))
  {
    hr = pVDC->SetVideoWindow(view_hwnd);
    hr = pVDC->SetAspectRatioMode(MFVideoARMode_None);
    SR(pVDC);
  }

  hr = pMediaSink->AddStreamSink(1000, NULL, &pStreamSink);
  ///////////////////////////////////////////////////////////////////////////
}

void CMainDlg::run()
{
}

void CMainDlg::stop()
{
}