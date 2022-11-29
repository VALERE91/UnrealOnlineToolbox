// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTAchievement.generated.h"

USTRUCT(BlueprintType)
struct ONLINETOOLBOX_API FOTAchievement
{
	GENERATED_BODY()

	FString Id;

	/** The percentage of completion **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Achievements", meta=(ClampMin=0, ClampMax=100))
	double Completion;
	
	/** The localized title of the achievement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	FText Title;

	/** The localized locked description of the achievement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	FText LockedDesc;

	/** The localized unlocked description of the achievement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	FText UnlockedDesc;

	/** Flag for whether the achievement is hidden */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	bool bIsHidden;

	/** The date/time the achievement was unlocked */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Achievements")
	FDateTime UnlockTime;

	/** Returns debugging string to print out achievement info */
	FString ToDebugString() const
	{
		return FString::Printf( TEXT("Title='%s', LockedDesc='%s', UnlockedDesc='%s', bIsHidden=%s, UnlockTime=%s"),
			*Title.ToString(),
			*LockedDesc.ToString(),
			*UnlockedDesc.ToString(),
			bIsHidden ? TEXT("true") : TEXT("false"),
			*UnlockTime.ToString()
			);
	}
};
