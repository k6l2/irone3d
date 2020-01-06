// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#ifndef TEMPLOG
#define TEMPLOG(PrintString, ...) \
	UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *FString(__FUNCTION__), \
		*FString::Printf(TEXT(PrintString), ##__VA_ARGS__));
#endif
#define ECC_PlayerPawn  ECC_GameTraceChannel1
#define ECC_EnemyPawn   ECC_GameTraceChannel2
#define ECC_EnemyVision ECC_GameTraceChannel3
#define ECC_Trigger     ECC_GameTraceChannel4
