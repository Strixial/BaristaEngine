#include "Graphics.h"

bool Graphics::Init(HWND hwnd, int width, int height) {
	OutputDebugStringA("!! - Initialising graphics system\n");

	auto DXInitialised = InitDirectX(hwnd, width, height);
	if (!DXInitialised) {
		OutputDebugStringA("!! - DirectX failed to initialise. Oops.\n");
		return false;
	}
	else {
		OutputDebugStringA("!! - DirectX is running fine.\n");
	}

	OutputDebugStringA("!! - Initialising shaders\n");

	auto ShadersInited = InitShaders();
	if (!ShadersInited) {
		OutputDebugStringA("!! - Shaders failed to initialise. Oops.\n");
		return false;
	}
	else {
		OutputDebugStringA("!! - Shaders are OK.\n");
	}

	auto SceneInited = InitScene();
	if (!SceneInited) {
		OutputDebugStringA("!! - Scene failed to initialise. Oops.\n");
		return false;
	}
	else {
		OutputDebugStringA("!! - Scene loaded.\n");
	}

	return true;
}

bool Graphics::InitDirectX(HWND hwnd, int width, int height) {
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	
	if (adapters.size() < 1) {
		ErrorLogger::CreateErrorBox("No IDXGI adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	ZeroMemory(&swapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapchainDesc.BufferDesc.Width = width;
	swapchainDesc.BufferDesc.Height = height;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1; 
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;

	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 1;
	swapchainDesc.OutputWindow = hwnd;
	swapchainDesc.Windowed = TRUE;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT dxDevice;
	dxDevice = D3D11CreateDeviceAndSwapChain(
		adapters[0].pAdapter, // First adapter found
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, // Software driver
		NULL, // Runtime layer flags
		NULL, // Feature levels array
		0, // # of feature levels in array
		D3D11_SDK_VERSION,
		&swapchainDesc,
		this->swapchain.GetAddressOf(),
		this->device.GetAddressOf(),
		NULL,
		this->deviceContext.GetAddressOf()
	);

	if (FAILED(dxDevice)) {
		ErrorLogger::CreateErrorBox(dxDevice, "Failed to create device and swapchain.");
	}
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	dxDevice = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(dxDevice)) {
		ErrorLogger::CreateErrorBox(dxDevice, "GetBuffer failed.");
		return false;
	}

	dxDevice = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(dxDevice)) {
		ErrorLogger::CreateErrorBox(dxDevice, "CreateRenderTargetView failed.");
		return false;
	}

	// Set output merger render target
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);

	// Create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;

	// Set viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	return true;
}

bool Graphics::InitShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION",
		0, 
		DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
		0,
		0,
		D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
		0},
		{"COLOR",
		0,
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
		0},
	};

	UINT numElements = ARRAYSIZE(layout);

	std::wstring shaderFolder = L"";
	#pragma region DetermineShaderPath
		if (IsDebuggerPresent() == TRUE)
		{
			#ifdef _DEBUG //Debug Mode
			#ifdef _WIN64 //x64
				shaderFolder = L"..\\x64\\Debug\\";
			#else  //x86 (Win32)
				shaderFolder = L"..\\Debug\\";
			#endif
			#else //Release Mode
			#ifdef _WIN64 //x64
				shaderFolder = L"..\\x64\\Release\\";
			#else  //x86 (Win32)
				shaderFolder = L"..\\Release\\";
			#endif
			#endif
		}

	if (!vertexShader.Init(this->device, L"..\\x64\\Debug\\vertexshader.cso", layout, numElements)) {
		return false;
	}

	if (!pixelShader.Init(this->device, shaderFolder + L"pixelshader.cso")) {
		return false;
	}
	
	return true;
}

bool Graphics::InitScene()
{
	Vertex v[] =
	{
		Vertex(-0.4f, -0.5f, 1.0f, 0.0f, 0.0f),
		Vertex(0.0f, 0.6f, 0.0f, 1.0f, 0.0f),
		Vertex(0.4f, -0.5f, 0.0f, 0.0, 1.0f),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::CreateErrorBox(hr, "Failed to create vertex buffer.");
		return false;
	}

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.025f, 0.035f, 0.05f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	this->deviceContext->Draw(3, 0);
	
	this->swapchain->Present(1, NULL);
}