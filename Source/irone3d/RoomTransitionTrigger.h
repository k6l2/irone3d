// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomTransitionTrigger.generated.h"
UCLASS()
class IRONE3D_API ARoomTransitionTrigger : public AActor
{
	GENERATED_BODY()
public:
	ARoomTransitionTrigger();
	FVector getExitVector() const;
	FVector getBottomLocation() const;
private:
	UFUNCTION()
		void onOverlapBegin(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	UPROPERTY(VisibleAnywhere, Category = Collision)
		class UBoxComponent* boxComponent;
	UPROPERTY(VisibleAnywhere, Category = Collision)
		class UArrowComponent* exitDirection;
};
