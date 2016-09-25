// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "UnrealNetwork.h"
#include "CanTakeDamage.h"
#include "RTSBasePlayerController.h"

ARTSBasePlayerController::ARTSBasePlayerController()
{
	Money = 1000;
	MaxMoney = Money;
}

void ARTSBasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float MoneyToAdd = DeltaTime * CalculateIncome();
	if (Money + MoneyToAdd < MaxMoney)
	{
		Money += MoneyToAdd;
	}
	else
	{
		Money = MaxMoney;
	}
}

void ARTSBasePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ARTSBasePlayerController, Money);
	DOREPLIFETIME(ARTSBasePlayerController, MaxMoney);
}

float ARTSBasePlayerController::CalculateEffeciency(float MaxCost)
{
	if (Money - MaxCost > 0)
	{
		Money -= MaxCost;
		return MaxCost;
	}
	else if (Money > 0)
	{	
		float temp = Money;
		Money = 0;
		return temp;
	}
	else
	{
		return 0;
	}

}

void ARTSBasePlayerController::SetTeamNumber_Implementation(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}

bool ARTSBasePlayerController::SetTeamNumber_Validate(int32 NewTeamNumber)
{
	return true;
}

void ARTSBasePlayerController::NewEconomyBuilding_Implementation(ABaseEconomyBuilding* NewEconomyBuilding)
{
	EconomyBuildings.Add(NewEconomyBuilding);
}

bool ARTSBasePlayerController::NewEconomyBuilding_Validate(ABaseEconomyBuilding* NewEconomyBuilding)
{
	return true;
}

float ARTSBasePlayerController::CalculateIncome() 
{
	float TotalIncome = 0;
	for (auto& EconomyBuilding : EconomyBuildings)
	{
		if (EconomyBuilding->ConstructionFinished && !ICanTakeDamage::Execute_GetIsDead(EconomyBuilding))
		{
			TotalIncome += EconomyBuilding->MoneyRate;
		}
	}
	return TotalIncome;
}