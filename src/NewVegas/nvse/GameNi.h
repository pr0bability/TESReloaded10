#pragma once

class NiNode;
class NiPick;
class NiTextKeyExtraData;
class NiFormArray;
class NiStringPalette;
class NiStream;
class NiTimeController;
class NiPropertyState;
class NiDynamicEffectState;
class NiProperty;
class NiFogProperty;
class NiAlphaProperty;
class NiShadeProperty;
class NiMaterialProperty;
class NiTexturingProperty;
class NiCullingProcess;
class NiDynamicEffect;
class NiControllerManager;
class NiVisibleArray;
class NiAdditionalGeometryData;
class NiGeometryData;
class NiGeometryGroup;
class NiRenderedCubeMap;
class NiObject;
class NiColor;
class NiColorAlpha;
class NiTexture;
class NiGeometry;
class NiFrustum;
class NiFrustumPlanes;
class NiTriBasedGeom;
class NiTriShape;
class NiTriStrips;
class NiPixelData;
class NiParticles;
class NiLines;
class NiFixedString;
class NiGeometryGroupManager;
class NiScreenElements;
class NiSourceCubeMap;
class NiRenderer;
class Ni2DBuffer;
class NiD3DRenderState;
class NiD3DRenderStateGroup;
class NiD3DShaderProgram;
class NiD3DShader;
class NiDX9LightManager;
class NiDX9VertexBufferManager;
class NiDX9ShaderConstantManager;
class NiDX9IndexBufferManager;
class NiDX9TextureManager;
class NiDX9RenderedTextureData;
class NiDX9RenderedCubeMapData;
class NiDX9DynamicTextureData;
class NiDX9AdapterDesc;
class NiDX9DeviceDesc;
class NiDX9Renderer;
class NiDX9RenderState;
class NiDX92DBufferData;

class BSShaderAccumulator;
class BSShaderProperty;
class BSFadeNode;
class BSMultiBound;
class BSMultiBoundShape;
class BSMultiBoundNode;
class BSSegmentedTriShape;
class BSResizableTriShape;
class BSShaderTextureSet;
class EffectShaderProperty;
class BSRenderPass;
class BSPortalGraph;

class BSFogProperty;
class ShadowSceneLight;
class ShadowSceneNode;
class AnimSequenceBase;

class TESObjectREFR;
class TESWaterCullingProcess;

class bhkRigidBody;
class bhkCollisionObject;
class bhkBlendCollisionObject;
class bhkLimitedHingeConstraint;

class ImageSpaceManager;
class ImageSpaceEffectParam;
class ImageSpaceShaderParam;

#define CREATE_OBJECT(CLASS, ADDRESS) static CLASS* CreateObject() { return StdCall<CLASS*>(ADDRESS); };

struct NiRTTI {
	const char* name;
	NiRTTI*		parent;

	const char* GetName() const { return name; }
	const NiRTTI* GetBase() const { return parent; }

	bool IsKindOf(const NiRTTI& apRTTI) const {
		for (const NiRTTI* i = this; i; i = i->GetBase()) {
			if (i == &apRTTI)
				return true;
		}
		return false;
	}

	bool IsKindOf(const NiRTTI* apRTTI) const {
		for (const NiRTTI* i = this; i; i = i->GetBase()) {
			if (i == apRTTI)
				return true;
		}
		return false;
	}

	template <typename T_RTTI>
	bool IsKindOf() const {
		return IsKindOf(T_RTTI::ms_RTTI);
	}

	bool IsExactKindOf(const NiRTTI* const apRTTI) const {
		return this == apRTTI;
	}

	bool IsExactKindOf(const NiRTTI& apRTTI) const {
		return this == &apRTTI;
	}

	template <typename T_RTTI>
	bool IsExactKindOf() const {
		return IsExactKindOf(T_RTTI::ms_RTTI);
	}
};
assert(sizeof(NiRTTI) == 0x008);

#define NIRTTI_ADDRESS(address) \
	static inline const NiRTTI* const ms_RTTI = (NiRTTI*)address;

enum NiMemEventType {
	NI_UNKNOWN = 0x0,
	NI_OPER_NEW = 0x1,
	NI_OPER_NEW_ARRAY = 0x2,
	NI_OPER_DELETE = 0x3,
	NI_OPER_DELETE_ARRAY = 0x4,
	NI_MALLOC = 0x5,
	NI_REALLOC = 0x6,
	NI_ALIGNEDMALLOC = 0x7,
	NI_ALIGNEDREALLOC = 0x8,
	NI_FREE = 0x9,
	NI_ALIGNEDFREE = 0xA,
	NI_EXTERNAL_MALLOC = 0xB,
	NI_EXTERNAL_REALLOC = 0xC,
	NI_EXTERNAL_ALIGNEDMALLOC = 0xD,
	NI_EXTERNAL_ALGINEDREALLOC = 0xE,
	NI_EXTERNAL_FREE = 0xF,
	NI_EXTERNAL_ALIGNEDFREE = 0x10,
};

class NiAllocator {
public:
	virtual void	Destroy(bool);
	virtual void*	Allocate(size_t& stSizeInBytes, size_t& stAlignment, NiMemEventType eEventType, bool bProvideAccurateSizeOnDeallocate, const char* pcFile, int iLine, const char* pcFunction);
	virtual void	Deallocate(void* pvMemory, NiMemEventType eEventType, size_t stSizeInBytes);
	virtual void*	Reallocate(void* pvMemory, size_t& stSizeInBytes, size_t& stAlignment, NiMemEventType eEventType, bool bProvideAccurateSizeOnDeallocate, size_t stSizeCurrent, const char* pcFile, int iLine, const char* pcFunction);
	virtual void*	AllocateExternal(size_t& stSizeInBytes, size_t& stAlignment, NiMemEventType eEventType, bool bProvideAccurateSizeOnDeallocate, const char* pcFile, int iLine, const char* pcFunction);
	virtual void	DeallocateExternal(void* pvMemory, NiMemEventType eEventType, size_t stSizeInBytes);
	virtual void*	ReallocateExternal(void* pvMemory, size_t* stSizeInBytes, size_t* stAlignment, NiMemEventType eEventType, bool bProvideAccurateSizeOnDeallocate, size_t stSizeCurrent, const char* pcFile, int iLine, const char* pcFunction);
	virtual void	Initialize();
	virtual void	Shutdown();
	virtual bool	VerifyAddress(const void* pvMemory);
};

struct NiMemManager {
	NiAllocator* m_pkAllocator;

	static NiMemManager* GetSingleton();
};

[[nodiscard]]
extern __declspec(allocator) void* NiNew(size_t stSize);
[[nodiscard]]
extern __declspec(allocator) void* NiAlloc(size_t stSize);
[[nodiscard]]
extern __declspec(allocator) void* NiAlignedAlloc(size_t stSize, size_t stAlignment);
extern void		NiFree(void* pvMem);
extern void		NiAlignedFree(void* pvMem);
extern void		NiDelete(void* pvMem, size_t stElementSize);

template <typename T_Data>
[[nodiscard]]
__declspec(restrict) __declspec(allocator) T_Data* NiNew() {
	return (T_Data*)NiNew(sizeof(T_Data));
}

template <typename T_Data>
[[nodiscard]]
__declspec(restrict) __declspec(allocator) T_Data* NiAlloc(UInt32 auiCount = 1) {
	return (T_Data*)NiAlloc(sizeof(T_Data) * auiCount);
}

template <typename T, const UInt32 ConstructorPtr = 0, typename... Args>
[[nodiscard]]
__declspec(restrict)T* NiCreate(Args &&... args) {
	auto* alloc = NiNew<T>();
	if constexpr (ConstructorPtr) {
		ThisStdCall(ConstructorPtr, alloc, std::forward<Args>(args)...);
	}
	else {
		memset(alloc, 0, sizeof(T));
	}
	return static_cast<T*>(alloc);
}

class NiPoint2 {
public:
	float	x;
	float	y;
};
assert(sizeof(NiPoint2) == 0x008);

class NiPoint3 {
public:
	NiPoint3() : x(0.f), y(0.f), z(0.f) {};
	NiPoint3(const float x, const float y, const float z) : x(x), y(y), z(z) {};

	float operator * (const NiPoint3 pt) const { return x * pt.x + y * pt.y + z * pt.z; }

	NiPoint3 operator- (const NiPoint3& pt) const { return NiPoint3(x - pt.x, y - pt.y, z - pt.z); };
	NiPoint3 operator- () const { return NiPoint3(-x, -y, -z); };
	NiPoint3& operator-= (const NiPoint3& pt) {
		x -= pt.x;
		y -= pt.y;
		z -= pt.z;
		return *this;
	};

	NiPoint3 operator/ (float fScalar) const {
		float fInvScalar = 1.0f / fScalar;
		return NiPoint3(fInvScalar * x, fInvScalar * y, fInvScalar * z);
	};

	NiPoint3& operator/= (float fScalar) {
		x /= fScalar;
		y /= fScalar;
		z /= fScalar;
		return *this;
	};

	__forceinline float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	__forceinline float Dot(const NiPoint3& pt) const {
		return x * pt.x + y * pt.y + z * pt.z;
	}

	__forceinline NiPoint3 Cross(const NiPoint3& pt) const {
		return NiPoint3(y * pt.z - z * pt.y, z * pt.x - x * pt.z, x * pt.y - y * pt.x);
	}

	__forceinline float Unitize() {
		float fLength = Length();

		if (fLength > 1e-06f) {
			float fRecip = 1.0f / fLength;
			x *= fRecip;
			y *= fRecip;
			z *= fRecip;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			fLength = 0.0f;
		}
		return fLength;
	}

	inline NiPoint3 UnitCross(const NiPoint3& pt) const {
		NiPoint3 cross(y * pt.z - z * pt.y, z * pt.x - x * pt.z, x * pt.y - y * pt.x);
		float fLength = cross.Length();
		if (fLength > 1e-06f)
			return cross / fLength;
		else
			return NiPoint3(0.0f, 0.0f, 0.0f);
	}

	void GetLookAt(NiPoint3* LookAt, NiPoint3* Rotation);

	float x;
	float y;
	float z;
	D3DXVECTOR3		toD3DXVEC3() {
		return D3DXVECTOR3(x, y, z);
	}
	D3DXVECTOR4		toD3DXVEC4() {
		return D3DXVECTOR4(x, y, z, 1.0);
	}

	static const NiPoint3 ZERO;
};
assert(sizeof(NiPoint3) == 0x00C);

class NiPoint4 {
public:
	float x;
	float y;
	float z;
	float w;
};
assert(sizeof(NiPoint4) == 0x10);

class NiVector4 {
public:
	void Normalize();

	float x;
	float y;
	float z;
	float w;
};
assert(sizeof(NiVector4) == 0x010);

class NiMatrix33 {
public:
	NiMatrix33() {}
	NiMatrix33(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22)
	{
		data[0][0] = m00;
		data[0][1] = m10;
		data[0][2] = m20;
		data[1][0] = m01;
		data[1][1] = m11;
		data[1][2] = m21;
		data[2][0] = m02;
		data[2][1] = m12;
		data[2][2] = m22;
	}

	NiPoint3 operator * (const NiPoint3 pt) const {
		return {
			data[0][0] * pt.x + data[0][1] * pt.y + data[0][2] * pt.z,
			data[1][0] * pt.x + data[1][1] * pt.y + data[1][2] * pt.z,
			data[2][0] * pt.x + data[2][1] * pt.y + data[2][2] * pt.z
		};
	}
	NiMatrix33 operator * (const NiMatrix33 mat) const {
		NiMatrix33 prd;

		prd.data[0][0] =
			data[0][0] * mat.data[0][0]+
			data[0][1] * mat.data[1][0]+
			data[0][2] * mat.data[2][0];
		prd.data[1][0] =
			data[1][0] * mat.data[0][0]+
			data[1][1] * mat.data[1][0]+
			data[1][2] * mat.data[2][0];
		prd.data[2][0] =
			data[2][0] * mat.data[0][0]+
			data[2][1] * mat.data[1][0]+
			data[2][2] * mat.data[2][0];
		prd.data[0][1] =
			data[0][0] * mat.data[0][1]+
			data[0][1] * mat.data[1][1]+
			data[0][2] * mat.data[2][1];
		prd.data[1][1] =
			data[1][0] * mat.data[0][1]+
			data[1][1] * mat.data[1][1]+
			data[1][2] * mat.data[2][1];
		prd.data[2][1] =
			data[2][0] * mat.data[0][1]+
			data[2][1] * mat.data[1][1]+
			data[2][2] * mat.data[2][1];
		prd.data[0][2] =
			data[0][0] * mat.data[0][2]+
			data[0][1] * mat.data[1][2]+
			data[0][2] * mat.data[2][2];
		prd.data[1][2] =
			data[1][0] * mat.data[0][2]+
			data[1][1] * mat.data[1][2]+
			data[1][2] * mat.data[2][2];
		prd.data[2][2] =
			data[2][0] * mat.data[0][2]+
			data[2][1] * mat.data[1][2]+
			data[2][2] * mat.data[2][2];
		return prd;
	}
	void GenerateRotationMatrixZXY(NiPoint3* v, bool degrees);

	float data[3][3];
};
assert(sizeof(NiMatrix33) == 0x024);

class NiTransform {
public:
	NiMatrix33	rot;		// 00
	NiPoint3	pos;		// 24
	float		scale;		// 30
};
assert(sizeof(NiTransform) == 0x034);

class NiPlane {
public:
	NiPlane() : Normal(0.0f, 0.0f, 0.0f), Constant(0.0f) {}
	NiPlane(const NiPoint3& kNormal, float fConstant) : Normal(kNormal), Constant(fConstant) {}
	NiPlane(const NiPoint3& kNormal, const NiPoint3& kPoint) : Normal(kNormal), Constant(kNormal.Dot(kPoint)) {}

	enum {
		NoSide = 0,
		PositiveSide = 1,
		NegativeSide = 2,
	};

	NiPoint3	Normal;
	float		Constant;

	float Distance(const NiPoint3& arPoint) const;
};
assert(sizeof(NiPlane) == 0x010);

class NiBound {
public:
	static NiBound* GetGlobalWorldBound();

	int WhichSide(const NiPlane& Plane) const;
	bool WithinFrustum(NiFrustumPlanes* arPlanes);

	NiPoint3	Center;
	float		Radius;
};
assert(sizeof(NiBound) == 0x010);

template <typename T>
class NiTArray {
public:
	UInt16			Add(T* Item);

	virtual ~NiTArray();	// 00
	T* data;			// 04
	UInt16	capacity;		// 08 - init'd to size of preallocation
	UInt16	end;			// 0A - index of the first free entry in the block of free entries at the end of the array (or numObjs if full)
	UInt16	numObjs;		// 0C - init'd to 0
	UInt16	growSize;		// 0E - init'd to size of preallocation
};
assert(sizeof(NiTArray<void>) == 0x010);

class NiMemObject {};

class NiRefObject {
public:
	virtual void		Destructor(bool freeThis);	// 00
	virtual void		Free(void);					// 01

	UInt32				m_uiRefCount;	// 004
};
assert(sizeof(NiRefObject) == 0x008);

class NiObject : public NiRefObject {
public:
	virtual const NiRTTI* GetRTTI() const;
	virtual NiNode* IsNiNode();
	virtual BSFadeNode* IsFadeNode();
	virtual BSMultiBoundNode* IsMultiBoundNode() const;
	virtual NiGeometryData* IsGeometry();
	virtual NiTriBasedGeom* IsTriBasedGeometry();
	virtual NiTriStrips* IsTriStrips();
	virtual NiTriShape* IsTriShape();
	virtual BSSegmentedTriShape* IsSegmentedTriShape();
	virtual BSResizableTriShape* IsResizableTriShape();
	virtual NiParticles* IsParticlesGeom();
	virtual NiLines* IsLinesGeom();
	virtual bhkCollisionObject* IsBhkNiCollisionObject();
	virtual bhkBlendCollisionObject* IsBhkBlendCollisionObject();
	virtual bhkRigidBody* IsBhkRigidBody();
	virtual bhkLimitedHingeConstraint* IsBhkLimitedHingeConstraint();
	virtual NiObject* Copy();			// 12
	virtual void							Load(NiStream* stream);
	virtual void							PostLoad(NiStream* stream);
	virtual void							FindNodes(NiStream* stream);
	virtual void							Save(NiStream* stream);
	virtual bool							Compare(NiObject* obj);
	virtual void							DumpAttributes(NiTArray <char*>* dst);
	virtual void							DumpChildAttributes(NiTArray <char*>* dst);
	virtual void							Unk_1A();
	virtual void							Unk_1B(UInt32 arg);
	virtual void							Unk_1C();
	virtual void							GetType2();
	virtual void							Unk_1E(UInt32 arg);
	virtual void							Unk_1F();
	virtual void							Unk_20();
	virtual void							Unk_21();
	virtual void							Unk_22();

