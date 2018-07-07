#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "irone3dGameMode.generated.h"
UCLASS()
class IRONE3D_API Airone3dGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	static const FVector TRANSITION_CAM_OFFSET;
	static const float LEVEL_TRANSITION_FADE_TIME;
	enum class TransitionDirection : uint8
	{
		NORTH,
		EAST,
		SOUTH,
		WEST
	};
public:
	Airone3dGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, 
		FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void StartPlay() override;
	virtual void Tick(float deltaSeconds) override;
	void startRoomTransition(class ARoomTransitionTrigger* trigger);
	// The following retarded function literally just calls the CreateLevelInstance
	//	function inside blueprints because it's not exposed in the C++ API...
	UFUNCTION(BlueprintImplementableEvent)
		void createLevelInstance(
			FName originalPackageName, 
			const FString& uniqueInstanceName);
	UFUNCTION(BlueprintImplementableEvent)
		void setFullscreenTextureColor(const FLinearColor& color);
	UFUNCTION(BlueprintImplementableEvent)
		void updateMinimapWidget(const ULevelMap* const levelMap);
	void fadeOut(const FLinearColor& fadeColor, float fadeTime);
	void fadeIn(const FLinearColor& fadeColor, float fadeTime);
	// this should only get called by AIrone3DPlayer when 
	//	they finish auto-moving inside the next room!
	void onEndTransition();
protected:
	virtual void BeginPlay() override;
private:
	void fade(bool in, const FLinearColor& fadeColor, float fadeTime);
	UFUNCTION(BlueprintCallable, Category = Transitions)
		void onLoadStreamLevelFinished();
private:
	UPROPERTY(Category = Widgets, EditDefaultsOnly,
		BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> classHUD;
	UPROPERTY(Category = Widgets, VisibleAnywhere,
		BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UUserWidget* widgetHUD;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		ACameraActor* transitionCamera;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		FString strTransitionLevelCameFrom;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		FString strTransitionLevelGoingTo;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		FVector exitVec;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		FLinearColor colorFullScreenTexture;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		float fadeTime;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		float fadeTimeTotal;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		bool fadingIn;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		bool transitioning;
	UPROPERTY(VisibleAnywhere, Category = Transitions)
		bool justLoadedRoom;
};
