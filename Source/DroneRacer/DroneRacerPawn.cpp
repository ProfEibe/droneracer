// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DroneRacer.h"
#include "DroneRacerPawn.h"

ADroneRacerPawn::ADroneRacerPawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	TiltSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 0.f;
	CurrentRightwardSpeed = 0.f;
	CurrentUpwardSpeed = 0.f;
}

void ADroneRacerPawn::Tick(float DeltaSeconds)
{
	FVector worldOffset;

	//Move Forward and Right just on XY-Plane and ignore the z-value
	FVector direction = GetActorForwardVector();
	direction.Z = 0.f;
	direction.Normalize();
	worldOffset = direction * GetActorRotation().Pitch * -Acceleration * DeltaSeconds;

	direction = GetActorRightVector();
	direction.Z = 0.f;
	direction.Normalize();
	worldOffset += direction * CurrentRightwardSpeed * DeltaSeconds;

	direction = GetActorUpVector();
	worldOffset += direction * CurrentUpwardSpeed * DeltaSeconds;

	AddActorWorldOffset(worldOffset, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);
	//AddActorWorldRotation(DeltaRotation);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void ADroneRacerPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation(RootComponent);
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void ADroneRacerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	InputComponent->BindAxis("Throttle", this, &ADroneRacerPawn::Throttle);
	InputComponent->BindAxis("Yaw", this, &ADroneRacerPawn::Yaw);
	InputComponent->BindAxis("Pitch", this, &ADroneRacerPawn::Pitch);
	InputComponent->BindAxis("Roll", this, &ADroneRacerPawn::Roll);
}

void ADroneRacerPawn::Throttle(float Val)
{
	CurrentUpwardSpeed = Val * 1000;
}

void ADroneRacerPawn::Yaw(float Val)
{
	CurrentYawSpeed = Val * TurnSpeed;
}

void ADroneRacerPawn::Pitch(float Val)
{
	// 20 is how much the drone will tilt (but no degrees)
	float TargetTilt = Val * 20.f;

	// difference between current rotation and target
	float TiltDifference = TargetTilt - GetActorRotation().Pitch;

	float TargetPitchSpeed = TiltDifference * 2.5;

	// Smoothly interpolate roll speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// New speed depends on how much the drone is tilted
	CurrentForwardSpeed = GetActorRotation().Pitch * -Acceleration;
}

void ADroneRacerPawn::Roll(float Val)
{
	// 20 is how much the drone will tilt (but no degrees)
	float TargetTilt = Val * 20.f;

	// difference between current rotation and target
	float TiltDifference = TargetTilt - GetActorRotation().Roll;

	float TargetRollSpeed = TiltDifference * 2.5;

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// New speed depends on how much the drone is tilted
	CurrentRightwardSpeed = GetActorRotation().Roll * Acceleration;
}
