// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerHUD.h"
#include "MyVehicle.h"
#include "WheeledVehicle.h"
#include "RenderResource.h"
#include "Shader.h"
#include "Engine/Canvas.h"
#include "WheeledVehicleMovementComponent.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

AMyPlayerHUD::AMyPlayerHUD()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Game/HUD/Font/Crotah_Font"));
	HUDFont = Font.Object;
	HUDFont->LegacyFontSize = 12;
}


void AMyPlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	// Calculate ratio from 720p
	const float HUDXRatio = Canvas->SizeX / 1920.f;
	const float HUDYRatio = Canvas->SizeY / 1080.f;

	bool bWantHUD = true;
#if HMD_MODULE_INCLUDED
	bWantHUD = !GEngine->IsStereoscopic3D();
#endif // HMD_MODULE_INCLUDED
	// We dont want the onscreen hud when using a HMD device	
	if (bWantHUD == true)
	{
		// Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
		AMyVehicle* Vehicle = Cast<AMyVehicle>(GetOwningPawn());
		if (Vehicle != nullptr)
		{
			FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

			// Timer
			FCanvasTextItem TimerTextItem(FVector2D(HUDXRatio * 1700.f, HUDYRatio * 60.f), Vehicle->TimerDisplay, HUDFont, FLinearColor::White);
			TimerTextItem.Scale = ScaleVec;
			Canvas->DrawItem(TimerTextItem);

			// CheckPoint
			FCanvasTextItem CheckPointTextItem(FVector2D(HUDXRatio * 1700.f, HUDYRatio * 100.f), Vehicle->CheckPointDisplay, HUDFont, FLinearColor::White);
			CheckPointTextItem.Scale = ScaleVec;
			Canvas->DrawItem(CheckPointTextItem);

			// Speed
			FCanvasTextItem SpeedTextItem(FVector2D(HUDXRatio * 1700.f, HUDYRatio * 800.f), Vehicle->SpeedDisplay, HUDFont, FLinearColor::White);
			SpeedTextItem.Scale = ScaleVec;
			Canvas->DrawItem(SpeedTextItem);

			// Gear
			FCanvasTextItem GearTextItem(FVector2D(HUDXRatio * 1700.f, HUDYRatio * 850.f), Vehicle->GearDisplay, HUDFont, Vehicle->InReverseGear == false ? Vehicle->GearDisplayColor : Vehicle->GearDisplayReverseColor);
			GearTextItem.Scale = ScaleVec;
			Canvas->DrawItem(GearTextItem);
		}
	}
}