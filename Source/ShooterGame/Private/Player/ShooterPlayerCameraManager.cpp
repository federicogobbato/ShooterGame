// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Player/ShooterPlayerCameraManager.h"


AShooterPlayerCameraManager::AShooterPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 60.0f;
	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}


void AShooterPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	AShooterCharacter* MyPawn = PCOwner ? Cast<AShooterCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn && MyPawn->IsFirstPerson())
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn && MyPawn->IsFirstPerson())
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
	}
}


//==================//
//Custom fields
//==================//

void AShooterPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	AMyShooterCharacter* MyPawn = Cast<AMyShooterCharacter>(PCOwner->GetPawn());

	if (MyPawn)
	{
		if (MyPawn->bPressedRewindTime)
		{
			//The rotation of the camera is based on the orientation of the vector forward of the player 
			OutVT.POV.Location = MyPawn->GetPawnViewLocation();
			OutVT.POV.Rotation = MyPawn->GetActorForwardVector().Rotation();
		}
		else if (MyPawn->GetPlayerShrinked())
		{
			OutVT.POV.Location = MyPawn->GetPawnViewLocation();
			OutVT.POV.Rotation = MyPawn->GetViewRotation();
		}
		else
		{
			Super::UpdateViewTargetInternal(OutVT, DeltaTime);
		}
	}
}