#pragma once
#include <../lib/tomlplusplus/include/toml++/toml.h>


struct SettingsMainStruct {

	struct MainStruct {
		bool	RemoveUnderwater;
		bool	ForceMSAA;
		bool	ForceReflections;
		bool	RemovePrecipitations;
		bool	MemoryHeapManagement;
		bool	MemoryTextureManagement;
		bool	ReplaceIntro;
        bool    SkipFog;
		UInt8	AnisotropicFilter;
		UInt16	ScreenshotKey;
		float	FarPlaneDistance;
	};
	
	struct FrameRateStruct {
		bool	SmartControl;
		bool	SmartBackgroundProcess;
		UInt32	SmartControlFPS;
		UInt32  BackgroundThreadPriority;
		float	FlowControl;
	};

	struct CullingProcessStruct {
		bool EnableCulling;
		bool EnableReflectionCulling;
		float CullMinSize;
		float CullMinSizeReflection;
	};

	struct OcclusionCullingStruct {
		bool	Enabled;
		bool	OccludingStatic;
		bool	OccludedStatic;
		bool	OccludedDistantStatic;
		bool	OccludedDistantStaticIC;
		UInt32	OcclusionMapRatio;
		float	OccludingStaticMin;
		float	OccludedStaticMin;
		float	OccludedStaticMax;
		float	OccludedDistantStaticMax;
	};

	struct CameraModeStruct {
		bool		Enabled;
		bool		ChasingFirst;
		bool		ChasingThird;
		UInt8		Crosshair;
		NiPoint3	Offset;
		NiPoint3	AimingOffset;
		NiPoint3	DialogOffset;
		float		FoV;
	};
	
	struct IKFootStruct {
		bool		Enabled;
		float		Coeff;
		float		CoeffThigh;
		float		CoeffCalf;
		float		CoeffFoot;
		float		CoeffToe;
	};

	struct EquipmentModeStruct {
		bool		Enabled;
		bool		SleepingEquipment;
		bool		SwimmingEquipment;
		UInt16		TorchKey;
		UInt16		CombatEquipmentKey;
		float		DualBlockDelay;
		NiPoint3	ShieldOnBackPos;
		NiPoint3	ShieldOnBackRot;
		NiPoint3	WeaponOnBackPos;
		NiPoint3	WeaponOnBackRot;
		NiPoint3	TwoHandWeaponOnBackPos;
		NiPoint3	TwoHandWeaponOnBackRot;
		NiPoint3	BowOnBackPos;
		NiPoint3	BowOnBackRot;
		NiPoint3	StaffOnBackPos;
		NiPoint3	StaffOnBackRot;
		NiPoint3	TorchOnBeltPos;
		NiPoint3	TorchOnBeltRot;
	};

	struct GrassModeStruct {
		bool	Enabled;
	};

	struct MountedCombatStruct {
		bool		Enabled;
		NiPoint3	WeaponOnBackPos;
		NiPoint3	WeaponOnBackRot;
		NiPoint3	TwoHandWeaponOnBackPos;
		NiPoint3	TwoHandWeaponOnBackRot;
		NiPoint3	BowOnBackPos;
		NiPoint3	BowOnBackRot;
		NiPoint3	StaffOnBackPos;
		NiPoint3	StaffOnBackRot;
	};
	
	struct SleepingModeStruct {
		bool	Enabled;
		UInt8	Mode;
	};
	
	struct WeatherModeStruct {
		bool		Enabled;
		NiPoint3	CoeffNight;
		NiPoint3	CoeffFog;
		NiPoint3	CoeffSun;
	};
	
	struct ShadowModeStruct {
		float	NearQuality;
	};

	struct ShadersStruct {
		bool Blood;
		bool Grass;
		bool HDR;
		bool NightEye;
		bool POM;
		bool Skin;
		bool Terrain;
		bool Water;
		bool Extra;
	};


	struct MenuStruct {
		char	TextFont[40];
		char	TextFontStatus[40];
		UInt8	TextColorNormal[3];
		UInt8	TextShadowColorNormal[3];
		UInt8	TextColorSelected[3];
		UInt8	TextShadowColorSelected[3];
		UInt8	TextColorEditing[3];
		UInt8	TextShadowColorEditing[3];
		UInt8	TextColorEnabled[3];
		UInt8	TextShadowColorEnabled[3];
		UInt8	TextSize;
		UInt8	TextSizeStatus;
		UInt8	KeyEnable;
		UInt8	KeyUp;
		UInt8	KeyDown;
		UInt8	KeyLeft;
		UInt8	KeyRight;
		UInt8	KeyPageUp;
		UInt8	KeyPageDown;
		UInt8	KeyAdd;
		UInt8	KeySubtract;
		UInt8	KeySave;
		UInt8	KeyEditing;
		int		PositionX;
		int		PositionY;
		int		TitleColumnSize;
		int		MainItemColumnSize;
		int		ItemColumnSize;
		int		RowSpace;
		int		RowsPerPage;
	};