	NIRTTI_ADDRESS(0x11F4418);

	template <class T_RTTI>
	bool IsKindOf() const {
		return IsKindOf(T_RTTI::ms_RTTI);
	}

	template <class T_RTTI>
	bool IsExactKindOf() const {
		return IsExactKindOf(T_RTTI::ms_RTTI);
	}

	bool IsKindOf(const NiRTTI& apRTTI) const;

	bool IsKindOf(const NiRTTI* const apRTTI) const;

	bool IsExactKindOf(const NiRTTI* const apRTTI) const;

	bool IsExactKindOf(const NiRTTI& apRTTI) const;

	static bool IsExactKindOf(const NiRTTI& apRTTI, NiObject* apObject);

	static bool IsExactKindOf(const NiRTTI* const apRTTI, NiObject* apObject);

	void LogObjectAttributes();
};
assert(sizeof(NiObject) == 0x008);

class BSMultiBound : public NiObject {
public:
	virtual bool GetPointWithin(const NiPoint3& arPoint);
	virtual void Nullsub024(void*);

	UInt32 uiBoundFrameCount;
	BSMultiBoundShape* spShape;

	static bool bIgnoreMultiBounds;

	CREATE_OBJECT(BSMultiBound, 0xC361A0);
};
assert(sizeof(BSMultiBound) == 0x10);

class NiFrustum {
public:
	NiFrustum() : Left(0.0f), Right(0.0f), Top(0.0f), Bottom(0.0f), Near(0.0f), Far(0.0f), Ortho(false) {}
	NiFrustum(float afLeft, float afRight, float afTop, float afBottom, float afNear, float afFar, bool abOrtho) {
		Left = afLeft;
		Right = afRight;
		Top = afTop;
		Bottom = afBottom;
		Near = afNear;
		Far = afFar;
		Ortho = abOrtho;
	}

	float	Left;		// 00
	float	Right;		// 04
	float	Top;		// 08
	float	Bottom;		// 0C
	float	Near;		// 10
	float	Far;		// 14
	bool	Ortho;		// 18
};
assert(sizeof(NiFrustum) == 0x01C);

class NiFrustumPlanes {
public:
	enum ActivePlanes {
		NearPlane = 0,
		FarPlane = 1,
		LeftPlane = 2,
		RightPlane = 3,
		TopPlane = 4,
		BottomPlane = 5,
		MaxPlanes = 6
	};

	NiPlane	CullingPlanes[MaxPlanes];	// 00
	UInt32	ActivePlanes;				// 60

	void Set(const NiFrustum& kFrust, const NiTransform& kXform);

	const NiPlane& GetPlane(UInt32 ePlane) const;
	bool IsPlaneActive(UInt32 ePlane) const;
	bool IsAnyPlaneActive() const;
	void EnablePlane(UInt32 ePlane);
	void DisablePlane(UInt32 ePlane);
	void SetActivePlaneState(UInt32 uiState);
};
assert(sizeof(NiFrustumPlanes) == 0x064);

class BSMultiBoundShape : public NiObject {
public:
	enum CullResult {
		BS_CULL_UNTESTED = 0,
		BS_CULL_VISIBLE = 1,
		BS_CULL_CULLED = 2,
		BS_CULL_OCCLUDED = 3,
	};

	enum ShapeType {
		BSMB_SHAPE_NONE = 0,
		BSMB_SHAPE_AABB = 1,
		BSMB_SHAPE_OBB = 2,
		BSMB_SHAPE_SPHERE = 3,
		BSMB_SHAPE_CAPSULE = 4,
	};

	enum IntersectResult
	{
		BS_INTERSECT_NONE = 0,
		BS_INTERSECT_PARTIAL = 1,
		BS_INTERSECT_CONTAINSTARGET = 2
	};

	virtual ShapeType		GetType() const;
	virtual double			GetRadius() const;
	virtual IntersectResult	CheckBSBound(BSMultiBound& arTargetBound) const;
	virtual IntersectResult	CheckBound(NiBound& arTargetBound) const;
	virtual bool			WithinFrustum(NiFrustumPlanes& arPlanes) const;
	virtual bool			CompletelyWithinFrustum(NiFrustumPlanes& arPlanes) const;
	virtual void			GetNiBound(NiBound& arBound) const;
	virtual void			CreateDebugGeometry(NiLines* apLines, NiTriShape* apGeometry, NiColorAlpha akColor);
	virtual UInt32			GetDebugGeomLineSize() const;
	virtual UInt32			GetDebugGeomShapeSize() const;
	virtual bool			GetPointWithin(NiPoint3& arPoint) const;
	virtual void			SetCenter(NiPoint3& arCenter);

	struct BoundVertices {
		NiPoint3 point[8];
	};

	CullResult eCullResult;

	inline void ResetCullResult() {
		eCullResult = BS_CULL_UNTESTED;
	};
};
assert(sizeof(BSMultiBoundShape) == 0xC);

class NiViewport {
public:
	float	l;
	float	r;
	float	t;
	float	b;
};
assert(sizeof(NiViewport) == 0x010);

template <class T> class NiRect {
public:
	NiRect(T left = T(0), T right = T(0), T top = T(0), T bottom = T(0))
	{
		m_left = left;
		m_right = right;
		m_top = top;
		m_bottom = bottom;
	}

	T m_left, m_right, m_top, m_bottom;
};
assert(sizeof(NiRect<float>) == 0x10);

class NiColor {
public:
	float	r;
	float	g;
	float	b;
};
assert(sizeof(NiColor) == 0x00C);

class NiColorAlpha {
public:
	float	r;
	float	g;
	float	b;
	float	a;
};
assert(sizeof(NiColorAlpha) == 0x010);

template <typename T>
class NiTList {
public:
	struct Entry {
		Entry* next;
		Entry* prev;
		T* data;
	};

	Entry*	start;		// 000
	Entry*	end;		// 004
	UInt32	numItems;	// 008
};
assert(sizeof(NiTList<void>) == 0x00C);

template <typename TKey, typename TData>
class NiTMap {
public:
	struct Entry {
		Entry*	next;
		TKey	key;
		TData*	data;
	};

	virtual UInt32	CalculateBucket(UInt32 key);
	virtual bool	CompareKey(UInt32 lhs, UInt32 rhs);
	virtual void	Fn_03(UInt32 arg0, UInt32 arg1, UInt32 arg2);	// assign to entry
	virtual void	Fn_04(UInt32 arg);
	virtual void	Fn_05();	// locked operations
	virtual void	Fn_06();	// locked operations

	UInt32			m_numBuckets;	// 4
	Entry**			m_buckets;		// 8
	UInt32			m_numItems;		// C
};
assert(sizeof(NiTMap<void, void>) == 0x010);

class NiPixelFormat {
public:
	enum Format {
		kFormat_RGB = 0,
		kFormat_RGBA,
		kFormat_PAL,
		kFormat_PALALPHA,
		kFormat_DXT1,
		kFormat_DXT3,
		kFormat_DXT5,
		kFormat_RGB24NONINTERLEAVED,
		kFormat_BUMP,
		kFormat_BUMPLUMA,
		kFormat_RENDERERSPECIFIC,
		kFormat_ONE_CHANNEL,
		kFormat_TWO_CHANNEL,
		kFormat_THREE_CHANNEL,
		kFormat_FOUR_CHANNEL,
		kFormat_DEPTH_STENCIL,
		kFormat_UNKNOWN,
		kFormat_MAX
	};

	enum Component {
		kComp_RED = 0,
		kComp_GREEN,
		kComp_BLUE,
		kComp_ALPHA,
		kComp_COMPRESSED,
		kComp_OFFSET_U,
		kComp_OFFSET_V,
		kComp_OFFSET_W,
		kComp_OFFSET_Q,
		kComp_LUMA,
		kComp_HEIGHT,
		kComp_VECTOR_X,
		kComp_VECTOR_Y,
		kComp_VECTOR_Z,
		kComp_PADDING,
		kComp_INTENSITY,
		kComp_INDEX,
		kComp_DEPTH,
		kComp_STENCIL,
		kComp_EMPTY,
		kComp_MAX
	};

	enum Representation {
		kRep_NORM_INT = 0,
		kRep_HALF,
		kRep_FLOAT,
		kRep_INDEX,
		kRep_COMPRESSED,
		kRep_UNKNOWN,
		kRep_INT,
		kRep_MAX
	};

	enum Tiling {
		kTile_NONE = 0,
		kTile_XENON,
		kTile_MAX
	};

	struct NiComponentSpec {
		Component		eComponent;			// 0
		Representation	eRepresentation;	// 4
		UInt8			BitsPerComponent;	// 8
		UInt8			Signed;				// 9
		UInt8			padA[2];			// A
	};

	UInt8			BitsPerPixel;	// 00
	UInt8			SRGBSpace;		// 01
	UInt8			pad02[2];
	Format			eFormat;		// 04
	Tiling			eTiling;		// 08
	UInt32			RendererHint;	// 0C
	UInt32			ExtraData;		// 10
	NiComponentSpec	Components[4];	// 14
};
assert(sizeof(NiPixelFormat) == 0x044);

class NiExtraData : public NiObject {
public:
	virtual	bool	Unk_23();
	virtual bool	Unk_24();

	char*	m_pcName;	// 08
};
assert(sizeof(NiExtraData) == 0x00C);

class NiObjectNET : public NiObject {
public:
	void				SetName(const char* Name) { char* (__cdecl* CreateString)(const char*) = (char* (__cdecl*)(const char*))0x00A5B690; char* S = CreateString(Name); ThisCall(0x00A5B950, this, &S); }

	const char*			m_pcName;						// 008
	NiTimeController*	m_controller;					// 00C
	NiExtraData**		m_extraDataList;				// 010
	UInt16				m_extraDataListLen;				// 014
	UInt16				m_extraDataListCapacity;		// 016

	NIRTTI_ADDRESS(0x11F4304);
};
assert(sizeof(NiObjectNET) == 0x018);

struct PropertyNode{
	PropertyNode* next;
	PropertyNode* prev;
	NiProperty* data;
};
struct PropertyList {
	PropertyNode* first;
	PropertyNode* last;
	UInt32 count;
};


template <std::size_t N> struct Debug; // No definition

template <> struct Debug<12> {}; // Definition for "old" sizeof;

template struct Debug<sizeof(PropertyNode)>; // Issue error if definition is missing

class NiAVObject : public NiObjectNET {
public:
	virtual void			UpdateControllers(float fTime);
	virtual void			Unk_24();
	virtual void			Unk_25();
	virtual NiAVObject*		GetObjectByName(const char* Name);
	virtual void*			Unk_27();
	virtual void			UpdateDownwardPass(float fTime, bool bUpdateControllers);
	virtual void			UpdateSelectedDownwardPass(float fTime);
	virtual void			UpdateRigidDownwardPass(float fTime);
	virtual void			UpdatePropertiesDownward(NiPropertyState* ParentState);
	virtual void			UpdateEffectsDownward(NiDynamicEffectState* ParentState);
	virtual void			UpdateTransformData();
	virtual void			UpdateTransformBound();
	virtual void			OnVisible(NiCullingProcess* CullingProcess);
	virtual void			Unk_30(void* arg);			// get NiMaterialProperty, pass to arg if found
	virtual void			Unk_31(void* arg);
	virtual void			Unk_32(void* arg);
	virtual void			Unk_33(void* arg);
	virtual void			Unk_34(void* arg);
	virtual void			Unk_35(void* arg);
	virtual void			Unk_36(void* arg);	// last is 036 verified

	enum NiFlags : UInt32 {
		APP_CULLED = 0x1,
		SELECTIVE_UPDATE = 0x2,
		SELECTIVE_UPDATE_TRANSFORMS = 0x4,
		SELECTIVE_UPDATE_CONTROLLER = 0x8,
		SELECTIVE_UPDATE_RIGID = 0x10,
		DISPLAY_OBJECT = 0x20,
		DISABLE_SORTING = 0x40,
		SELECTIVE_UPDATE_TRANSFORMS_OVERRIDE = 0x80,
		IS_NODE = 0x100,
		SAVE_EXTERNAL_GEOM_DATA = 0x200,
		NO_DECALS = 0x400,
		ALWAYS_DRAW = 0x800,
		MESH_LOD = 0x1000,
		FIXED_BOUND = 0x2000,
		TOP_FADE_NODE = 0x4000,
		IGNORE_FADE = 0x8000,
		NO_ANIM_SYNC_X = 0x10000,
		NO_ANIM_SYNC_Y = 0x20000,
		NO_ANIM_SYNC_Z = 0x40000,
		NO_ANIM_SYNC_S = 0x80000,
		NO_DISMEMBER = 0x100000,
		NO_DISMEMBER_VALIDITY = 0x200000,
		RENDER_USE = 0x400000,
		MATERIALS_APPLIED = 0x800000,
		HIGH_DETAIL = 0x1000000,
		FORCE_UPDATE = 0x2000000,
		PREPROCESSED_NODE = 0x4000000,
		UNK_27 = 0x8000000,
		UNK_28 = 0x10000000,
		IS_POINTLIGHT = 0x20000000,
		DONE_INIT_LIGHTS = 0x40000000,
		IS_INSERTED = 0x80000000,
	};

	float GetDistance(NiPoint3* Point);
	NiBound*	GetWorldBound() const;
	float		GetWorldBoundRadius();
	
	NiNode*					m_parent;				// 018
	UInt32					unk001C;				// 01C
	NiBound*				m_kWorldBound;			// 020
	PropertyList			propertyList;			// 024
	UInt32					m_flags;				// 030
	NiTransform				m_localTransform;		// 034
	NiTransform				m_worldTransform;		// 068
	
	bool WithinFrustum(NiFrustumPlanes* arPlanes);
};
assert(sizeof(NiAVObject) == 0x9C);

class NiNode : public NiAVObject {
public:
	virtual void		AddObject(NiAVObject* nChild, UInt8 FirstAvail); // Add child node
	virtual NiAVObject* RemoveObject(NiAVObject** RemovedChild, NiAVObject* nChild);
	virtual NiAVObject* RemoveObjectAt(NiAVObject** RemovedChild, UInt32 Index);
	virtual NiAVObject* SetObjectAt(NiAVObject** SetChild, UInt32 Index, NiAVObject* nChild);
	virtual void		ApplyTransform(const NiMatrix33& Mat, const NiPoint3& Trn, bool OnLeft);
	virtual bool		Unk_3B();
	virtual bool		Unk_3C();
	virtual bool		Unk_3D();
	virtual bool		Unk_3E();
	virtual bool		Unk_3F();

	NIRTTI_ADDRESS(0x11F4428);
	
	void				New(UInt16 Children);

	NiTArray<NiAVObject*>	m_children;	// 09C
};
assert(sizeof(NiNode) == 0xAC);

class BSNiNode : public NiNode {
public:
	virtual void ReparentSkinInstances(NiNode* apNode, NiAVObject* apParent);

	NIRTTI_ADDRESS(0x1204380);
};
assert(sizeof(BSNiNode) == sizeof(NiNode));

class BSMultiBoundNode : public BSNiNode {
public:
	virtual UInt32				GetMultiBoundRoom();
	virtual bool				GetPointWithin(NiPoint3& akPoint);
	virtual UInt32				CheckBound(BSMultiBound*);
	virtual UInt32				CheckBoundAlt(NiBound*);

