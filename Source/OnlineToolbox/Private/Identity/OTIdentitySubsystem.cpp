// Fill out your copyright notice in the Description page of Project Settings.


#include "Identity/OTIdentitySubsystem.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"

UOTIdentitySubsystem::UOTIdentitySubsystem() :
	LoginCompleteDelegate(FOnLoginCompleteDelegate::CreateUObject(this, &UOTIdentitySubsystem::HandleLoginComplete)),
LogoutCompleteDelegate(FOnLogoutCompleteDelegate::CreateUObject(this, &UOTIdentitySubsystem::HandleLogoutComplete)),
LoginChangedDelegate(FOnLoginChangedDelegate::CreateUObject(this, &UOTIdentitySubsystem::HandleLoginChange))
{
}

void UOTIdentitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const auto* Subsystem = IOnlineSubsystem::Get();
	checkf(Subsystem != nullptr, TEXT("Unable to get the SubSytem"));

	IdentityInterface = Subsystem->GetIdentityInterface();
	checkf(IdentityInterface != nullptr, TEXT("Unable to get the Identity interface"));

	LoginCompleteDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegate);
	LogoutCompleteDelegateHandle = IdentityInterface->AddOnLogoutCompleteDelegate_Handle(0, LogoutCompleteDelegate);
	LoginChangedDelegateHandle = IdentityInterface->AddOnLoginChangedDelegate_Handle(LoginChangedDelegate);
}

void UOTIdentitySubsystem::Deinitialize()
{
	Super::Deinitialize();

	IdentityInterface->ClearOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegateHandle);
	IdentityInterface->ClearOnLogoutCompleteDelegate_Handle(0, LogoutCompleteDelegateHandle);
	IdentityInterface->ClearOnLoginChangedDelegate_Handle(LoginChangedDelegateHandle);
}

void UOTIdentitySubsystem::Login()
{
	checkf(IdentityInterface != nullptr, TEXT("Unable to get the Identity interface"));

	IdentityInterface->AutoLogin(0);
}

void UOTIdentitySubsystem::Logout()
{
	checkf(IdentityInterface != nullptr, TEXT("Unable to get the Identity interface"));
	
	IdentityInterface->Logout(0);
}

void UOTIdentitySubsystem::HandleLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error) const
{
	OnLoginComplete.Broadcast(bWasSuccessful, UserId.ToString());
}

void UOTIdentitySubsystem::HandleLogoutComplete(int LocalUserNum, bool bWasSuccessful)
{
	OnLogoutComplete.Broadcast(bWasSuccessful);
}

void UOTIdentitySubsystem::HandleLoginChange(int LocalUserNum)
{
	OnLoginChanged.Broadcast(true);
}
