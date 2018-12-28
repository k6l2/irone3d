// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableFloor.generated.h"
UCLASS()
class IRONE3D_API ABreakableFloor : public AActor
{
	GENERATED_BODY()
public:	
	ABreakableFloor();
	virtual void Tick(float DeltaTime) override;
	void trigger();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* boxComponentRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = BreakableFloor, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* decalComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
		Category = BreakableFloor, meta = (AllowPrivateAccess = "true"))
		class AStaticMeshActor* floorActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = BreakableFloor, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ADestructibleActor> destructibleTileClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
		Category = BreakableFloor, meta = (AllowPrivateAccess = "true"))
		// when integrity reaches <= 0.f, the floor trap is triggered
		//	and we destroy the static mesh actor we're on top of
		float integritySeconds;
	UPROPERTY(VisibleAnywhere, Category = BreakableFloor)
		class UMaterialInstanceDynamic* decalMaterial = nullptr;
	UPROPERTY(VisibleAnywhere, Category = BreakableFloor)
		int32 frameParamIndex;
	UPROPERTY(EditAnywhere, Category = BreakableFloor)
		bool manualTrigger = false;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxCrack;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxDestroyed;
	UPROPERTY(VisibleAnywhere, Category = BreakableFloor)
		float prevFrame = 0;
};
