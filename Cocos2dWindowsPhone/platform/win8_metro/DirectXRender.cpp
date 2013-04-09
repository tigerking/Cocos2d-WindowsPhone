/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* 
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
* 
* http://www.apache.org/licenses/LICENSE-2.0 
* 
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and limitations under the License.
*/

#include "pch.h"
#include "DirectXRender.h" 
#include "DXGI.h"
#include "exception\CCException.h"
#include "CCEGLView.h"
#include "CCApplication.h"

#include "CCDrawingPrimitives.h"

//#include "Classes\HelloWorldScene.h"
#include "d3d10.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;


#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
typedef FTTextPainter TextPainter;
#else
typedef DXTextPainter TextPainter;
using namespace D2D1;
#endif


USING_NS_CC;

static CCPoint getCCPointFromScreen(Point point)
{
	CCSize viewSize = cocos2d::CCEGLView::sharedOpenGLView()->getSize();

	CCPoint ccPoint;
	ccPoint.x = ceilf(point.X);
	ccPoint.y = ceilf(point.Y);

	return ccPoint;
}

static DirectXRender^ s_pDXRender;

// Constructor.
DirectXRender::DirectXRender()
	: m_dpi(-1.0f)
	, m_windowClosed(true)
{
	s_pDXRender = this;
}
void DirectXRender::Initialize(_In_ ID3D11Device1* device)
{
	m_d3dDevice = device;
	CreateDeviceResources();
}
// Initialize the Direct3D resources required to run.
//void DirectXRender::Initialize(CoreWindow^ window, float dpi)
//{
//	m_window = window;
//	m_windowClosed = false;
//	m_textPainter = ref new TextPainter();
//
//	window->Closed += 
//		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &DirectXRender::OnWindowClosed);
//
//	window->VisibilityChanged +=
//		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXRender::OnWindowVisibilityChanged);
//
//	window->SizeChanged += 
//		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &DirectXRender::OnWindowSizeChanged);
//
//	window->PointerPressed += 
//		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &DirectXRender::OnPointerPressed);
//
//	window->PointerReleased +=
//		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &DirectXRender::OnPointerReleased);
//
//	window->PointerMoved +=
//		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &DirectXRender::OnPointerMoved);
//
//	window->CharacterReceived += 
//		ref new TypedEventHandler<CoreWindow^, CharacterReceivedEventArgs^>(this, &DirectXRender::OnCharacterReceived);
//	 
//	CreateDeviceIndependentResources();
//	CreateDeviceResources();
//	SetDpi(dpi);
//	SetRasterState();
//	//Render();
//	//Present();
//}

// These are the resources required independent of hardware.

void DirectXRender::CreateDeviceIndependentResources()
{
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#else
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	DX::ThrowIfFailed(
		D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1),
		&options,
		&m_d2dFactory
		)
		);

	DX::ThrowIfFailed(
		DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		&m_dwriteFactory
		)
		);

	DX::ThrowIfFailed(
		CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_wicFactory)
		)
		);
#endif
}

