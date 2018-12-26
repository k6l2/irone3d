// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainMenuCameraPawn.generated.h"
UCLASS()
class IRONE3D_API AMainMenuCameraPawn : public APawn
{
	GENERATED_BODY()
public:
	AMainMenuCameraPawn();
	virtual void Tick(float DeltaTime) override;
	///virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	class AIrone3DPlayer* getPlayerActor() const;
	void yawToPlayerActor();
	void moveToPanTarget1();
	void moveToPanTarget2();
	void pitchStraightDown(float deltaSeconds);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
		class UCameraComponent* componentCamera;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		class AIrone3DPlayer* playerActor;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		class ATargetPoint* panTarget1;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		class ATargetPoint* panTarget2;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		FVector velocity = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		float toTargetAcceleration = 1000;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		float toTargetSpeedMax = 1000;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		float toTargetDecelDistance = 100;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		float pitchSpeedMax = 180;
	UPROPERTY(EditAnywhere, Category = MainMenuCameraPawn)
		float pitchDecelDistance = 45;
	class ATargetPoint* currTargetPoint = nullptr;
};
