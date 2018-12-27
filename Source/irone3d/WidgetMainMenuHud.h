// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetMainMenuHud.generated.h"
UCLASS()
class IRONE3D_API UWidgetMainMenuHud : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void setFullscreenTextureColor(const FLinearColor& color);
};
