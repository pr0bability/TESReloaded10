#pragma once

class RenderManager: public RenderManagerBase {
public:
	void				Initialize();
	void				ResolveDepthBuffer(IDirect3DTexture9* Buffer);
	void				CreateD3DMatrix(D3DMATRIX* Matrix, NiTransform* Transform);
	void				GetScreenSpaceBoundSize(NiPoint2* BoundSize, NiBound* Bound, float ZeroTolerance = 1e-5f);
	void				UpdateSceneCameraData();
	void				SetupSceneCamera();
	void				CheckAndTakeScreenShot(IDirect3DSurface9* RenderTarget, bool HDR);
    float               GetObjectDistance(NiBound* Bound);
	bool				IsReversedDepth();
	D3DXMATRIX			WorldViewProjMatrix;
	D3DXMATRIX			ViewProjMatrix;
	D3DXMATRIX			InvViewProjMatrix;
	D3DXMATRIX			InvProjMatrix;
	D3DXMATRIX			InvViewMatrix;
	D3DXMATRIX			ViewMatrix;
	D3DXVECTOR4			CameraData;
	D3DXVECTOR4			FOVData;
	D3DXVECTOR4			CameraForward;
	D3DXVECTOR4			CameraPosition;
	IDirect3DSurface9*	BackBuffer;
	D3DXVECTOR4			DepthConstants;
	RECT				SaveGameScreenShotRECT;
	bool				IsSaveGameScreenShot;
	bool				RESZ;
	bool				DXVK;
	bool				ILS;
};

class DWNode : public NiNode {
public:
	static void		Create();
	static DWNode*	Get();
	static void		AddNode(const char* Name, NiAVObject* Child0, NiAVObject* Child1);
};
