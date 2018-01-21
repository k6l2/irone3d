// YOLO SWAG 420
#include "AiControllerCreep.h"
#include "creep.h"
#include "Irone3dGameState.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include <GameFramework/PawnMovementComponent.h>
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
    //Blackboard->SetValueAsObject("target", enemyPawn);
	aggro(enemyPawn);
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
bool AAiControllerCreep::flock()
{
	///todo: only flock with nearby creeps!
	///we leave this state and set aggrod to false at some point!
	///also move the speed change somewhere where it can make sense.
	///have this set a flocking value and then actually apply it in tick()?
	///also probably don't normalize the source vectors probably
	///you know, and do some game design on the ai stuff
	//exit conditions? nothing to flock to -> sleep, see player -> aggro

	//1. get the giant list of other creeps
	UWorld* world = GetWorld();
	if (!world)
	{
		return true; //I mean... we should stop flocking if there's no world I guess?
	}
	AIrone3dGameState* gs = world->GetGameState<AIrone3dGameState>();
	if (!gs)
	{
		return true;
	}
	TArray<TWeakObjectPtr<AActor>> actorList = gs->getCurrentRoomActorSet();
	//2. make a list of all the direction vectors of awake creeps to average them
	//because we're normalizing anyway we can get away with just adding them
	//"premature optimization is awesome" --Donald Knuth, probably
	FVector flockDirection(0.0);
	for (auto actorPtr : actorList)
	{
		if (!actorPtr.IsValid())
		{
			continue;
		}
		Acreep* cptr = Cast<Acreep>(actorPtr.Get());
		if (!cptr)
		{
			continue;
		}
		auto theirController = cptr->GetController();
		if (!theirController)
		{
			continue;
		}
		auto theirCreepController = Cast<AAiControllerCreep>(theirController);
		if (!theirCreepController)
		{
			continue;
		}
		auto theirBlackboard = theirCreepController->GetBlackboardComponent();
		if (!theirBlackboard)
		{
			continue;
		}
		if (!theirBlackboard->GetValueAsBool("aggrod"))
		{
			continue;
		}
		auto theirMovementComponent = cptr->GetMovementComponent();
		if (!theirMovementComponent)
		{
			continue;
		}
		//maybe make creeps with targets have greater weight here
		FVector theirVelocity = FVector::VectorPlaneProject(
			theirMovementComponent->Velocity,cptr->GetActorUpVector());
		theirVelocity.Normalize();
		flockDirection += theirVelocity;
	}
	if (!flockDirection.Normalize())
	{
		return true; //flockDirection is zero(ish) so, provisionally, we're done flocking
	}
	//some kind of flock power here? what's the scale of velocity anyway?
	auto const pawn = GetPawn();
	if (!pawn)
	{
		return true; //stop flocking if you don't exist I guess
	}
	auto moveComp = pawn->GetMovementComponent();
	if (!moveComp)
	{
		return true;
	}
	float scaleFactor = 1.0;
	moveComp->Velocity += flockDirection * scaleFactor;

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
	Blackboard->SetValueAsBool("aggrod", true);
	Blackboard->SetValueAsObject("target", targetObject);
	const auto targetLocation = targetObject->GetActorLocation();
	Blackboard->SetValueAsVector("locationLastKnownTarget", targetLocation);
}
void AAiControllerCreep::BeginPlay()
{
    Super::BeginPlay();
}
