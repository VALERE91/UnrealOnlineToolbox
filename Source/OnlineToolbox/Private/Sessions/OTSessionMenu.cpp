// Fill out your copyright notice in the Description page of Project Settings.


#include "Sessions/OTSessionMenu.h"

#include "Sessions/OTSessionsSubsystem.h"

void UOTSessionMenu::MenuSetup(const bool ShouldAddToViewport,const bool ShouldBeVisible, const bool ShouldSetInputModeToUIOnly, const bool ShowMouseCursor)
{
	// Set the widget according to the given parameters
	if(ShouldAddToViewport)
	{
		AddToViewport();
	}
	if(ShouldBeVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	bIsFocusable = true;


	if(UWorld* World = GetWorld())
	{
		if(APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			if(ShouldSetInputModeToUIOnly)
			{
				FInputModeUIOnly InputModeData;
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PlayerController->SetInputMode(InputModeData);
			}
			
			PlayerController->SetShowMouseCursor(ShowMouseCursor);
		}

		if(UGameInstance* Instance = World->GetGameInstance())
		{
			OTSessionsSubsystem = Instance->GetSubsystem<UOTSessionsSubsystem>();
			checkf(OTSessionsSubsystem != nullptr, TEXT("Multiplayer Session Subsystem cannot be found"));

			OTSessionsSubsystem->ToolboxOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
			OTSessionsSubsystem->ToolboxOnFindSessionComplete.AddDynamic(this, &ThisClass::OnFindSession);
			OTSessionsSubsystem->ToolboxOnJoinSessionComplete.AddDynamic(this, &ThisClass::OnJoinSession);
		}
	}

	
	
}


void UOTSessionMenu::HostSession(const TSoftObjectPtr<UWorld> LobbyLevel,
	int32 NumPublicConnection /*= 4*/,
	const FString& MatchType /*= "FreeForAll"*/,
	const FString& SessionName,
	const bool bIsPrivate /*= false*/,
	const FString& Password)
{
	if(!ensureMsgf(OTSessionsSubsystem != nullptr,
		TEXT("Multiplayer Session Subsystem is not set. Did you call MenuSetup?"))) return;

	//
	// Get the full path of the soft reference of the level
	// something like : /Game/Levels/Lobbies/LVL_Lobby.LVL_Lobby
	// Then remove the extension and save it to LobbyMap
	//
	LobbyLevel.ToSoftObjectPath().ToString().Split(FString("."),&LobbyMap, nullptr );

	//UE_LOG(LogTemp,Display,TEXT("%s"), *LobbyMap);
	
	OTSessionsSubsystem->CreateSession(NumPublicConnection, MatchType, SessionName, bIsPrivate, Password);
}

void UOTSessionMenu::FindSession(int32 MaxSessionNumber, const FString& MatchType)
{
	if(!ensureMsgf(OTSessionsSubsystem != nullptr,
		TEXT("Multiplayer Session Subsystem is not set. Did you call MenuSetup?"))) return;

	OTSessionsSubsystem->FindSessions(MaxSessionNumber, MatchType);
}

void UOTSessionMenu::JoinSession(const FOTSessionSearchResult& session)
{
	if(!ensureMsgf(OTSessionsSubsystem != nullptr,
		TEXT("Multiplayer Session Subsystem is not set. Did you call MenuSetup?"))) return;

	OTSessionsSubsystem->JoinSession(session);
}

void UOTSessionMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UOTSessionMenu::OnCreateSession(bool bWasSuccessful)
{
	OnSessionCreateComplete.Broadcast(bWasSuccessful);

	if(!bWasSuccessful) return;
	if(auto* World = GetWorld())
	{
		World->ServerTravel(LobbyMap.Append("?listen"));
	}
}

void UOTSessionMenu::OnFindSession(const TArray<FOTSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	OnSessionSearchComplete.Broadcast(bWasSuccessful, SessionResults);
}

void UOTSessionMenu::OnJoinSession(bool bWasSuccessful, EOTJoinSessionResultType Type, const FString& Address)
{
	OnSessionJoinedComplete.Broadcast(bWasSuccessful, Type);
	
	if(auto* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameOnly InputModeData;

			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UOTSessionMenu::MenuTearDown()
{
	RemoveFromParent();
	if(auto* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameOnly InputModeData;

			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}

	if(OTSessionsSubsystem == nullptr) return;

	OTSessionsSubsystem->ToolboxOnCreateSessionComplete.RemoveDynamic(this, &ThisClass::OnCreateSession);
	OTSessionsSubsystem->ToolboxOnFindSessionComplete.RemoveDynamic(this, &ThisClass::OnFindSession);
	OTSessionsSubsystem->ToolboxOnJoinSessionComplete.RemoveDynamic(this, &ThisClass::OnJoinSession);
}