// These are the resources that depend on the device.
void DirectXRender::CreateDeviceResources()
{
//	// This flag adds support for surfaces with a different color channel ordering than the API default.
//	// It is recommended usage, and is required for compatibility with Direct2D.
//	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//	ComPtr<IDXGIDevice> dxgiDevice;
//
//#if defined(_DEBUG)
//	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
//	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//
//	// This array defines the set of DirectX hardware feature levels this app will support.
//	// Note the ordering should be preserved.
//	// Don't forget to declare your application's minimum required feature level in its
//	// description.  All applications are assumed to support 9.1 unless otherwise stated.
//	D3D_FEATURE_LEVEL featureLevels[] = 
//	{
//#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
//		D3D_FEATURE_LEVEL_9_3,
//		D3D_FEATURE_LEVEL_9_2,
//		D3D_FEATURE_LEVEL_9_1
//#else
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0,
//		D3D_FEATURE_LEVEL_10_1,
//		D3D_FEATURE_LEVEL_10_0,
//		D3D_FEATURE_LEVEL_9_3,
//		D3D_FEATURE_LEVEL_9_2,
//		D3D_FEATURE_LEVEL_9_1
//#endif
//	};
//
//	// Create the DX11 API device object, and get a corresponding context.
//	ComPtr<ID3D11Device> device;
//	ComPtr<ID3D11DeviceContext> context;
//
//#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
//	DX::ThrowIfFailed(D3D11CreateDevice(
//		nullptr,                   // specify null to use the default adapter
//		D3D_DRIVER_TYPE_HARDWARE,
//		nullptr,                          // leave as 0 unless software device
//		creationFlags,              // optionally set debug and Direct2D compatibility flags
//		featureLevels,              // list of feature levels this app can support
//		ARRAYSIZE(featureLevels),   // number of entries in above list
//		D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION for Metro style apps
//		&device,                    // returns the Direct3D device created
//		&m_featureLevel,            // returns feature level of device created
//		&context                    // returns the device immediate context
//		));
//#else
//	ComPtr<IDXGIFactory> factory;
//	DX::ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)(&factory)));
//
//	// find a video card support the feature we need
//	UINT i = 0;
//
//	ComPtr<IDXGIAdapter> adapter;
//
//	while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
//	{
//		DX::ThrowIfFailed(D3D11CreateDevice(
//			nullptr,                   // specify null to use the default adapter
//			D3D_DRIVER_TYPE_HARDWARE,
//			nullptr,                          // leave as 0 unless software device
//			creationFlags,              // optionally set debug and Direct2D compatibility flags
//			featureLevels,              // list of feature levels this app can support
//			ARRAYSIZE(featureLevels),   // number of entries in above list
//			D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION for Metro style apps
//			&device,                    // returns the Direct3D device created
//			&m_featureLevel,            // returns feature level of device created
//			&context                    // returns the device immediate context
//			));
//		// if this video card is supported by dx11, use this one
//		if (m_featureLevel >= D3D_FEATURE_LEVEL_11_0)
//			break;
//		++i;
//	}
//#endif

//	//if (m_featureLevel < D3D_FEATURE_LEVEL_11_0)
//	//	throw ref new Platform::COMException(kCCExceptionNoSupportDX11);
//
//	// Get the DirectX11.1 device by QI off the DirectX11 one.
//	DX::ThrowIfFailed(
//		device.As(&m_d3dDevice)
//		);
//
//	// And get the corresponding device context in the same way.
//	DX::ThrowIfFailed(
//		context.As(&m_d3dContext)
//		);
//
//	// Obtain the underlying DXGI device of the Direct3D11.1 device.
//	DX::ThrowIfFailed(
//		m_d3dDevice.As(&dxgiDevice)
//		);
//
//#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
//#else
//	// Obtain the Direct2D device for 2-D rendering.
//	DX::ThrowIfFailed(
//		m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)
//		);
//
//	// And get its corresponding device context object.
//	DX::ThrowIfFailed(
//		m_d2dDevice->CreateDeviceContext(
//		D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
//		&m_d2dContext
//		)
//		);
//
//	//Init textPainter
//	m_textPainter->Initialize(
//		m_d2dContext.Get(),
//		m_wicFactory.Get(),
//		m_dwriteFactory.Get()
//		);
//#endif
//	// Release the swap chain (if it exists) as it will be incompatible with
//	// the new device.
//	m_swapChain = nullptr;
}

//// Helps track the DPI in the helper class.
//// This is called in the dpiChanged event handler in the view class.
//void DirectXRender::SetDpi(float dpi)
//{
//	if (dpi != m_dpi)
//	{
//		// Save the DPI of this display in our class.
//		m_dpi = dpi;
//
//		// Update Direct2D's stored DPI.
//#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
//#else
//		m_d2dContext->SetDpi(m_dpi, m_dpi);
//#endif
//		// Often a DPI change implies a window size change. In some cases Windows will issues
//		// both a size changed event and a DPI changed event. In this case, the resulting bounds 
//		// will not change, and the window resize code will only be executed once.
//		UpdateForWindowSizeChange();
//	}
//}

