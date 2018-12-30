// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"
UENUM(BlueprintType)
enum class ItemType : uint8
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
	UFUNCTION(Category = Callbacks)
		void onOverlapBegin(UPrimitiveComponent * OverlappedComponent,
			AActor * OtherActor, UPrimitiveComponent * OtherComp,
			int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult & SweepResult);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Components, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* capsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Components, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* meshComponent;
	UPROPERTY(EditAnywhere, Category = Components)
		class UParticleSystemComponent* sparkleParticleComponent;
	UPROPERTY(EditAnywhere, Category = Item)
		class UParticleSystem* sparkleBurst;
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
	UPROPERTY(EditAnywhere, Category = Meta)
		FItemMeta itemMeta;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxItemPickup;
};
