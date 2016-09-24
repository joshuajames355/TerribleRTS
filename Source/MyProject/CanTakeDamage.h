// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CanTakeDamage.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UCanTakeDamage : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class MYPROJECT_API ICanTakeDamage
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
		
	UFUNCTION(BlueprintNativeEvent)
	void Repair(float MaxAmount);

	UFUNCTION(BlueprintCallable , BlueprintNativeEvent, Category = CanTakeDamage)
	int32 GetTeamNumber();

	UFUNCTION(BlueprintCallable , BlueprintNativeEvent, Category = CanTakeDamage)
	void SetTeamNumber(int32 NewTeamNumber);

	UFUNCTION(BlueprintCallable , BlueprintNativeEvent, Category = CanTakeDamage)
	bool GetIsDead();
	
	
};
