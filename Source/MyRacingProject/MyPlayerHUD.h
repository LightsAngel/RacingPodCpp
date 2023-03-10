// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYRACINGPROJECT_API AMyPlayerHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	AMyPlayerHUD();
	virtual void DrawHUD() override;
	UFont* HUDFont;

};
