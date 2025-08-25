#include "pch.h"
#include "Mesh.h"
#include "SBEngine.h"
#include "Device.h"

Mesh::Mesh(std::string&& name, std::vector<Vertex>&& verties, std::vector<UINT>&& indices)
	:mName(name), mVertices(verties), mIndices(indices)
{
	//InitMesh();
}

Mesh::~Mesh()
{
	SafeRelease(mRootSignature);
	SafeRelease(mCbvHeap);
	SafeRelease(mPSO);

	SafeRelease(mVertexBufferGPU);
	SafeRelease(mIndexBufferGPU);

	SafeRelease(mVSByteCode);
	SafeRelease(mPSByteCode);

	SafeRelease(mVertexBufferCPU);
	SafeRelease(mIndexBufferCPU);
	SafeRelease(mVertexBufferUploader);
	SafeRelease(mIndexBufferUploader);
}

//Mesh::Mesh()
//	: Mesh("Triangle",
//		vector<Vertex>({
//		{ { 0.0f, 0.25f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
//		{ { 0.25f, -0.25f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
//		{ { -0.25f, -0.25f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
//		}), 
//		vector<UINT>({ 0, 1, 2 }))
//{
//
//
//}

void Mesh::InitMesh()
{
	const UINT vbByteSize = (UINT)mVertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)mIndices.size() * sizeof(UINT);

	// Vertex Buffer 积己
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mVertexBufferCPU));
	CopyMemory(mVertexBufferCPU->GetBufferPointer(), mVertices.data(), vbByteSize);

	// Index Buffer 积己
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mIndexBufferCPU));
	CopyMemory(mIndexBufferCPU->GetBufferPointer(), mIndices.data(), ibByteSize);
	
	auto device = SBEngine::Get()->GetDevice();
	assert(device);
	auto commandList = SBEngine::Get()->GetCommandList();
	assert(commandList);

	mVertexBufferGPU = CreateDefaultBuffer(device.Get(), commandList.Get(), mVertices.data(), vbByteSize, &mVertexBufferUploader);

	mIndexBufferGPU = CreateDefaultBuffer(device.Get(), commandList.Get(), mIndices.data(), ibByteSize, &mIndexBufferUploader);

	mVertextByteStride = sizeof(Vertex);
	mVertexBufferByteSize = vbByteSize;
	mIndexFormat = DXGI_FORMAT_R32_UINT;
	mIndexBufferByteSize = ibByteSize;


	//SubmeshGeometry submesh;
	//submesh.IndexCount = (UINT)indices.size();
	//submesh.StartIndexLocation = 0;
	//submesh.BaseVertexLocation = 0;

	//mDrawArgs["box"] = submesh;

}

void Mesh::InitDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;


	auto device = SBEngine::Get()->GetDevice();
	assert(device);

	ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&mCbvHeap)));
}

void Mesh::InitConstantBuffers()
{
	auto device = SBEngine::Get()->GetDevice();
	assert(device);

	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device.Get(), 1, true);

	UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(ObjectConstants));

	device->CreateConstantBufferView(
		&cbvDesc,
		mCbvHeap->GetCPUDescriptorHandleForHeapStart());

}

void Mesh::InitRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature.  

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig, &errorBlob);

	if(errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);


	auto device = SBEngine::Get()->GetDevice();
	assert(device);

	ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void Mesh::InitPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature;
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mVSByteCode->GetBufferPointer()),
		mVSByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mPSByteCode->GetBufferPointer()),
		mPSByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM  /*mBackBufferFormat*/;
	psoDesc.SampleDesc.Count =1;
	psoDesc.SampleDesc.Quality =  0;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT/*mDepthStencilFormat*/;

	auto device = SBEngine::Get()->GetDevice();
	assert(device);
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

void Mesh::CompileShaderAndInputLayout()
{
	//  TMP :: shader 窍靛 内爹
	mVSByteCode= CompileShader(L"..\\Engine\\Shader\\shaders.hlsl", nullptr, "VSMain", "vs_5_0");
	mPSByteCode= CompileShader(L"..\\Engine\\Shader\\shaders.hlsl", nullptr, "PSMain", "ps_5_0");

	//ThrowIfFailed(D3DCompileFromFile(L"Shader\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &mVSByteCode, nullptr));
	//ThrowIfFailed(D3DCompileFromFile(L"../Shader/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &mPSByteCode, nullptr));

	mInputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	 
}


D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = mVertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = mVertextByteStride;
	vbv.SizeInBytes = mVertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = mIndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = mIndexFormat;
	ibv.SizeInBytes = mIndexBufferByteSize;
	return ibv;
}

void Mesh::DisposeUploaders()
{
}

void Mesh::TMPInit()
{
	{
		SBEngine::Get()->StartWriteCommadList();

		InitDescriptorHeap();
		InitConstantBuffers();
		InitRootSignature();
		CompileShaderAndInputLayout();
		InitMesh();
		InitPSO();


		SBEngine::Get()->FinishAndFlushCommandList();
	}
	SafeRelease(mVertexBufferUploader);
	SafeRelease(mIndexBufferUploader);
}

void Mesh::Render()
{
	ObjectConstants objConstants;
	Matrix worldViewProj = Matrix::Identity;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);

	auto commandList = SBEngine::Get()->GetCommandList();

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetPipelineState(mPSO);
	commandList->SetGraphicsRootSignature(mRootSignature);
	commandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

	auto VBView = VertexBufferView();
	auto IBView = IndexBufferView();
	commandList->IASetVertexBuffers(0, 1, &VBView);
	commandList->IASetIndexBuffer(&IBView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->DrawIndexedInstanced((UINT)mIndices.size(), 1, 0, 0, 0);
}
