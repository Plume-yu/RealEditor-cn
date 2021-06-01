#pragma once
#include "UObject.h"

class UTexture;
class UTexture2D;
class UMaterialExpression;

struct FTextureLookup
{
  int32 TexCoordIndex = 0;
  int32 TextureIndex = 0;

  float UScale = 1.;
  float VScale = 1.;

  friend FStream& operator<<(FStream& s, FTextureLookup& l);
};

class FMaterial {
public:
  void Serialize(FStream& s, UObject* owner);

  std::vector<FString> CompoilerErrors;
  std::map<PACKAGE_INDEX, int32> TextureDependencyLengthMap;
  int32 MaxTextureDependencyLength = 0;
  FGuid Id;
  uint32 NumUserTexCoords = 0;
  FObjectArray<UObject*> UniformExpressionTextures;
  bool bUsesSceneColor = false;
  bool bUsesSceneDepth = false;
  bool bUsesDynamicParameter = false;
  bool bUsesLightmapUVs = false;
  bool bUsesMaterialVertexPositionOffset = false;
  uint32 UsingTransforms = 0;
  std::vector<FTextureLookup> TextureLookups;
  uint32 FallbackComponents = 0;
  uint32 Unk1 = 1;
  uint32 Unk2 = 0;
  uint32 Unk3 = 0;
  uint32 Unk4 = 0;
};

struct FStaticSwitchParameter {
  FName ParameterName;
  bool Value = false;
  bool bOverride = false;
  FGuid ExpressionGUID;

  friend FStream& operator<<(FStream& s, FStaticSwitchParameter& p);
};

struct FStaticComponentMaskParameter {
  FName ParameterName;
  bool R = false;
  bool G = false;
  bool B = false;
  bool A = false;
  bool bOverride = false;
  FGuid ExpressionGUID;

  friend FStream& operator<<(FStream& s, FStaticComponentMaskParameter& p);
};

struct FNormalParameter {
  FName ParameterName;
  uint8 CompressionSettings = 1;
  bool bOverride = false;
  FGuid ExpressionGUID;

  friend FStream& operator<<(FStream& s, FNormalParameter& p);
};

struct FStaticTerrainLayerWeightParameter {
  FName ParameterName;
  bool bOverride = false;
  FGuid ExpressionGUID;
  int32 WeightmapIndex = 0;

  friend FStream& operator<<(FStream& s, FStaticTerrainLayerWeightParameter& p);
};

struct FStaticParameterSet {
  FGuid BaseMaterialId;
  std::vector<FStaticSwitchParameter> StaticSwitchParameters;
  std::vector<FStaticComponentMaskParameter> StaticComponentMaskParameters;
  std::vector<FNormalParameter> NormalParameters;
  std::vector<FStaticTerrainLayerWeightParameter> TerrainLayerWeightParameters;

  friend FStream& operator<<(FStream& s, FStaticParameterSet& ps);
};

struct FTextureParameter {
  UTexture* Texture = nullptr;
  bool AlphaChannelUsed = false;
};

class UMaterialInterface : public UObject {
public:
  DECL_UOBJ(UMaterialInterface, UObject);

  bool RegisterProperty(FPropertyTag* property) override;
  
  std::map<FString, float> GetScalarParameters() const;
  std::map<FString, FTextureParameter> GetTextureParameters() const;
  std::map<FString, UTexture*> GetTextureCubeParameters() const;
  std::map<FString, FLinearColor> GetVectorParameters() const;
  std::vector<UTexture*> GetTextureSamples() const;
  std::map<FString, bool> GetStaticBoolParameters() const;

  UTexture2D* GetTextureParameterValue(const FString& name) const;
  UTexture2D* GetDiffuseTexture() const;
  EBlendMode GetBlendMode() const;
  UObject* GetParent() const;

protected:
  UPROP_NOINIT(std::vector<FPropertyValue*>, ScalarParameterValues);
  UPROP_NOINIT(std::vector<FPropertyValue*>, TextureParameterValues);
  UPROP_NOINIT(std::vector<FPropertyValue*>, VectorParameterValues);
  UPROP(EBlendMode, BlendMode, EBlendMode::BLEND_Opaque);
  UPROP(PACKAGE_INDEX, Parent, INDEX_NONE);
};

class UMaterial : public UMaterialInterface {
public:
  DECL_UOBJ(UMaterial, UMaterialInterface);
  UPROP(bool, TwoSided, false);
  UPROP_CREATABLE_ARR_PTR(Expressions);
  std::vector<FPropertyTag*> MaterialInputs;

  void Serialize(FStream& s) override;
  bool RegisterProperty(FPropertyTag* property) override;

  std::vector<UMaterialExpression*> GetExpressions() const;

protected:
  FMaterial MaterialResource;
};

class UMaterialInstance : public UMaterialInterface {
public:
  DECL_UOBJ(UMaterialInstance, UMaterialInterface);

  UPROP(bool, bHasStaticPermutationResource, false);

  void Serialize(FStream& s) override;

  bool RegisterProperty(FPropertyTag* property) override;

  FMaterial StaticPermutationResource;
  FStaticParameterSet StaticParameters;
};

class UMaterialInstanceConstant : public UMaterialInstance {
public:
  DECL_UOBJ(UMaterialInstanceConstant, UMaterialInstance);
};