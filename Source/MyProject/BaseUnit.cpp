// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseUnit.h"
#include "UnrealNetwork.h"
#include "RTSBasePlayerController.h"
#include "BaseBuilding.h"

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
	DOREPLIFETIME(ABaseUnit, StartingHealth);
}

// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	if (Role == ROLE_Authority)
	{
		SetStartingHealth();
	}
	SetupHealthBar();
	ai = Cast<AAIController>(GetController());
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GetWorld()->IsServer())
	{
		return;
	}
	if (TargetActor)
	{
		if (TargetActor->GetClass()->ImplementsInterface(UCanTakeDamage::StaticClass()))
		{
			if (ICanTakeDamage::Execute_GetIsDead(TargetActor))
			{
				TargetActor = nullptr;
			}
		}
	}

	if (CanFire && TargetActor && Damage > 0)
	{
		FVector distance = TargetActor->GetActorLocation() - GetActorLocation();
		if (distance.Size() > Range)
		{
			if (!IsMovingTowardsTarget)
			{
				IsMovingTowardsTarget = true;
				ai->MoveToLocation(TargetActor->GetActorLocation(), 50.0f);
			}
		}
		else
		{	
			if (IsMovingTowardsTarget)
			{
				ai->StopMovement();
				IsMovingTowardsTarget = false;
			}
			AttackUnit(TargetActor);
		}
	}
	else if (BuildRate > 0 && TargetActor)
	{
		if (!ICanTakeDamage::Execute_NeedsRepair(TargetActor))
		{
			TargetActor = nullptr;
			return;
		}
		FVector distance = TargetActor->GetActorLocation() - GetActorLocation();
		if (distance.Size() > Range)
		{
			if (!IsMovingTowardsTarget)
			{
				IsMovingTowardsTarget = true;
				ai->MoveToLocation(TargetActor->GetActorLocation(), 50.0f);
			}
		}
		else
		{
			if (IsMovingTowardsTarget)
			{
				ai->StopMovement();
				IsMovingTowardsTarget = false;
			}
			BuildRepair(TargetActor, DeltaTime);
		}
	}
	else if (CanFire && Damage > 0)
	{
		FindTarget();
	}
}

void ABaseUnit::FindTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("Find Target"));
	TArray<FHitResult> HitOut;
	FVector EndPos = GetActorLocation() + FVector(Range, 0.0f, 0.0f);
	FCollisionQueryParams collisionParam = FCollisionQueryParams();
	collisionParam.AddIgnoredActor(this);
	GetWorld()->SweepMultiByChannel(HitOut, GetActorLocation(), EndPos, FQuat(), ECC_Visibility, FCollisionShape::MakeSphere(Range), collisionParam);
	for (FHitResult hit : HitOut)
	{
		if (hit.GetActor()->GetClass()->ImplementsInterface(UCanTakeDamage::StaticClass()))
		{
			if(!ICanTakeDamage::Execute_GetIsDead(hit.GetActor()) && ICanTakeDamage::Execute_GetTeamNumber(hit.GetActor()) != TeamNumber)
			{
				AttackUnit(hit.GetActor());
				return;
			}
		}
	}
}

void ABaseUnit::SetTarget_Implementation(AActor* Target)
{
	IsMovingTowardsTarget = true;
	ai->MoveToActor(Target, 50.0f);
	TargetActor = Target;
}

bool ABaseUnit::SetTarget_Validate(AActor* Target)
{
	return true;
}

bool ABaseUnit::CheckLineOfSight(AActor* Target)
{
	FCollisionQueryParams collisionParam = FCollisionQueryParams();
	FHitResult HitOut;
	FVector endPos = Target->GetActorLocation();
	collisionParam.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitOut, GetActorLocation(), endPos, ECC_Visibility, collisionParam);
	if (HitOut.GetActor()) {
		if (HitOut.GetActor() == Target)
		{
			return true;
		}
		return false;
	}
	return false;
}

void ABaseUnit::AttackUnit_Implementation(AActor* Target)
{
	if(!IsDead && CanFire && Damage > 0 && Target->GetClass()->ImplementsInterface(UCanTakeDamage::StaticClass()))
    {
		FVector distance = Target->GetActorLocation() - GetActorLocation();
		if(ICanTakeDamage::Execute_GetTeamNumber(Target) != TeamNumber && distance.Size() <= Range && !ICanTakeDamage::Execute_GetIsDead(Target) && CheckLineOfSight(Target))
		{
				CanFire = false;
				UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, UDamageType::StaticClass());
				GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ABaseUnit::ResetFire, FireRate, false);
				AttackAnimationsMulticast(Target);
		}
    }
}

bool ABaseUnit::AttackUnit_Validate(AActor* Target)
{
	return true;
}

void ABaseUnit::BuildRepair(AActor* Target , float DeltaTime)
{
	if (Target->GetClass()->ImplementsInterface(UCanTakeDamage::StaticClass()))
	{
		if (!ICanTakeDamage::Execute_GetIsDead(Target) && ICanTakeDamage::Execute_GetTeamNumber(Target) == TeamNumber)
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				ARTSBasePlayerController* PC = Cast<ARTSBasePlayerController>(*Iterator);
				if (PC)
				{
					UE_LOG(LogTemp, Warning, TEXT("PlayerController has team %d."), PC->TeamNumber);
					if (PC->TeamNumber == TeamNumber)
					{
						ICanTakeDamage::Execute_Repair(Target, PC->CalculateEffeciency(BuildRate * DeltaTime));
						return;
					}
				}
			}
		}
	}
}

float ABaseUnit::TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser)
{
  Health -= DamageAmount;
  if(Health <= 0)
  {
	IsDead = true;
	DeathAnimationMulticast();
    GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ABaseUnit::DestroyActor, DestroyActorDelay, false);
  }
  return DamageAmount;
  
}

void ABaseUnit::MoveTo_Implementation(FVector Target)
{
	TargetActor = nullptr;
	if(!IsDead)
	{
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

void ABaseUnit::SetStartingHealth_Implementation()
{
	StartingHealth = Health;
}

bool ABaseUnit::SetStartingHealth_Validate()
{
	return true;
}

bool ABaseUnit::GetIsDead_Implementation()
{
	return IsDead;
}

void ABaseUnit::Repair_Implementation(float MaxAmount)
{
	float CostHPRatio = StartingHealth / Cost;
	float HPIncrease = MaxAmount * CostHPRatio;
	if (!IsDead && GetWorld()->IsServer())
	{
		if (Health + HPIncrease <= StartingHealth)
		{
			Health += HPIncrease;
		}
		else if (Health < StartingHealth)
		{
			Health = StartingHealth;
		}
	}
 }

int32 ABaseUnit::GetTeamNumber_Implementation()
{
	return TeamNumber;
 }

void ABaseUnit::SetTeamNumber_Implementation(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}

bool ABaseUnit::NeedsRepair_Implementation()
{
	return Health < StartingHealth;
}