	BSMultiBound*   spMultiBound;
	UInt32			uiCullingMode;

	CREATE_OBJECT(BSMultiBoundNode, 0xC46DF0);
	NIRTTI_ADDRESS(0x1202E74);
};
assert(sizeof(BSMultiBoundNode) == 0xB4);

class NiBillboardNode : public NiNode {
public:
	virtual void	Unk_40();

	UInt32			unkAC[2];		// AC

	NIRTTI_ADDRESS(0x11D5E70);
};
assert(sizeof(NiBillboardNode) == 0xB4);

class NiSwitchNode : public NiNode {
public:
	enum {
		UPDATE_ONLY_ACTIVE_CHILD = 1,
		UPDATE_CONTROLLERS = 2,
	};

	UInt16						m_usFlags;
	SInt32						m_iIndex;
	float						m_fSavedTime;
	UInt32						m_uiRevID;
	NiTArray<UInt32>			m_kChildRevID;

	CREATE_OBJECT(NiSwitchNode, 0xA94550);
	NIRTTI_ADDRESS(0x11F5EB4);
};
assert(sizeof(NiSwitchNode) == 0xCC);

class NiLODNode : public NiSwitchNode {
public:
	void*			m_spLODData;
	bool			m_bLODActive;

	static SInt32 ms_iGlobalLOD;
};
assert(sizeof(NiLODNode) == 0xD4);

class NiCamera : public NiAVObject {
public:
	D3DMATRIX 	    WorldToCam;	        // 94
	NiFrustum		Frustum;			// D4
	float			MinNearPlaneDist;	// F8
	float			MaxFarNearRatio;	// FC
	NiViewport		ViewPort;			// 100
	float			LODAdjust;			// 110

	CREATE_OBJECT(NiCamera, 0xA71430);

	bool LookAtWorldPoint(const NiPoint3& kWorldPt, const NiPoint3& kWorldUp);
};
assert(sizeof(NiCamera) == 0x114);

class SceneGraph : public NiNode {
public:
	void				UpdateParticleShaderFoV(float FoV);
	void				SetNearDistance(float Distance);
	
	NiCamera*			camera;					// AC
	UInt32				unkB0;					// B0
	NiCullingProcess*	cullingProcess;			// B4
	UInt8				IsMinFarPlaneDistance;	// B8 The farplane is set to 20480.0 when the flag is true. Probably used for interiors.
	UInt8				padB8[3];
	float				cameraFOV;				// BC
};
assert(sizeof(SceneGraph) == 0xC0);

class NiDynamicEffect : public NiAVObject {
public:
	UInt8			On;				// 9C
	UInt8			EffectType;		// 9D
	UInt8			CastShadows;	// 9E
	UInt8			CanCarry;		// 9F
	UInt32			Index;			// A0
	UInt32			unkA4;			// A4
	UInt32			unkA8;			// A8
	UInt32			unkAC;			// AC
	UInt32			unkB0;			// B0
	UInt32			unkB4;			// B4
	UInt32			unkB8;			// B8
	UInt32			unkBC;			// BC
	UInt32			unkC0;			// C0

	enum EffectTypes {
		AMBIENT_LIGHT = 0,
		POINT_LIGHT = 2,
		DIR_LIGHT = 3,
		SPOT_LIGHT = 4,
		TEXTURE_EFFECT = 5,
		MAX_TYPES = TEXTURE_EFFECT + 1,
	};
};
assert(sizeof(NiDynamicEffect) == 0xC4);

class NiLight : public NiDynamicEffect {
public:
	float			Dimmer;	// C4
	NiColor			Amb;	// C8
	NiColor			Diff;	// D4
	NiColor			Spec;	// E0
	void*			unk104;	// EC
};
assert(sizeof(NiLight) == 0xF0);

class NiPointLight : public NiLight {
public:
	float			Atten0;		// F0
	float			Atten1;		// F4
	float			Atten2;		// F8
};
assert(sizeof(NiPointLight) == 0xFC);

class NiDirectionalLight : public NiLight {
public:
	NiPoint3		direction;			// F0
};
assert(sizeof(NiDirectionalLight) == 0xFC);

class NiSpotLight : public NiPointLight {
public:

	NiPoint3	direction;
	float		OuterSpotAngle;
	float		InnerSpotAngle;
	float		SpotExponent;

	CREATE_OBJECT(NiSpotLight, 0xA94E20);
};
assert(sizeof(NiSpotLight) == 0x114);

class NiVBChip {
public:
	UInt32					Index;      // 00
	UInt32					Unk04;		// 04 union { NiVBBlock* m_pkBlock; NiVBDynamicSet* m_pkDynamicSet; };
	IDirect3DVertexBuffer9* VB;			// 08
	UInt32					Offset;		// 0C
	UInt32					LockFlags;	// 10
	UInt32					Size;		// 14
	NiVBChip*				Next;		// 18
	NiVBChip*				Prev;		// 1C
};
assert(sizeof(NiVBChip) == 0x020);

class NiGeometryBufferData {
public:
	virtual void		Destructor(bool freeThis);			// 00
	virtual void		Free();								// 01

	UInt32							Flags;					// 04
	NiGeometryGroup*				GeometryGroup;			// 08
	UInt32							FVF;					// 0C
	IDirect3DVertexDeclaration9*	VertexDeclaration;		// 10
	UInt32							unk10;					// 14
	UInt32							VertCount;				// 18
	UInt32							MaxVertCount;			// 1C
	UInt32							StreamCount;			// 20
	UInt32*							VertexStride;			// 24
	NiVBChip**						VBChip;					// 28
	UInt32							IndexCount;				// 2C
	UInt32							IBSize;					// 30
	IDirect3DIndexBuffer9*			IB;						// 34
	UInt32							BaseVertexIndex;		// 38
	D3DPRIMITIVETYPE				PrimitiveType;			// 3C
	UInt32							TriCount;				// 40
	UInt32							MaxTriCount;			// 44
	UInt32							NumArrays;				// 48
	UInt16*							ArrayLengths;			// 4C
	UInt16*							IndexArray;				// 50
};
assert(sizeof(NiGeometryBufferData) == 0x54);

class NiGeometryData : public NiObject {
public:
	virtual void				SetActiveVertexCount(uint16_t usActive);
	virtual uint16_t			GetActiveVertexCount() const;
	virtual UInt32				IsStripsData() const;
	virtual UInt32				IsShapeData() const;
	virtual bool				ContainsVertexData(UInt32 eParameter) const;
	virtual void				CalculateNormals();

	enum Consistency {
		MUTABLE = 0x0000,
		STATIC = 0x4000,
		CONSISTENCY_MASK = 0x7000,
	};

	enum KeepFlags {
		KEEP_NONE = 0,
		KEEP_XYZ = 1 << 0,
		KEEP_NORM = 1 << 1,
		KEEP_COLOR = 1 << 2,
		KEEP_UV = 1 << 3,
		KEEP_INDICES = 1 << 4,
		KEEP_BONEDATA = 1 << 5,
		KEEP_ALL = 0x3F,
	};

	enum DataFlags {
		NBT_METHOD_NONE = 0x0000,
		NBT_METHOD_NDL = 0x1000,
		NBT_METHOD_DEPRECATED = 0x2000,
		NBT_METHOD_ATI = 0x3000,
		NBT_METHOD_MASK = 0xF000,
		TEXTURE_SET_MASK = 0x3F
	};

	enum Compression {
		COMPRESS_NORM = 1 << 0,
		COMPRESS_COLOR = 1 << 1,
		COMPRESS_UV = 1 << 2,
		COMPRESS_WEIGHT = 1 << 3,
		COMPRESS_POSITION = 1 << 4,
		COMPRESS_ALL = 0x1F,
	};

	enum MarkAsChangedFlags {
		VERTEX_MASK = 1 << 0,
		NORMAL_MASK = 1 << 1,
		COLOR_MASK = 1 << 2,
		TEXTURE_MASK = 1 << 3,
		ALL_MASK = VERTEX_MASK | NORMAL_MASK | COLOR_MASK | TEXTURE_MASK,
		DIRTY_MASK = 0xFFF,
	};

	uint16_t					m_usVertices;
	uint16_t					m_usID;
	uint16_t					m_usDataFlags;
	uint16_t					m_usDirtyFlags;
	NiBound						m_kBound;
	NiPoint3*					m_pkVertex;
	NiPoint3*					m_pkNormal;
	NiColorAlpha*				m_pkColor;
	NiPoint2*					m_pkTexture;
	NiAdditionalGeometryData*	m_spAdditionalGeomData;
	NiGeometryBufferData*		m_pkBuffData;
	uint8_t						m_ucKeepFlags;
	uint8_t						m_ucCompressFlags;
	bool						bIsReadingData;
	bool						bUnk3B;
	bool						bCanSave;
};
assert(sizeof(NiGeometryData) == 0x40);

class NiSkinPartition : public NiObject {
public:
	class Partition {
	public:
		void**					_vtbl;			// 00
		UInt16*					pBones;			// 04
		float*					Weights;		// 08
		UInt32					Unk04[4];		// 0C
		UInt16					Vertices;		// 1C
		UInt16					Triangles;		// 1E
		UInt16					Bones;			// 20
		UInt16					Strips;			// 22
		UInt16					BonesPerVertex; // 24
		UInt16					pad28;
		NiGeometryBufferData*	BuffData;		// 28
	};

	UInt32		PartitionsCount;		// 08
	Partition*	Partitions;				// 0C
};
assert(sizeof(NiSkinPartition) == 0x10);

class NiSkinData : public NiObject {
public:
	class BoneVertData {
	public:
		UInt16	Verts;		// 00
		float	Weight;		// 04
	};

	class BoneData {
	public:
		NiTransform		SkinToBone;		// 00
		NiBound			Bound;			// 34
		BoneVertData*	BoneVertData;	// 44
		UInt16			Verts;			// 48
		UInt16			pad44;
	};

	NiSkinPartition*	SkinPartition;		// 08
	NiTransform			RootParentToSkin;	// 0C
	BoneData*			BoneData;			// 40
	UInt32				Bones;				// 44
};
assert(sizeof(NiSkinData) == 0x48);

class NiSkinInstance : public NiObject {
public:
	NiSkinData*			SkinData;				// 08
	NiSkinPartition*	SkinPartition;			// 0C
	NiAVObject*			RootParent;				// 10
	NiAVObject**		BoneObjects;			// 14
	UInt32				FrameID;				// 18
	UInt32				Bones;					// 1C
	UInt32				BoneRegisters;			// 20
	UInt32				BoneSize;				// 24
	void*				BoneMatrixes;			// 28 Matrixes array for the bones D3DMATRIX
	void*				SkinToWorldWorldToSkin;	// 2C D3DMATRIX
	UInt32				Unk30;					// 30

	NIRTTI_ADDRESS(0x11F5B2C);
	
	bool IsPartitionEnabled(UInt32 partitionIndex);
	
};
assert(sizeof(NiSkinInstance) == 0x34);

class DismemberPartition {
public:
	UInt8 Enabled;
	UInt8 StartCap;  //Questionable, but it's the only way I can make sense of that code
	UInt16 bodyPart;
}; 
assert(sizeof(DismemberPartition) == 4);

class BSDismemberSkinInstance : public NiSkinInstance {
public:
	UInt32 partitionNumber;
	DismemberPartition* partitions;
	UInt8  IsRenderable;  //In Load this is made in OR with every partition->Enabled flag
	UInt8  pad[3];

	NIRTTI_ADDRESS(0x11F49D8);
};
assert(sizeof(BSDismemberSkinInstance) == 0x40);

class NiProperty : public NiObjectNET {
public:
	enum PropertyType {
		kType_Alpha,
		kType_Culling,
		kType_Material,
		kType_Shade,
		kType_Stencil,
		kType_Texturing,
	};
	virtual PropertyType GetPropertyType();
	virtual void		 Update(UInt16 apUpdateData);

	NIRTTI_ADDRESS(0x11F4420);
};
assert(sizeof(NiProperty) == 0x18);

class NiStencilProperty : public NiProperty {
public:
	enum TestFunc {
		TEST_NEVER,
		TEST_LESS,
		TEST_EQUAL,
		TEST_LESSEQUAL,
		TEST_GREATER,
		TEST_NOTEQUAL,
		TEST_GREATEREQUAL,
		TEST_ALWAYS,
		TEST_MAX
	};

	enum Action {
		ACTION_KEEP,
		ACTION_ZERO,
		ACTION_REPLACE,
		ACTION_INCREMENT,
		ACTION_DECREMENT,
		ACTION_INVERT,
		ACTION_MAX
	};

	enum {
		ENABLE_MASK = 0x1,
		FAILACTION_MASK = 0xE,
		FAILACTION_POS = 0x1,
		ZFAILACTION_MASK = 0x70,
		ZFAILACTION_POS = 0x4,
		PASSACTION_MASK = 0x380,
		PASSACTION_POS = 0x7,
		DRAWMODE_MASK = 0xC00,
		DRAWMODE_POS = 0xA,
		TESTFUNC_MASK = 0xF000,
		TESTFUNC_POS = 0xC,
	};

	enum DrawMode {
		DRAW_CCW_OR_BOTH = 0,
		DRAW_CCW = 1,
		DRAW_CW = 2,
		DRAW_BOTH = 3,
		DRAW_MAX,
	};

	Bitfield16	m_usFlags;
	UInt32		m_uiRef;
	UInt32		m_uiMask;

	CREATE_OBJECT(NiStencilProperty, 0xA6F410);

	bool IsEnabled() const {
		return m_usFlags.GetBit(ENABLE_MASK);
	}

	void SetDrawMode(NiStencilProperty::DrawMode aeDraw) {
		m_usFlags.SetField(aeDraw, DRAWMODE_MASK, DRAWMODE_POS);
	}

	NiStencilProperty::DrawMode GetDrawMode() const {
		return (NiStencilProperty::DrawMode)m_usFlags.GetField(DRAWMODE_MASK, DRAWMODE_POS);
	}
};
assert(sizeof(NiStencilProperty) == 0x24);

class NiPropertyState {
public:
	enum PropertyID {
		ALPHA = 0,
		CULLING = 1,
		MATERIAL = 2,
		SHADE = 3,
		STENCIL = 4,
		TEXTURING = 5,
		UNK = 6,
		MAX,
	};

	union {
		struct {
			NiAlphaProperty*		m_spAlphaProperty;
			NiProperty*				m_spCullingProperty;
			NiMaterialProperty*		m_spMaterialProperty;
			NiShadeProperty*		m_spShadeProperty;
			NiStencilProperty*		m_spStencilProperty;
			NiTexturingProperty*	m_spTextureProperty;
			NiProperty*				m_spUnknownProperty;
		};
		NiProperty*		m_aspProps[MAX];
	};

	template <class T>
	T* GetShadeProperty() const { return static_cast<T*>(m_spShadeProperty); };
};
assert(sizeof(NiPropertyState) == 0x1C);

class NiGeometry : public NiAVObject {
public:

	virtual void	RenderImmediate(NiRenderer* pkRenderer);
	virtual void	RenderImmediateAlt(NiRenderer* pkRenderer);
	virtual void	SetModelData(NiGeometryData* pkModelData);
	virtual void	CalculateNormals();
	virtual void	CalculateConsistency(bool bTool);

	NiProperty*			GetProperty(NiProperty::PropertyType Type);

	NiPropertyState		propertyState;	// 9C
	NiGeometryData*		geomData;		// B8
	NiSkinInstance*		skinInstance;	// BC This seems to be a BSDismemberSkinInstance (old NiSkinInstance constructor is never used)
	NiD3DShader*		shader;			// C0
};
assert(sizeof(NiGeometry) == 0xC4);

class NiDX9TextureData : public NiObject {
public:
	NiTexture*				parent;			// 08
	UInt32					width;			// 0C
	UInt32					height;			// 10
	NiPixelFormat			PixelFormat;	// 14
	UInt8					Unk58;			// 58
	UInt8					Unk59;			// 59
	UInt8					Unk5A;			// 5A
	UInt8					Unk5B;			// 5B
	UInt32					Unk5C;			// 5C
	NiDX9Renderer*			pRenderer;		// 60
	IDirect3DBaseTexture9*	dTexture;		// 64
	UInt32					Levels;			// 68
};
assert(sizeof(NiDX9TextureData) == 0x6C);

