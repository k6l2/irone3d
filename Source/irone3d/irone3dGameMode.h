#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "irone3dGameMode.generated.h"
UCLASS()
class IRONE3D_API Airone3dGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	Airone3dGameMode();
	void InitGame(const FString& MapName, const FString& Options, 
		FString& ErrorMessage) override;
protected:
	// The following retarded function literally just calls the CreateLevelInstance
	//	function inside blueprints because it's not exposed in the C++ API...
	UFUNCTION(BlueprintImplementableEvent)
	void createLevelInstance(
		FName originalPackageName, 
		const FString& uniqueInstanceName);
};
