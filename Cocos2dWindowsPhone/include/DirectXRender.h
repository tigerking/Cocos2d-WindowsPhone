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

#pragma once

#include "pch.h"
#include <wrl.h>
#include <agile.h>
#include <d3d11_1.h>

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#include "FTTextPainter.h"
#else
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include "DXTextPainter.h"
#endif

#include <XInput.h>
#include "DirectXHelper.h"

// forward declare

NS_CC_BEGIN;
struct ModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};
struct VertexPositionColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};
class CCEGLView;
class CCImage;

// Helper class that initializes the DirectX APIs in the sample apps.
public ref class DirectXRender sealed
{
public:
	DirectXRender();

	//void Initialize(Windows::UI::Core::CoreWindow^ window, float dpi);
	void CreateDeviceIndependentResources();
	void CreateDeviceResources();
	//void SetDpi(float dpi);
	void UpdateForWindowSizeChange(float width, float height);


	void UpdateForRenderResolutionChange(float width, float height);
	void CreateWindowSizeDependentResources();
	void Render();
	//void Present();
	void SetBackBufferRenderTarget();
	//void CloseWindow();
	//bool GetWindowsClosedState();
	static DirectXRender^ SharedDXRender();
	float GetWindowsWidth();
	float GetWindowsHeight();

	// 更新时间相关对象的方法。
	//void Update(float timeTotal, float timeDelta);

internal:
	void Initialize();
	//virtual void CreateDeviceResources();
	//virtual void UpdateDevice(_In_ ID3D11Device1* device, _In_ ID3D11DeviceContext1* context, _In_ ID3D11RenderTargetView* renderTargetView);
		ID3D11Texture2D* GetTexture()
	{
		return m_renderTarget.Get();
	}
private:
	//Platform::Agile<Windows::UI::Core::CoreWindow>                  m_window;
	bool m_loadingComplete;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	uint32 m_indexCount;
	ModelViewProjectionConstantBuffer m_constantBufferData;
private:
	bool SetRasterState();

private:
	friend class cocos2d::CCEGLView;
	friend class cocos2d::CCImage;


#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
	FTTextPainter^									m_textPainter;
#else
	DXTextPainter^									m_textPainter;
	//// Declare Direct2D Objects
	Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>            m_d2dTargetBitmap;

	//// Declare DirectWrite & Windows Imaging Component Objects
	Microsoft::WRL::ComPtr<IDWriteFactory1>         m_dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2>     m_wicFactory;
#endif

	// Direct3D Objects
	Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;
//	Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTarget;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

	D3D_FEATURE_LEVEL                               m_featureLevel;
	Windows::Foundation::Size                       m_renderTargetSize;
	Windows::Foundation::Rect                       m_windowBounds;
	float                                           m_dpi;

	bool m_windowClosed;

//protected:
//	void OnWindowClosed(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::CoreWindowEventArgs^ args
//		);
//
//	void OnWindowVisibilityChanged(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::VisibilityChangedEventArgs^ args
//		);
//
//	void OnWindowSizeChanged(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args
//		);
//
//	void OnPointerPressed(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::PointerEventArgs^ args
//		);
//
//	void OnPointerReleased(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::PointerEventArgs^ args
//		);
//
//	void OnPointerMoved(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::PointerEventArgs^ args
//		);
//
//	void OnCharacterReceived(
//		_In_ Windows::UI::Core::CoreWindow^ sender,
//		_In_ Windows::UI::Core::CharacterReceivedEventArgs^ args
//		);
//
};
NS_CC_END;