// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionTrigger.generated.h"
UCLASS()
class IRONE3D_API ALevelTransitionTrigger : public AActor
{
	GENERATED_BODY()
public:	
	ALevelTransitionTrigger();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
		void onBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, 
			const FHitResult & SweepResult);
private:
	UPROPERTY(EditDefaultsOnly, Category = Components)
		class UBoxComponent* componentBox;
};
