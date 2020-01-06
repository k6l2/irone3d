// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "AiControllerCreep.generated.h"
UCLASS()
class IRONE3D_API AAiControllerCreep : public AAIController
{
	GENERATED_BODY()
public:
    AAiControllerCreep(const FObjectInitializer & ObjectInitializer);
    virtual void Tick(float DeltaSeconds) override;
    void onSeeEnemyPawn(APawn* enemyPawn);
    UFUNCTION(BlueprintCallable, Category = BehaviorTree)
		bool moveToTargetActor();
	void aggro(AActor* targetObject);
	bool isAggrod() const;
protected:
	virtual void OnPossess(APawn* InPawn) override;
    virtual void BeginPlay() override;
private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Misc, 
			meta = (AllowPrivateAccess = "true"))
		UBlackboardData* bbDataAsset;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Misc, 
			meta = (AllowPrivateAccess = "true"))
		UBehaviorTree* behaviorTree;
};
