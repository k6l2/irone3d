// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "Chest.generated.h"
UCLASS()
class IRONE3D_API AChest : public AActor
{
	GENERATED_BODY()
public:	
	AChest();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = Components, 
			meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* meshComponent;
	UPROPERTY(EditInstanceOnly, Category = ChestContents, 
			meta = (AllowPrivateAccess = "true"))
		FItemMeta loot;
	UPROPERTY(EditInstanceOnly, Category = ChestContents, 
			meta = (AllowPrivateAccess = "true"))
		bool randomLoot = true;
	UPROPERTY(EditDefaultsOnly, Category = ChestContents)
		TSubclassOf<class AActor> bpItemClass;
};
