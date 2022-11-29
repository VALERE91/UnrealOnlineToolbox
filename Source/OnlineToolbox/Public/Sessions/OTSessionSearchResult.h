// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OTSessionSearchResult.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct ONLINETOOLBOX_API FOTSessionSearchResult
{
	GENERATED_BODY()

	FOnlineSession Session;

	int32 PingInMs;

	FOnlineSessionSearchResult Native;
};
