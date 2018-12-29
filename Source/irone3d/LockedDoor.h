// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LockedDoor.generated.h"
UCLASS()
class IRONE3D_API ALockedDoor : public AActor
{
	GENERATED_BODY()
public:	
	ALockedDoor();
///	virtual void Tick(float DeltaTime) override;
	void unlock();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = LockedDoor)
		UParticleSystem* particleSystemUnlock;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxDestroyed;
};
