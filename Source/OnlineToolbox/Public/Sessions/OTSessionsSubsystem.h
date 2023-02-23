// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Sessions/OTJoinSessionResultType.h"
#include "Sessions/OTSessionSearchResult.h"

#include "OTSessionsSubsystem.generated.h"

//
// Declaring custom delegates
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolboxOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FToolboxOnFindSessionComplete, const TArray<FOTSessionSearchResult>&, SessionResults, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FToolboxOnJoinSessionComplete, bool, bWasSuccessful, EOTJoinSessionResultType, Type, const FString&, Address);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolboxOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolboxOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class ONLINETOOLBOX_API UOTSessionsSubsystem : public UGameInstanceSubsystem
{
private:
	GENERATED_BODY()
public:
	UOTSessionsSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	/**
	 * @brief 
	 * @param NumPublicConnections 
	 * @param MatchType
	 * @param SessionName
	 * @param bIsPrivate
	 * @param Password
	 */
	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 NumConnections, const FString& MatchType, const FString& SessionName,const bool bIsPrivate = false, const FString & Password = "");

	/**
	 * @brief 
	 * @param MaxSearchResults 
	 */
	UFUNCTION(BlueprintCallable)
	void FindSessions(int32 MaxSearchResults, const FString& MatchType);

	/**
	 * @brief 
	 * @param SessionResult 
	 */
	UFUNCTION(BlueprintCallable)
	void JoinSession(const FOTSessionSearchResult& SessionResult);

	/**
	 * @brief 
	 */
	UFUNCTION(BlueprintCallable)
	void DestroySession();

	/**
	 * @brief 
	 */
	UFUNCTION(BlueprintCallable)
	void StartSession();

	//
	// Public delegates for class to bind callbacks
	//
	UPROPERTY(BlueprintAssignable)
	FToolboxOnCreateSessionComplete ToolboxOnCreateSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnDestroySessionComplete ToolboxOnDestroySessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnFindSessionComplete ToolboxOnFindSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnJoinSessionComplete ToolboxOnJoinSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnStartSessionComplete ToolboxOnStartSessionComplete;
protected:
	//
	// Internal callbacks for the delegates.
	//
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	//
	// Helpers
	//
	UFUNCTION(BlueprintCallable)
	void GetSessionInformations(const FOTSessionSearchResult& Session, int32& SessionPing, int32& NumberOfConnectedPlayers, int32& MaxConnectedPlayers,
				   FString& SessionName, FString& SessionId, bool& bIsPrivate, FString& SessionPassword);
private:
	IOnlineSessionPtr SessionInterface;

	bool bCreateSessionOnDestroy {false};
	int32 LastNumPublicConnections;
	FString LastMatchType;
	FString LastSessionName;
	bool bLastSessionIsPrivate;
	FString LastSessionPassword;
	
	
	
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	//
	// Delegates for the OnlineSubsystem session interfaces
	//
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
