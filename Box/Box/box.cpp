//#include "../../Common/d3dApp.h"
//#include "../../Common/MathHelper.h"
//#include "../../Common/UploadBuffer.h"
//#include "box.h"
//
//using Microsoft::WRL::ComPtr;
//using namespace DirectX;
//using namespace DirectX::PackedVector;
//
//struct Vertex
//{
//	XMFLOAT3 Pos;
//	XMFLOAT4 Color;
//};
//
//struct ObjectConstants
//{
//	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
//};
//
//class Box : public D3DApp
//{
//public:
//	Box(HINSTANCE hInstance);
//	Box(const Box& rhs) = delete;
//	Box& operator=(const Box& rhs) = delete;
//	~Box();
//
//	virtual bool Initialize() override;
//
//private:
//	virtual void OnResize() override;
//	virtual void Update(const GameTimer& gt) override;
//	virtual void Draw(const GameTimer& gt) override;
//
//	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
//	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
//	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
//
//	void BuildDescriptorHeaps();
//	void BuildConstantBuffers();
//	void BuildRootSignature();
//	void BuildShaderAndInputLayout();
//	void BuildBoxGeometry();
//	void BuildPSO();
//
//private:
//	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
//	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
//
//	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
//	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
//
//	ComPtr<ID3DBlob> mvsByteCode = nullptr;
//	ComPtr<ID3DBlob> mpsByteCode = nullptr;
//
//	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
//
//	ComPtr<ID3D12PipelineState> mPSO = nullptr;
//
//	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
//	XMFLOAT4X4 mView = MathHelper::Identity4x4();
//	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
//
//	float mTheta = 1.5f * XM_PI;
//	float mPhi = XM_PIDIV4;
//	float mRadius = 5.0f;
//
//	POINT mLastMousePos;
//};
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
//{
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	try
//	{
//		Box theApp(hInstance);
//		if (!theApp.Initialize())
//			return 0;
//
//		return theApp.Run();
//	}
//	catch (DxException& e)
//	{
//		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
//		return 0;
//	}
//}
//
//Box::Box(HINSTANCE hInstance) : D3DApp(hInstance)
//{
//}
//
//Box::~Box()
//{
//}
//
//bool Box::Initialize()
//{
//	if (!D3DApp::Initialize())
//		return false;
//
//	// Initialize Commands�� �غ��ϱ� ���� Command List�� Reset�Ѵ�
//	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
//
//	BuildDescriptorHeaps();
//	BuildConstantBuffers();
//	BuildRootSignature();
//	BuildShaderAndInputLayout();
//	BuildBoxGeometry();
//	BuildPSO();
//
//	// Initialize Commands�� ����
//	ThrowIfFailed(mCommandList->Close());
//	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
//	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
//
//	FlushCommandQueue();
//
//	return true;
//}
//
//void Box::OnResize()
//{
//	D3DApp::OnResize();
//
//	// â�� ũ�Ⱑ �ٲ�����Ƿ� aspect ratio(��Ⱦ��)�� �����ϰ�
//	// ���� ����� �ٽ� ���
//	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
//	XMStoreFloat4x4(&mProj, P);
//}
//
//void Box::Update(const GameTimer & gt)
//{
//	float x = mRadius * sinf(mPhi) * cosf(mTheta);
//	float z = mRadius * sinf(mPhi) * sinf(mTheta);
//	float y = mRadius * cosf(mPhi);
//
//	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
//	XMVECTOR target = XMVectorZero();
//	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//
//	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
//	XMStoreFloat4x4(&mView, view);
//
//	XMMATRIX world = XMLoadFloat4x4(&mWorld);
//	XMMATRIX proj = XMLoadFloat4x4(&mProj);
//	XMMATRIX worldViewProj = world * view * proj;
//
//	// constant buffer Update
//	ObjectConstants objConstants;
//	XMStoreFloat4x4(&objConstants.WorldViewProj, worldViewProj);
//	// Buffer�� Ư�� �׸� ����
//	// CPU���� Upload buffer�� ������ �����ؾ��� ��(ex �þ� ��� ����)
//	mObjectCB->CopyData(0, objConstants);
//}
//
//void Box::Draw(const GameTimer& gt)
//{
//	// command��Ͽ� ���� �޸� ����
//	// GPU�� ���õ� Command List ������ �� ������ Reset
//	ThrowIfFailed(mDirectCmdListAlloc->Reset());
//
//	// ExecuteCommandList�� ���ؼ� Command Queue�� �߰� �� �Ŀ� Command List�� Reset �ȴ�
//	// Command List�� ���� �Ǹ� Memory�� ����ȴ�
//	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));
//
//	// RS = Rasterizer Stage
//	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
//	mCommandList->RSSetViewports(1, &mScreenViewport);
//	mCommandList->RSSetScissorRects(1, &mScissorRect);
//
//	// �ڿ� ��뿡 ���õ� State Transition�� ����
//	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), 
//		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
//
//	// Clear the Back buffer and Depth buffer
//	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Blue, 0, nullptr);
//	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
//
//	// Rendering ����� ��ϵ� Buffer���� �����Ѵ�
//	// OM = Output Merger
//	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
//
//	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
//	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//	mCommandList->SetComputeRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());
//
//	// Input Assembly
//	mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
//	mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
//	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	mCommandList->DrawIndexedInstanced(
//		mBoxGeo->DrawArgs["box"].IndexCount,
//		1, 0, 0, 0);
//
//	// �ڿ� ��뿡 ���õ� State Transition ����
//	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), 
//		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
//
//	// Done Recording commands
//	ThrowIfFailed(mCommandList->Close());
//
//	// CommandList�� command Queue �� �߰�
//	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
//	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
//
//	// swap the back Buffer and frount buffer
//	ThrowIfFailed(mSwapChain->Present(0, 0));
//	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
//
//	FlushCommandQueue();
//}
//
//void Box::BuildDescriptorHeaps()
//{
//	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
//	cbvHeapDesc.NumDescriptors = 1;
//	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	cbvHeapDesc.NodeMask = 0;
//
//	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
//		IID_PPV_ARGS(&mCbvHeap)));
//}