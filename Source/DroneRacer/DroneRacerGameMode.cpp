// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DroneRacer.h"
#include "DroneRacerGameMode.h"
#include "DroneRacerPawn.h"

ADroneRacerGameMode::ADroneRacerGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = ADroneRacerPawn::StaticClass();
}
