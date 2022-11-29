// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OTJoinSessionResultType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EOTJoinSessionResultType
{
	/** The join worked as expected */
	Success  UMETA(DisplayName = "Success"),
	/** There are no open slots to join */
	SessionIsFull UMETA(DisplayName = "Full"),
	/** The session couldn't be found on the service */
	SessionDoesNotExist UMETA(DisplayName = "DoesNotExist"),
	/** There was an error getting the session server's address */
	CouldNotRetrieveAddress UMETA(DisplayName = "CouldNotRetrieveAddress"),
	/** The user attempting to join is already a member of the session */
	AlreadyInSession UMETA(DisplayName = "AlreadyIn"),
	/** An error not covered above occurred */
	UnknownError UMETA(DisplayName = "UnknownError")
};