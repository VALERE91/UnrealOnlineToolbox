// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OTIdentitySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOTLoginCompleteDelegate, bool, bWasSuccessful, const FString, UserId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTLogoutCompleteDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTLoginChangeDelegate, bool, bWasSuccessful);

UCLASS()
class ONLINETOOLBOX_API UOTIdentitySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	explicit UOTIdentitySubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category="Identity")
	void Login();

	UFUNCTION(BlueprintCallable, Category="Identity")
	void Logout();

	UPROPERTY(BlueprintAssignable, Category="Identity")
	FOTLoginCompleteDelegate OnLoginComplete;

	UPROPERTY(BlueprintAssignable, Category="Identity")
	FOTLogoutCompleteDelegate OnLogoutComplete;

	UPROPERTY(BlueprintAssignable, Category="Identity")
	FOTLoginChangeDelegate OnLoginChanged;

private:
	IOnlineIdentityPtr IdentityInterface;

	FDelegateHandle LoginCompleteDelegateHandle;
	FDelegateHandle LogoutCompleteDelegateHandle;
	FDelegateHandle LoginChangedDelegateHandle;

	FOnLoginCompleteDelegate LoginCompleteDelegate;
	FOnLogoutCompleteDelegate LogoutCompleteDelegate;
	FOnLoginChangedDelegate LoginChangedDelegate;

	void HandleLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error) const;
	void HandleLogoutComplete(int LocalUserNum, bool bWasSuccessful);
	void HandleLoginChange(int LocalUserNum);
};