class NiDX9SourceTextureData : public NiDX9TextureData {
public:
	UInt8					ReplacementData;	// 6C
	UInt8					Mipmap;				// 6D
	UInt8					pad64[2];
	UInt32					FormattedSize;		// 70
	UInt32					Palette;			// 74 NiPalette
	UInt32					LevelsSkipped;		// 78
	UInt32					SourceRevID;		// 7C
	UInt32					PalRevID;			// 80
};
assert(sizeof(NiDX9SourceTextureData) == 0x84);

class NiTexture : public NiObjectNET {
public:
	virtual UInt32	GetWidth();
	virtual UInt32	GetHeight();
	virtual void	Unk_25();
	virtual void	Unk_26();
	virtual void	Unk_27();
	virtual void	Unk_28();

	enum PixelLayout {
		kPixelLayout_Palettized8 = 0,
		kPixelLayout_HighColor16,
		kPixelLayout_TrueColor32,
		kPixelLayout_Compressed,
		kPixelLayout_Bumpmap,
		kPixelLayout_Palettized4,
		kPixelLayout_PixDefault,
		kPixelLayout_SingleColor8,
		kPixelLayout_SingleColor16,
		kPixelLayout_SingleColor32,
		kPixelLayout_DoubleColor32,
		kPixelLayout_DoubleColor64,
		kPixelLayout_FloatColor32,
		kPixelLayout_FloatColor64,
		kPixelLayout_FloatColor128,
		kPixelLayout_SingleColor4,
	};

	enum AlphaFormat {
		kAlpha_None = 0,
		kAlpha_Binary,	// 1bpp
		kAlpha_Smooth,	// 8bpp
		kAlpha_Default,
	};

	enum MipMapFlag {
		kMipMap_Disabled = 0,
		kMipMap_Enabled,
		kMipMap_Default,
	};

	struct FormatPrefs {
		PixelLayout pixelLayout;
		AlphaFormat alphaFormat;
		MipMapFlag	mipmapFormat;
	};

	FormatPrefs			formatPrefs;	// 018
	NiDX9TextureData*	rendererData;	// 024
	NiTexture*			nextTex;		// 028 - linked list updated in ctor/dtor
	NiTexture*			prevTex;		// 02C


	NiDX9TextureData* GetDX9RendererData() {
		return reinterpret_cast<NiDX9TextureData*>(rendererData);
	};
};
assert(sizeof(NiTexture) == 0x30);

class NiSourceTexture : public NiTexture {
public:
	virtual void	Unk_29();
	virtual void	Unk_2A();
	virtual void	Unk_2B();

	char*			ddsPath1;		// 30
	char*			ddsPath2;		// 34
	UInt32			unk38;			// 38
	UInt32			unk3C;			// 3C
	UInt32			unk40;			// 40
	UInt32			unk44;			// 44
};
assert(sizeof(NiSourceTexture) == 0x48);

class NiRenderedTexture : public NiTexture {
public:
	virtual Ni2DBuffer* GetBuffer();

	Ni2DBuffer* buffer;		// 030
	UInt32		unk034;		// 034
	UInt32		unk038;		// 038
	UInt32		unk03C;		// 03C
};
assert(sizeof(NiRenderedTexture) == 0x040);

class NiD3DTextureStage;

class NiShaderConstantMap : public NiRefObject {
public:
	virtual ~NiShaderConstantMap();

	virtual uint32_t	AddEntry(const char* pszKey, uint32_t uiFlags, uint32_t uiExtra, uint32_t uiShaderRegister, uint32_t uiRegisterCount, const char* pszVariableName = "", uint32_t uiDataSize = 0, uint32_t uiDataStride = 0, const void* pvDataSource = 0, bool bCopyData = false);
	virtual uint32_t	AddEntryAlt(const char* pszKey, uint32_t uiExtra, uint32_t uiShaderRegister, const char* pszVariableName);
	virtual uint32_t	AddAttributeEntry(const char* pszKey, uint32_t uiFlags, uint32_t uiExtra, uint32_t uiShaderRegister, uint32_t uiRegisterCount, const char* pszVariableName, uint32_t uiDataSize, uint32_t uiDataStride, const void* pvDataSource, bool bCopyData = false);
	virtual uint32_t	AddConstantEntry(const char* pszKey, uint32_t uiFlags, uint32_t uiExtra, uint32_t uiShaderRegister, uint32_t uiRegisterCount, const char* pszVariableName, uint32_t uiDataSize, uint32_t uiDataStride, const void* pvDataSource, bool bCopyData = false);
	virtual uint32_t	AddGlobalEntry(const char* pszKey, uint32_t uiFlags, uint32_t uiExtra, uint32_t uiShaderRegister, uint32_t uiRegisterCount, const char* pszVariableName, uint32_t uiDataSize, uint32_t uiDataStride, const void* pvDataSource, bool bCopyData = false);
	virtual uint32_t	AddOperatorEntry(const char* pszKey, uint32_t uiFlags, uint32_t uiExtra, uint32_t uiShaderRegister, uint32_t uiRegisterCount, const char* pszVariableName);
	virtual uint32_t	AddObjectEntry(const char* pszKey, uint32_t uiShaderRegister, const char* pszVariableName, uint32_t uiObjectIndex, UInt32 eObjectType);
	virtual void		SortByEnabled();

	UInt32 m_eProgramType;
};
assert(sizeof(NiShaderConstantMap) == 0xC);

class NiShaderConstantMapEntry : public NiRefObject {
public:
	bool			bEnabled;
	char*			m_kKey;
	uint32_t		m_uiInternal;
	Bitfield32		m_uiFlags;
	uint32_t		m_uiExtra;
	uint32_t		m_uiShaderRegister;
	uint32_t		m_uiRegisterCount;
	char*			m_kVariableName;
	uint32_t		m_uiDataSize;
	uint32_t		m_uiDataStride;
	void*			m_pvDataSource;
	bool			m_bOwnData;
	bool			m_bVariableHookupValid;
	bool			m_bColumnMajor;
};
assert(sizeof(NiShaderConstantMapEntry) == 0x38);

class NiD3DShaderConstantMap : public NiShaderConstantMap {
public:
	virtual ~NiD3DShaderConstantMap();

	virtual NiRTTI						GetRTTI();
	virtual NiNode*						IsNode();
	virtual BSFadeNode*					IsFadeNode();
	virtual BSMultiBoundNode*			IsMultiBoundNode();
	virtual NiGeometry*					IsGeometry();
	virtual NiTriBasedGeom*				IsTriBasedGeometry();
	virtual NiTriStrips*				IsTriStrips();
	virtual NiTriShape*					IsTriShape();
	virtual BSSegmentedTriShape*		IsSegmentedTriShape();
	virtual BSResizableTriShape*		IsResizableTriShape();
	virtual NiParticles*				IsParticlesGeom();
	virtual NiLines*					IsLinesGeom();
	virtual UInt32						IsBhkNiCollisionObject();
	virtual bhkBlendCollisionObject*	IsBhkBlendCollisionObject();
	virtual bhkRigidBody*				IsBhkRigidBody();
	virtual bhkLimitedHingeConstraint*	IsBhkLimitedHingeConstraint();
	virtual uint32_t					AddPredefinedEntry(const char* apszKey, uint32_t auiExtra, uint32_t auiShaderRegister = 0, const char* apszVariableName = "", uint32_t auiDataSize = 0, uint32_t auiDataStride = 0, void* pvDataSource = 0, bool bCopyData = false);
	virtual uint32_t					RemoveEntry(const char* apszKey);
	virtual NiShaderConstantMapEntry*	GetEntry(const char* apszKey);
	virtual int							GetEntryAtIndex(uint32_t auiIndex);
	virtual uint32_t					SetShaderConstants(NiD3DShaderProgram* apShaderProgram, NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apBuffData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound, uint32_t auiPass = 0, bool abGlobal = false);
	virtual uint32_t					GetEntryIndex(const char* apszKey);
	virtual uint32_t					InsertEntry(NiShaderConstantMapEntry* apEntry);
	virtual uint32_t					InsertDefinedEntry(NiShaderConstantMapEntry* apEntry);
	virtual uint32_t					SetupPredefinedEntry(NiShaderConstantMapEntry* apEntry);
	virtual uint32_t					SetDefinedConstant(NiD3DShaderProgram* apShaderProgram, NiShaderConstantMapEntry* apEntry, NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apBuffData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound, uint32_t auiPass);
	virtual uint32_t					SetConstantConstant(NiD3DShaderProgram* apShaderProgram, NiShaderConstantMapEntry* apEntry, uint32_t auiPass);
	virtual uint32_t					SetAttributeConstant(NiD3DShaderProgram* apShaderProgram, NiShaderConstantMapEntry* apEntry, NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apBuffData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound, uint32_t auiPass, bool abGlobal, NiExtraData* apExtraData);
	virtual uint32_t					SetGlobalConstant(NiD3DShaderProgram* apShaderProgram, NiShaderConstantMapEntry* apEntry, uint32_t auiPass);
	virtual uint32_t					SetOperatorConstant(NiD3DShaderProgram* apShaderProgram, NiShaderConstantMapEntry* apEntry, NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound, uint32_t auiPass);
	virtual uint32_t					SetupObjectEntry(NiShaderConstantMapEntry* apEntry);
	virtual uint32_t					SetObjectConstant(NiD3DShaderProgram* apShaderProgram, NiShaderConstantMapEntry* apEntry, NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apBuffData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound, uint32_t auiPass);

	NiTArray<NiShaderConstantMapEntry*>			m_aspEntries;
	NiTArray<NiShaderConstantMapEntry*>			m_aspDefinedEntries;
	bool										m_bModified;
	NiD3DShaderProgram*							m_pkLastShaderProgram;
	uint32_t									m_eLastError;
	LPDIRECT3DDEVICE9							m_pkD3DDevice;
	NiDX9Renderer*								m_pkD3DRenderer;
	NiD3DRenderState*							m_pkD3DRenderState;

	NIRTTI_ADDRESS(0x126F7C4);
};
assert(sizeof(NiD3DShaderConstantMap) == 0x44);

enum ShaderDefinitionEnum
{
  kShaderDefinition_ShadowLightShader = 0x1,
  kShaderDefinition_TallGrassShader = 0x2,
  kShaderDefinition_DistantLODShader = 0x3,
  kShaderDefinition_SpeedTreeBranchShader = 0x4,
  kShaderDefinition_SpeedTreeFrondShader = 0x5,
  kShaderDefinition_SpeedTreeLeafShader = 0x6,
  kShaderDefinition_BSShaderBloodSplatter = 0x7,
  kShaderDefinition_BSDistantTreeShader = 0x8,
  kShaderDefinition_NiD3DDefaultShader = 0x9,
  kShaderDefinition_SkyShader = 0xA,
  kShaderDefinition_HairShader = 0xD,
  kShaderDefinition_SkinShader = 0xE,
  kShaderDefinition_ParallaxShader = 0xF,
  kShaderDefinition_NiDX9ShaderDeclaration = 0x10,
  kShaderDefinition_WaterShader = 0x11,
  kShaderDefinition_ParticleShader = 0x18,
  kShaderDefinition_BoltShader = 0x19,
  kShaderDefinition_BeamShader = 0x1A,
  kShaderDefinition_Lighting30Shader = 0x1D,
  kShaderDefinition_PrecipitationShader = 0x1E,
  kShaderDefinition_TileShader = 0x20,
  kShaderDefinition_BSShaderNoLighting = 0x21,
  kShaderDefinition_VolumetricFogShader = 0x22,
};

class  NiShaderDeclaration : public NiObject {
public:
	NiShaderDeclaration();
	virtual ~NiShaderDeclaration();

	enum ShaderParameter
	{
		SHADERPARAM_INVALID = 0xFFFFFFFF,
		SHADERPARAM_NI_POSITION = 0x0,
		SHADERPARAM_NI_BLENDWEIGHT = 0x1,
		SHADERPARAM_NI_BLENDINDICES = 0x2,
		SHADERPARAM_NI_NORMAL = 0x3,
		SHADERPARAM_NI_COLOR = 0x4,
		SHADERPARAM_NI_TEXCOORD0 = 0x5,
		SHADERPARAM_NI_TEXCOORD1 = 0x6,
		SHADERPARAM_NI_TEXCOORD2 = 0x7,
		SHADERPARAM_NI_TEXCOORD3 = 0x8,
		SHADERPARAM_NI_TEXCOORD4 = 0x9,
		SHADERPARAM_NI_TEXCOORD5 = 0xA,
		SHADERPARAM_NI_TEXCOORD6 = 0xB,
		SHADERPARAM_NI_TEXCOORD7 = 0xC,
		SHADERPARAM_NI_TANGENT = 0xD,
		SHADERPARAM_NI_BINORMAL = 0xE,
		SHADERPARAM_DATA_ZERO = 0xF,
		SHADERPARAM_DATA_ONE = 0x10,
		SHADERPARAM_SKIP_COUNT_MASK = 0x40000000,
		SHADERPARAM_EXTRA_DATA_MASK = 0x80000000,
	};

	enum ShaderParameterType
	{
		SPTYPE_FLOAT1 = 0x0,
		SPTYPE_FLOAT2 = 0x1,
		SPTYPE_FLOAT3 = 0x2,
		SPTYPE_FLOAT4 = 0x3,
		SPTYPE_UBYTECOLOR = 0x4,
		SPTYPE_UBYTE4 = 0x5,
		SPTYPE_SHORT2 = 0x6,
		SPTYPE_SHORT4 = 0x7,
		SPTYPE_NORMUBYTE4 = 0x8,
		SPTYPE_NORMSHORT2 = 0x9,
		SPTYPE_NORMSHORT4 = 0xA,
		SPTYPE_NORMUSHORT2 = 0xB,
		SPTYPE_NORMUSHORT4 = 0xC,
		SPTYPE_UDEC3 = 0xD,
		SPTYPE_NORMDEC3 = 0xE,
		SPTYPE_FLOAT16_2 = 0xF,
		SPTYPE_FLOAT16_4 = 0x10,
		SPTYPE_NONE = 0x11,
		SPTYPE_COUNT = 0x12,
	};

	enum ShaderParameterTesselator
	{
		SPTESS_DEFAULT = 0x0,
		SPTESS_PARTIALU = 0x1,
		SPTESS_PARTIALV = 0x2,
		SPTESS_CROSSUV = 0x3,
		SPTESS_UV = 0x4,
		SPTESS_LOOKUP = 0x5,
		SPTESS_LOOKUPPRESAMPLED = 0x6,
		SPTESS_COUNT = 0x7,
	};

	enum ShaderParameterUsage
	{
		SPUSAGE_POSITION = 0x0,
		SPUSAGE_BLENDWEIGHT = 0x1,
		SPUSAGE_BLENDINDICES = 0x2,
		SPUSAGE_NORMAL = 0x3,
		SPUSAGE_PSIZE = 0x4,
		SPUSAGE_TEXCOORD = 0x5,
		SPUSAGE_TANGENT = 0x6,
		SPUSAGE_BINORMAL = 0x7,
		SPUSAGE_TESSFACTOR = 0x8,
		SPUSAGE_POSITIONT = 0x9,
		SPUSAGE_COLOR = 0xA,
		SPUSAGE_FOG = 0xB,
		SPUSAGE_DEPTH = 0xC,
		SPUSAGE_SAMPLE = 0xD,
		SPUSAGE_COUNT = 0xE,
	};

	class ShaderRegisterEntry
	{
	public:
		UInt32						m_uiRegister;
		UInt32						m_uiPackingOffset;
		ShaderParameter				m_eInput;
		ShaderParameterType			m_eType;
		ShaderParameterTesselator	m_eTesselator;
		ShaderParameterUsage		m_eUsage;
		UInt32						m_uiUsageIndex;
	};

	class ShaderRegisterStream
	{
	public:
		bool					m_bValid;
		UInt32					m_uiEntryCount;
		ShaderRegisterEntry* m_pkEntries;
		UInt32					m_uiStride;
	};

