// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyRacingProjectGameMode.h"
#include "MyVehicle.h"
#include "MyPlayerHUD.h"

AMyRacingProjectGameMode::AMyRacingProjectGameMode()
{
	DefaultPawnClass = AMyVehicle::StaticClass();
	HUDClass = AMyPlayerHUD::StaticClass();
}
