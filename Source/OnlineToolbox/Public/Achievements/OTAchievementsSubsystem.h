// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OTAchievement.h"
#include "OTAchievementError.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OTAchievementsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOTAchievementsUpdatedDelegate,
	bool, bSuccessful,
	const TArray<FOTAchievement>&, Achievements);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTAchievementsWrittenDelegate, bool, bSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTAchievementsErrorDelegate, FOTAchievementError, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTAchievementUnlockedDelegate, FOTAchievement, Achievement);

UCLASS()
class ONLINETOOLBOX_API UOTAchievementsSubsystem : public UGameInstanceSubsystem
{ 
	GENERATED_BODY()

public:
	explicit UOTAchievementsSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Achievements")
	void UpdateAchievements();

	UFUNCTION(BlueprintCallable, Category="Achievements")
	void WriteAchievement(const FOTAchievement& Achievement);

	UFUNCTION(BlueprintCallable, Category="Achievements")
	void WriteAchievementById(const FName& AchievementID, float Completion);

	UFUNCTION(BlueprintCallable, Category="Achievements")
	void WriteAchievements(const TArray<FOTAchievement>& Achievements);

	UFUNCTION(BlueprintCallable, Category="Achievements")
	void ClearAchievements();

	UFUNCTION(BlueprintCallable, Category="Achievements")
	bool GetAchievementFromCache(const FString& AchievementId, FOTAchievement& OutAchievement) const;

	UFUNCTION(BlueprintCallable, Category="Achievements")
	bool GetAchievementsFromCache(TArray<FOTAchievement>& OutAchievements) const;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementsUpdatedDelegate OnAchievementsUpdated;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementsWrittenDelegate OnAchievementsWritten;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementsErrorDelegate OnAchievementsError;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementUnlockedDelegate OnAchievementUnlocked;
private:
	FOnQueryAchievementsCompleteDelegate QueryAchievementsCompleteDelegate;
	FOnQueryAchievementsCompleteDelegate QueryAchievementsDescCompleteDelegate;
	FOnAchievementsWrittenDelegate AchievementsWrittenDelegate;
	FOnAchievementUnlockedDelegate AchievementUnlockedDelegate;
	FDelegateHandle AchievementUnlockedDelegateHandle;

	TSharedPtr<FOnlineAchievementsWrite> WriteRequest;
	
	void HandleQueryAchievements(const FUniqueNetId& PlayerID, const bool bSuccessful) const;
	void HandleQueryAchievementsDesc(const FUniqueNetId& PlayerID, const bool bSuccessful);
	void HandleAchievementWritten(const FUniqueNetId& PlayerID, const bool bSuccessful);
	void HandleAchievementUnlocked(const FUniqueNetId& UniqueNetId, const FString& AchievementId);

	IOnlineAchievementsPtr AchievementsInterface;

	FCriticalSection WaitingAchievementsSection;
	TArray<FString> WaitingAchievementsUnlocked;
	bool bUpdatingAchievements = false;
};
