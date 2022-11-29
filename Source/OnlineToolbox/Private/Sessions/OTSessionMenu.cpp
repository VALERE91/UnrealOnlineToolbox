// Fill out your copyright notice in the Description page of Project Settings.


#include "Sessions/OTSessionMenu.h"

#include "Sessions/OTSessionsSubsystem.h"

void UOTSessionMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	if(UWorld* World = GetWorld())
	{
		if(APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if(UGameInstance* Instance = GetGameInstance())
	{
		OTSessionsSubsystem = Instance->GetSubsystem<UOTSessionsSubsystem>();
		checkf(OTSessionsSubsystem != nullptr, TEXT("Multiplayer Session Subsystem cannot be found"));

		OTSessionsSubsystem->ToolboxOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		OTSessionsSubsystem->ToolboxOnFindSessionComplete.AddDynamic(this, &ThisClass::OnFindSession);
		OTSessionsSubsystem->ToolboxOnJoinSessionComplete.AddDynamic(this, &ThisClass::OnJoinSession);
	}
}

void UOTSessionMenu::HostSession(const FString& Lobby,
	int32 NumPublicConnection /*= 4*/,
	const FString& MatchType /*= "FreeForAll"*/)
{
	if(!ensureMsgf(OTSessionsSubsystem != nullptr,
		TEXT("Multiplayer Session Subsystem is not set. Did you call MenuSetup?"))) return;

	LobbyMap = Lobby;
	OTSessionsSubsystem->CreateSession(NumPublicConnection, MatchType);
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

	OTSessionsSubsystem->ToolboxOnCreateSessionComplete.RemoveDynamic(this, &ThisClass::OnCreateSession);
	OTSessionsSubsystem->ToolboxOnFindSessionComplete.RemoveDynamic(this, &ThisClass::OnFindSession);
	OTSessionsSubsystem->ToolboxOnJoinSessionComplete.RemoveDynamic(this, &ThisClass::OnJoinSession);
}
