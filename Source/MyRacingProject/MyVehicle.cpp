// Fill out your copyright notice in the Description page of Project Settings.


#include "MyVehicle.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WheeledVehicleMovementcomponent4w.h"
#include "MyRacingProjectWheelFront.h"
#include "MyRacingProjectWheelRear.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerHUD.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printf(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))
#define LOCTEXT_NAMESPACE "VehiclePawn"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

AMyVehicle::AMyVehicle()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());


	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = UMyRacingProjectWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UMyRacingProjectWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UMyRacingProjectWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UMyRacingProjectWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);




	// Adjust the tire loading
	Vehicle4W->MinNormalizedTireLoad = MinTireLoad;
	Vehicle4W->MinNormalizedTireLoadFiltered = MinTireLoadFiltered;
	Vehicle4W->MaxNormalizedTireLoad = MaxTireLoad;
	Vehicle4W->MaxNormalizedTireLoadFiltered = MaxTireLoadFiltered;

	// Torque setup
	Vehicle4W->MaxEngineRPM = MaxEngineRPM;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(FirstTorqueMin, FirstTorqueMax);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(SecondTorqueMin, SecondTorqueMax);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(ThirdTorqueMin, ThirdTorqueMax);

	// Adjust the steering
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(FirstSteeringMin, FirstSteeringMax);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(SecondSteeringMin, SecondSteeringMax);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(ThirdSteeringMin, ThirdSteeringMax);

	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Vehicle4W->DifferentialSetup.FrontRearSplit = FrontRearSplit;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = GearAutoBox;
	Vehicle4W->TransmissionSetup.GearSwitchTime = GearSwitchTime;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = GearAutoBoxLatency;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bUsePawnControlRotation = true;

	timerStarted = false;
	timerPaused = false;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.0f;

	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);
}

static FTimerHandle _timer;
void AMyVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Setup the flag to say we are in reverse gear
	InReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();
}

void AMyVehicle::BeginPlay()
{
	Super::BeginPlay();
	setRespawn();
}

void AMyVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Throttle", this, &AMyVehicle::ApplyThrottle);
	PlayerInputComponent->BindAxis("Steer", this, &AMyVehicle::ApplySteering);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyVehicle::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AMyVehicle::Turn);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AMyVehicle::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AMyVehicle::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("Reset", IE_Released, this, &AMyVehicle::Reset);
	PlayerInputComponent->BindAction("Escape", IE_Released, this, &AMyVehicle::endGame);
}

void AMyVehicle::HandleTimerStartPause(float Val)
{
	if (timerStarted == false && Val != 0)
	{
		GetWorldTimerManager().SetTimer(_timer, this, &AMyVehicle::startTimer, 1, true);
		timerStarted = true;
	}
	else if (timerPaused == true && Val != 0)
	{
		timerPaused = false;
		GetWorldTimerManager().UnPauseTimer(_timer);
	}
}

void AMyVehicle::ApplyThrottle(float Val)
{
	HandleTimerStartPause(Val);
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AMyVehicle::ApplySteering(float Val)
{
	HandleTimerStartPause(Val);
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AMyVehicle::LookUp(float Val)
{
	if (Val != 0.f)
	{
		AddControllerPitchInput(Val * -1);
	}
}

void AMyVehicle::Reset()
{
	if (FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f >= 150) {
		checkpointNB = 15;
		endGame();
	}
	else {
		FName levelname = GetWorld()->GetFName();
		UGameplayStatics::OpenLevel(GetWorld(), levelname, true);
	}

}

void AMyVehicle::Redress()
{
	this->SetActorRotation(FRotator::ZeroRotator);
}


void AMyVehicle::Turn(float Val)
{
	if (Val != 0.f)
	{
		AddControllerYawInput(Val);
	}
}

void AMyVehicle::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AMyVehicle::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AMyVehicle::setRespawn()
{
	RespawnLocation = this->GetActorLocation();
	RespawnRotation = this->GetActorRotation();
}

FVector AMyVehicle::getRespawnLocation()
{
	return FVector(RespawnLocation);
}

FRotator AMyVehicle::getRespawnRotation()
{
	return FRotator(RespawnRotation);
}

void AMyVehicle::endGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuMap"), true);
}

void AMyVehicle::startTimer()
{
	seconds++;
}


//For the HUD


void AMyVehicle::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int KPH_int = FMath::FloorToInt(KPH);
	int Minutes = seconds / 60;
	int Seconds = seconds % 60;
	// Using FText because this is display text that should be localizable
	TimerDisplay = FText::Format(LOCTEXT("TimeFormat", "Timer: {1}:{0}"), FText::AsNumber(Seconds), FText::AsNumber(Minutes));
	CheckPointDisplay = FText::Format(LOCTEXT("SpeedFormat", "CheckPoint: {0}/15"), FText::AsNumber(checkpointNB));
	SpeedDisplay = FText::Format(LOCTEXT("SpeedFormat", "Speed: {0} km/h"), FText::AsNumber(KPH_int));

	if (InReverseGear == true)
	{
		GearDisplay = FText(LOCTEXT("ReverseGear", "Gear :R"));
	}
	else
	{
		int Gear = GetVehicleMovement()->GetCurrentGear();
		if (Gear == 0) {
			GearDisplay = FText(LOCTEXT("ReverseGear", "Gear: N"));
		}
		else {
			GearDisplay = FText::Format(LOCTEXT("SpeedFormat", "Gear: {0}"), FText::AsNumber(Gear));
		}
	}
}
