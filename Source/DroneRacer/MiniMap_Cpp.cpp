// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneRacer.h"
#include "MiniMap_Cpp.h"


// Sets default values
AMiniMap_Cpp::AMiniMap_Cpp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMiniMap_Cpp::BeginPlay()
{
	Super::BeginPlay();
	USceneCaptureComponent2D* sceneCaptureComponent = FindComponentByClass<USceneCaptureComponent2D>();
	sceneCaptureComponent->TextureTarget->ClearColor = FLinearColor::Transparent;
}

// Called every frame
void AMiniMap_Cpp::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