	struct PurgerStruct {
		bool	Enabled;
		bool	PurgeTextures;
		bool	PurgeCells;
		UInt16	Key;
		int		Time;
	};

	struct GravityStruct {
		bool	Enabled;
		float	Value;
	};

	struct DodgeStruct {
		bool	Enabled;
		bool	DoubleTap;
		UInt8	AcrobaticsLevel;
		float	DoubleTapTime;
	};
	
	struct FlyCamStruct {
		bool	Enabled;
		UInt8	KeyAdd;
		UInt8	KeySubtract;
		float	ScrollMultiplier;
		float	StepValue;
	};

	struct DevelopStruct {
		UInt8	CompileShaders;  // 1 Compile All, 2 Compile modified or missing only, 3 Compile only in menu
		UInt8	CompileEffects;
		bool    DebugMode;       // enables hotkeys to print textures
		UInt8	TraceShaders;
	};

	MainStruct					Main;
	CullingProcessStruct		CullingProcess;
	OcclusionCullingStruct		OcclusionCulling;
	CameraModeStruct			CameraMode;
	IKFootStruct				IKFoot;
	EquipmentModeStruct			EquipmentMode;
	FrameRateStruct				FrameRate;
	GrassModeStruct				GrassMode;
	MountedCombatStruct			MountedCombat;
	SleepingModeStruct			SleepingMode;
	WeatherModeStruct			WeatherMode;
	ShadowModeStruct			ShadowMode;
	ShadersStruct				Shaders;
	MenuStruct					Menu;
	PurgerStruct				Purger;
	GravityStruct				Gravity;
	DodgeStruct					Dodge;
	FlyCamStruct				FlyCam;
	DevelopStruct				Develop;
};

struct SettingsShadowStruct {
	typedef std::vector<UInt32> ExcludedFormsList;

	struct FormsStruct {
		bool				Activators;
		bool				Actors;
		bool				Apparatus;
		bool				Books;
		bool				Containers;
		bool				Doors;
		bool				Furniture;
		bool				Misc;
		bool				Statics;
		bool				Terrain;
		bool				Trees;
		bool				Lod;
		float				MinRadius;
	};

	struct ExteriorsStruct {
		bool				Enabled;
        bool                BlurShadowMaps;
		FormsStruct			Forms[5];
		bool				AlphaEnabled[5];
		int					Quality;
		int					ShadowMapResolution;
		float				ShadowRadius;
		float				Darkness;
		float				NightMinDarkness;
        UInt8               ShadowMode;
		float				ShadowMapRadius[5];
		float				ShadowMapFarPlane;
		ExcludedFormsList	ExcludedForms;
	};
	
	struct InteriorsStruct {
		bool				Enabled;
		FormsStruct			Forms;
		bool				AlphaEnabled;
		bool				TorchesCastShadows;
		int					LightPoints;
		int					Quality;
		int					ShadowCubeMapSize;
		float				Darkness;
		float				LightRadiusMult;
		ExcludedFormsList	ExcludedForms;
	};

	struct ScreenSpaceStruct {
		bool				Enabled;
		float				BlurRadius;
		float				RenderDistance;
	};

	ScreenSpaceStruct	ScreenSpace;
	ExteriorsStruct		Exteriors;
	InteriorsStruct		Interiors;
};

struct SettingsWaterStruct {
	float choppiness;
	float waveWidth;
	float waveSpeed;
	float reflectivity;
	float causticsStrength;
	float causticsStrengthS;
	float shoreFactor;
	float shoreMovement;
	float turbidity;
	float inScattCoeff;
	float inExtCoeff_R;
	float inExtCoeff_G;
	float inExtCoeff_B;
	float depthDarkness;
};


struct SettingsSpecularStruct {
	bool Enabled;
	
	struct ExteriorStruct {
		float SpecularStrength;
		float BlurMultiplier;
		float Glossiness;
		float DistanceFade;
		float FresnelStrength;
		float SkyTintStrength;
		float SpecLumaTreshold;
		float SkyTintSaturation;
	};

	struct RainStruct {
		float SpecularStrength;
		float BlurMultiplier;
		float Glossiness;
		float DistanceFade;
		float FresnelStrength;
		float SkyTintStrength;
		float SpecLumaTreshold;
		float SkyTintSaturation;
	};

	ExteriorStruct Exterior;
	RainStruct Rain;

};

