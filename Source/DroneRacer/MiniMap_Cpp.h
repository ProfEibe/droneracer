// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MiniMap_Cpp.generated.h"

UCLASS()
class DRONERACER_API AMiniMap_Cpp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMiniMap_Cpp();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
