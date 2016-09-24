// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "UnrealNetwork.h"
#include "RTSBasePlayerController.h"

ARTSBasePlayerController::ARTSBasePlayerController()
{
	Money = 1000;
	MaxMoney = Money;
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

