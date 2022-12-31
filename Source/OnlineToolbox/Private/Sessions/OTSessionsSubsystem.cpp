// Fill out your copyright notice in the Description page of Project Settings.
#include "Sessions/OTSessionsSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UOTSessionsSubsystem::UOTSessionsSubsystem() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOTSessionsSubsystem::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UOTSessionsSubsystem::OnFindSessionComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOTSessionsSubsystem::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UOTSessionsSubsystem::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &UOTSessionsSubsystem::OnStartSessionComplete))
{
	
}

void UOTSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	checkf(Subsystem != nullptr, TEXT("Unable to get the SubSytem"));

	SessionInterface = Subsystem->GetSessionInterface();
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;
}

void UOTSessionsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOTSessionsSubsystem::CreateSession(int32 NumConnections, const FString& MatchType, const FString& SessionName,const bool bIsPrivate, const FString & Password)
{
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;

	//Destroy existing session if it exists
	const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumConnections;
		LastMatchType = MatchType;
		LastSessionName = SessionName;
		bLastSessionIsPrivate = bIsPrivate;
		LastSessionPassword = Password;
		DestroySession();
		return;
	}

	//Register the delegate for when the creation complete and store its handle for later removal
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	//Create the session parameters. We could not use the MakeShareable and thus the new
	//but this way we can make SessionSettings a member of our class for future reuse.
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	//If we are using the NULL subsystem it is a LAN match. Otherwise it is an online match
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSettings->NumPublicConnections = NumConnections;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(FName("IsPrivate"),bIsPrivate,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(FName("Password"),Password,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(FName("SessionName"),SessionName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

#if !UE_BUILD_SHIPPING
	//Enforce a specific Build ID in not shipping so we can
	//easily test session creation
	LastSessionSettings->BuildUniqueId = 1;
#endif

	//Get the local player because create session need the creator Unique Net ID
	const ULocalPlayer* LocalPLayer = GetWorld()->GetFirstLocalPlayerFromController();
	
	//Create the session
	const bool success = SessionInterface->CreateSession(*LocalPLayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);
	if(!success)
	{
		//We failed to create the session simply remove the delegate for completion
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		ToolboxOnCreateSessionComplete.Broadcast(false);
	}
}

void UOTSessionsSubsystem::FindSessions(int32 MaxSearchResults, const FString& MatchType)
{
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;

	//Register the delegate for when the find session complete and store its handle for later removal
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	LastSessionSearch->QuerySettings.Set(FName("MatchType"), MatchType, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* LocalPLayer = GetWorld()->GetFirstLocalPlayerFromController();
	const bool success = SessionInterface->FindSessions(*LocalPLayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef());
	if(!success)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		TArray<FOTSessionSearchResult> Results;
		ToolboxOnFindSessionComplete.Broadcast(Results, false);
	}
}

void UOTSessionsSubsystem::JoinSession(const FOTSessionSearchResult& SessionResult)
{
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;

	//Register the delegate for when the join session complete and store its handle for later removal
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	
	//Get the local player because create session need the creator Unique Net ID
	const ULocalPlayer* LocalPLayer = GetWorld()->GetFirstLocalPlayerFromController();
	const bool success = SessionInterface->JoinSession(*LocalPLayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult.Native);
	if(!success)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		ToolboxOnJoinSessionComplete.Broadcast(false, EOTJoinSessionResultType::UnknownError, "");
	}
}

void UOTSessionsSubsystem::DestroySession()
{
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;

	//Register the delegate for when the destroy session complete and store its handle for later removal
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
	
	//Destroy existing session if it exists
	const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
	else //No session found
	{
		//Remove the destroy session completion delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		
		ToolboxOnDestroySessionComplete.Broadcast(false);
	}
}

void UOTSessionsSubsystem::StartSession()
{
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;

	//Register the delegate for when the join session complete and store its handle for later removal
	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);
	
	if(!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		ToolboxOnStartSessionComplete.Broadcast(false);
	}
}

void UOTSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		//Remove the create session completion delegate
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	//Fire the delegate
	ToolboxOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UOTSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	if(SessionInterface)
	{
		//Remove the join session completion delegate
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if(!LastSessionSearch.IsValid() || LastSessionSearch->SearchResults.Num() <= 0)
	{
		TArray<FOTSessionSearchResult> Results;
		ToolboxOnFindSessionComplete.Broadcast(Results, false);
		return;
	}

	TArray<FOTSessionSearchResult> Results;
	Results.Reserve(LastSessionSearch->SearchResults.Num());
	for(const auto& result : LastSessionSearch->SearchResults)
	{
		FOTSessionSearchResult r {};
		r.Native = result;
		r.Session = result.Session;
		r.PingInMs = result.PingInMs;
		Results.Add(r);
	}

	ToolboxOnFindSessionComplete.Broadcast(Results, true);
}

void UOTSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(SessionInterface)
	{
		//Remove the join session completion delegate
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	else
	{
		//SessionInterface is null it is not supported
		ToolboxOnJoinSessionComplete.Broadcast(Result == EOnJoinSessionCompleteResult::Type::Success, UnknownError, "");
		return;
	}
	
	EOTJoinSessionResultType ResultBP = EOTJoinSessionResultType::UnknownError;
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success:
		ResultBP = Success;
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		ResultBP = SessionIsFull;
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		ResultBP = SessionDoesNotExist;
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		ResultBP = CouldNotRetrieveAddress;
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		ResultBP = AlreadyInSession;
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		ResultBP = UnknownError;
		break;
	}

	//Get the session address to make a client travel
	FString Address;
	SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

	//Fire our own delegate
	ToolboxOnJoinSessionComplete.Broadcast(Result == EOnJoinSessionCompleteResult::Type::Success, ResultBP, Address);
}

void UOTSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		//Remove the destroy session completion delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	
	ToolboxOnDestroySessionComplete.Broadcast(bWasSuccessful);

	if(!bWasSuccessful || !bCreateSessionOnDestroy) return;

	CreateSession(LastNumPublicConnections, LastMatchType, LastSessionName, bLastSessionIsPrivate, LastSessionPassword);
}

void UOTSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		//Remove the create session completion delegate
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}
	
	ToolboxOnStartSessionComplete.Broadcast(bWasSuccessful);
}

void UOTSessionsSubsystem::GetSessionInformations(const FOTSessionSearchResult& Session, int32& SessionPing,
	int32& NumberOfConnectedPlayers, int32& MaxConnectedPlayers, FString& SessionName, FString& SessionId,
	bool& bIsPrivate, FString& SessionPassword)
{
	const auto SessionSettings = Session.Session.SessionSettings;
	
	SessionPing = Session.PingInMs;
	MaxConnectedPlayers = SessionSettings.NumPublicConnections;
	NumberOfConnectedPlayers = MaxConnectedPlayers - Session.Session.NumOpenPublicConnections;
	
	SessionId = Session.Session.GetSessionIdStr();
	SessionSettings.Get(FName("IsPrivate"),bIsPrivate);
	SessionSettings.Get(FName("SessionName"),SessionName);
	SessionSettings.Get(FName("Password"),SessionPassword);
}