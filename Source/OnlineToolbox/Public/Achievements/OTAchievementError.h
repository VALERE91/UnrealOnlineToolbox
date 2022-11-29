// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OTAchievementError.generated.h"

UENUM(BlueprintType)
enum class EOTAchievementErrorCode : uint8
{
	DecreasedCompletion,
	NotFound,
	Unknown
};

USTRUCT(BlueprintType)
struct ONLINETOOLBOX_API FOTAchievementError
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	EOTAchievementErrorCode ErrorCode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	FString ErrorMessage;
};