using namespace Windows::Graphics::Display;
// This routine is called in the event handler for the view SizeChanged event.
void DirectXRender::UpdateForWindowSizeChange(float width, float height)
{
	m_windowBounds.Width  = width;
	m_windowBounds.Height = height;
}

// Allocate all memory resources that change on a window SizeChanged event.
void DirectXRender::CreateWindowSizeDependentResources()
{
	// 创建深度模具视图。
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(m_renderTargetSize.Width),
		static_cast<UINT>(m_renderTargetSize.Height),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
		);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&depthStencil
			)
		);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(
		m_d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			&m_depthStencilView
			)
		);
}

// Method to call cocos2d's main loop for game update and draw.
void DirectXRender::Render()
{
}

float DirectXRender::GetWindowsWidth()
{
	return m_windowBounds.Width;
}

float DirectXRender::GetWindowsHeight()
{
	return m_windowBounds.Height;
}


//// Method to deliver the final image to the display.
//void DirectXRender::Present()
//{
//	//int r = rand() % 255;
//	//int g = rand() % 255;
//	//int b = rand() % 255;
//
//
//	// The first argument instructs DXGI to block until VSync, putting the application
//	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
//	// frames that will never be displayed to the screen.
//	// We 
//	HRESULT hr = m_swapChain->Present(1, 0);
//
//	// If the device was removed either by a disconnect or a driver upgrade, we 
//	// must completely reinitialize the renderer.
//	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
//	{
////		Initialize(m_window.Get(), m_dpi);
//	}
//	else
//	{
//		DX::ThrowIfFailed(hr);
//	}
//}

void DirectXRender::SetBackBufferRenderTarget()
{
	m_d3dContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView.Get());
}

//void DirectXRender::CloseWindow()
//{
//	if (nullptr != m_window.Get())
//	{
//		m_window->Close();
//		m_window = nullptr;
//	}
//	m_windowClosed = true;
//}

//bool DirectXRender::GetWindowsClosedState()
//{
//	return m_windowClosed;
//}

DirectXRender^ DirectXRender::SharedDXRender()
{
	return s_pDXRender;
}

//bool DirectXRender::SetRasterState()
//{
//	ID3D11RasterizerState* rasterState;
//	D3D11_RASTERIZER_DESC rasterDesc;
//	ZeroMemory(&rasterDesc,sizeof(D3D11_RASTERIZER_DESC));
//	rasterDesc.FillMode = D3D11_FILL_SOLID;
//	rasterDesc.CullMode = D3D11_CULL_NONE;
//	rasterDesc.DepthBias = 0;
//	rasterDesc.DepthBiasClamp = 0.0f;
//	rasterDesc.SlopeScaledDepthBias = 0.0f;
//	rasterDesc.DepthClipEnable = TRUE;
//	rasterDesc.AntialiasedLineEnable = TRUE;
//	rasterDesc.FrontCounterClockwise = TRUE;
//	rasterDesc.MultisampleEnable = FALSE;
//	rasterDesc.ScissorEnable = FALSE;
//
//	// Create the rasterizer state from the description we just filled out.
//	if(FAILED(m_d3dDevice->CreateRasterizerState(&rasterDesc, &rasterState)))
//	{
//		return FALSE;
//	}
//	m_d3dContext->RSSetState(rasterState);
//	if(rasterState)
//	{
//		rasterState->Release();
//		rasterState = 0;
//	}
//	return TRUE;
//}