	virtual bool SetEntry(UInt32 uiEntry, UInt32 uiRegister, NiShaderDeclaration::ShaderParameter eInput, NiShaderDeclaration::ShaderParameterType eType, UInt32 uiStream);
	virtual bool RemoveEntry(UInt32 uiEntry, UInt32 uiStream = 0);
};
assert(sizeof(NiShaderDeclaration) == 0x8);

class NiD3DShaderDeclaration : public NiShaderDeclaration {
public:
	virtual bool							SetEntryAlt(UInt32 uiStream, UInt32 uiEntry, UInt32 uiRegister, NiShaderDeclaration::ShaderParameter eInput, NiShaderDeclaration::ShaderParameterType eType, NiShaderDeclaration::ShaderParameterUsage eUsage, UInt32 uiIndex, NiD3DShaderDeclaration::ShaderParameterTesselator eTess);
	virtual void							SetConstantData(UInt32 uiStartRegister, UInt32 uiNumRegisters, float* pfData);
	virtual void							ClearAllEntries(UInt32 uiStream);
	virtual UInt32							GetVertexStride(UInt32 uiStream);
	virtual void							SetSoftwareVertexProcessing();
	virtual bool							GetSoftwareVertexProcessing();
	virtual NiVBChip*						PackUntransformedVB(NiGeometryData* pkData, NiSkinInstance* pkSkin, NiSkinPartition::Partition* pkPartition, UInt16 usDirtyFlags, NiVBChip* pkOldVBChip, UInt32 uiStream, void* pvLockedBuff = nullptr, void* a = nullptr, void* b = nullptr);
	virtual bool							GenerateVertexParameters(LPDIRECT3DVERTEXDECLARATION9* pkDeclaration, UInt32* uiNumStreams);
	virtual LPDIRECT3DVERTEXDECLARATION9	GetD3DDeclaration();

	NiDX9Renderer*				Renderer;		// 008
	NiDX9VertexBufferManager*	BufferManager;	// 00C
	IDirect3DDevice9*			Device;			// 010
	UInt32						Unk014;			// 014
	UInt32						Unk018;			// 018
	UInt32						Unk01C;			// 01C
	UInt32						StreamCount;	// 020
	UInt32						Unk024;			// 024
	UInt8						Unk028;			// 028
	UInt8						Unk029[3];		// 029

};
assert(sizeof(NiD3DShaderDeclaration) == 0x02C);

class NiDX9ShaderDeclaration : public NiD3DShaderDeclaration {
public:
	NiDX9ShaderDeclaration();
	virtual ~NiDX9ShaderDeclaration();

	D3DVERTEXELEMENT9* m_pkElements;
	LPDIRECT3DVERTEXDECLARATION9	m_hVertexDecl;
	bool							m_bSoftwareVB;
	D3DDECLTYPE						ms_aeTypes[18];
	D3DDECLMETHOD					ms_aeMethods[7];
	D3DDECLUSAGE					ms_aeUsage[14];

	static NiDX9ShaderDeclaration* Create(NiDX9Renderer* apRenderer, UInt32 auiMaxStreamEntryCount, UInt32 auiStreamCount);
};
assert(sizeof(NiDX9ShaderDeclaration) == 0x0D4);

class ShaderRecord;

class NiD3DShaderProgram : public NiRefObject {
public:
	enum ProgramType {
		PROGRAM_VERTEX = 0,
		PROGRAM_PIXEL,
		PROGRAM_GEOMETRY,
		PROGRAM_MAX
	};

	union {
		ProgramType				eProgramType;		// 08

		struct {
			UInt8				ucProgramType;				// 08
			UInt8				Unk09;					// 09
			UInt8				Unk0A;					// 0A
			bool				Enabled;				// 0B
		};
	};
	const char*	const		Name;				// 0C
	ShaderRecord*			ShaderProg[3];
	IUnknown*				ShaderHandleBackup;
	IDirect3DDevice9*		Device;				// 20
	NiDX9Renderer*			Renderer;			// 24
	NiDX9RenderState*		RenderState;		// 28

	ShaderRecord* GetShaderRecord(int Type) {
		return ShaderProg[Type];
	};

	void SetName(const char* name) {
		ThisCall(0xBE0920, this, name);
	}
};
assert(sizeof(NiD3DShaderProgram) == 0x2C);

class NiD3DVertexShader : public NiD3DShaderProgram {
public:
	UInt8							SoftwareVP;		// 2C
	UInt8							pad[3];			// 2D
	UInt32							Usage;			// 30
	IDirect3DVertexShader9*			ShaderHandle;	// 34
	IDirect3DVertexDeclaration9*	Declaration;	// 38
};
assert(sizeof(NiD3DVertexShader) == 0x3C);

class NiD3DPixelShader : public NiD3DShaderProgram {
public:
	IDirect3DPixelShader9* ShaderHandle;	// 2C
};
assert(sizeof(NiD3DPixelShader) == 0x30);

class NiD3DRSEntry : public NiMemObject {
public:
	D3DRENDERSTATETYPE	m_uiRenderState;
	UInt32				m_uiValue;
	NiD3DRSEntry*		m_pkNext;
	NiD3DRSEntry*		m_pkPrev;
};
assert(sizeof(NiD3DRSEntry) == 0x10);

class NiD3DRenderStateGroup : public NiMemObject {
public:
	bool			m_bRendererOwned;
	UInt32			m_uiStateCount;
	NiD3DRSEntry*	m_pkRenderStates;
	UInt32			m_uiSavedStateCount;
	NiD3DRSEntry*	m_pkSavedRenderStates;

	NiD3DRSEntry*	FindRenderStateEntry(UInt32 auiState, bool& abInSaveList);
};
assert(sizeof(NiD3DRenderStateGroup) == 0x14);

class NiD3DPass {
public:
	void**							_vtbl;						// 00
	char							Name[16];					// 04
	UInt32							CurrentStage;				// 14
	UInt32							StageCount;					// 18
	UInt32							TexturesPerPass;			// 1C
	NiTArray<NiD3DTextureStage*>	Stages;						// 20
	NiD3DRenderStateGroup*			RenderStateGroup;			// 30
	NiD3DShaderConstantMap*			PixelConstantMap;			// 34
	char*							PixelShaderProgramFile;		// 38
	char*							PixelShaderEntryPoint;		// 3C
	char*							PixelShaderTarget;			// 40
	NiD3DPixelShader*				PixelShader;				// 44
	NiD3DShaderConstantMap*			VertexConstantMap;			// 48
	UInt8							Unk04C;						// 4C
	UInt8							pad04C[3];
	char*							VertexShaderProgramFile;	// 50
	char*							VertexShaderEntryPoint;		// 54
	char*							VertexShaderTarget;			// 58
	NiD3DVertexShader*				VertexShader;				// 5C
	UInt8							SoftwareVP;					// 60
	UInt8							RendererOwned;				// 61
	UInt8							pad[2];
	UInt32							RefCount;					// 64
};
assert(sizeof(NiD3DPass) == 0x68);

class NiShader : public NiRefObject {
public:
	NiShader();
	virtual ~NiShader();

