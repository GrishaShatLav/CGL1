#include "TriangleComponent.h"

TriangleComponent::TriangleComponent(float offset) {
	vertexBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	vertexData = std::make_shared<D3D11_SUBRESOURCE_DATA>();
	indexBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	indexData = std::make_shared<D3D11_SUBRESOURCE_DATA>();
	rastDesc = std::make_shared<CD3D11_RASTERIZER_DESC>();
	this->offset=offset;
	pointsAmount = 8;
}

void TriangleComponent::Initialize() {
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
	ID3DBlob* errorPixelCode = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	// Compile pixel shader
	Game::instance->SetRes(
		D3DCompileFromFile(
			L"./Shaders/MyVeryFirstShader.hlsl",
			Shader_Macros /*macros*/,
			nullptr /*include*/,
			"PSMain",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			pixelShaderByteCode.GetAddressOf(),
			&errorPixelCode
		)
	);

	// Compile vertex shader
	Game::instance->SetRes(
		D3DCompileFromFile(
			L"./Shaders/MyVeryFirstShader.hlsl",
			nullptr /*macros*/,
			nullptr /*include*/,
			"VSMain",
			"vs_5_0", // Shader target (Pixel / Vertex)
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Different flags (for example code translation)
			0,
			vertexShaderByteCode.GetAddressOf(),
			&errorVertexCode
		)
	);

	if (FAILED(Game::instance->GetRes())) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
			MessageBox(Game::instance->GetDisplay()->GetHWnd(), L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);

		return;
	}

	Game::instance->GetDevice()->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr,
		pixelShader.GetAddressOf()
	);

	Game::instance->GetDevice()->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr,
		vertexShader.GetAddressOf()
	);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA, // Per vertex or per instance
			0
		}
	};

	Game::instance->GetDevice()->CreateInputLayout(
		inputElements,
		2,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		layout.GetAddressOf()
	);

	// Creating set of points
	points.push_back(DirectX::XMFLOAT4(0.2f+offset, 0.2f, 0.2f, 1.0f));
	points.push_back(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	points.push_back(DirectX::XMFLOAT4(-0.2f + offset, -0.2f, 0.2f, 1.0f));
	points.push_back(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	points.push_back(DirectX::XMFLOAT4(0.2f + offset, -0.2f, 0.2f, 1.0f));
	points.push_back(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	points.push_back(DirectX::XMFLOAT4(-0.2f + offset, 0.2f, 0.2f, 1.0f));
	points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


	vertexBufDesc.get()->Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.get()->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.get()->CPUAccessFlags = 0;
	vertexBufDesc.get()->MiscFlags = 0;
	vertexBufDesc.get()->StructureByteStride = 0;
	vertexBufDesc.get()->ByteWidth = sizeof(DirectX::XMFLOAT4) * pointsAmount;
	
	vertexData.get()->pSysMem = points.data();
	vertexData.get()->SysMemPitch = 0;
	vertexData.get()->SysMemSlicePitch = 0;

	Game::instance->GetDevice()->CreateBuffer(vertexBufDesc.get(), vertexData.get(), vb.GetAddressOf());

	int indeces[] = { 0, 1, 2, 1, 0, 3 };
	indexBufDesc.get()->ByteWidth = sizeof(int) * std::size(indeces);
	indexBufDesc.get()->Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.get()->BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.get()->CPUAccessFlags = 0;
	indexBufDesc.get()->MiscFlags = 0;
	indexBufDesc.get()->StructureByteStride = 0;

	indexData.get()->pSysMem = indeces;
	indexData.get()->SysMemPitch = 0;
	indexData.get()->SysMemSlicePitch = 0;

	Game::instance->GetDevice()->CreateBuffer(indexBufDesc.get(), indexData.get(), ib.GetAddressOf());

	rastDesc.get()->CullMode = D3D11_CULL_NONE; // Try to change
	rastDesc.get()->FillMode = D3D11_FILL_SOLID; // Try to change

	Game::instance->SetRes(Game::instance->GetDevice()->CreateRasterizerState(rastDesc.get(), rastState.GetAddressOf()));
	Game::instance->GetContext()->RSSetState(rastState.Get());

	strides[0] = 32;
	offsets[0] = 0;
}

void TriangleComponent::Update() {
	
}

void TriangleComponent::Draw() {
	 // Main function for draw (DrawCall)
	Game::instance->GetContext()->IASetInputLayout(layout.Get());
	Game::instance->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Game::instance->GetContext()->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
	Game::instance->GetContext()->IASetVertexBuffers(0, 1, vb.GetAddressOf(), strides, offsets);

	Game::instance->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	Game::instance->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);

	Game::instance->GetContext()->RSSetState(rastState.Get());
	Game::instance->GetContext()->DrawIndexed(6, 0, 0);
}

void TriangleComponent::Reload() {

}

void TriangleComponent::DestroyResources() {

}
