// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OTSearchSessionType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EOTSearchSessionType
{
 /* Search for public and private sessions */
 All UMETA(DisplayName="All"),
 /* Search for private sessions */
 Private UMETA(DisplayName="Private"),
 /* Search for public sessions */
 Public UMETA(DisplayName="Public"),
};
