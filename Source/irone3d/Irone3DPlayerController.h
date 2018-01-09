// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Irone3DPlayerController.generated.h"
/**
 * 
 */
UCLASS()
class IRONE3D_API AIrone3DPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    AIrone3DPlayerController();
	void transitionExitToLocation(const FVector& exitLocation);
	void transitionEnterToLocation(const FVector& enterLocation);
	virtual void Tick(float deltaSeconds) override;
	bool isAutoMoving() const;
protected:
    virtual void SetupInputComponent() override;
private:
    void jumpPressed();
    void jumpReleased();
    void attackPressed();
    void attackReleased();
    void moveForward(float value);
    void moveRight(float value);
    void turn(float value);
    void turnRate(float value);
    void lookUp(float value);
    void lookUpRate(float value);
private:
	FVector autoMoveToLocationPoint;
	FVector autoMoveToLocationRay;
	UPROPERTY(VisibleAnywhere, Category=Camera)
		// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
		float baseTurnRate;
	UPROPERTY(VisibleAnywhere, Category=Camera)
		// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
		float baseLookUpRate;
	float deadzone;
	bool autoMoveToLocation;
};
