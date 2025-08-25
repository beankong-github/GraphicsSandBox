#include "pch.h"

SBException::SBException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
	:ErrorCode(hr)
	, FunctionName(functionName)
	, Filename(filename)
	, LineNumber(lineNumber)
{
}

std::wstring SBException::ToString() const
{   
	// Get the string description of the error code.
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();

	return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}

ID3D12Resource* CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, ID3D12Resource** uploadBuffer)
{
	ID3D12Resource* defaultBuffer;

	// Create the actual default buffer resource.
	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto decriptor = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&decriptor,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&defaultBuffer)));

	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	decriptor = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&decriptor,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer)));


	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->ResourceBarrier(1, &barrier);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	UpdateSubresources<1>(cmdList, defaultBuffer, *uploadBuffer, 0, 0, 1, &subResourceData);
	cmdList->ResourceBarrier(1, &barrier);

	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.


	return defaultBuffer;
}

ID3DBlob* CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		ID3DBlob* byteCode = nullptr;
		ID3DBlob* errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			LogShaderError(hr, errors);

		ThrowIfFailed(hr);

		return byteCode;
	}
}

void LogShaderError(HRESULT hr, ID3DBlob* errors)
{
	if (errors)
	{
		const char* msg = (const char*)errors->GetBufferPointer();
		OutputDebugStringA("=== HLSL COMPILE ERROR ===\n");
		OutputDebugStringA(msg);
		OutputDebugStringA("\n==========================\n");
	}

	// HRESULT 도 같이 로그
	_com_error err(hr);
	std::wstring wmsg = L"D3DCompile failed. HRESULT=" + std::to_wstring(hr) + L" (" + err.ErrorMessage() + L")\n";
	OutputDebugStringW(wmsg.c_str());
}
