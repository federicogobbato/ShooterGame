// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "MyShooterCharacter.h"


AMyShooterCharacter::AMyShooterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMyShooterCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
}


void AMyShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMyShooterCharacter::OnTeleport);
	PlayerInputComponent->BindAction("Rewind", IE_Pressed, this, &AMyShooterCharacter::OnRewindTime);
}


void AMyShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMyShooterCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	if (bPressedRewindTime)
	{
		Super::OnCameraUpdate(CameraLocation, GetActorRotation());
	}
	else 
	{
		Super::OnCameraUpdate(CameraLocation, CameraRotation);
	}	
}


void AMyShooterCharacter::AddControllerPitchInput(float Val)
{
	if (!bPressedRewindTime)
	{
		Super::AddControllerPitchInput(Val);
	}
}

void AMyShooterCharacter::AddControllerYawInput(float Val)
{
	if (!bPressedRewindTime)
	{
		Super::AddControllerYawInput(Val);
	}
}


void AMyShooterCharacter::OnTeleport()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !bPressedTeleport)
	{
		bPressedTeleport = true;
	}
}


void AMyShooterCharacter::OnRewindTime()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && 
		!bPressedRewindTime && !bRewindCharging)
	{
		MyPC->DisableInput(MyPC);

		bPressedRewindTime = true;
	}
}


void AMyShooterCharacter::OnStartRewindTime()
{
	if (IsLocallyControlled())
	{
		AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
		if (MyPC)
		{
			DisableInput(MyPC);
			//disable only colliders
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		//disable collider and visibility 
	}
}


void AMyShooterCharacter::OnEndRewindTime()
{
	if (IsLocallyControlled())
	{
		AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
		if (MyPC)
		{
			//enable input
			EnableInput(MyPC);

			//enable only colliders
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		//enable colliders and visibility 
	}
}