struct SettingsColoringStruct {
	float Strength;
	float BaseGamma;
	float Fade;
	float Contrast;
	float Saturation;
	float Bleach;
	float BleachLuma;
	float ColorCurve;
	float ColorCurveR;
	float ColorCurveG;
	float ColorCurveB;
	float EffectGamma;
	float EffectGammaR;
	float EffectGammaG;
	float EffectGammaB;
	float Linearization;
};


struct SettingsWeatherStruct {
	char					LowerLayer[80];
	char					UpperLayer[80];
	UInt8					WindSpeed;
	UInt8					CloudSpeedLower;
	UInt8					CloudSpeedUpper;
	UInt8					TransDelta;
	UInt8					SunGlare;
	UInt8					SunDamage;
	float					FogNearDay;
	float					FogNearNight;
	float					FogFarDay;
	float					FogFarNight;
	TESWeather::ColorData	Colors[TESWeather::kNumColorTypes];
	float					HDR[14];
};

typedef std::map<std::string, SettingsWaterStruct> SettingsWaterMap;
typedef std::map<std::string, SettingsColoringStruct> SettingsColoringMap;
typedef std::map<std::string, SettingsWeatherStruct> SettingsWeatherMap;
typedef std::vector<std::string> StringList;

class SettingManager : public SettingManagerBase {
public:
	class Configuration {
	public:
		enum NodeType {
			Boolean	 = 0,
			Integer  = 1,
			Float    = 2,
			String   = 3,
		};
		
		struct SectionPosition {
			char* Start;
			char* End;
			bool Found;
		};

		class ConfigNode {
		public:
			char		Section[40];
			char		Key[40];
			char		Value[80];
			bool		Reboot;
			UInt32		Type;
			
			bool operator < (const ConfigNode& Node) const { return (strcoll(Key, Node.Key) < 0 ? true : false); }
		};

		typedef	std::vector<ConfigNode> SettingList;

		void			Init();
		bool			FillNode(ConfigNode* Node, const char* Section, const char* Key);
		void			FillSections(StringList* Sections, const char* ParentSection);
		void			FillSettings(SettingList* Nodes, const char* Section);
		void			SetValue(ConfigNode* Node);
		void			CreateWeatherSection(const char* WeatherName, TESWeather* Weather);

		toml::table		TomlConfig;
		toml::table		DefaultConfig;
		bool			configLoaded;
		char*			Config;
		char*			ConfigB;
		UInt32			FileSize;
	};

	static void				Initialize();
	void					LoadSettings();
	void					SaveSettings();
	int						GetSettingI(const char* Section, const char* Key);
	float					GetSettingF(const char* Section, const char* Key);
	char*					GetSettingS(const char* Section, const char* Key, char* Value);
	void					SetSetting(const char* Section, const char* Key, float Value);
	void					SetSetting(const char* Section, const char* Key, UINT8 Value);
	void					SetSetting(const char* Section, const char* Key, bool Value);
	void					SetSettingS(const char* Section, const char* Key, char* Value);
	void					SetSetting(Configuration::ConfigNode* Node);
	void					SetSettingWeather(const char* Section, const char* Key, float Value);
	void					FillMenuSections(StringList* Sections, const char* ParentSection);
	void					FillMenuSettings(Configuration::SettingList* Settings, const char* Section);
	void					CreateNode(Configuration::ConfigNode* Node, const char* Section, const char* Key, float Value, bool Reboot);
	void					CreateNode(Configuration::ConfigNode* Node, const char* Section, const char* Key, UINT8 Value, bool Reboot);
	void					CreateNode(Configuration::ConfigNode* Node, const char* Section, const char* Key, bool Value, bool Reboot);
	void					CreateNodeS(Configuration::ConfigNode* Node, const char* Section, const char* Key, const char* Value, bool Reboot);
	bool					GetMenuShaderEnabled(const char* Name);
	void					SetMenuShaderEnabled(const char* Name, bool enabled);
	bool*					GetMenuShaderSetting(const char* Name);
	SettingsWaterStruct*	GetSettingsWater(const char* PlayerLocation);
	SettingsColoringStruct* GetSettingsColoring(const char* PlayerLocation);
	SettingsWeatherStruct*	GetSettingsWeather(const char* WeatherName);
	void					SetSettingsWeather(TESWeather* Weather);

	template <typename T> static std::string	ToString(const T Value);
	template <typename T> static T				FromString(const char* Value);
	static void									SplitString(const char* String, const char* Delimiter, StringList* Values);
	template <typename T> static void			FillFromString(const char* String, const char* Delimiter, T* Values);

				
	Configuration					Config;
	bool							GameLoading;
	SettingsMainStruct				SettingsMain;
	SettingsShadowStruct			SettingsShadows;
	SettingsWaterMap				SettingsWater;
	SettingsColoringMap				SettingsColoring;
	SettingsWeatherMap				SettingsWeather;
	SettingsSpecularStruct			SettingsSpecular;
};
