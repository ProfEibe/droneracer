// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneRacer.h"
#include "MyActor.h"


// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//trigger = GetRootPrimitiveComponent();

	//trigger->OnComponentEndOverlap.AddDynamic(this, &AMyActor::OnEndOverlap);
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	//trigger->SetWorldRotation(FRotator(10, 10, 10));
	
}

void AMyActor::OnEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//trigger->SetWorldRotation(FRotator(10, 10, 10));
}
