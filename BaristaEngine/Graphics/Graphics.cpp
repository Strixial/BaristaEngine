#include "Graphics.h"

bool Graphics::Init(HWND hwnd, int width, int height) {
	this->windowWidth = width;
	this->windowHeight = height;

	OutputDebugStringA("!! - Initialising graphics system\n");

	auto DXInitialised = InitDirectX(hwnd);
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

bool Graphics::InitDirectX(HWND hwnd) {
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	
	if (adapters.size() < 1) {
		ErrorLogger::CreateErrorBox("No IDXGI adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	ZeroMemory(&swapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapchainDesc.BufferDesc.Width = this->windowWidth;
	swapchainDesc.BufferDesc.Height = this->windowHeight;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1; 
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapchainDesc.SampleDesc.Count = 4;
	swapchainDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;

	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 1;
	swapchainDesc.OutputWindow = hwnd;
	swapchainDesc.Windowed = TRUE;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
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

	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "Failed to create device and swapchain.");
	}
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "GetBuffer failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "CreateRenderTargetView failed.");
		return false;
	}

	//Create depth buffer
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	depthStencilTextureDesc.Width = this->windowWidth;
	depthStencilTextureDesc.Height = this->windowHeight;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.SampleDesc.Count = 4;
	depthStencilTextureDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;
	hr = this ->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "Failed to create depth stencil buffer");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "Failed to create depth stencil view");
		return false;
	}

	// Set output merger render target
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	// Set depth buffer state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::CreateErrorBox(hr, "Failed to create depth stencil state.");
		return false;
	}

	// Create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = this->windowWidth;
	viewport.Height = this->windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	// Set viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	// Create rasteriser state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (hr != S_OK) {
		ErrorLogger::CreateErrorBox(hr, "Failed to create rasterizer state.");
		return false;
	}

	// Create sampler description
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "Failed to create sampler state");
		return false;
	}

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\cascadiamono16.spritefont");
	OutputDebugStringA("!! - Loaded spritefont\n");

	return true;
}

bool Graphics::InitShaders() {
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION",
		0, 
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
		0},
		{"TEXCOORD",
		0,
		DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
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
	// Textured Quad
	Vertex v[] =
	{
		Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f), // Bottom left - 0
		Vertex(-0.5f,  0.5f, 0.0f, 0.0f, 0.0f), // Top left - 1
		Vertex( 0.5f,  0.5f, 0.0f, 1.0f, 0.0f), // Top right - 2
		Vertex( 0.5f, -0.5f, 0.0f, 1.0f, 1.0f), // Bottom right - 3
	};

	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	HRESULT hr = this->vertexBuffer.Init(this->device.Get(), v, ARRAYSIZE(v));

	if (FAILED(hr))
	{
		ErrorLogger::CreateErrorBox(hr, "Failed to create vertex buffer.");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\neco.jpg", nullptr, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::CreateErrorBox(hr, "Failed to create wic texture from file.");
		return false;
	}

	hr = this->constantBuffer.Init(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr)) {
		ErrorLogger::CreateErrorBox(hr, "Failed to initialise constant buffer");
	}

	//Load index data
	hr = this->indicesBuffer.Init(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::CreateErrorBox(hr, "Failed to create indices buffer.");
		return false;
	}

	float fovDegrees = 60.0f;
	float aspectRatio = static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight);

	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.SetProjectionValues(fovDegrees, aspectRatio, 0.1f, 100.0f);

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.025f, 0.035f, 0.05f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	
	auto cameraCFrame = camera.GetViewMatrix();
	auto projectionMatrix = camera.GetProjectionMatrix();


	// Store final camera matrix to constant buffer
	DirectX::XMStoreFloat4x4(&constantBuffer.data.mat, DirectX::XMMatrixTranspose(worldMatrix * cameraCFrame * projectionMatrix));

	if (!constantBuffer.ApplyChanges()) {
		return;
	}

	this->deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	// Draw quad
	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);

	// Draw text
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"Barista Engine", DirectX::XMFLOAT2(10, 10), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	this->swapchain->Present(1, NULL);
}