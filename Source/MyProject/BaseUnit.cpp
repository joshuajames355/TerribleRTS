// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseUnit.h"
#include "UnrealNetwork.h"
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
	SetupHealthBar();
	SetStartingHealth();
	ai = Cast<AAIController>(GetController());
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetActor)
	{
		bool IsEnemyDead;
		if (TargetActor->IsA(ABaseUnit::StaticClass()))
		{
			IsEnemyDead = Cast<ABaseUnit>(TargetActor)->IsDead;
		}
		if (TargetActor->IsA(ABaseBuilding::StaticClass()))
		{
			IsEnemyDead = Cast<ABaseBuilding>(TargetActor)->IsDead;
		}
		if (IsEnemyDead)
		{
			TargetActor = nullptr;
		}
	}

	if (CanFire && TargetActor && HasWeapons)
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
	else if (CanBuild && TargetActor)
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
			BuildRepair(TargetActor, DeltaTime);
		}
	}
	else if (CanFire && HasWeapons)
	{
		FindTarget();
	}
	

}

void ABaseUnit::FindTarget()
{
	TArray<FHitResult> HitOut;
	FVector EndPos = GetActorLocation() + FVector(Range, 0.0f, 0.0f);
	FCollisionQueryParams collisionParam = FCollisionQueryParams();
	collisionParam.AddIgnoredActor(this);
	GetWorld()->SweepMultiByChannel(HitOut, GetActorLocation(), EndPos, FQuat(), ECC_Visibility, FCollisionShape::MakeSphere(Range), collisionParam);
	for (FHitResult hit : HitOut)
	{
		if (hit.GetActor())
		{
			if (hit.GetActor()->IsA(ABaseUnit::StaticClass()) || hit.GetActor()->IsA(ABaseBuilding::StaticClass()))
			{
				int32 EnemyTeamNumber;
				bool IsEnemyDead;
				if (hit.GetActor()->IsA(ABaseUnit::StaticClass()))
				{
					ABaseUnit* EnemyUnit = Cast<ABaseUnit>(hit.GetActor());
					EnemyTeamNumber = EnemyUnit->TeamNumber;
					IsEnemyDead = EnemyUnit->IsDead;
				}
				if (hit.GetActor()->IsA(ABaseBuilding::StaticClass()))
				{
					ABaseBuilding* EnemyUnit = Cast<ABaseBuilding>(hit.GetActor());
					EnemyTeamNumber = EnemyUnit->TeamNumber;
					IsEnemyDead = EnemyUnit->IsDead;
				}
				if (EnemyTeamNumber != TeamNumber && !IsEnemyDead)
				{
					AttackUnit(hit.GetActor());
					break;
				}
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
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void ABaseUnit::AttackUnit_Implementation(AActor* Target)
{
	if(!IsDead && CanFire && HasWeapons)
    {
		int32 EnemyTeamNumber;
		bool IsEnemyDead;
		if (Target->IsA(ABaseUnit::StaticClass()))
		{
			ABaseUnit* EnemyUnit = Cast<ABaseUnit>(Target);
			EnemyTeamNumber = EnemyUnit->TeamNumber;
			IsEnemyDead = EnemyUnit->IsDead;
		}
		else if (Target->IsA(ABaseBuilding::StaticClass()))
		{
			ABaseBuilding* EnemyUnit = Cast<ABaseBuilding>(Target);
			EnemyTeamNumber = EnemyUnit->TeamNumber;
			IsEnemyDead = EnemyUnit->IsDead;

		}
		else
		{
			return;
		}
		FVector distance = Target->GetActorLocation() - GetActorLocation();
		if(EnemyTeamNumber != TeamNumber && distance.Size() <= Range && !IsEnemyDead)
		{
			if (CheckLineOfSight(Target))
			{
				CanFire = false;
				UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, UDamageType::StaticClass());
				GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ABaseUnit::ResetFire, FireRate, false);
				AttackAnimationsMulticast(Target);
			}
		
      }
    }
}

bool ABaseUnit::AttackUnit_Validate(AActor* Target)
{
	return true;
}

void ABaseUnit::BuildRepair_Implementation(AActor* Target , float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("BuildRepair"));
	int32 TargetTeamNumber;
	bool IsTargetDead;
	float TargetHealth;
	float TargetStartingHealth;
	if (Target->IsA(ABaseUnit::StaticClass()))
	{
		ABaseUnit* EnemyUnit = Cast<ABaseUnit>(Target);
		TargetTeamNumber = EnemyUnit->TeamNumber;
		IsTargetDead = EnemyUnit->IsDead;
		TargetHealth = EnemyUnit->Health;
		TargetStartingHealth = EnemyUnit->StartingHealth;
	}
	else if (Target->IsA(ABaseBuilding::StaticClass()))
	{
		ABaseBuilding* EnemyUnit = Cast<ABaseBuilding>(Target);
		TargetTeamNumber = EnemyUnit->TeamNumber;
		IsTargetDead = EnemyUnit->IsDead;
		TargetHealth = EnemyUnit->Health;
		TargetStartingHealth = EnemyUnit->StartingHealth;
	}
	else
	{
		return;
	}
	if (!IsTargetDead && TargetTeamNumber == TeamNumber && TargetHealth < TargetStartingHealth)
	{
		if (Target->IsA(ABaseUnit::StaticClass()))
		{
			ABaseUnit* EnemyUnit = Cast<ABaseUnit>(Target);
			EnemyUnit->HealUnit(BuildRate * DeltaTime);
		}
		else if(Target->IsA(ABaseBuilding::StaticClass()))
		{
			ABaseBuilding* EnemyUnit = Cast<ABaseBuilding>(Target);
			UE_LOG(LogTemp, Warning, TEXT("BuildRepair Sent Assist"));
			EnemyUnit->AssistBuilding(BuildRate * DeltaTime);
		}
	}
}

bool ABaseUnit::BuildRepair_Validate(AActor* Target, float DeltaTime)
{
	return true;
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

void ABaseUnit::HealUnit_Implementation(float MaxAmount)
{
	if (!IsDead)
	{
		if (Health + MaxAmount <= StartingHealth)
		{
			Health += MaxAmount;
		}
		else if (Health < StartingHealth)
		{
			Health = StartingHealth;
		}
	}
}
bool ABaseUnit::HealUnit_Validate(float MaxAmount)
{
	return true;
}