// YOLO SWAG 420
#include "AiControllerCreep.h"
#include "creep.h"
#include "Irone3dGameState.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include <GameFramework/PawnMovementComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "irone3d.h"
AAiControllerCreep::AAiControllerCreep(
		const FObjectInitializer & ObjectInitializer)
    :Super(ObjectInitializer)
{
    //PrimaryActorTick.bCanEverTick = true;
    //PrimaryActorTick.bStartWithTickEnabled = true;
}
void AAiControllerCreep::Tick(float DeltaSeconds)
{
    //UE_LOG(LogTemp, Warning, TEXT("AAiControllerCreep::Tick"));
    Super::Tick(DeltaSeconds);
    APawn*const pawn = GetPawn();
    if (!Blackboard || !pawn)
    {
        return;
    }
	Acreep*const creep = Cast<Acreep>(pawn);
    if (!creep)
    {
        return;
    }
    UObject*const target = Blackboard->GetValueAsObject("target");
    if (target)
    {
		AActor*const targetActor = Cast<AActor>(target);
		if (creep->getPushAwaySeconds() > 0)
		{
			const FVector toTargetActor =
				(targetActor->GetActorLocation() - creep->GetActorLocation()).
					GetSafeNormal2D();
			UPawnMovementComponent*const moveComp =
				creep->GetMovementComponent();
			if (moveComp)
			{
				creep->AddMovementInput(toTargetActor);
			}
		}
        if (targetActor)
        {
            const auto targetLocation = targetActor->GetActorLocation();
            if (creep->canSee(targetActor))
            {
				Blackboard->SetValueAsFloat("persistSeconds", 2.f);
				Blackboard->SetValueAsVector(
					"locationLastKnownTarget", targetLocation);
            }
            else
            {
				TEMPLOG("Lost vision of targetActor!!!");
                Blackboard->SetValueAsObject("target", nullptr);
				StopMovement();
            }
        }
        else
        {
			//TEMPLOG("target is not an actor?? Nullifying...");
            Blackboard->SetValueAsObject("target", nullptr);
        }
    }
	else
	{
		const float persistSeconds =
			Blackboard->GetValueAsFloat("persistSeconds");
		if (persistSeconds > 0)
		{
			Blackboard->SetValueAsFloat("persistSeconds", 
				persistSeconds - DeltaSeconds);
		}
		else
		{
			Blackboard->SetValueAsBool("aggrod", false);
		}
	}
}
void AAiControllerCreep::OnPossess(APawn * InPawn)
{
    Super::OnPossess(InPawn);
    Acreep const*const creep = Cast<Acreep>(InPawn);
    if (!creep)
    {
        return;
    }
    if (bbDataAsset)
    {
        // UseBlackboard creates a new Blackboard object
        UseBlackboard(bbDataAsset, Blackboard);
        if (Blackboard)
        {
            Blackboard->SetValueAsVector(
				"locationHome", InPawn->GetActorLocation());
        }
    }
    if (behaviorTree)
    {
        RunBehaviorTree(behaviorTree);
    }
}
void AAiControllerCreep::onSeeEnemyPawn(APawn * enemyPawn)
{
    if (!Blackboard)
    {
        return;
    }
    //UE_LOG(LogTemp, Warning, TEXT("I see you!!!"));
    //Blackboard->SetValueAsObject("target", enemyPawn);
	aggro(enemyPawn);
}
bool AAiControllerCreep::moveToTargetActor()
{
	///TODO: debug program and step through this function when I see a creep who is
	/// just frozen for no reason, because apparently that can happen sometimes
    if (!Blackboard)
    {
		///UE_LOG(LogTemp, Warning, TEXT("NO BB~"));
        return false;
    }
    auto const target = Blackboard->GetValueAsObject("target");
    if (!target)
    {
        // if we have no target, then we should stop execution
        //  of this behavior by returning true!
		///UE_LOG(LogTemp, Warning, TEXT("NO target"));
		return true;
    }
    auto const targetActor = Cast<AActor>(target);
    if (!targetActor)
    {
		///UE_LOG(LogTemp, Warning, TEXT("NO targetActor"));
		return false;
    }
	Acreep*const creep = Cast<Acreep>(GetPawn());
	///UE_LOG(LogTemp, Warning, TEXT("GUCCI??? targetActor?=%s creep?=%s"), 
	///	targetActor?TEXT("yep"):TEXT("no"), creep ? TEXT("yep") : TEXT("no"));
	///if (creep)
	///{
	///	UE_LOG(LogTemp, Warning, TEXT("creep->getPushAwaySeconds()=%f"),
	///		creep->getPushAwaySeconds());
	///}
	if (creep && creep->getPushAwaySeconds() > 0)
	{
		/// const FVector toTargetActor = 
		/// 	(targetActor->GetActorLocation() - creep->GetActorLocation()).
		/// 		GetSafeNormal2D();
		/// UPawnMovementComponent *const moveComp = 
		/// 	creep->GetMovementComponent();
		/// if (moveComp)
		/// {
		/// 	//if (FVector::DotProduct(moveComp->Velocity, toTargetActor) >= 0)
		/// 	// {
		/// 	// 	moveComp->Velocity = -toTargetActor * 1000;
		/// 	// }
		/// 	///creep->AddMovementInput(toTargetActor);
		/// 	//moveComp->AddInputVector(toTargetActor);
		/// 	//UE_LOG(LogTemp, Warning, TEXT("MOVE AWAY!!!!!!!!!!"));
		/// }
		return false;
	}
    auto result = MoveToActor(targetActor,10);
    switch (result)
    {
    case EPathFollowingRequestResult::AlreadyAtGoal:
		TEMPLOG("At goal!");
		return true;
    case EPathFollowingRequestResult::RequestSuccessful:
		TEMPLOG("SUCCESS? targetActorLocation=%s", 
			*targetActor->GetActorLocation().ToString());
		break;
    case EPathFollowingRequestResult::Failed:
		TEMPLOG("Failed");
        break;
    }
    return false;
}
void AAiControllerCreep::aggro(AActor * targetObject)
{
	Acreep*const creep = Cast<Acreep>(GetPawn());
	if (!Blackboard || !ensure(creep))
	{
		return;
	}
	const bool wasAggro = isAggrod();
	auto target = Blackboard->GetValueAsObject("target");
	if (target)
	{
		return;
	}
	Blackboard->SetValueAsBool("aggrod", true);
	Blackboard->SetValueAsObject("target", targetObject);
	const auto targetLocation = targetObject->GetActorLocation();
	Blackboard->SetValueAsVector("locationLastKnownTarget", targetLocation);
	if (!wasAggro)
	{
		// stop previous movement in case we are in the middle of moving to our
		//	home location or something
		StopMovement();
		TArray<AActor*> overlappingAggroActors;
		creep->getOverlappingAggroActors(overlappingAggroActors);
		for (auto actor : overlappingAggroActors)
		{
			Acreep*const aggroCreep = Cast<Acreep>(actor);
			if (!aggroCreep || !aggroCreep->Controller)
			{
				continue;
			}
			AAiControllerCreep*const aggroCreepController =
				Cast<AAiControllerCreep>(aggroCreep->Controller);
			if (!aggroCreepController)
			{
				continue;
			}
			if (!aggroCreepController->isAggrod())
			{
				aggroCreepController->aggro(targetObject);
			}
		}
	}
}
bool AAiControllerCreep::isAggrod() const
{
	if (!Blackboard)
	{
		return false;
	}
	return Blackboard->GetValueAsBool("aggrod");
}
void AAiControllerCreep::BeginPlay()
{
    Super::BeginPlay();
}
