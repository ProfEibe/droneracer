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
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 0.f;
	CurrentRightwardSpeed = 0.f;
	CurrentUpwardSpeed = 0.f;
}

void ADroneRacerPawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, CurrentRightwardSpeed * DeltaSeconds, CurrentUpwardSpeed * DeltaSeconds);

	// Move plan forwards (with sweep so we stop when we collide with things)
	//AddActorLocalOffset(LocalMove, true);
	//AddActorLocalOffset(LocalMove, true);

	FVector vec = GetActorRightVector();
	vec.Z = 0.f;
	vec.Normalize();
	FVector final = vec * CurrentRightwardSpeed * DeltaSeconds;

	vec = GetActorForwardVector();
	vec.Z = 0.f;
	vec.Normalize();
	final += vec * CurrentForwardSpeed * DeltaSeconds;

	vec = GetActorUpVector();
	final += vec * CurrentUpwardSpeed * DeltaSeconds;

	AddActorWorldOffset(final, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

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

	// Bind our control axis' to callback functions
	//InputComponent->BindAxis("Thrust", this, &ADroneRacerPawn::ThrustInput);
	//InputComponent->BindAxis("MoveUp", this, &ADroneRacerPawn::MoveUpInput);
	//InputComponent->BindAxis("MoveRight", this, &ADroneRacerPawn::MoveRightInput);

	InputComponent->BindAxis("Throttle", this, &ADroneRacerPawn::Throttle);
	InputComponent->BindAxis("Yaw", this, &ADroneRacerPawn::Yaw);
	InputComponent->BindAxis("Pitch", this, &ADroneRacerPawn::Pitch);
	InputComponent->BindAxis("Roll", this, &ADroneRacerPawn::Roll);
}

void ADroneRacerPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void ADroneRacerPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ADroneRacerPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
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
	/*CurrentPitchSpeed = -(Val) * TurnSpeed;

	CurrentForwardSpeed = CurrentPitchSpeed;*/
	
	const bool bIsPitching = FMath::Abs(Val) > 0.2f;

	float TargetPitchSpeed = bIsPitching ? Val * TurnSpeed * 0.5f : GetActorRotation().Pitch * -2.f;
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	CurrentForwardSpeed = GetActorRotation().Pitch * -Acceleration;
}

void ADroneRacerPawn::Roll(float Val)
{
	//CurrentRollSpeed = Val * TurnSpeed;
	

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? ((Val * TurnSpeed) * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	//CurrentRightwardSpeed = bIsTurning ? CurrentRollSpeed*100 : 0.f;

	// 100 noch durch geeigneteren Wert ersetzen.
	CurrentRightwardSpeed = GetActorRotation().Roll * Acceleration;
}