	virtual NiRTTI*						GetRTTI();
	virtual NiNode*						IsNiNode();
	virtual BSFadeNode*					IsFadeNode();
	virtual BSMultiBoundNode*			IsMultiBoundNode();
	virtual NiGeometry*					IsGeometry();
	virtual NiTriBasedGeom*				IsTriBasedGeometry();
	virtual NiTriStrips*				IsTriStrips();
	virtual NiTriShape*					IsTriShape();
	virtual BSSegmentedTriShape*		IsSegmentedTriShape();
	virtual BSResizableTriShape*		IsResizableTriShape();
	virtual NiParticles*				IsParticlesGeom();
	virtual NiLines*					IsLinesGeom();
	virtual void*						IsBhkNiCollisionObject();
	virtual bhkBlendCollisionObject*	IsBhkBlendCollisionObject();
	virtual bhkRigidBody*				IsBhkRigidBody();
	virtual bhkLimitedHingeConstraint*	IsBhkLimitedHingeConstraint();
	virtual bool						IsInitialized();
	virtual void						Initialize();
	virtual bool						SetupGeometry(NiGeometry* apGeometry);
	virtual UInt32						PreProcessPipeline(NiGeometry* apGeometry, const NiSkinInstance* apSkin, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual void						UpdatePipeline(NiGeometry* apGeometry, const NiSkinInstance* apSkin, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual void						SetupRenderingPass(NiGeometry* apGeometry, const NiSkinInstance* apSkin, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual UInt32						SetupTransformations(NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual UInt32						SetupTransformationsAlt(NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual UInt32						SetupShaderPrograms(NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	// Do not use
	virtual NiGeometryBufferData*		vPrepareGeometryForRendering(NiGeometry* apGeometry, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties);
	virtual UInt32						PostRender(NiGeometry* apGeometry, const NiSkinInstance* apSkin, const NiSkinPartition::Partition* apPartition, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual UInt32						PostProcessPipeline(NiGeometry* apGeometry, const NiSkinInstance* apSkin, NiGeometryBufferData* apRendererData, const NiPropertyState* apProperties, const NiDynamicEffectState* apEffects, const NiTransform& arWorld, const NiBound& arWorldBound);
	virtual void						SetupTextures(const NiPropertyState* apProperties);
	virtual void						UpdateConstants(const NiPropertyState* apProperties);
	virtual void						SetBlendAlpha(const NiPropertyState* apProperties);
	virtual void						SetTestAlpha(const NiPropertyState* apProperties);
	virtual void						PreRenderSetup(const NiPropertyState* apProperties, bool abFirstPass);
	virtual void						PostRenderSetup(const NiPropertyState* apProperties);
	virtual int							HasPasses();
	virtual int							EndPass();
	virtual void						StreamCanSkip();
	virtual NiShaderDeclaration*		CreateShaderDeclaration(UInt32 auiMaxStreamEntryCount, UInt32 auiStreamCount = 1);
	virtual bool						AppendRenderPass(UInt32& auiPassId);
	virtual bool						SetAlphaOverride(UInt32 auiPassId, bool abAlphaBlend, bool abUsePreviousSrcBlendMode, UInt32 aeSrcBlendMode, bool abUsePreviousDestBlendMode, UInt32 aeDestBlendMode);
	virtual void						SetUsesNiRenderState(bool abUses);
	virtual bool						SetGPUProgram(UInt32 auiPassId, void* apProgram, UInt32& auiProgramId);
	virtual NiShaderConstantMap*		CreateShaderConstantMap(UInt32 auiPassId, UInt32 auiProgramId);
	virtual bool						AppendTextureSampler(UInt32 auiPassId, UInt32& auiSamplerId, const NiFixedString& arSemantic, const NiFixedString& arVariableName, UInt32 auiInstance = 0);
	virtual void*						GetShaderInstanceDesc();

	char*		Name;					// 008
	uint32_t	m_uiImplementation;
	bool		m_bInitialized;

	NIRTTI_ADDRESS(0x11F5D08);
};
assert(sizeof(NiShader) == 0x14);

class NiD3DShaderInterface : public NiShader {
public:
	virtual NiD3DShaderDeclaration*		GetShaderDecl();
	virtual void						SetShaderDecl(NiD3DShaderDeclaration* pkShaderDecl);
	virtual void						HandleResetDevice(void* pkShaderDecl);
	virtual void						HandleLostDevice();
	virtual void						DestroyRendererData();
	virtual bool						RecreateRendererData();
	virtual bool						GetVSPresentAllPasses();
	virtual bool						GetVSPresentAnyPass();
	virtual bool						GetPSPresentAllPasses();
	virtual bool						GetPSPresentAnyPass();
	virtual bool						GetIsBestImplementation();
	virtual void						SetIsBestImplementation(bool bIsBest);

	IDirect3DDevice9*	D3DDevice;		// 014
	NiDX9Renderer*		D3DRenderer;	// 018
	NiDX9RenderState*	D3DRenderState;	// 01C
	UInt8				Unk020;			// 020

	NIRTTI_ADDRESS(0x126F75C);
};
assert(sizeof(NiD3DShaderInterface) == 0x24);

class NiD3DShader : public NiD3DShaderInterface {
public:
	virtual void ResetPasses();

	bool					m_bUsesNiRenderState;
	bool					m_bUsesNiLightState;
	NiD3DShaderDeclaration* ShaderDeclaration;	// 028
	NiD3DRenderStateGroup*  RenderStateGroup;	// 02C
	NiD3DShaderConstantMap* m_spPixelConstantMap;
	NiD3DShaderConstantMap* m_spVertexConstantMap;
	UInt32					m_uiCurrentPass;

	NIRTTI_ADDRESS(0x126F754);
};
assert(sizeof(NiD3DShader) == 0x3C);

class NiD3DDefaultShader : public NiD3DShader {
public:
	enum PassFogStatus {
		NO_FOG = 0,
		BLACK_FOG = 1,
		STANDARD_FOG = 2,
	};

	bool							Unk3C;
	bool							Unk3D;
	uint32_t						m_uiMaxTextureIndex;
	uint32_t						m_uiRemainingStages;
	uint32_t						m_uiRemainingTextures;
	uint32_t						m_uiOldLightingValue;
	PassFogStatus*					m_peFogPassArray;
	uint32_t						m_uiFogPassArraySize;
	const NiDynamicEffectState*		m_pkLastState;

	NIRTTI_ADDRESS(0x126F554);
};
assert(sizeof(NiD3DDefaultShader) == 0x5C);

class BSShader : public NiD3DDefaultShader {
public:
	enum ShaderType {
		TYPE_ShadowLightShader = 0x1,
		TYPE_HairShader = 0x2,
		TYPE_ParallaxShader = 0x3,
		TYPE_SkinShader = 0x4,
		TYPE_SpeedTreeBranchShader = 0x5,
		TYPE_TallGrassShader = 0x6,
		TYPE_DistantLODShader = 0x7,
		TYPE_SpeedTreeFrondShader = 0x8,
		TYPE_SpeedTreeLeafShader = 0x9,
		TYPE_NiD3DDefaultShader = 0xA,
		TYPE_SkyShader = 0xB,
		TYPE_GeometryDecalShader = 0xC,
		TYPE_WaterShader = 0xD,
		TYPE_ParticleShader = 0xE,
		TYPE_BoltShader = 0xF,
		TYPE_BeamShader = 0x10,
		TYPE_Lighting30Shader = 0x11,
		TYPE_PrecipitationShader = 0x12,
		TYPE_VolumetricFogShader = 0x13,
		TYPE_TileShader = 0x14,
		TYPE_BSShaderNoLighting = 0x16,
		TYPE_BSShaderBloodSplatter = 0x17,
		TYPE_BSImagespaceShader = 0x18,
		TYPE_BSDistantTreeShader = 0x19,
	};

	virtual void SetRenderPass(UInt32 aeType);
	virtual void SetShaders(UInt32 aeType);
	// Seems to focus on tree shadows in most shaders
	virtual void SetupStaticTextures(UInt32 aeType);
	// Seems to focus on the fog in most shaders
	virtual void Func_63(UInt32 aeType);
	virtual void RestoreSavedStates(UInt32 aeType);
	virtual void RestoreTechnique(UInt32 uiLastPass);
	// Checks for bUnk64
	virtual void ConfigureTextureStages();
	virtual void ConfigureAllTextureStages();
	virtual void StartMultiPass(const NiPropertyState* apProperties);
	virtual void StopMultiPass();
	virtual NiDX9ShaderDeclaration* GetShaderDeclaration(NiGeometry*, BSShaderProperty* property);
	virtual void InitShaderConstants();
	// Reloads/ loads shader files and setups passes
	virtual void Reinitialize();
	virtual void ClearAllTextureStages();
	virtual void ClearTextureStages(NiD3DPass* apPass);
	virtual void Func_75();
	virtual void CreateNewPass();
	virtual void Func_77();
	virtual void Func_78(UInt32* apeType, int a2);

	NiD3DPass* pass;
	void* Unk60;
	bool Unk64;
	UInt32 iShaderType;
};
assert(sizeof(BSShader) == 0x6C);

class WaterShader : public BSShader {
public:
	NiD3DPass*				pPasses[74];
	NiD3DVertexShader*		pVertexShaders[3];
	NiD3DPixelShader*		pPixelShaders[38];
	NiRefObject*			spObject238;
	NiRefObject*			spObject23C;
	int						dword240;
	NiRefObject*			spObject244;
	NiRefObject*			spObject248;
	int						dword24C;
};
assert(sizeof(WaterShader) == 0x250);


class ShadowLightShader : public BSShader{
public:
	NiDX9ShaderDeclaration* pShaderDeclarationGlowParallaxSkin;
	NiDX9ShaderDeclaration* pShaderDeclaration70;
	NiDX9ShaderDeclaration* pShaderDeclarationModelSpaceNormals;
	NiDX9ShaderDeclaration* pShaderDeclarationLandscape;
	NiD3DShaderConstantMap* spPixelConstantMap;
	NiD3DShaderConstantMap* spVertexConstantMap;
	NiD3DShaderConstantMap* spPixelConstantMap2;
	NiD3DShaderConstantMap* spVertexConstantMap2;

	static __forceinline NiPoint4* GetConstant(int index) {
		return &((NiPoint4*)0x11FA0C0)[index];
	}

	struct VertexConstants {
		// 8
		static D3DXMATRIX* const WorldTranspose;

		// 12
		static __forceinline NiPoint4* const GetHighDetailRange() {
			return GetConstant(34);
		}

		// 19
		static NiPoint4* const LODLandParams;
	};
};
assert(sizeof(ShadowLightShader) == 0x8C);


class ParallaxShader : public ShadowLightShader{
public:
	NiD3DVertexShader*	pVertexShaders[20];
	NiD3DPixelShader*	pPixelShaders[33];
};
assert(sizeof(ParallaxShader) == 0x160);

class GeometryDecalShader : public BSShader {
public:
	virtual void Func_79();

	NiD3DPass*						spPasses[2];
	NiD3DVertexShader*				spVertexShaders[2];
	NiD3DPixelShader*				spPixelShaders[2];
	void*							spWorldViewProjTranspose;
	void*							spSkinModelViewProj;
	void*							spBones;

	static GeometryDecalShader* __stdcall CreateShader();

	static GeometryDecalShader* GetShader();
};
assert(sizeof(GeometryDecalShader) == 0x90);

class Lighting30Shader : public BSShader {
public:
	NiD3DShaderConstantMap*	spPixelConstantMapTexEffect;
	NiD3DShaderConstantMap*	spVertexConstantMapTexEffect;
	NiD3DShaderConstantMap*	spPixelConstantMapDefault;
	NiD3DShaderConstantMap*	spVertexConstantMapDefault;
	NiDX9ShaderDeclaration*	spShaderDeclaration7C;
	NiDX9ShaderDeclaration*	spShaderDeclaration80;
	NiDX9ShaderDeclaration*	spShaderDeclaration84_VC;
	NiDX9ShaderDeclaration*	spShaderDeclaration88;
	NiDX9ShaderDeclaration*	spShaderDeclaration8C_S;
	NiDX9ShaderDeclaration*	spShaderDeclaration90;
	NiDX9ShaderDeclaration*	spShaderDeclaration94_VC_S;
	NiDX9ShaderDeclaration*	spShaderDeclaration98;
	DWORD					dword9C;
	DWORD					dwordA0;

	static NiD3DPass** GetAllPasses();
};
assert(sizeof(Lighting30Shader) == 0xA4);

class TallGrassShader : public BSShader {
public:
	virtual void InitShaders();
	virtual void InitPasses();

	NiD3DPass*					spPasses[44];
	float						fWindTime;
	int							unk120;
	bool						unk124;
	NiD3DVertexShader*			spVertexShaders[24];
	NiD3DPixelShader*			spPixelShaders[7];
	NiPoint4*					pInstanceData;
	NiShaderConstantMapEntry*	pInstanceDataEntry;
	UInt16						ucInstanceCount;
	NiColorAlpha				kLightColor;
	NiPoint4					kLightData;
	NiColorAlpha				kAlphaTestRef;

	static inline const NiRTTI* const ms_RTTI = (NiRTTI*)0x1200508;

	static TallGrassShader* Create(NiDX9ShaderDeclaration* apShaderDeclaration);
	static TallGrassShader* __stdcall CreateShader();

	NiD3DPass* GetPass(UInt32 auID) { return spPasses[auID]; }
	NiD3DVertexShader* GetVertexShader(UInt32 auID) { return spVertexShaders[auID]; };
	NiD3DPixelShader* GetPixelShader(UInt32 auID) { return spPixelShaders[auID]; };

	static TallGrassShader* GetShader();

	struct VertexConstants {
		struct DrawDistanceSettings {
			float		fGrass_11F9EE0;
			float		fGrass_11F9EE4;
			float		fGrassStartFadeDistance;
			float		fGrassFadeRange;
		};

		struct ConstantGroup {
			NiPoint4				kModelViewProj;
			NiColorAlpha			kDiffuseColor;
			NiPoint4				kScaleMask;
			NiPoint4				kPointLighting;
			NiPoint2				kGrassSway;
			NiPoint2				kGrassWindMagnitude;
			DrawDistanceSettings	kDrawDistances;
			NiColorAlpha			kAmbientColor;
			NiColorAlpha			kDimmer;
			NiPoint4				kShadowProjData;
		};

		static ConstantGroup* const GroupedConstants;

	};
};

class SkyShader : public BSShader {
public:
	enum VertexShader {
		VS_SKY = 0,
		VS_SKYT = 1,
		VS_SKYMM = 2,
		VS_NONE = 3,
		VS_SKYSTARS = 4,
		VS_SKYOCC = 5,
		VS_SKYCLOUDS = 6,
		VS_SKYCLOUDSFADE = 7,
		VS_SKYFAR = 8,
		VS_SKYCLOUDSI = 9,
		VS_SKYQUADSI = 10,
		VS_SKY_MAX = 11
	};

	enum PixelShader {
		PS_SKY = 0,
		PS_SKYTEX = 1,
		PS_SKYTEXFADE = 2,
		PS_SKYSUNOCCL = 3,
		PS_SKYSHORIZFADE = 4,
		PS_SKYSISUN = 5,
		PS_SKYSICLOUDS = 6,
		PS_SKYSI = 7,
		PS_SKY_MAX = 8
	};

	enum Pass {
		SP_OCCLUSION = 0,
		SP_SUNGLARE = 1,
		SP_MOON_STARS_MASK = 2,
		SP_STARS = 3,
		SP_CLOUDS = 4,
		SP_UNDERWATER = 5,
		SP_TEXTURE = 6,
		SP_SKY = 7,
		SP_SELFILLUM_QUAD = 8,
		SP_SUN = 9,
		SP_SELFILLUM_CLOUDS = 10,
		SP_MAX = 11
	};

	enum SkyBlendLevel {
		BOTTOM = 0,
		CENTER = 1,
		TOP = 2,
	};


	NiD3DPass*				spPasses[SP_MAX];
	NiD3DVertexShader*		spVertexShaders[VS_SKY_MAX];
	NiD3DPixelShader*	    spPixelShaders[PS_SKY_MAX];
	float				    fTexCoordYOff;
	NiPoint2			    Params;
};
assert(sizeof(SkyShader) == 0xF0);

class ImageSpaceEffect {
public:
	virtual void Destroy(bool doFree);
	virtual void RenderShader(NiGeometry* apScreenShape, NiDX9Renderer* pkRenderer, ImageSpaceEffectParam* apParam, bool abEndFrame);
	virtual void Setup(ImageSpaceManager* pISManager, ImageSpaceEffectParam* apParam);
	virtual void Shutdown(void);
	virtual void BorrowTextures(ImageSpaceEffectParam* apParam);
	virtual void ReturnTextures(void);
	virtual bool IsActive(void);
	virtual bool UpdateParams(ImageSpaceEffectParam* apParam);

	bool							bIsActive;
	bool							bParamsChanged;
	NiTArray<ImageSpaceEffect>		EffectList;
	NiTArray<int>					EffectParamList;
	NiTArray<int>					Textures;
	NiTArray<int>					EffectInputs;
	NiTArray<int>					EffectOutput;
};
assert(sizeof(ImageSpaceEffect) == 0x58);

class BSImageSpaceShader : public BSShader {
public:
	ImageSpaceEffect		Effect;
	NiD3DVertexShader*		pVertexShader;
	NiD3DPixelShader*		pPixelShader;
	ImageSpaceShaderParam*	pParam;
};
assert(sizeof(BSImageSpaceShader) == 0xD0);

class WaterShaderHeightMap : public BSImageSpaceShader {
public:
};
assert(sizeof(WaterShaderHeightMap) == 0xD0);

class Ni2DBuffer : public NiObject {
public:
	UInt32				width;	// 008
	UInt32				height;	// 00C
	NiDX92DBufferData*	data;	// 010
};
assert(sizeof(Ni2DBuffer) == 0x014);

class NiDepthStencilBuffer : public Ni2DBuffer {};
assert(sizeof(NiDepthStencilBuffer) == 0x014);

class NiDX92DBufferData : public NiRefObject {
public:
	virtual void		func02();												// 02
	virtual void		func03();												// 03
	virtual void		func04();												// 04
	virtual	void		func05();												// 05
	virtual	void		func06();												// 06
	virtual void		func07();												// 07
	virtual void		func08();												// 08
	virtual void		func09();												// 09
	virtual void		func0A();												// 0A
	virtual void		func0B();												// 0B
	virtual void		func0C();												// 0C
	virtual void		func0D();												// 0D
	virtual void		func0E();												// 0E
	virtual void		func0F();												// 0F
	virtual void		func10();												// 10
	virtual void		func11();												// 11
	virtual void		func12();												// 12
	virtual void		func13();												// 13
	virtual void		func14();												// 14
	virtual void		func15();												// 15
	virtual void		func16();												// 16
	virtual void		func17();												// 17
	virtual void		func18();												// 18
	virtual void		func19();												// 19
	virtual void		func1A();												// 1A

	Ni2DBuffer*			ParentData;												// 08
	UInt32				Unk0C;													// 0C
	UInt32				Unk10;													// 10
	IDirect3DSurface9*	Surface;												// 14
};

class NiRenderTargetGroup : public NiObject {
public:
	virtual UInt32					GetWidth(UInt32 Index);											// 23
	virtual UInt32					GetHeight(UInt32 Index);										// 24
	virtual UInt32					GetDepthStencilWidth();											// 25
	virtual UInt32					GetDepthStencilHeight();										// 26
	virtual const NiPixelFormat*	GetPixelFormat(UInt32 Index);									// 27
	virtual const NiPixelFormat*	GetDepthStencilPixelFormat();									// 28
	virtual UInt32					GetBufferCount();											// 29
	virtual bool					AttachBuffer(Ni2DBuffer* Buffer, UInt32 Index);					// 2A
	virtual bool					AttachDepthStencilBuffer(NiDepthStencilBuffer* DepthBuffer);	// 2B
	virtual Ni2DBuffer*				GetBuffer(UInt32 Index);										// 2C
	virtual NiDepthStencilBuffer*	GetDepthStencilBuffer();										// 2D
	virtual void*					GetRendererData();												// 2E
	virtual void					SetRendererData(void* RendererData);							// 2F
	virtual void*					GetRenderTargetData(UInt32 RenderTargetIndex);					// 30
	virtual void*					GetDepthStencilBufferRendererData();							// 31
	virtual void					func32();													// 32
	virtual void					func33();													// 33

	UInt32							Unk08;						// 08
	Ni2DBuffer*						RenderTargets[4];			// 0C
	UInt32							numRenderTargets;			// 1C
	NiDepthStencilBuffer*			DepthStencilBuffer;			// 20
	void*							RenderData;					// 24

	NiDX9TextureData* GetDX9RendererData() {
		return reinterpret_cast<NiDX9TextureData*>(RenderData);
	}

};
assert(sizeof(NiRenderTargetGroup) == 0x28);

class NiDX9RenderState : public NiRefObject {
public:
	virtual void							UpdateRenderState(const NiPropertyState* pkNew);// 02
	virtual void							SetAlpha(void* AlphaProperty);					// 03
	virtual void							SetDither(void* DitherProperty);				// 04
	virtual void							SetFog(NiFogProperty* FogProperty);				// 05
	virtual void							SetMaterial(void* u1);							// 06
	virtual void							SetShadeMode(void* ShadeProperty);				// 07
	virtual void							SetSpecular(void* SpecularProperty);			// 08
	virtual void							SetStencil(void* StencilProperty);				// 09
	virtual void							SetWireframe(void* WireframeProperty);			// 0A
	virtual void 							SetZBuffer(void* ZBufferProperty);				// 0B
	virtual void							RestoreAlpha();									// 0C
	virtual void							SetVertexBlending(UInt16 u1);					// 0D
	virtual void							SetNormalization(void* u1);						// 0E
	virtual void							func_0F(UInt32 u1);								// 0F
	virtual float							GetVar088();									// 10
	virtual void							GetCameraNearAndFar(float* fNear, float* fFar);	// 11
	virtual void							SetCameraNearAndFar(float fNear, float fFar);	// 12
	virtual bool							GetLeftHanded();
	virtual void							SetLeftHanded(bool bLeft);
	virtual void							PurgeMaterial(NiMaterialProperty* pkMaterial);	// 15
	virtual void							InitializeRenderStates();						// 16
	virtual void							BackUpAllStates();								// 17
	virtual void 							RestoreRenderState(D3DRENDERSTATETYPE state);	// 18
	virtual void							RestoreAllRenderStates();						// 19
	virtual void 							SetRenderState(D3DRENDERSTATETYPE state, UInt32 value, UInt8 Arg3, UInt8 Arg4);					// 1A
	virtual UInt32							GetRenderState(D3DRENDERSTATETYPE state);														// 1B
	virtual void							ClearPixelShaders();																			// 1C
	virtual void							BackUpPixelShader();																			// 1D
	virtual void							func_1E();																						// 1E
	virtual void							SetPixelShader(IDirect3DPixelShader9* PixelShader, UInt8 BackUp);								// 1F
	virtual IDirect3DPixelShader9*			GetPixelShader();																				// 20
	virtual	void							RestorePixelShader();																			// 21
	virtual void							RemovePixelShader(IDirect3DPixelShader9* PixelShader);											// 22
	virtual void							SetVertexShader(IDirect3DVertexShader9* pShader, UInt8 BackUp);									// 23
	virtual IDirect3DVertexShader9*			GetVertexShader();																				// 24
	virtual void							RestoreVertexShader();																			// 25
	virtual void							RemoveVertexShader(IDirect3DVertexShader9* pShader);											// 26
	virtual void							SetFVF(UInt32 FVF, UInt8 BackUp);																// 27
	virtual UInt32							GetFVF();																						// 28
	virtual void							RestoreFVF();																					// 29
	virtual void							ClearFVF(UInt32 FVF);																			// 2A
	virtual void							SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl, UInt8 BackUp);							// 2B
	virtual IDirect3DVertexDeclaration9*	GetVertexDeclaration();																			// 2C
	virtual void							RestoreVertexDeclaration();																		// 2D
	virtual void							ClearVertexDeclatation(IDirect3DVertexDeclaration9* pDecl);										// 2E
	virtual void							func_02F();																						// 2F
	virtual void 							SetTextureStageState(UInt32 Stage, D3DTEXTURESTAGESTATETYPE Type, UInt32 Value, UInt8 BackUp);	// 30
	virtual UInt32							GetTextureStageState(UInt32 Stage, D3DTEXTURESTAGESTATETYPE Type);								// 31
	virtual void							func_032();																						// 32
	virtual HRESULT							SetSamplerState(UInt32 Sampler, D3DSAMPLERSTATETYPE Type, UInt32 Value, UInt8 BackUp);			// 33
	virtual UInt32 							GetSamplerState(UInt32 Sampler, D3DSAMPLERSTATETYPE Type);										// 34
	virtual void							RestoreSamplerState(UInt32 Sampler, D3DSAMPLERSTATETYPE Type);									// 35
	virtual void							ClearTextureList();																				// 36
	virtual void							SetTexture(UInt32 Sampler, IDirect3DBaseTexture9* pTexture);									// 37
	virtual IDirect3DBaseTexture9*			GetTexture(UInt32 Sampler);																		// 38
	virtual void							RemoveTexture(IDirect3DBaseTexture9* pTexture);													// 39
	virtual void							SetSoftwareVertexProcessing(UInt8 bSoftware);													// 3A
	virtual UInt8							GetSoftwareVertexProcessing();																	// 3B
	virtual void							SetVar_0FF4(UInt8 u1);																			// 3C
	virtual UInt8							GetVar_0FF4();																					// 3D
	virtual void							SetVar_0FF5(UInt8 u1);																			// 3E
	virtual UInt8							GetVar_0FF5();																					// 3F
	virtual void							Reset();																						// 40
	virtual void							func_041();																						// 41
	
	struct NiRenderStateSetting {
		UInt32 CurrentValue;
		UInt32 PreviousValue;
	};

	UInt32							Flags;							// 0008
	UInt32							unk000C[(0x0074 - 0x000C) >> 2];// 000C
	NiAlphaProperty*				DisabledAlphaProperty;			// 0074
	float							CameraNear;						// 0078
	float							CameraFar;						// 007C
	float							CameraDepthRange;				// 0080
	float							MaxFogFactor;					// 0084
	float							MaxFogValue;					// 0088
	NiColor							CurrentFogColor;				// 008C
	UInt32							m_uiCurrentFogColor;
	D3DCMPFUNC						m_auiStencilTestMapping[NiStencilProperty::TEST_MAX];
	D3DSTENCILOP					m_auiStencilActionMapping[NiStencilProperty::ACTION_MAX];
	D3DCULL							m_auiCullModeMapping[NiStencilProperty::DRAW_MAX][2];
	UInt32							LeftHanded;						// 00F4
	UInt32							Unk0F8[10];						// 00F8
	NiRenderStateSetting			RenderStateSettings[256];		// 0120
	UInt32							Unk0920[64];					// 0920
	NiRenderStateSetting			TextureStageStateSettings[128];	// 0A20
	NiRenderStateSetting			SamplerStateSettings[80];		// 0E20
	UInt32							unk10A0[20];					// 10A0
	NiDX9ShaderConstantManager*		ShaderConstantManager;			// 10F0
	UInt8							ForceNormalizeNormals;			// 10F4
	UInt8							InternalNormalizeNormals;		// 10F5
	UInt8							UsingSoftwareVP;				// 10F6
	UInt8							Declaration;					// 10F7
	IDirect3DDevice9*				Device;							// 10F8
	NiDX9Renderer*					Renderer;						// 10FC
	UInt32							unk1000[(0x1018 - 0x1000) >> 2];// 1100
	D3DCAPS9						Caps;							// 1118

	void SetCullMode(NiStencilProperty::DrawMode aeMode);
};
assert(offsetof(NiDX9RenderState, Device) == 0x10F8);
assert(sizeof(NiDX9RenderState) == 0x1248);

class NiRenderer : public NiObject {
public:
	enum ClearFlags {
		kClear_BACKBUFFER = 0x1,
		kClear_STENCIL = 0x2,
		kClear_ZBUFFER = 0x4,
		kClear_NONE = 0,
		kClear_ALL = kClear_BACKBUFFER | kClear_STENCIL | kClear_ZBUFFER
	};

	virtual void			Unk_23();
	virtual void			Unk_24();
	virtual void			Unk_25();
	virtual void			Unk_26();
	virtual void			Unk_27();
	virtual void			Unk_28();
	virtual void			SetDepthClear(float afValue);
	virtual float			GetDepthClear() const;
	virtual void			Unk_2B();
	virtual void			Unk_2C();
	virtual void			Unk_2D();
	virtual void			Unk_2E();
	virtual void			Unk_2F();
	virtual void			Unk_30();
	virtual void			Unk_31();
	virtual NiRenderTargetGroup* GetDefaultRT();	// get back buffer rt
	virtual NiRenderTargetGroup* GetCurrentRT();	// get currentRTGroup
	virtual void			Unk_34();
	virtual void			Unk_35();
	virtual void			Unk_36();
	virtual void			Unk_37();
	virtual void 			Unk_38();
	virtual void 			Unk_39();
	virtual void			Unk_3A();
	virtual void			Unk_3B();
	virtual void			PurgeGeometry(NiGeometryData* geo);
	virtual void			PurgeMaterial(NiMaterialProperty* material);
	virtual void			PurgeEffect(NiDynamicEffect* effect);
	virtual void			PurgeScreenTexture();
	virtual void			PurgeSkinPartition(NiSkinPartition* skinPartition);
	virtual void			PurgeSkinInstance(NiSkinInstance* skinInstance);
	virtual void			Unk_42();
	virtual bool			Unk_43();
	virtual void			Unk_44();
	virtual bool			FastCopy(void* src, void* dst, RECT* srcRect, SInt32 xOffset, SInt32 yOffset);
	virtual bool			Copy(void* src, void* dst, RECT* srcRect, RECT* dstRect, UInt32 filterMode);
	virtual void			Unk_47();
	virtual bool			Unk_48(void* arg);
	virtual void			Unk_49();
	virtual void			Unk_4A(float arg);
	virtual void 			Unk_4B(UInt32 size);
	virtual void			Unk_4C(UInt32 arg0, UInt32 arg1);
	virtual void			Unk_4D(UInt32 arg0, UInt32 arg1);
	virtual void			Unk_4E(void* buf);
	virtual void			CreateSourceTexture(NiSourceTexture* texture);
	virtual bool			CreateRenderedTexture(NiRenderedTexture* arg);
	virtual bool			CreateSourceCubeMap(NiSourceCubeMap* arg);
	virtual bool			CreateRenderedCubeMap(NiRenderedCubeMap* arg);
	virtual bool			CreateDynamicTexture(void* arg);
	virtual void			Unk_54();
	virtual bool			CreateDepthStencil(NiDepthStencilBuffer* arg, void* textureFormat);
	virtual void			Unk_56();
	virtual void			Unk_57();
	virtual void			Unk_58();
	virtual void			Unk_59();
	virtual void			Unk_5A();
	virtual void			Unk_5B();
	virtual void			Unk_5C();
	virtual void			Unk_5D();
	virtual void			Unk_5E();
	virtual bool			BeginScene();
	virtual bool			EndScene();
	virtual void			DisplayScene();
	virtual void			Clear(float* rect, UInt32 flags);
	virtual void			SetupCamera(NiPoint3* pos, NiPoint3* at, NiPoint3* up, NiPoint3* right, NiFrustum* frustum, float* viewport);
	virtual void			SetupScreenSpaceCamera(float* viewport);
	virtual bool			BeginUsingRenderTargetGroup(NiRenderTargetGroup* renderTarget, ClearFlags clearFlags);
	virtual bool			EndUsingRenderTargetGroup();
	virtual void			BeginBatch(UInt32 arg0, UInt32 arg1);
	virtual void			EndBatch();
	virtual void			BatchRenderShape(void* arg);
	virtual void			BatchRenderStrips(void* arg);
	virtual void			RenderTriShape(NiTriShape* obj);
	virtual void			RenderTriStrips(NiTriStrips* obj);
	virtual void			RenderTriShape2(NiTriShape* obj);
	virtual void			RenderTriStrips2(NiTriStrips* obj);
	virtual void			E70140(void*);
	virtual void			RenderParticles(NiParticles* obj);
	virtual void			RenderLines(NiLines* obj);
	virtual void			RenderScreenTexture();

	BSShaderAccumulator*	m_spBSShaderAccum;
	NiPropertyState*		m_pkCurrProp;
	NiDynamicEffectState*	m_pkCurrEffects;
	NiShader*				m_spErrorShader;
	void*					m_spInitialDefaultMaterial;
	void*					m_spCurrentDefaultMaterial;
	void*					m_spAccum;
	float					unk024;
	float					unk028;
	NiRenderer*				ms_pkRenderer;
	NiRect<float>			m_kDisplaySafeZone;
	char					unk040[62];
	UInt32					m_kRendererLock[8];
	char					unk0A0[94];
	UInt32					m_kPrecacheCriticalSection[8];
	char					unk120[95];
	UInt32					m_kSourceDataCriticalSection[8];
	char					unk1AC[92];
	UInt32					m_eSavedFrameState;
	UInt32					SceneState;					// 200
	UInt32					m_uiFrameID;
	bool					m_bRenderTargetGroupActive;
	bool					m_bBatchRendering;
	int						unk20C[29];
};
assert(sizeof(NiRenderer) == 0x280);

class NiDX9Renderer : public NiRenderer {
public:
	enum FrameBufferFormat {
		FBFMT_UNKNOWN = 0,
		FBFMT_R8G8B8,
		FBFMT_A8R8G8B8,
		FBFMT_X8R8G8B8,
		FBFMT_R5G6B5,
		FBFMT_X1R5G5B5,
		FBFMT_A1R5G5B5,
		FBFMT_A4R4G4B4,
		FBFMT_R3G3B2,
		FBFMT_A8,
		FBFMT_A8R3G3B2,
		FBFMT_X4R4G4B4,
		FBFMT_R16F,
		FBFMT_G16R16F,
		FBFMT_A16B16G16R16F,
		FBFMT_R32F,
		FBFMT_G32R32F,
		FBFMT_A32B32G32R32F,
		FBFMT_NUM
	};

	enum DepthStencilFormat {
		DSFMT_UNKNOWN = 0,
		DSFMT_D16_LOCKABLE = 70,
		DSFMT_D32 = 71,
		DSFMT_D15S1 = 73,
		DSFMT_D24S8 = 75,
		DSFMT_D16 = 80,
		DSFMT_D24X8 = 77,
		DSFMT_D24X4S4 = 79,
	};

	enum PresentationInterval {
		PRESENT_INTERVAL_IMMEDIATE = 0,
		PRESENT_INTERVAL_ONE = 1,
		PRESENT_INTERVAL_TWO = 2,
		PRESENT_INTERVAL_THREE = 3,
		PRESENT_INTERVAL_FOUR = 4,
		PRESENT_INTERVAL_NUM
	};

	enum SwapEffect {
		SWAPEFFECT_DEFAULT,
		SWAPEFFECT_DISCARD,
		SWAPEFFECT_FLIP,
		SWAPEFFECT_COPY,
		SWAPEFFECT_NUM
	};

	enum FrameBufferMode {
		FBMODE_DEFAULT,
		FBMODE_LOCKABLE,
		FBMODE_MULTISAMPLES_2 = 0x00010000,
		FBMODE_MULTISAMPLES_3 = 0x00020000,
		FBMODE_MULTISAMPLES_4 = 0x00030000,
		FBMODE_MULTISAMPLES_5 = 0x00040000,
		FBMODE_MULTISAMPLES_6 = 0x00050000,
		FBMODE_MULTISAMPLES_7 = 0x00060000,
		FBMODE_MULTISAMPLES_8 = 0x00070000,
		FBMODE_MULTISAMPLES_9 = 0x00080000,
		FBMODE_MULTISAMPLES_10 = 0x00090000,
		FBMODE_MULTISAMPLES_11 = 0x000a0000,
		FBMODE_MULTISAMPLES_12 = 0x000b0000,
		FBMODE_MULTISAMPLES_13 = 0x000c0000,
		FBMODE_MULTISAMPLES_14 = 0x000d0000,
		FBMODE_MULTISAMPLES_15 = 0x000e0000,
		FBMODE_MULTISAMPLES_16 = 0x000f0000,
		FBMODE_MULTISAMPLES_NONMASKABLE = 0x80000000,
		FBMODE_QUALITY_MASK = 0x0000FFFF,
		FBMODE_NUM = 18
	};

	enum RefreshRate {
		REFRESHRATE_DEFAULT = 0
	};

	enum TexFormat {
		TEX_RGB555 = 0x0,
		TEX_RGB565 = 0x1,
		TEX_RGB888 = 0x2,
		TEX_RGBA5551 = 0x3,
		TEX_RGBA4444 = 0x4,
		TEX_RGBA8888 = 0x5,
		TEX_PAL8 = 0x6,
		TEX_PALA8 = 0x7,
		TEX_DXT1 = 0x8,
		TEX_DXT3 = 0x9,
		TEX_DXT5 = 0xA,
		TEX_BUMP88 = 0xB,
		TEX_BUMPLUMA556 = 0xC,
		TEX_BUMPLUMA888 = 0xD,
		TEX_L8 = 0xE,
		TEX_A8 = 0xF,
		TEX_R16F = 0x10,
		TEX_RG32F = 0x11,
		TEX_RGBA64F = 0x12,
		TEX_R32F = 0x13,
		TEX_RG64F = 0x14,
		TEX_RGBA128F = 0x15,
		TEX_DEFAULT = 0x16,
		TEX_NUM = 0x16,
	};

	enum DeviceDesc
	{
		DEVDESC_PURE = 0x0,
		DEVDESC_HAL_HWVERTEX = 0x1,
		DEVDESC_HAL_MIXEDVERTEX = 0x2,
		DEVDESC_HAL_SWVERTEX = 0x3,
		DEVDESC_REF = 0x4,
		DEVDESC_REF_HWVERTEX = 0x5,
		DEVDESC_REF_MIXEDVERTEX = 0x6,
		DEVDESC_NUM = 0x7,
	};

	class PrePackObject {
	public:
		NiGeometryData*					m_pkData;
		NiSkinInstance*					m_pkSkin;
		NiSkinPartition::Partition*		m_pkPartition;
		NiD3DShaderDeclaration*			m_pkShaderDecl;
		UInt32							m_uiBonesPerPartition;
		UInt32							m_uiBonesPerVertex;
		NiGeometryBufferData*			m_pkBuffData;
		UInt32							m_uiStream;
		PrePackObject*					m_pkNext;
	};

	void								SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD Value);
	void								PackGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiD3DShaderDeclaration* ShaderDeclaration);
	void								PackSkinnedGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiSkinPartition::Partition* Partition, NiD3DShaderDeclaration* ShaderDeclaration);
	void								CalculateBoneMatrixes(NiSkinInstance* SkinInstance, NiTransform* WorldTrasform);

	LPDIRECT3DVERTEXDECLARATION9		hParticleVertexDecls[2];
	IDirect3DDevice9*					device;
	D3DCAPS9							m_kD3DCaps9;
	HWND								m_kWndDevice;
	HWND								m_kWndFocus;
	char								m_acDriverDesc[512];
	UInt32								adapterIdx;						// 5C4
	D3DDEVTYPE							d3dDevType;						// 5C8 - D3DDEVTYPE
	UInt32								m_uiBehaviorFlags;
	D3DFORMAT							m_eAdapterFormat;
	bool								m_bSWVertexCapable;
	bool								m_bSWVertexSwitchable;
	const NiDX9AdapterDesc*				m_pkAdapterDesc;
	const NiDX9DeviceDesc*				m_pkDeviceDesc;
	UInt32								m_uiBackground;
	float								m_fZClear;
	UInt32								m_uiStencilClear;
	UInt32								m_uiRendFlags;
	char								m_acBehavior[32];
	NiTMap<void*, void*>				PrePackObjects;					// 610 - NiTPointerMap <NiVBBlock *, NiDX9Renderer::PrePackObject *>
	void*								m_pkBatchHead;
	void*								m_pkBatchTail;
	NiPropertyState*					m_pkBatchedPropertyState;
	NiDynamicEffectState*				m_pkBatchedEffectState;
	NiD3DShaderInterface*				m_spBatchedShader;
	NiPoint3							m_kCamRight;
	NiPoint3							m_kCamUp;
	NiPoint3							m_kModelCamRight;
	NiPoint3							m_kModelCamUp;
	NiBound								m_kWorldBound;
	const NiBound						m_kDefaultBound;
	float								m_fNearDepth;
	float								m_fDepthRange;
	D3DXMATRIX							m_kD3DIdentity;
	D3DVIEWPORT9						m_kD3DPort;
	UInt32								m_uiHWBones;
	UInt32								m_uiMaxStreams;
	UInt32								m_uiMaxPixelShaderVersion;
	UInt32								m_uiMaxVertexShaderVersion;
	bool								m_bMipmapCubeMaps;
	bool								m_bDynamicTexturesCapable;
	UInt32								m_uiResetCounter;
	bool								m_bDeviceLost;
	NiPixelFormat*						m_aapkTextureFormats[4][22];
	NiPixelFormat*						m_apkDefaultTextureFormat[4];
	NiPixelData*						m_aspDefaultTextureData[4];
	TexFormat							m_eReplacementDataFormat;
	NiRenderTargetGroup*				defaultRTGroup;					// 884 - back buffer
	NiRenderTargetGroup*				currentRTGroup;					// 888
	NiRenderTargetGroup*				currentscreenRTGroup;			// 88C
	NiTMap<HWND*, NiRenderTargetGroup*>	screenRTGroups;					// 890 - NiTPointerMap <HWND *, NiPointer <NiRenderTargetGroup> >
	UInt32								m_uiMaxNumRenderTargets;
	bool								m_bIndependentBitDepths;
	bool								m_bMRTPostPixelShaderBlending;
	UInt32								Unk8A0[4];						// 8A0
	NiDX9RenderState*					renderState;					// 8B8
	UInt32								Unk8BC[33];						// 8BC
	D3DXMATRIXA16						worldMatrix;				// 940
	D3DXMATRIX							viewMatrix;					// 980
	D3DXMATRIX							projMatrix;					// 9C0
	D3DXMATRIX							viewProjMatrix;			    // A00
	D3DXMATRIX							invViewMatrix;				// A40
	UInt32								UnkA80[3];					// A80
	UInt16								m_usNumScreenTextureVerts;
	UInt16*								m_pusScreenTextureIndices;
	UInt32								m_uiNumScreenTextureIndices;
	UInt32								width;						// A98
	UInt32								height;						// A9C
	UInt32								flags;						// AA0
	HWND								windowDevice;				// AA4
	HWND								windowFocus;				// AA8
	UInt32								adapterType;				// AAC
	DeviceDesc							deviceType;					// AB0
	FrameBufferFormat					frameBufferFormat;			// AB4
	DepthStencilFormat					depthStencilFormat;			// AB8
	PresentationInterval				presentationInterval;		// ABC
	SwapEffect							swapEffect;					// AC0
	FrameBufferMode						frameBufferMode;			// AC4
	UInt32								backBufferCount;			// AC8
	RefreshRate							refreshRate;				// ACC
	bool								unkAD0;                     // AD0
	UInt32								m_kResetNotifyFuncsp[4];
	UInt32								m_kResetNotifyFuncData[4];
	UInt32								m_kLostDeviceNotifyFuncs[4];
	UInt32								m_kLostDeviceNotifyFuncData[4];
	NiTMap<D3DFORMAT, NiPixelFormat*>	m_kDepthStencilFormats;
	NiFrustum							m_kCachedFrustum;
	NiRect<float>						m_kCachedPort;
	void*								unkB50[12];

	static NiDX9Renderer* GetSingleton() { return *(NiDX9Renderer**)0x11C73B4; };
};
assert(sizeof(NiDX9Renderer) == 0xB80);

class NiControllerSequence : public NiObject {
public:
	virtual void	Unk_23();

	struct ControlledBlock {
		void*	unk00;		// 00 NiInterpolator*
		void*	unk04;		// 04 NiMultiTargetTransformController*
		UInt32	unk08;		// 08
		UInt8	unk0C;		// 0C
		UInt8	pad0C[3];
	};

	const char*				sequenceName;			// 08
	UInt32					numControlledBlocks;	// 0C
	UInt32					arrayGrowBy;			// 10
	ControlledBlock**		controlledBlocks;		// 14
	const char**			unkNodeName;			// 18
	float					weight;					// 1C
	NiTextKeyExtraData*		textKeyData;			// 20
	UInt32					cycleType;				// 24
	float					frequency;				// 28
	float					startTime;				// 2C
	float					stopTime;				// 30
	float					flt34;					// 34
	float					flt38;					// 38
	float					flt3C;					// 3C
	NiControllerManager*	manager;				// 40
	UInt32					unk44;					// 44
	UInt32					unk48;					// 48
	float					flt4C;					// 4C
	float					flt50;					// 50
	float					flt54;					// 54
	UInt32					unk58;					// 58
	const char*				rootNodeName;			// 5C
	UInt32					unk60[5];				// 60
};
assert(sizeof(NiControllerSequence) == 0x74);

class BSRenderedTexture : public NiObject {
public:
	NiRenderTargetGroup*	RenderTargetGroups[6];	// 008
	UInt32					unk020;				// 020
	UInt32					unk024;				// 024
	UInt32					unk028;				// 028
	UInt32					eType;				// 02C // BSTextureManager::RenderTargetTypes
	NiRenderedTexture*		RenderedTextures[4];// 030

	IDirect3DTexture9* GetD3DTexture(const UINT32 auIndex) {
		return (IDirect3DTexture9*)GetTexture(auIndex)->GetDX9RendererData()->dTexture;
	}

	NiRenderedTexture* GetTexture(const UInt32 uiIndex) {
		return RenderedTextures[uiIndex];
	}
};
assert(sizeof(BSRenderedTexture) == 0x40);

class BSShaderManager {
public:
	enum SceneGraphType : UInt32 {
		BSSM_SSN_WORLD				= 0,
		BSSM_SSN_MENU_OBJECT_3D		= 1,
		BSSM_SSN_UNK_2				= 2,
		BSSM_SSN_MENU_PLAYER_3D		= 3,
		BSSM_SSN_COUNT				= 4,
	};

	enum ShaderType : UInt32 {
		BSSM_SHADER_DEFAULT			= 0,
		BSSM_SHADER_SHADOWLIGHT		= 1,
		BSSM_SHADER_TALL_GRASS		= 2,
		BSSM_SHADER_DISTANT_LOD		= 3,
		BSSM_SHADER_ST_BRANCH		= 4,
		BSSM_SHADER_ST_FROND		= 5,
		BSSM_SHADER_ST_LEAF			= 6,
		BSSM_SHADER_BLOOD_SPLATTER	= 7,
		BSSM_SHADER_DISTANT_TREE	= 8,
		BSSM_SHADER_DEBUG			= 9,
		BSSM_SHADER_SKY				= 10,
		//							  11
		//							  12
		BSSM_SHADER_HAIR			= 13,
		BSSM_SHADER_SKIN			= 14,
		BSSM_SHADER_PARALLAX		= 15,
		BSSM_SHADER_GEOM_DECAL		= 16,
		BSSM_SHADER_WATER			= 17,
		// 						      18
		// 						      19
		// 						      20
		// 						      21
		// 						      22
		// 						      23
		BSSM_SHADER_PARTICLE		= 24,
		BSSM_SHADER_BOLT			= 25,
		BSSM_SHADER_BEAM			= 26,
		//							  27,
		//							  28,
		BSSM_SHADER_LIGHTING30		= 29,
		BSSM_SHADER_PRECIPITATION	= 30,
		// 						      31
		BSSM_SHADER_TILE			= 32,
		BSSM_SHADER_NOLIGHTING		= 33,
		BSSM_SHADER_VOLUMETRIC_FOG	= 34
	};

	static float* const fDepthBias;
	static float* const fLODLandDrop;
	static NiPoint3* const kCameraPos;
	static NiPoint4* const kLoadedRange;

	static BSShader** pspShaders;

	static ShadowSceneNode* GetShadowSceneNode(UInt32 aeType);

	static NiDX9Renderer* GetRenderer();

	static BSShader* GetShader(ShaderType aiShaderIndex);
};

class NiAlphaProperty : public NiProperty {
public:
	enum AlphaFlags {
		ALPHA_BLEND_MASK    = 0x0001,
		SRC_BLEND_MASK      = 0x001e,
		SRC_BLEND_POS       = 1,
		DEST_BLEND_MASK     = 0x01e0,
		DEST_BLEND_POS      = 5,
		TEST_ENABLE_MASK    = 0x0200,
		TEST_FUNC_MASK      = 0x1c00,
		TEST_FUNC_POS       = 10,
		ALPHA_NOSORTER_MASK = 0x2000
	};

	UInt16	flags;			// 018
	UInt8	alphaTestRef;	// 01A
	UInt8	unk01B;			// 01B
};
assert(sizeof(NiAlphaProperty) == 0x01C);

class NiMaterialProperty : public NiProperty {
public:
	SInt32		iIndex;
	NiColor		spec;
	NiColor		emit;
	NiColor*	pExternalEmittance;
	float		fShine;
	float		fAlpha;
	float		fEmitMult;
	UInt32		uiRevID;
	void*		pvRendererData;
};
assert(sizeof(NiMaterialProperty) == 0x04C);

class NiShadeProperty : public NiProperty {
public:
	enum ShaderPropType : UInt32
	{
		kProp_Lighting = 0x1,
		kProp_DistantLOD = 0x2,
		kProp_GeometryDecal = 0x3,
		kProp_TallGrass = 0x4,
		kProp_SpeedTreeLeaf = 0x6,
		kProp_PPLighting = 0x8,
		kProp_Hair = 0x9,
		kProp_SpeedTreeBranch = 0xA,
		kProp_SpeedTreeBillboard = 0xB,
		kProp_Lighting30 = 0xC,
		kProp_Sky = 0xD,
		kProp_Water = 0xE,
		kProp_Bolt = 0xF,
		kProp_Particle = 0x11,
		kProp_Precipitation = 0x12,
		kProp_Tile = 0x13,
		kProp_NoLighting = 0x15,
		kProp_VolumetricFog = 0x16,
		kProp_BloodSplatter = 0x17,
		kProp_DistantTree = 0x18,
	};

	enum ExtraFlags {
		kSmooth		 = 1 << 0, // always set by default, doesn't do anything
		kFirstPerson = 1 << 1, // marks player's view model geometry
		kThirdPerson = 1 << 2, // marks player's world model geometry
	};

	Bitfield16		m_usFlags;
	ShaderPropType	m_eShaderType;

	NIRTTI_ADDRESS(0x11F5AE0);
};
assert(sizeof(NiShadeProperty) == 0x20);


class BSFadeNode : public NiNode {
public:
	float			unkAC;			// AC
	float			unkB0;			// B0
	float			unkB4;			// B4
	float			FadeAlpha;		// B8
	float			BoundRadius;	// BC
	UInt32			unkC0;			// C0
	UInt32			MultType;		// C4
	UInt32			unkC8;			// C8
	TESObjectREFR*	unkCC;			// CC
	UInt32			unkD0[5];		// D0
};
assert(sizeof(BSFadeNode) == 0xE4);

class BSTreeModel : public NiRefObject {
public:
	UInt32					Unk08;				// 08
	UInt32					Unk0C;				// 0C
	UInt32					Unk10;				// 10
	UInt32					Unk14;				// 14
	UInt32					Unk18;				// 18
	UInt32					Unk1C;				// 1C
	UInt32					Unk20;				// 20
	UInt32					Unk24;				// 24
	UInt32					Unk28;				// 28
	void*					Unk2C;				// 2C BSShaderTextureSet*
	NiSourceTexture*		LeavesTexture;		// 30
	void*					Unk34;				// 34 BSShaderTextureSet*
	UInt32					Unk38;				// 38
	float					Unk3C;				// 3C
	UInt32					Unk40;				// 40
	UInt8					Unk44;				// 44
	UInt8					pad44[3];
	float					Unk48;				// 48
	float					Unk4C;				// 4C
};
assert(sizeof(BSTreeModel) == 0x50);

class BSTreeNode : public BSFadeNode {
public:
	BSTreeModel*		TreeModel;		// E4
	UInt32				UnkE0;			// E8
	UInt32				UnkE4;			// EC
	UInt32				UnkE8;			// F0
	float				UnkEC;			// F4
};
assert(sizeof(BSTreeNode) == 0xF8);

class ShadowSceneLight : public NiRefObject {
public:

	UInt32					unk008;
	float					fLuminance;
	D3DXMATRIX				kViewProjMatrix;
	D3DXMATRIX				kViewMatrix;
	D3DXMATRIX				kProjMatrix;
	float					fLODDimmer;
	float					fFade;
	float					fShadowFadeTime0D8;
	float					fShadowFadeTime0DC;
	NiTList<NiGeometry>		kGeometryList;
	bool					bIsShadowCasting;
	UInt8					byte0ED;
	NiAVObject*				spObject0F0;
	bool					bPointLight;
	bool					bAmbientLight;
	NiPointLight*			sourceLight;
	bool					bDynamicLight;
	NiPoint3				kPointPosition;
	UInt32					spShadowRenderTarget;
	UInt16					bIsEnabled;
	NiAVObject*				spObject114;
	bool					bUnk118;
	float					fUnk11C;
	float					fUnk120;
	bool					bShowDebugTexture;
	NiNode*					spShadowCasterNode;
	UInt32					kList12C[3];
	void*					pGeomListFence;
	void*					spFenceObject;
	NiCamera*				spShadowMapCamera;
	NiFrustumPlanes			kFrustumPlanes;
	float					fClipPlanes[24];
	bool					bUnk208;
	BSShaderAccumulator*	spShadowAccum;
	UInt32					kMultiBoundRooms[4];	// BSSimpleArray<BSMultiBoundRoom*>
	UInt32					kPortals[4];			// BSSimpleArray<BSPortal*>
	UInt32					kProcessedNodes[4];		// BSSimpleArray<NiNode>
	BSPortalGraph*			pPortalGraph;
	UInt32					unk244;
	UInt32					unk248;
	UInt32					unk24C;
};
assert(sizeof(ShadowSceneLight) == 0x250);

class ShadowSceneNode : public NiNode {
public:
	UInt32								unk0AC[2];			// 0AC
	NiTList<ShadowSceneLight>			lights;				// 0B4
	NiTList<ShadowSceneLight>			actorShadowCasters;	// 0C0
	NiTList<ShadowSceneLight>::Entry*	unk0CC;				// 0CC
	NiTList<ShadowSceneLight>::Entry*	unk0D0;				// 0D0
	NiTList<ShadowSceneLight>::Entry*	unk0D4;				// 0D4
	ShadowSceneLight*					sceneLight1;		// 0D8
	ShadowSceneLight*					sceneLight2;		// 0DC
	ShadowSceneLight*					directionalLight;	// 0E0
	UInt32								unk0E4[6];			// 0E4
	void*								ptr0FC;				// 0FC
	void*								ptr100;				// 100
	UInt32								unk104;				// 104
	UInt32								unk108[3];			// 108
	void*								ptr114;				// 114
	void*								ptr118;				// 118
	UInt32								unk11C;				// 11C
	UInt32								unk120;				// 120
	UInt32								unk124;				// 124
	NiCamera*							cubeMapCam;			// 128 BSCubeMapCamera*
	UInt32								unk12C;				// 12C
	UInt8								byte130;			// 130
	UInt8								byte131;			// 131
	UInt8								pad132[2];			// 132
	BSFogProperty*						fogProperty;		// 134 BSFogProperty*
	UInt32								unk138;				// 138
	UInt32								array13C[4];		// 13C BSSimpleArray<NiFrustumPlanes>
	UInt32								array14C[4];		// 14C BSSimpleArray<void>
	UInt32								unk15C[3];			// 15C
	NiVector4							unk168;				// 168
	NiVector4							unk178;				// 178
	NiVector4							unk188;				// 188
	NiVector4							unk198;				// 198
	NiVector4							unk1A8;				// 1A8
	NiVector4							unk1B8;				// 1B8
	UInt32								lightingPasses;		// 1C8
	float								flt1CC[3];			// 1CC
	UInt32								unk1D8;				// 1D8
	UInt8								byte1DC;			// 1DC
	UInt8								pad1DD[3];			// 1DD
	void*								portalGraph;		// 1E0 BSPortalGraph*
	UInt32								unk1E4[3];			// 1E4
	float								flt1F0[3];			// 1F0
	UInt8								byte1FC;			// 1FC
	UInt8								pad1FD[3];			// 1FD
};
assert(sizeof(ShadowSceneNode) == 0x200);
