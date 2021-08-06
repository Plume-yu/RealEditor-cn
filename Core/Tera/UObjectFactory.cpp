#include "UObject.h"
#include "FObjectResource.h"
#include "FName.h"

#include "UClass.h"
#include "UProperty.h"
#include "UMetaData.h"

// Components
#include "UComponent.h"

// Objects
#include "UObjectRedirector.h"
#include "UMaterial.h"
#include "UMaterialExpression.h"
#include "UPersistentCookerData.h"
#include "UTexture.h"
#include "USkeletalMesh.h"
#include "UStaticMesh.h"
#include "USoundNode.h"
#include "UAnimation.h"
#include "UPhysAsset.h"
#include "ULevelStreaming.h"
#include "USpeedTree.h"
#include "UPrefab.h"
#include "UObjectReferencer.h"
#include "UModel.h"

#include "UActor.h"
#include "ULevel.h"
#include "UTerrain.h"
#include "ULight.h"

UObject* UObject::Object(FObjectExport* exp)
{
  UObject* result = nullptr;
  const FString c = exp->GetClassNameString();
  if (c == UClass::StaticClassName())
  {
    result = new UClass(exp, false);
  }
  else if (c == UPackage::StaticClassName())
  {
    result = new UPackage(exp);
  }
  else if (c == UTexture2D::StaticClassName())
  {
    result = new UTexture2D(exp);
  }
  else if (c == UTextureCube::StaticClassName())
  {
    result = new UTextureCube(exp);
  }
  else if (c == UTextureFlipBook::StaticClassName())
  {
    result = new UTextureFlipBook(exp);
  }
  else if (c == UShadowMapTexture2D::StaticClassName())
  {
    result = new UShadowMapTexture2D(exp);
  }
  else if (c == ULightMapTexture2D::StaticClassName())
  {
    result = new ULightMapTexture2D(exp);
  }
  else if (c == USkeletalMesh::StaticClassName())
  {
    result = new USkeletalMesh(exp);
  }
  else if (c == UMaterial::StaticClassName())
  {
    result = new UMaterial(exp);
  }
  else if (c == UMaterialInstance::StaticClassName())
  {
    result = new UMaterialInstance(exp);
  }
  else if (c == UMaterialInstanceConstant::StaticClassName())
  {
    result = new UMaterialInstanceConstant(exp);
  }
  else if (c == UStaticMesh::StaticClassName())
  {
    result = new UStaticMesh(exp);
  }
  else if (c == URB_BodySetup::StaticClassName())
  {
    result = new URB_BodySetup(exp);
  }
  else if (c == UPhysicsAssetInstance::StaticClassName())
  {
    result = new UPhysicsAssetInstance(exp);
  }
  else if (c == USpeedTree::StaticClassName())
  {
    result = new USpeedTree(exp);
  }
  else if (c == UPrefab::StaticClassName())
  {
    result = new UPrefab(exp);
  }
  else if (c == UModel::StaticClassName())
  {
    result = new UModel(exp);
  }
  else if (c == UPolys::StaticClassName())
  {
    result = new UPolys(exp);
  }
  else if (c == UAeroVolume::StaticClassName())
  {
    result = new UAeroVolume(exp);
  }
  else if (c == UAeroInnerVolume::StaticClassName())
  {
    result = new UAeroInnerVolume(exp);
  }
  else if (c == US1MusicVolume::StaticClassName())
  {
    result = new US1MusicVolume(exp);
  }
  else if (c == UActor::StaticClassName())
  {
    result = new UActor(exp);
  }
  else if (c == UTerrain::StaticClassName())
  {
    result = new UTerrain(exp);
  }
  else if (c == UTerrainWeightMapTexture::StaticClassName())
  {
    result = new UTerrainWeightMapTexture(exp);
  }
  else if (c == UTerrainMaterial::StaticClassName())
  {
    result = new UTerrainMaterial(exp);
  }
  else if (c == UTerrainLayerSetup::StaticClassName())
  {
    result = new UTerrainLayerSetup(exp);
  }
  else if (c == UBrush::StaticClassName())
  {
    result = new UBrush(exp);
  }
  else if (c == UBrushComponent::StaticClassName())
  {
    result = new UBrushComponent(exp);
  }
  else if (c == ULevel::StaticClassName())
  {
    result = new ULevel(exp);
  }
  else if (c == ULevelStreamingAlwaysLoaded::StaticClassName())
  {
    result = new ULevelStreamingAlwaysLoaded(exp);
  }
  else if (c == ULevelStreamingDistance::StaticClassName())
  {
    result = new ULevelStreamingDistance(exp);
  }
  else if (c == ULevelStreamingKismet::StaticClassName())
  {
    result = new ULevelStreamingKismet(exp);
  }
  else if (c == ULevelStreamingPersistent::StaticClassName())
  {
    result = new ULevelStreamingPersistent(exp);
  }
  else if (c == US1LevelStreamingDistance::StaticClassName())
  {
    result = new US1LevelStreamingDistance(exp);
  }
  else if (c == US1LevelStreamingBaseLevel::StaticClassName())
  {
    result = new US1LevelStreamingBaseLevel(exp);
  }
  else if (c == US1LevelStreamingSound::StaticClassName())
  {
    result = new US1LevelStreamingSound(exp);
  }
  else if (c == US1LevelStreamingSuperLow::StaticClassName())
  {
    result = new US1LevelStreamingSuperLow(exp);
  }
  else if (c == US1LevelStreamingVOID::StaticClassName())
  {
    result = new US1LevelStreamingVOID(exp);
  }
  else if (c == US1WaterVolume::StaticClassName())
  {
    result = new US1WaterVolume(exp);
  }
  else if (c == ULevelStreamingVolume::StaticClassName())
  {
    result = new ULevelStreamingVolume(exp);
  }
  else if (c == UBlockingVolume::StaticClassName())
  {
    result = new UBlockingVolume(exp);
  }
  else if (c == UStaticMeshActor::StaticClassName())
  {
    result = new UStaticMeshActor(exp);
  }
  else if (c == USkeletalMeshActor::StaticClassName())
  {
    result = new USkeletalMeshActor(exp);
  }
  else if (c == USpeedTreeActor::StaticClassName())
  {
    result = new USpeedTreeActor(exp);
  }
  else if (c == ULight::StaticClassName())
  {
    result = new ULight(exp);
  }
  else if (c == UPointLight::StaticClassName())
  {
    result = new UPointLight(exp);
  }
  else if (c == UPointLightMovable::StaticClassName())
  {
    result = new UPointLightMovable(exp);
  }
  else if (c == UPointLightToggleable::StaticClassName())
  {
    result = new UPointLightToggleable(exp);
  }
  else if (c == USpotLight::StaticClassName())
  {
    result = new USpotLight(exp);
  }
  else if (c == USpotLightMovable::StaticClassName())
  {
    result = new USpotLightMovable(exp);
  }
  else if (c == USpotLightToggleable::StaticClassName())
  {
    result = new USpotLightToggleable(exp);
  }
  else if (c == UDirectionalLight::StaticClassName())
  {
    result = new UDirectionalLight(exp);
  }
  else if (c == UDirectionalLightToggleable::StaticClassName())
  {
    result = new UDirectionalLightToggleable(exp);
  }
  else if (c == USkyLight::StaticClassName())
  {
    result = new USkyLight(exp);
  }
  else if (c == USkyLightToggleable::StaticClassName())
  {
    result = new USkyLightToggleable(exp);
  }
  else if (c == UInterpActor::StaticClassName())
  {
    result = new UInterpActor(exp);
  }
  else if (c == UEmitter::StaticClassName())
  {
    result = new UEmitter(exp);
  }
  else if (c == UHeightFog::StaticClassName())
  {
    result = new UHeightFog(exp);
  }
  else if (c == UPrefabInstance::StaticClassName())
  {
    result = new UPrefabInstance(exp);
  }
  else if (c == UAnimSet::StaticClassName())
  {
    result = new UAnimSet(exp);
  }
  else if (c == UAnimSequence::StaticClassName())
  {
    result = new UAnimSequence(exp);
  }
  else if (c == USoundNodeWave::StaticClassName())
  {
    result = new USoundNodeWave(exp);
  }
  else if (c == USoundNodeAmbient::StaticClassName())
  {
    result = new USoundNodeAmbient(exp);
  }
  else if (c == USoundNodeAttenuation::StaticClassName())
  {
    result = new USoundNodeAttenuation(exp);
  }
  else if (c == USoundNodeAmbientNonLoop::StaticClassName())
  {
    result = new USoundNodeAmbientNonLoop(exp);
  }
  else if (c == USoundNodeAmbientNonLoopToggle::StaticClassName())
  {
    result = new USoundNodeAmbientNonLoopToggle(exp);
  }
  else if (c == USoundNodeConcatenator::StaticClassName())
  {
    result = new USoundNodeConcatenator(exp);
  }
  else if (c == USoundNodeLooping::StaticClassName())
  {
    result = new USoundNodeLooping(exp);
  }
  else if (c == USoundNodeMixer::StaticClassName())
  {
    result = new USoundNodeMixer(exp);
  }
  else if (c == USoundNodeModulator::StaticClassName())
  {
    result = new USoundNodeModulator(exp);
  }
  else if (c == USoundNodeRandom::StaticClassName())
  {
    result = new USoundNodeRandom(exp);
  }
  else if (c == USoundCue::StaticClassName())
  {
    result = new USoundCue(exp);
  }
  else if (c == UField::StaticClassName())
  {
    result = new UField(exp);
  }
  else if (c == UStruct::StaticClassName())
  {
    result = new UStruct(exp);
  }
  else if (c == UScriptStruct::StaticClassName())
  {
    result = new UScriptStruct(exp);
  }
  else if (c == UState::StaticClassName())
  {
    result = new UState(exp);
  }
  else if (c == UEnum::StaticClassName())
  {
    result = new UEnum(exp);
  }
  else if (c == UConst::StaticClassName())
  {
    result = new UConst(exp);
  }
  else if (c == UFunction::StaticClassName())
  {
    result = new UFunction(exp);
  }
  else if (c == UTextBuffer::StaticClassName())
  {
    result = new UTextBuffer(exp);
  }
  else if (c == UIntProperty::StaticClassName())
  {
    result = new UIntProperty(exp);
  }
  else if (c == UBoolProperty::StaticClassName())
  {
    result = new UBoolProperty(exp);
  }
  else if (c == UByteProperty::StaticClassName())
  {
    result = new UByteProperty(exp);
  }
  else if (c == UFloatProperty::StaticClassName())
  {
    result = new UFloatProperty(exp);
  }
  else if (c == UObjectProperty::StaticClassName())
  {
    result = new UObjectProperty(exp);
  }
  else if (c == UClassProperty::StaticClassName())
  {
    result = new UClassProperty(exp);
  }
  else if (c == UComponentProperty::StaticClassName())
  {
    result = new UComponentProperty(exp);
  }
  else if (c == UNameProperty::StaticClassName())
  {
    result = new UNameProperty(exp);
  }
  else if (c == UStrProperty::StaticClassName())
  {
    result = new UStrProperty(exp);
  }
  else if (c == UStructProperty::StaticClassName())
  {
    result = new UStructProperty(exp);
  }
  else if (c == UArrayProperty::StaticClassName())
  {
    result = new UArrayProperty(exp);
  }
  else if (c == UMapProperty::StaticClassName())
  {
    result = new UMapProperty(exp);
  }
  else if (c == UInterfaceProperty::StaticClassName())
  {
    result = new UInterfaceProperty(exp);
  }
  else if (c == UDelegateProperty::StaticClassName())
  {
    result = new UDelegateProperty(exp);
  }
  else if (c == UMetaData::StaticClassName())
  {
    result = new UMetaData(exp);
  }
  else if (c == UObjectRedirector::StaticClassName())
  {
    result = new UObjectRedirector(exp);
  }
  else if (c == UObjectReferencer::StaticClassName())
  {
    result = new UObjectReferencer(exp);
  }
  else if (c == UPersistentCookerData::StaticClassName())
  {
    result = new UPersistentCookerData(exp);
  }
  else if (c == UDynamicSMActor::StaticClassName())
  {
    result = new UDynamicSMActor(exp);
  }
  else if (c == UComponent::StaticClassName())
  {
    result = new UComponent(exp);
  }
  else if (c == UStaticMeshComponent::StaticClassName())
  {
    result = new UStaticMeshComponent(exp);
  }
  else if (c == USkeletalMeshComponent::StaticClassName())
  {
    result = new USkeletalMeshComponent(exp);
  }
  else if (c == USpeedTreeComponent::StaticClassName())
  {
    result = new USpeedTreeComponent(exp);
  }
  else if (c == ULightComponent::StaticClassName())
  {
    result = new ULightComponent(exp);
  }
  else if (c == UPointLightComponent::StaticClassName())
  {
    result = new UPointLightComponent(exp);
  }
  else if (c == USpotLightComponent::StaticClassName())
  {
    result = new USpotLightComponent(exp);
  }
  else if (c == UDirectionalLightComponent::StaticClassName())
  {
    result = new UDirectionalLightComponent(exp);
  }
  else if (c == UDominantDirectionalLightComponent::StaticClassName())
  {
    result = new UDominantDirectionalLightComponent(exp);
  }
  else if (c == UDominantSpotLightComponent::StaticClassName())
  {
    result = new UDominantSpotLightComponent(exp);
  }
  else if (c == USkyLightComponent::StaticClassName())
  {
    result = new USkyLightComponent(exp);
  }
  else if (c == UHeightFogComponent::StaticClassName())
  {
    result = new UHeightFogComponent(exp);
  }
  else if (c == UDistributionFloatConstant::StaticClassName())
  {
    result = new UDistributionFloatConstant(exp);
  }
  else if (c == UDistributionFloatUniform::StaticClassName())
  {
    result = new UDistributionFloatUniform(exp);
  }
  else
  {
    // MaterialExpressions must be before the UComponent due to UMaterialExpressionComponentMask
    if (c.StartWith("MaterialExpression"))
    {
      result = UMaterialExpression::StaticFactory(exp);
    }
    // Fallback for unimplemented components. *Component => UComponent
    else if ((c.Find(UComponent::StaticClassName()) != std::string::npos) ||
             (c.Find("Distribution") != std::string::npos) ||
              c.StartWith("UIComp_") || c == "RB_Handle")
    {
      result = new UComponent(exp);
    }
    // Fallback for all *Actor classes except components
    else if (c.Find(NAME_Actor) != std::string::npos && c != NAME_ActorFactory)
    {
      result = new UActor(exp);
    }
    else
    {
      result = new UObject(exp);
    }
  }
  return result;
}