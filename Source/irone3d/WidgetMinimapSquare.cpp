// YOLO SWAG 420
#include "WidgetMinimapSquare.h"
#include <Runtime/UMG/Public/Components/Image.h>
UWidgetMinimapSquare::UWidgetMinimapSquare(
	const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
	, imgPlayerIcon   (nullptr)
	, imgTile         (nullptr)
	, imgTileWallNorth(nullptr)
	, imgTileWallEast (nullptr)
	, imgTileWallSouth(nullptr)
	, imgTileWallWest (nullptr)
{
}
void UWidgetMinimapSquare::setTileProperties(bool visited, bool playerLocation,
	bool exitN, bool exitE, bool exitS, bool exitW)
{
	imgTile->SetVisibility(ESlateVisibility::Visible);
	imgPlayerIcon->SetVisibility(
		playerLocation ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	//	if the square is visited, draw the tile and only the closed walls where
	//		there is no exit in that given exitVec direction
	if (visited)
	{
		const FLinearColor color{ 1.f, 1.f, 1.f };
		imgTile         ->SetColorAndOpacity(color);
		imgTileWallNorth->SetColorAndOpacity(color);
		imgTileWallEast ->SetColorAndOpacity(color);
		imgTileWallSouth->SetColorAndOpacity(color);
		imgTileWallWest ->SetColorAndOpacity(color);
		imgTileWallNorth->SetVisibility(
			!exitN ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		imgTileWallEast ->SetVisibility(
			!exitE ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		imgTileWallSouth->SetVisibility(
			!exitS ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		imgTileWallWest ->SetVisibility(
			!exitW ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	//	otherwise, draw the tile and all 4 closed
	//		walls in a dark color
	else
	{
		imgTileWallNorth->SetVisibility(ESlateVisibility::Visible);
		imgTileWallEast ->SetVisibility(ESlateVisibility::Visible);
		imgTileWallSouth->SetVisibility(ESlateVisibility::Visible);
		imgTileWallWest ->SetVisibility(ESlateVisibility::Visible);
		const FLinearColor color{ .2f, .2f, .2f };
		imgTile         ->SetColorAndOpacity(color);
		imgTileWallNorth->SetColorAndOpacity(color);
		imgTileWallEast ->SetColorAndOpacity(color);
		imgTileWallSouth->SetColorAndOpacity(color);
		imgTileWallWest ->SetColorAndOpacity(color);
	}
}
void UWidgetMinimapSquare::setImageWidgets(
	UImage* playerIcon,
	UImage* tile,
	UImage* tileN,
	UImage* tileE,
	UImage* tileS,
	UImage* tileW)
{
	imgPlayerIcon    = playerIcon;
	imgTile          = tile;
	imgTileWallNorth = tileN;
	imgTileWallEast  = tileE;
	imgTileWallSouth = tileS;
	imgTileWallWest  = tileW;
}
