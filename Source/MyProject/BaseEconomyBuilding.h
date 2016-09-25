// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseBuilding.h"
#include "BaseEconomyBuilding.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABaseEconomyBuilding : public ABaseBuilding
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = Buildings)
	float MoneyRate;
	
};
