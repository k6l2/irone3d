// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetMinimap.generated.h"
UCLASS()
class IRONE3D_API UWidgetMinimap : public UUserWidget
{
	GENERATED_BODY()
public:
	UWidgetMinimap(const FObjectInitializer & ObjectInitializer);
	UFUNCTION(BlueprintCallable, Category = Minimap)
		void updateMinimap(const class ULevelMap* const levelMap);
protected:
	UFUNCTION(BlueprintCallable, Category = Minimap)
		void setGridPanel(class UUniformGridPanel* gp);
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		Category = BreakableFloor, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UWidgetMinimapSquare> minimapSquareWidgetClass;
	UPROPERTY()
		class UUniformGridPanel* gridPanel;
	UPROPERTY()
		TArray<class UWidgetMinimapSquare*> gridSquares;
};
