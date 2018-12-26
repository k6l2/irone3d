// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"
UCLASS()
class IRONE3D_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void Tick(float deltaSeconds) override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:
	void onAttackPressed();
private:
	UPROPERTY(EditAnywhere, Category = MainMenuController)
		bool playerWokenUp = false;
	UPROPERTY(EditAnywhere, Category = MainMenuController)
		float wakeUpDelaySeconds = 1.5f;
	UPROPERTY(EditAnywhere, Category = MainMenuController)
		float playerRunSeconds = 1.5f;
	UPROPERTY(EditAnywhere, Category = MainMenuController)
		float fadeoutSecondsTotal = 3.f;
	UPROPERTY(EditAnywhere, Category = MainMenuController)
		float fadeoutSeconds;
};
