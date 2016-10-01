// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseProductionBuilding.h"

void ABaseProductionBuilding::AddToQueue_Implementation(TSubclassOf<ABaseUnit> NewUnit)
{
	BuildQueue.Add(NewUnit);
	if (!IsBuilding)
	{
		StartBuilding();
	}
}
bool ABaseProductionBuilding::AddToQueue_Validate(TSubclassOf<ABaseUnit> NewUnit)
{
	return true;
}

void ABaseProductionBuilding::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseProductionBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsBuilding && UnitToConstruct && GetWorld()->IsServer())
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ARTSBasePlayerController* PC = Cast<ARTSBasePlayerController>(*Iterator);
			if (PC)
			{
				if (PC->TeamNumber == TeamNumber)
				{
					CurrentProgress += PC->CalculateEffeciency(BuildRate * DeltaTime);
					if (CurrentProgress > MoneyTarget)
					{
						CurrentProgress -= MoneyTarget;
						FinishBuilding();
					}
					break;
				}
			}
		}
	}
}

void ABaseProductionBuilding::RemoveFromQueue(TSubclassOf<ABaseUnit> NewUnit)
{
	BuildQueue.RemoveSingle(NewUnit);
}

void ABaseProductionBuilding::StartBuilding()
{
	if (BuildQueue.Num() > 0)
	{
		IsBuilding = true;
		UnitToConstruct = BuildQueue[0];
		ABaseUnit* TempOBJ = Cast<ABaseUnit>(UnitToConstruct->GetDefaultObject());
		MoneyTarget = TempOBJ->Cost;
	}
	else
	{
		IsBuilding = false;
		UnitToConstruct = nullptr;
		MoneyTarget = 0;
	}
}

void ABaseProductionBuilding::FinishBuilding()
{
	FActorSpawnParameters SpawnInfo;
	ABaseUnit* NewUnit = GetWorld()->SpawnActor<ABaseUnit>(
		UnitToConstruct,
		GetActorLocation() + UnitSpawnPoint,
		GetActorRotation(),
		SpawnInfo);
	if (NewUnit)
	{
		NewUnit->TeamNumber = TeamNumber;
		NewUnit->SpawnDefaultController();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set team of new player"));
	}
	BuildQueue.RemoveSingle(UnitToConstruct);
	if (InfiniteQueue)
	{
		BuildQueue.Add(UnitToConstruct);
	}
	StartBuilding();
}

