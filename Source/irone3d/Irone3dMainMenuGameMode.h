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
	UFUNCTION(BlueprintImplementableEvent)
		void setFullscreenTextureColor(const FLinearColor& color);
};
