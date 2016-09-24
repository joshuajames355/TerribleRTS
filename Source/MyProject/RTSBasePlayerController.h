// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "RTSBasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ARTSBasePlayerController : public APlayerController
{
	GENERATED_BODY()


public:

	ARTSBasePlayerController();

	UPROPERTY(BlueprintReadonly, Replicated, Category = Economy)
	float Money;

	UPROPERTY(BlueprintReadonly, Replicated, Category = Economy)
	float MaxMoney;

	UFUNCTION(BlueprintCallable, Category = Economy)
	float CalculateEffeciency(float MaxCost);

	UPROPERTY(BlueprintReadonly, Replicated, Category = Economy)
	int32 TeamNumber;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Economy)
	void SetTeamNumber(int32 NewTeamNumber);

};
