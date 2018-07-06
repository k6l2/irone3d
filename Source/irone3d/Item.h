// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"
UENUM()
enum class ItemType : int8
{
	HEART,
	SWORD,
	BOOT,
	KEY
};
USTRUCT()
struct FItemMeta
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
		ItemType type;
};
UCLASS()
class IRONE3D_API AItem : public AActor
{
	GENERATED_BODY()
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;
	void setMeta(const FItemMeta& newMeta);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Components, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* capsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Components, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* meshComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, 
		Category = ItemMeshes, meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* meshHeart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, 
		Category = ItemMeshes, meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* meshSword;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, 
		Category = ItemMeshes, meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* meshBoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, 
		Category = ItemMeshes, meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* meshKey;
	UPROPERTY(EditDefaultsOnly, Category = Meta)
		FItemMeta itemMeta;
};
