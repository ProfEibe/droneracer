// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneRacer.h"
#include "HUDBlueprintLibrary.h"
#include "WidgetLayoutLibrary.h"

//Todo: https://forums.unrealengine.com/showthread.php?59398-Easy-Offscreen-Indicator-Blueprint-Node&p=489106&viewfull=1#post489106


void UHUDBlueprintLibrary::FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const FVector2D& InViewportSize, const FVector2D& InOffset, const int PlayerID, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D m_ScreenPosition = FVector2D();

	const FVector2D m_ViewportSize = InViewportSize;// FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  m_ViewportCenter = FVector2D(m_ViewportSize.X / 2, m_ViewportSize.Y / 2);

	float m_invert = 1.0f;
	float m_rotateInvert = 0.0f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);

	if (!World) return;

	APlayerController* m_PlayerController = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, PlayerID) : NULL);
	ACharacter *m_PlayerCharacter = static_cast<ACharacter *> (m_PlayerController->GetPawn());

	if (!m_PlayerCharacter) return;

	FVector m_PlayerLocation = m_PlayerCharacter->GetActorLocation();

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(m_PlayerController, InLocation, m_ScreenPosition);

	//PlayerController->ProjectWorldLocationToScreen(InLocation, *m_ScreenPosition);

	m_ScreenPosition += InOffset;

	if (m_ScreenPosition.X == 0.0f)
	{
		FVector location = FMath::Lerp(InLocation, m_PlayerLocation, 10.0f);
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(m_PlayerController, location, m_ScreenPosition);
		//m_PlayerController->ProjectWorldLocationToScreen(location, m_ScreenPosition);
		m_ScreenPosition += InOffset;
		m_invert = -1.0f;
		m_rotateInvert = 180.0f;
	}
	else
	{
		// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
		if (m_ScreenPosition.X >= 0 && m_ScreenPosition.X <= m_ViewportSize.X 
			&& m_ScreenPosition.Y >= 0 && m_ScreenPosition.Y <= m_ViewportSize.Y)
		{
			OutScreenPosition = m_ScreenPosition;
			OutRotationAngleDegrees = 0.0f;
			bIsOnScreen = true;
			return;
		}
	}

	m_ScreenPosition -= m_ViewportCenter;

	float m_AngleRadians = FMath::Atan2(m_ScreenPosition.Y, m_ScreenPosition.X);
	m_AngleRadians -= FMath::DegreesToRadians(90.f);

	OutRotationAngleDegrees = FMath::RadiansToDegrees(m_AngleRadians) + 180.f + m_rotateInvert;

	float m_cos = cosf(m_AngleRadians);
	float m_sin = -sinf(m_AngleRadians);

	//*m_ScreenPosition = FVector2D(m_ViewportCenter.X + (m_sin * 150.f), m_ViewportCenter.Y + m_cos * 150.f);

	float m = m_cos / m_sin;

	FVector2D ScreenBounds = m_ViewportCenter * EdgePercent;

	if (m_cos > 0)
	{
		m_ScreenPosition = FVector2D(ScreenBounds.Y / m, ScreenBounds.Y);
	}
	else
	{
		m_ScreenPosition = FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y);
	}

	if (m_ScreenPosition.X > ScreenBounds.X)
	{
		m_ScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X*m);
	}
	else if (m_ScreenPosition.X < -ScreenBounds.X)
	{
		m_ScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X*m);
	}

	m_ScreenPosition *= m_invert;
	m_ScreenPosition += m_ViewportCenter;

	OutScreenPosition = m_ScreenPosition;
}
