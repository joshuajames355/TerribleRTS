// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Units)
	int32 TeamNumber;
	
	UPROPERTY(EditDefaultsOnly, Category = Units)
	float StartingHealth;
	
	UPROPERTY(BlueprintReadOnly, Category = Units)
	float CurrentHealth;
	
	UPROPERTY(EditDefaultsOnly, Category = Units)
	float Range;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Units)
	float Health;
	
	UPROPERTY(EditDefaultsOnly, Category = Units)
	float Damage;
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent , Category = Units)
	void MoveTo(FVector Target);
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent , Category = Units)
	void StartSelection();
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent , Category = Units)
	void StopSelection();
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category = Units)
	UTexture2D* UnitIcon;
	
	UFUNCTION(BlueprintImplementableEvent , Category = Units)
	void AttackAnimations(AActor* Target);
	
	UFUNCTION(BlueprintCallable,Category = Units)
	void AttackUnit(AActor* Target);
};

