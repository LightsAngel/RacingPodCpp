// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointBox.h"
#include <DrawDebugHelpers.h>
#include "MyVehicle.h"



// Sets default values
ACheckpointBox::ACheckpointBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnActorBeginOverlap.AddDynamic(this, &ACheckpointBox::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ACheckpointBox::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ACheckpointBox::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Black, true, -1, 0, 5);
}

// Called every frame
void ACheckpointBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpointBox::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (AMyVehicle* Vehicle = Cast<AMyVehicle>(OtherActor))
	{
		if (Vehicle->checkpointNB == 15) {
			Vehicle->endGame();
		}
		else {
			Vehicle->checkpointNB++;
		}
	}
}

void ACheckpointBox::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
	FVector pos = this->GetActorLocation();
	pos.Z = -1500;
	this->SetActorLocation(pos);
}

