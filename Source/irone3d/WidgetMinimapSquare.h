// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetMinimapSquare.generated.h"
UCLASS()
class IRONE3D_API UWidgetMinimapSquare : public UUserWidget
{
	GENERATED_BODY()
public:
	UWidgetMinimapSquare(const FObjectInitializer & ObjectInitializer);
	void setTileProperties(bool visited, bool playerLocation,
		bool exitN, bool exitE, bool exitS, bool exitW);
protected:
	UFUNCTION(BlueprintCallable, Category = MinimapSquare)
		void setImageWidgets(
			class UImage* playerIcon,
			class UImage* tile,
			class UImage* tileN,
			class UImage* tileE,
			class UImage* tileS,
			class UImage* tileW);
private:
	UPROPERTY()
		class UImage* imgPlayerIcon;
	UPROPERTY()
		class UImage* imgTile;
	UPROPERTY()
		class UImage* imgTileWallNorth;
	UPROPERTY()
		class UImage* imgTileWallEast;
	UPROPERTY()
		class UImage* imgTileWallSouth;
	UPROPERTY()
		class UImage* imgTileWallWest;
};
