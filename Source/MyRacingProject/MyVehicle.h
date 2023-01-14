// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "CheckpointBox.h"
#include "MyVehicle.generated.h"


UCLASS()
class MYRACINGPROJECT_API AMyVehicle : public AWheeledVehicle
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	AMyVehicle();

	virtual void Tick(float DeltaTime) override; // called every frame 

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void HandleTimerStartPause(float Val);

	// Moving forward / right
	void ApplyThrottle(float Val); // Val = how much force we want to apply
	void ApplySteering(float Val);

	void LookUp(float Val);
	void Turn(float Val);

	void Reset();
	void Redress();

	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void setRespawn();
	FVector getRespawnLocation();
	FRotator getRespawnRotation();

	void endGame();
	void startTimer();

	void UpdateHUDStrings();


	// Timer variables
	UPROPERTY(Category = bool, EditDefaultsOnly, BlueprintReadOnly)
		bool timerStarted;
	UPROPERTY(Category = bool, EditDefaultsOnly, BlueprintReadOnly)
		bool timerPaused;
	UPROPERTY(Category = Timer, EditDefaultsOnly, BlueprintReadOnly)
		int seconds;
	// Camera variables
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

	// Tire variables
	UPROPERTY(Category = Tire, EditDefaultsOnly, BlueprintReadOnly)
		float MinTireLoad;
	UPROPERTY(Category = Tire, EditDefaultsOnly, BlueprintReadOnly)
		float MinTireLoadFiltered = 0.2f;
	UPROPERTY(Category = Tire, EditDefaultsOnly, BlueprintReadOnly)
		float MaxTireLoad = 2.0f;
	UPROPERTY(Category = Tire, EditDefaultsOnly, BlueprintReadOnly)
		float MaxTireLoadFiltered = 2.0f;

	// Torque variables
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float MaxEngineRPM = 5700.0f;
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float FirstTorqueMin = 0.0f;
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float FirstTorqueMax = 400.0f;
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float SecondTorqueMin = 1890.0f;
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float SecondTorqueMax = 500.0f;
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float ThirdTorqueMin = 5730.0f;
	UPROPERTY(Category = Torque, EditDefaultsOnly, BlueprintReadOnly)
		float ThirdTorqueMax = 400.0f;

	// Steering variables
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float FirstSteeringMin = 0.0f;
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float FirstSteeringMax = 1.0f;
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float SecondSteeringMin = 40.0f;
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float SecondSteeringMax = 0.7f;
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float ThirdSteeringMin = 120.0f;
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float ThirdSteeringMax = 0.6f;
	UPROPERTY(Category = Steering, EditDefaultsOnly, BlueprintReadOnly)
		float FrontRearSplit = 0.40f;

	// Gearbox variables
	UPROPERTY(Category = Gearbox, EditDefaultsOnly, BlueprintReadOnly)
		bool GearAutoBox = true;
	UPROPERTY(Category = Gearbox, EditDefaultsOnly, BlueprintReadOnly)
		float GearSwitchTime = 0.15f;
	UPROPERTY(Category = Gearbox, EditDefaultsOnly, BlueprintReadOnly)
		float GearAutoBoxLatency = 1.0f;

	UPROPERTY()
		int checkpointNB = 0;
	
	bool InReverseGear;
	FText SpeedDisplay;
	FText GearDisplay;
	FText TimerDisplay;
	FText CheckPointDisplay;
	FColor GearDisplayColor;
	FColor GearDisplayReverseColor;

private:
	FVector RespawnLocation;
	FRotator RespawnRotation;
};

