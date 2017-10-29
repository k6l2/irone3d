// YOLO SWAG 420
#include "AiControllerCreep.h"
#include "creep.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
AAiControllerCreep::AAiControllerCreep(const FObjectInitializer & ObjectInitializer)
    :Super(ObjectInitializer)
{
    //PrimaryActorTick.bCanEverTick = true;
    //PrimaryActorTick.bStartWithTickEnabled = true;
}
void AAiControllerCreep::Tick(float DeltaSeconds)
{
    //UE_LOG(LogTemp, Warning, TEXT("AAiControllerCreep::Tick"));
    Super::Tick(DeltaSeconds);
    auto const pawn = GetPawn();
    if (!Blackboard || !pawn)
    {
        return;
    }
    auto const creep = Cast<Acreep>(pawn);
    if (!creep)
    {
        return;
    }
    auto target = Blackboard->GetValueAsObject("target");
    if (target)
    {
        auto targetActor = Cast<AActor>(target);
        if (targetActor)
        {
            const auto targetLocation = targetActor->GetActorLocation();
            if (creep->canSee(targetActor))
            {
                Blackboard->SetValueAsVector("locationLastKnownTarget", targetLocation);
            }
            else
            {
                Blackboard->SetValueAsObject("target", nullptr);
            }
        }
        else
        {
            Blackboard->SetValueAsObject("target", nullptr);
        }
    }
}
void AAiControllerCreep::Possess(APawn * InPawn)
{
    Super::Possess(InPawn);
    Acreep* creep = Cast<Acreep>(InPawn);
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
            Blackboard->SetValueAsVector("locationHome", InPawn->GetActorLocation());
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
    Blackboard->SetValueAsObject("target", enemyPawn);
}
bool AAiControllerCreep::moveToTargetActor()
{
	///TODO: debug program and step through this function when I see a creep who is
	/// just frozen for no reason, because apparently that can happen sometimes
    if (!Blackboard)
    {
        return false;
    }
    auto const target = Blackboard->GetValueAsObject("target");
    if (!target)
    {
        // if we have no target, then we should stop execution
        //  of this behavior by returning true!
        return true;
    }
    auto const targetActor = Cast<AActor>(target);
    if (!targetActor)
    {
        return false;
    }
    auto result = MoveToActor(targetActor);
    switch (result)
    {
    case EPathFollowingRequestResult::AlreadyAtGoal:
    case EPathFollowingRequestResult::RequestSuccessful:
        return true;
    case EPathFollowingRequestResult::Failed:
		//UE_LOG(LogTemp, Warning, TEXT("Failed"));
        break;
    }
    return false;
}
void AAiControllerCreep::aggro(AActor * targetObject)
{
	if (!Blackboard)
	{
		return;
	}
	auto target = Blackboard->GetValueAsObject("target");
	if (target)
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("WUT WUT IN THE BUTT"));
	Blackboard->SetValueAsObject("target", targetObject);
	const auto targetLocation = targetObject->GetActorLocation();
	Blackboard->SetValueAsVector("locationLastKnownTarget", targetLocation);
}
void AAiControllerCreep::BeginPlay()
{
    Super::BeginPlay();
}
