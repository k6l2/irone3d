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
	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(VisibleAnywhere, Category=Camera)
	float baseTurnRate;
	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(VisibleAnywhere, Category=Camera)
	float baseLookUpRate;
	float deadzone;
};
