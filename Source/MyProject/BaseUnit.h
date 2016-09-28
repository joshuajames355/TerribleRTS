// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "GameFramework/Actor.h"
#include "CanTakeDamage.h"
#include "BaseUnit.generated.h"

UCLASS()
class MYPROJECT_API ABaseUnit : public ACharacter , public ICanTakeDamage
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseUnit();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Units)
	int32 TeamNumber;
	
	UPROPERTY(EditDefaultsOnly, Category = Units)
	float Range;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Units)
	int32 Cost;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Replicated, Category = Units)
	float Health;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Units)
	float StartingHealth;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Units)
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = Units)
	FName UnitTypeName;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Units)
	bool IsDead = false;

	UFUNCTION(BlueprintCallable,Server, Reliable, WithValidation, Category = Units)
	void MoveTo(FVector Target);
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent , Category = Units)
	void StartSelection();
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent , Category = Units)
	void StopSelection();
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category = Units)
	UTexture2D* UnitIcon;
	
	UFUNCTION(BlueprintImplementableEvent , Category = Units)
	void AttackAnimations(AActor* Target);
	
	UFUNCTION(BlueprintImplementableEvent , Category = Units)
	void DeathAnimations();

	UFUNCTION(BlueprintImplementableEvent, Category = Units)
	void SetupHealthBar();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void DeathAnimationMulticast();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void AttackAnimationsMulticast(AActor* Target);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Units)
	void SetTarget(AActor* Target);
	
	UFUNCTION(Server, Reliable, WithValidation, Category = Units)
	void AttackUnit(AActor* Target);

	UFUNCTION()
	void BuildRepair(AActor* Target, float DeltaTime);
	
	UPROPERTY(EditDefaultsOnly,Category = Units)
	float DestroyActorDelay;
	
	UPROPERTY(EditDefaultsOnly,Category = Units)
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Units)
	float BuildRate;

	virtual bool GetIsDead_Implementation() override;

	virtual void Repair_Implementation(float MaxAmount) override;

	virtual int32 GetTeamNumber_Implementation() override;

	virtual void SetTeamNumber_Implementation(int32 NewTeamNumber) override;

	virtual bool NeedsRepair_Implementation() override;

private: 

	UFUNCTION(Server, Reliable, WithValidation)
	void SetStartingHealth();

	AActor* TargetActor;

	AAIController* ai;
  
	FTimerHandle DeathTimer;

	void FindTarget();

	bool CheckLineOfSight(AActor* Target);
  
	void DestroyActor();
  
	FTimerHandle FireTimer;
  
	void ResetFire();
  
	bool CanFire = true;

	bool IsMovingTowardsTarget = false;
};

