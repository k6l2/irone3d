// YOLO SWAG 420
#include "Item.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
AItem::AItem()
	:capsuleComponent(CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule")))
	,meshComponent(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh")))
{
	PrimaryActorTick.bCanEverTick = true;
	capsuleComponent->SetupAttachment(RootComponent);
	meshComponent->SetupAttachment(capsuleComponent);
}
void AItem::Tick(float DeltaTime)
{
	static const float MESH_ROTATION_SPEED = 90;
	const float rotSpeed = MESH_ROTATION_SPEED*DeltaTime;
	meshComponent->AddRelativeRotation(FRotator(0.f, rotSpeed, 0.f));
	//meshComponent->SetCollisionProfileName("Default");
	Super::Tick(DeltaTime);
}
void AItem::setMeta(const FItemMeta & newMeta)
{
	itemMeta = newMeta;
	switch (itemMeta.type)
	{
	case ItemType::HEART:
		meshComponent->SetStaticMesh(meshHeart);
		break;
	case ItemType::SWORD:
		meshComponent->SetStaticMesh(meshSword);
		break;
	case ItemType::BOOT:
		meshComponent->SetStaticMesh(meshBoot);
		break;
	case ItemType::KEY:
		meshComponent->SetStaticMesh(meshKey);
		break;
	}
}
void AItem::BeginPlay()
{
	Super::BeginPlay();
	setMeta(itemMeta);
}
