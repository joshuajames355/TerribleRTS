// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseBuilding.h"
#include "BaseUnit.h"
#include "RTSBasePlayerController.h"
#include "BaseProductionBuilding.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABaseProductionBuilding : public ABaseBuilding
{
	GENERATED_BODY()
	

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = Buildings)
	TArray<TSubclassOf<ABaseUnit>> UnitList;

	UPROPERTY(EditDefaultsOnly, Category = Buildings)
	float BuildRate;
	
	UFUNCTION(BlueprintCallable, Server , Reliable, WithValidation, Category = Buildings)
	void AddToQueue(TSubclassOf<ABaseUnit> NewUnit);

	UFUNCTION(BlueprintCallable, Category = Buildings)
	void RemoveFromQueue(TSubclassOf<ABaseUnit> NewUnit);

	UPROPERTY(BlueprintReadWrite, Category = Buildings)
	bool InfiniteQueue = false;

	UPROPERTY(BlueprintReadOnly, Category = Buildings)
	TArray<TSubclassOf<ABaseUnit>> BuildQueue;

	UPROPERTY(EditDefaultsOnly, Category = Buildings)
	FVector UnitSpawnPoint;

private:
	bool IsBuilding = false;

	TSubclassOf<ABaseUnit> UnitToConstruct;

	float CurrentProgress = 0;

	float MoneyTarget = 0;

	void StartBuilding();

	void FinishBuilding();

	
};
