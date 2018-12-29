// YOLO SWAG 420
#include "Irone3DPlayer.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Runtime/Engine/Classes/Components/StaticMeshComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h>
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <Runtime/Engine/Classes/Camera/CameraActor.h>
#include <EngineGlobals.h>
#include "CombatComponent.h"
#include "Irone3DPlayer.h"
#include "Irone3DPlayerController.h"
#include "Inventory.h"
#include "Irone3dGameState.h"
#include "irone3dGameMode.h"
#include "UnitComponent.h"
#include "Irone3dGameInstance.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Particles/ParticleSystem.h>
#include <Runtime/Engine/Classes/Particles/ParticleSystemComponent.h>
#include <Sound/SoundCue.h>
#include "LockedDoor.h"
const FLinearColor AIrone3DPlayer::HURT_OUTLINE_COLOR = { 0.f,0.f,0.f,1.f };
AIrone3DPlayer::AIrone3DPlayer()
    :cameraBoom           (CreateDefaultSubobject<USpringArmComponent     >(TEXT("CameraBoom")))
    ,camera               (CreateDefaultSubobject<UCameraComponent        >(TEXT("FollowCamera")))
    ,meshCharacter        (CreateDefaultSubobject<USkeletalMeshComponent  >(TEXT("MeshCharacter")))
    ,meshAttack           (CreateDefaultSubobject<UStaticMeshComponent    >(TEXT("MeshAttack")))
    ,attackCombatComponent(CreateDefaultSubobject<UCombatComponent        >(TEXT("AttackCombat")))
    ,unitComponent        (CreateDefaultSubobject<UUnitComponent          >(TEXT("Unit")))
    ,dashParticleComponent(CreateDefaultSubobject<UParticleSystemComponent>(TEXT("dashParticles")))
    ,hardLanding(false)
    ,canAttack(false)
    ,tryingAttack(false)
    ,attackAnimationPlaying(false)
    ,attackAnimationProgress(0.5f)
{
	PrimaryActorTick.bCanEverTick = true;
    cameraBoom->SetupAttachment(RootComponent);
    camera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	meshCharacter->SetupAttachment(RootComponent);
    meshAttack->SetupAttachment(RootComponent);
	attackCombatComponent->SetupAttachment(meshAttack);
}
void AIrone3DPlayer::moveForward(float value)
{
	if (!Controller || value == 0.0f || hardLanding)
    {
        return;
    }
	auto pController = Cast<AIrone3DPlayerController>(Controller);
	if (pController && pController->isAutoMoving())
	{
		AddMovementInput(GetActorRotation().Vector());
		return;
	}
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    // get forward vector
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(Direction, value);
}
void AIrone3DPlayer::moveRight(float value)
{
    if (!Controller || value == 0.0f || hardLanding)
    {
        return;
    }
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    // get forward vector
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(Direction, value);
}
void AIrone3DPlayer::turnRate(float value)
{
	UWorld const*const world = GetWorld();
	ensure(world);
    AddControllerYawInput(value * world->GetDeltaSeconds());
}
void AIrone3DPlayer::lookUpRate(float value)
{
    UWorld const*const world = GetWorld();
	ensure(world);
    AddControllerPitchInput(value * world->GetDeltaSeconds());
}
float AIrone3DPlayer::lateralSpeed() const
{
    auto movementComp = GetMovementComponent();
    ensure(movementComp);
    const float scalarProjection = FVector::DotProduct(
		movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    const FVector lateralVelocity = movementComp->Velocity - upVelocity;
    return lateralVelocity.Size();
}
float AIrone3DPlayer::verticalSpeed() const
{
    auto movementComp = GetMovementComponent();
    ensure(movementComp);
    const float scalarProjection = FVector::DotProduct(
		movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    return upVelocity.Size();
}
float AIrone3DPlayer::verticalVelocity1D() const
{
    auto movementComp = GetMovementComponent();
    ensure(movementComp);
    const float scalarProjection = FVector::DotProduct(
		movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    const float upDotUpVelocity = FVector::DotProduct(
		upVelocity, GetActorUpVector());
    return upVelocity.Size()*(upDotUpVelocity<0 ? -1:1);
}
bool AIrone3DPlayer::isFalling() const
{
	if (Controller)
	{
		auto pController = Cast<AIrone3DPlayerController>(Controller);
		if (pController && pController->isAutoMoving())
		{
			return false;
		}
	}
    auto movementComp = GetMovementComponent();
    ensure(movementComp);
    return movementComp->IsFalling();
}
void AIrone3DPlayer::hardLandingStart()
{
    hardLanding = true;
}
void AIrone3DPlayer::hardLandingEnd()
{
    hardLanding = false;
}
void AIrone3DPlayer::canAttackStart()
{
    canAttack = true;
}
void AIrone3DPlayer::canAttackEnd()
{
    canAttack = false;
}
bool AIrone3DPlayer::isSleeping() const
{
	return sleeping;
}
void AIrone3DPlayer::setSleeping(bool value)
{
	sleeping = value;
}
void AIrone3DPlayer::tryAttackStart()
{
    tryingAttack = true;
}
void AIrone3DPlayer::tryAttackEnd()
{
    tryingAttack = false;
}
void AIrone3DPlayer::dashStart()
{
	dashing = true;
	dashParticleComponent->Activate();
}
void AIrone3DPlayer::dashEnd()
{
	dashing = false;
	dashParticleComponent->Deactivate();
}
bool AIrone3DPlayer::isTryingToAttack() const
{
	//UE_LOG(LogTemp, Warning,
	//	TEXT("tryingAttack=%s"),
	//	(tryingAttack ? "true":"false"))
    return tryingAttack;
}
void AIrone3DPlayer::attackStart()
{
	UWorld*const world = GetWorld();
	if (world)
	{
		UGameplayStatics::PlaySoundAtLocation(world, sfxAttack, GetActorLocation());
	}
    attackAnimationPlaying = true;
	attackCombatComponent->startAttack();
}
void AIrone3DPlayer::attackEnd()
{
    attackAnimationPlaying = false;
	attackCombatComponent->stopAttack();
}
void AIrone3DPlayer::updateAttackAnimationProgress(float value)
{
    attackAnimationProgress = value;
}
bool AIrone3DPlayer::isKilled() const
{
	UWorld* const world = GetWorld();
	if (!ensure(world))
	{
		return false;
	}
	AIrone3dGameState const*const gs =
		world->GetGameState<AIrone3dGameState>();
	if (!ensure(gs))
	{
		return false;
	}
	return gs->getInventory()->getItemCount(ItemType::HEART) < 1;
}
void AIrone3DPlayer::copyCameraPropertiesTo(ACameraActor*const otherCam) const
{
	otherCam->SetActorLocation(camera->GetComponentLocation());
	otherCam->SetActorRotation(camera->GetForwardVector().Rotation());
}
void AIrone3DPlayer::cutSceneStopVelocity()
{
	auto movementComp = GetMovementComponent();
	ensure(movementComp);
	movementComp->Velocity *= 0.9f;
}
void AIrone3DPlayer::setInvulnerable(bool value)
{
	if (!ensure(unitComponent)) return;
	unitComponent->setInvulnerable(value);
}
void AIrone3DPlayer::BeginPlay()
{
	Super::BeginPlay();
	dashParticleComponent->SetRelativeLocation(FVector::ZeroVector);
	FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, true);
	dashParticleComponent->AttachToComponent(RootComponent, attachRules);
	dashParticleComponent->Deactivate();
	unitComponent->addVulnerablePrimitiveComponent(GetCapsuleComponent());
	if (!GetCapsuleComponent()->OnComponentHit.IsAlreadyBound(
		this, &AIrone3DPlayer::onCapsuleHit))
	{
		GetCapsuleComponent()->OnComponentHit.AddDynamic(
			this, &AIrone3DPlayer::onCapsuleHit);
	}
    dynMaterialSlash = 
		meshAttack->CreateAndSetMaterialInstanceDynamicFromMaterial(
			0, meshAttack->GetMaterial(0));
	const int32 outlineMaterialIndex = 
		meshCharacter->GetMaterialIndex("MaterialOutline");
	const int32 eyesMaterialIndex = 
		meshCharacter->GetMaterialIndex("MaterialEyes");
	outlineMaterial =
		meshCharacter->CreateAndSetMaterialInstanceDynamic(
			outlineMaterialIndex);
	eyesMaterial =
		meshCharacter->CreateAndSetMaterialInstanceDynamic(eyesMaterialIndex);
	FMaterialParameterInfo outlineColorInfo;
	outlineColorInfo.Name = "color";
	outlineMaterial->GetVectorParameterValue(
		outlineColorInfo, defaultOutlineColor);
}
void AIrone3DPlayer::onCapsuleHit(UPrimitiveComponent* HitComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	UWorld* const world = GetWorld();
	ensure(world);
	AIrone3dGameState const*const gs = 
		world->GetGameState<AIrone3dGameState>();
	if (!ensure(gs))
	{
		return;
	}
	ALockedDoor*const lockedDoor = Cast<ALockedDoor>(OtherActor);
	if (lockedDoor &&
		gs->getInventory()->removeItem(ItemType::KEY))
	{
		lockedDoor->unlock();
	}
}
///void AIrone3DPlayer::onAttackOverlap(UPrimitiveComponent* OverlappedComponent,
///	AActor* OtherActor, UPrimitiveComponent* OtherComp,
///	int32 OtherBodyIndex, bool bFromSweep,
///	const FHitResult & SweepResult)
///{
///	// if the attack is active and it collides with a boss orb, reflect it //
///}
void AIrone3DPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UWorld const*const world = GetWorld();
	ensure(world);
	Airone3dGameMode*const gm = 
		Cast<Airone3dGameMode>(world->GetAuthGameMode());
	ensure(gm);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	UIrone3dGameInstance*const gi =
		Cast<UIrone3dGameInstance>(GetGameInstance());
	ensure(gi);
    if (attackAnimationPlaying)
    {
        dynMaterialSlash->SetScalarParameterValue(
			"swipeOffset", attackAnimationProgress);
    }
    else
    {
        dynMaterialSlash->SetScalarParameterValue(
			"swipeOffset", 0.f);
    }
	// hurt effects //
	{
		///const float hurtPercent =
		///	FMath::Clamp(hurtFlashSeconds / HURT_FLASH_SECONDS, 0.f, 1.f);
		if (hurtFlashSeconds)
		{
			///UE_LOG(LogTemp, Warning, TEXT("hurtFlashSeconds=%f"), hurtFlashSeconds);
			hurtFlashSeconds -= DeltaTime;
		}
		///const float colorLerpProgress = FMath::Pow(hurtPercent, 2);
		const float colorLerp = hurtFlashSeconds > 0 ?
			FMath::Abs(FMath::Sin(hurtFlashSeconds*10)) : 0.f;
		const FLinearColor outlineColor = FLinearColor::LerpUsingHSV(
			defaultOutlineColor, HURT_OUTLINE_COLOR, colorLerp);
		outlineMaterial->SetVectorParameterValue("color", outlineColor);
		eyesMaterial->SetVectorParameterValue("color", outlineColor);
	}
	///UE_LOG(LogTemp, Warning, TEXT("WTF M8???"));
	if (gm)
	{
		gm->updateCompassWidget(camera->GetForwardVector());
	}
	if (gs)
	{
		const int32 numHeartsInInventory =
			gs->getInventory()->getItemCount(ItemType::HEART);
		unitComponent->setHitpoints(numHeartsInInventory);
	}
	UCharacterMovementComponent*const moveComp = 
		Cast< UCharacterMovementComponent>(GetMovementComponent());
	if (moveComp)
	{
		moveComp->MaxWalkSpeed = dashing ? 1000.f : 600.f;
	}
}
void AIrone3DPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
float AIrone3DPlayer::TakeDamage(float DamageAmount,
	FDamageEvent const& DamageEvent, AController * EventInstigator,
	AActor * DamageCauser)
{
	UWorld const*const world = GetWorld();
	ensure(world);
	if (world)
	{
		UGameplayStatics::PlaySoundAtLocation(world, sfxHit, GetActorLocation());
	}
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	if (world)
	{
		auto particleComp = UGameplayStatics::SpawnEmitterAtLocation(world,
			particleSystemBlood, GetActorLocation());
		///particleComp->SetVectorParameter("color", FVector{ 1.f,0.f,0.f });
	}
	// remove HEART items from inventory to the amount of damage //
	for (float dmg = 0.f; dmg < DamageAmount; dmg += 1.f)
	{
		if (!gs->getInventory()->removeItem(ItemType::HEART))
		{
			break;
		}
	}
	if (!isKilled())
	{
		hurtFlashSeconds = unitComponent->getGlobalHitInvincibleDuration();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator,
		DamageCauser);
}
