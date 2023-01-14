// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTrapUpDown.h"

FVector InitPos;
bool First = false;
// Sets default values
AMyTrapUpDown::AMyTrapUpDown()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyTrapUpDown::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyTrapUpDown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int AMyTrapUpDown::ActiveTrap(int i)
{
	if (First == false) {
		InitPos = this->GetActorLocation();
		First = true;
	}
	FVector WorldPos = this->GetActorLocation();
	if (i < 100) {
		WorldPos.Z--;
		this->SetActorLocation(WorldPos);
		i++;
	}
	else if (i < 200) {
		WorldPos.Z++;
		this->SetActorLocation(WorldPos);
		i++;
	}
	else {
		i = 0;
	}
	return i;
}

