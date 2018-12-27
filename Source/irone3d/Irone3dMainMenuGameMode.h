// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Irone3dMainMenuGameMode.generated.h"
UCLASS()
class IRONE3D_API AIrone3dMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AIrone3dMainMenuGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options,
		FString& ErrorMessage) override;
	void setFullscreenTextureColor(const FLinearColor& color);
	///UFUNCTION(BlueprintImplementableEvent)
	///	void setFullscreenTextureColor(const FLinearColor& color);
private:
	UPROPERTY()
		class UWidgetMainMenuHud* widgetHUD;
	UPROPERTY()
		TSubclassOf<class UWidgetMainMenuHud> classHUD;
};
