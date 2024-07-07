#pragma once
extern void(__thiscall* Render)(Main*, BSRenderedTexture*, int, int);
void __fastcall RenderHook(Main* This, UInt32 edx, BSRenderedTexture* RenderedTexture, int Arg2, int Arg3);

extern void(__thiscall* SetShaders)(BSShader*, UInt32);
void __fastcall SetShadersHook(BSShader* This, UInt32 edx, UInt32 PassIndex);

extern HRESULT(__thiscall* SetSamplerState)(NiDX9RenderState*, UInt32, D3DSAMPLERSTATETYPE, UInt32, UInt8);
HRESULT __fastcall SetSamplerStateHook(NiDX9RenderState* This, UInt32 edx, UInt32 Sampler, D3DSAMPLERSTATETYPE Type, UInt32 Value, UInt8 Save);

extern void(__thiscall* RenderWorldSceneGraph)(Main*, Sun*, UInt8, UInt8, UInt8);
void __fastcall RenderWorldSceneGraphHook(Main* This, UInt32 edx, Sun* SkySun, UInt8 IsFirstPerson, UInt8 WireFrame, UInt8 Arg4);

extern void(__thiscall* RenderFirstPerson)(Main*, NiDX9Renderer*, NiGeometry*, Sun*, BSRenderedTexture*);
void __fastcall RenderFirstPersonHook(Main* This, UInt32 edx, NiDX9Renderer* Renderer, NiGeometry* Geo, Sun* SkySun, BSRenderedTexture* RenderedTexture);

extern void(__thiscall* RenderReflections)(WaterManager*, NiCamera*, ShadowSceneNode*);
void __fastcall RenderReflectionsHook(WaterManager* This, UInt32 edx, NiCamera* Camera, ShadowSceneNode* SceneNode);

extern void(__thiscall* RenderPipboy)(Main*, NiGeometry*, NiDX9Renderer*);
void __fastcall RenderPipboyHook(Main* This, UInt32 edx, NiGeometry* Geo, NiDX9Renderer* Renderer);

extern float(__thiscall* GetWaterHeightLOD)(TESWorldSpace*);
float __fastcall GetWaterHeightLODHook(TESWorldSpace* This, UInt32 edx);

extern void(__cdecl* ProcessImageSpaceShaders)(NiDX9Renderer*, BSRenderedTexture*, BSRenderedTexture*);
void __cdecl ProcessImageSpaceShadersHook(NiDX9Renderer* Renderer, BSRenderedTexture* RenderedTexture1, BSRenderedTexture* RenderedTexture2);

extern void* (__thiscall* ShowDetectorWindow)(DetectorWindow*, HWND, HINSTANCE, NiNode*, char*, int, int, int, int);
void* __fastcall ShowDetectorWindowHook(DetectorWindow* This, UInt32 edx, HWND Handle, HINSTANCE Instance, NiNode* RootNode, char* FormCaption, int X, int Y, int Width, int Height);

extern BSRenderedTexture* (__cdecl* CreateBSRenderedTexture)(BSString*, const UInt32, const UInt32, NiTexture::FormatPrefs*, UInt32, bool, NiDepthStencilBuffer*, UInt32, UInt32);
BSRenderedTexture* __cdecl CreateSaveTextureHook(BSString* apName, const UInt32 uiWidth, const UInt32 uiHeight, NiTexture::FormatPrefs* kPrefs, UInt32 eMSAAPref, bool bUseDepthStencil, NiDepthStencilBuffer* pkDSBuffer, UInt32 a7, UInt32 uiBackgroundColor);

void RenderInterfaceHook();
void SetTileShaderConstantsHook();
void DetectorWindowCreateTreeViewHook();
void DetectorWindowDumpAttributesHook();
void DetectorWindowConsoleCommandHook();
void DetectorWindowDumpAttributesHook();

void __fastcall MuzzleLightCullingFix(MuzzleFlash* This);

void DetectorWindowDumpAttributes(HWND TreeView, UInt32 Msg, WPARAM wParam, LPTVINSERTSTRUCTA lParam);

void DetectorWindowSetNodeName(char* Buffer, int Size, char* Format, char* ClassName, char* Name, float LPosX, float LPosY, float LPosZ);

//__forceinline NiVector4* GetConstant(int index);
//__forceinline NiColorAlpha* GetLightConstant(int index);
//__forceinline void ScaleColor(NiColorAlpha* Color, float scale);
//void __fastcall ShadowLightShader__UpdateLights(void* apThis, void*, void* apShaderProp, void* apRenderPass, D3DXMATRIX aMatrix, void* apTransform, UInt32 aeRenderPassType, void* apSkinInstance);
