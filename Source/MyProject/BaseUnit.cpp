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

void ABaseUnit::AttackUnit(AActor* Target)
{
    if(!IsDead)
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
	UGameplayStatics::ApplyDamage(HitOut.GetActor(), Damage, GetController(), this, UDamageType::StaticClass());
	AttackAnimations(Target);
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
    DeathAnimations();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ABaseUnit::DestroyActor, DestroyActorDelay, false);
  }
  return DamageAmount;
  
}


void ABaseUnit::MoveTo(FVector Target)
{
  
  if(!IsDead)
  {
    AAIController* ai = Cast<AAIController>(GetController());
    ai->MoveToLocation(Target,50.0f);
  }
  
}

void ABaseUnit::DestroyActor()
{
  Destroy();
}