//////////////////////////////////////////////////////////////////////////////////
//// Event callback functions
//////////////////////////////////////////////////////////////////////////////////
//
//void DirectXRender::OnWindowClosed(
//	_In_ CoreWindow^ sender,
//	_In_ CoreWindowEventArgs^ args
//	)
//{
//	m_window = nullptr;
//	m_windowClosed = true;
//}
//
//void DirectXRender::OnWindowVisibilityChanged(
//	_In_ Windows::UI::Core::CoreWindow^ sender,
//	_In_ Windows::UI::Core::VisibilityChangedEventArgs^ args
//	)
//{
//	if (args->Visible)
//	{
//		CCApplication::sharedApplication()->applicationWillEnterForeground();
//	} 
//	else
//	{
//		CCApplication::sharedApplication()->applicationDidEnterBackground();
//	}
//}
//
//void DirectXRender::OnWindowSizeChanged(
//	_In_ CoreWindow^ sender,
//	_In_ WindowSizeChangedEventArgs^ args
//	)
//{
//	UpdateForWindowSizeChange();
//	cocos2d::CCEGLView::sharedOpenGLView()->OnWindowSizeChanged();
//}
//
//void DirectXRender::OnPointerPressed(
//	_In_ Windows::UI::Core::CoreWindow^ sender,
//	_In_ Windows::UI::Core::PointerEventArgs^ args
//	)
//{
//	CCPoint point = getCCPointFromScreen(args->CurrentPoint->Position);
//	cocos2d::CCEGLView::sharedOpenGLView()->OnPointerPressed(args->CurrentPoint->PointerId, point);
//}
//
//void DirectXRender::OnPointerReleased(
//	_In_ Windows::UI::Core::CoreWindow^ sender,
//	_In_ Windows::UI::Core::PointerEventArgs^ args
//	)
//{
//	CCPoint point = getCCPointFromScreen(args->CurrentPoint->Position);
//	cocos2d::CCEGLView::sharedOpenGLView()->OnPointerReleased(args->CurrentPoint->PointerId, point);
//}
//
//void DirectXRender::OnPointerMoved(
//	_In_ Windows::UI::Core::CoreWindow^ sender,
//	_In_ Windows::UI::Core::PointerEventArgs^ args
//	)
//{
//	CCPoint point = getCCPointFromScreen(args->CurrentPoint->Position);
//	cocos2d::CCEGLView::sharedOpenGLView()->OnPointerMoved(args->CurrentPoint->PointerId, point);
//}
//
//void DirectXRender::OnCharacterReceived(
//	_In_ Windows::UI::Core::CoreWindow^ sender,
//	_In_ Windows::UI::Core::CharacterReceivedEventArgs^ args
//	)
//{
//	cocos2d::CCEGLView::sharedOpenGLView()->OnCharacterReceived(args->KeyCode);
//}

//// 以下是依赖设备的资源。
//void DirectXRender::CreateDeviceResources()
//{
//}

void DirectXRender::UpdateDevice(_In_ ID3D11Device1* device, _In_ ID3D11DeviceContext1* context, _In_ ID3D11RenderTargetView* renderTargetView)
{
	m_d3dContext = context;
	m_renderTargetView = renderTargetView;

	if (m_d3dDevice.Get() != device)
	{
		m_d3dDevice = device;
		CreateDeviceResources();

		// Force call to CreateWindowSizeDependentResources.
		m_renderTargetSize.Width  = -1;
		m_renderTargetSize.Height = -1;
	}

	ComPtr<ID3D11Resource> renderTargetViewResource;
	m_renderTargetView->GetResource(&renderTargetViewResource);

	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(
		renderTargetViewResource.As(&backBuffer)
		);

	// 在我们的帮助程序类中缓存呈现目标维度以方便使用。
    D3D11_TEXTURE2D_DESC backBufferDesc;
    backBuffer->GetDesc(&backBufferDesc);

    if (m_renderTargetSize.Width  != static_cast<float>(backBufferDesc.Width) ||
        m_renderTargetSize.Height != static_cast<float>(backBufferDesc.Height))
    {
        m_renderTargetSize.Width  = static_cast<float>(backBufferDesc.Width);
        m_renderTargetSize.Height = static_cast<float>(backBufferDesc.Height);
        CreateWindowSizeDependentResources();
    }

	// 设置用于确定整个窗口的呈现视区。
	CD3D11_VIEWPORT viewport(
		0.0f,
		0.0f,
		m_renderTargetSize.Width,
		m_renderTargetSize.Height
		);

	m_d3dContext->RSSetViewports(1, &viewport);
}
