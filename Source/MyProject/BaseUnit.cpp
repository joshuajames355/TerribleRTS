// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseUnit.h"


// Sets default values
ABaseUnit::ABaseUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUnit::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

