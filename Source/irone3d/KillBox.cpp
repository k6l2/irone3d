// YOLO SWAG 420
#include "KillBox.h"
#include "Classes/Components/BoxComponent.h"
#include "CombatComponent.h"
AKillBox::AKillBox()
	:componentBox(CreateDefaultSubobject<UBoxComponent>(TEXT("box")))
	,componentCombat(CreateDefaultSubobject<UCombatComponent>(TEXT("combat")))
{
	RootComponent = componentBox;
	componentCombat->SetupAttachment(RootComponent);
	componentCombat->setAlwaysAttacking(true);
	componentCombat->startAttack(9999.f);
	PrimaryActorTick.bCanEverTick = false;
}
