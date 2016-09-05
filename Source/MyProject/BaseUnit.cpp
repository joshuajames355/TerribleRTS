// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseUnit.h"
#include "UnrealNetwork.h"


// Sets default values
ABaseUnit::ABaseUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseUnit::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseUnit, Health);
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

void ABaseUnit::AttackUnit(AActor* Target)
{
    if(!IsDead && CanFire)
    {
      ABaseUnit* TargetUnit = Cast<ABaseUnit>(Target);
      FVector distance = Target->GetActorLocation() - GetActorLocation();
      if(TargetUnit->TeamNumber != TeamNumber && distance.Size() <= Range)
      {
	FCollisionQueryParams collisionParam = FCollisionQueryParams();
	FHitResult HitOut;
	FVector endPos = Target->GetActorLocation();
	collisionParam.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitOut,GetActorLocation(),endPos,ECC_Visibility,collisionParam);
      
	if(HitOut.GetActor()->GetClass()->IsChildOf(ABaseUnit::StaticClass()))
	{
	CanFire = false;
	UGameplayStatics::ApplyDamage(HitOut.GetActor(), Damage, GetController(), this, UDamageType::StaticClass());
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ABaseUnit::ResetFire, FireRate, false);
	AttackAnimationsMulticast(Target);
	}
      }
    }
}

float ABaseUnit::TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser)
{
  IsDead = true;
  Health -= DamageAmount;
  DebugTest();
  if(Health <= 0)
  {
	DeathAnimationMulticast();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ABaseUnit::DestroyActor, DestroyActorDelay, false);
  }
  return DamageAmount;
  
}


void ABaseUnit::MoveTo_Implementation(FVector Target)
{
  
  if(!IsDead)
  {
	AAIController* ai = Cast<AAIController>(GetController());
    ai->MoveToLocation(Target,50.0f);
  }
  
}

bool ABaseUnit::MoveTo_Validate(FVector Target)
{
	return true;
}

void ABaseUnit::AttackAnimationsMulticast_Implementation(AActor* Target)
{
	AttackAnimations(Target);
}

bool ABaseUnit::AttackAnimationsMulticast_Validate(AActor* Target)
{
	return true;
}

void ABaseUnit::DeathAnimationMulticast_Implementation()
{
	DeathAnimations();
}

bool ABaseUnit::DeathAnimationMulticast_Validate()
{
	return true;
}

void ABaseUnit::DestroyActor()
{
  Destroy();
}

void ABaseUnit::ResetFire()
{
  CanFire = true;
}

