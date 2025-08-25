#pragma once
#include "Core\engine.h"
#include "Core\Device.h"
#include "UploadBuffer.h"

struct SubMesh
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	// This is used in later chapters of the book.
	DirectX::BoundingBox Bounds;
};

class Engine_API Mesh
{
	// tmp
	friend class Device;

public:
	Mesh(std::string&& name, std::vector<Vertex>&& verties, std::vector<UINT>&& indices);
	// tmp
	Mesh();
	~Mesh() = default;

	const string& Name() const { return mName; }
	void SetName(string name) { mName = name; }

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

	//GPU�� ���ε� �� �޸� Free
	void DisposeUploaders();

	void TMPInit();
	void Render();

private:
	void InitMesh();

	// TODO ::  Mesh �� Render Path���� ���� �и�
	void InitDescriptorHeap();
	void InitConstantBuffers();
	void InitRootSignature();
	void CompileShaderAndInputLayout();
	void InitPSO();

private:
	std::string mName;

	std::vector<Vertex> mVertices;	// ���� ����
	std::vector<UINT> mIndices;		// ���� �ε��� ����

	// TODO ::  Mesh �� Render Path���� ���� �и�
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;    
	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	ComPtr<ID3D12PipelineState> mPSO = nullptr;
	// TMP  :: Shader ������ ���
	ComPtr<ID3DBlob> mVSByteCode = nullptr;
	ComPtr<ID3DBlob> mPSByteCode = nullptr;
	// TMP :: inputLayout
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;


	ComPtr<ID3DBlob> mVertexBufferCPU = nullptr;
	ComPtr<ID3DBlob> mIndexBufferCPU = nullptr;

	ComPtr<ID3D12Resource> mVertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> mIndexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> mVertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> mIndexBufferUploader = nullptr;

	UINT mVertextByteStride = 0;
	UINT mVertexBufferByteSize = 0;
	DXGI_FORMAT mIndexFormat = DXGI_FORMAT_R16_UINT;
	UINT mIndexBufferByteSize = 0;



	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	///std::unordered_map<std::string, SubMesh> DrawArgs;
};

