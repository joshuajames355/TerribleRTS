// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BaseUnit.generated.h"

UCLASS()
class MYPROJECT_API ABaseUnit : public ACharacter
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
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Units)
	float Health;
	
	UPROPERTY(EditDefaultsOnly, Category = Units)
	float Damage;
	
	bool IsDead = false;
	
	UFUNCTION(BlueprintCallable, Category = Units)
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
	
	UFUNCTION(BlueprintImplementableEvent , Category = Units)
	void DebugTest();
	
	UFUNCTION(BlueprintCallable,Category = Units)
	void AttackUnit(AActor* Target);
	
	UPROPERTY(EditDefaultsOnly,Category = Units)
	float DestroyActorDelay;
	
private: 
  
  FTimerHandle DeathTimer;
  
  void DestroyActor();
};

