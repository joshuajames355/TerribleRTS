// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseBuilding.generated.h"

UCLASS()
class MYPROJECT_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuilding();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Buildings)
	float Health;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Buildings)
	bool IsDead = false;

	UPROPERTY(EditDefaultsOnly, Category = Buildings)
	float DestroyActorDelay;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void DeathAnimationMulticast();

	UFUNCTION(BlueprintImplementableEvent, Category = Units)
	void DeathAnimations();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Buildings)
	float StartingHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Buildings)
	int32 TeamNumber;

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void SetStartingHealth();

	FTimerHandle DeathTimer;
	
	void DestroyActor();
	
};
