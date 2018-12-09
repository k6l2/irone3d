// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillBox.generated.h"
UCLASS()
class IRONE3D_API AKillBox : public AActor
{
	GENERATED_BODY()
public:	
	AKillBox();
private:
	UPROPERTY()
		class UBoxComponent* componentBox;
	UPROPERTY()
		class UCombatComponent* componentCombat;
};
