// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "MyShooterCharacter.h"
#include "Weapons/ShooterWeapon.h"


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

void AMyShooterCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate HiddenPlayer.
	DOREPLIFETIME(AMyShooterCharacter, HiddenPlayer);
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
		DisableInput(MyPC);
		bPressedRewindTime = true;
	}
}


void AMyShooterCharacter::OnStartRewindTime()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		HiddenPlayer = true;
	}
	
	if(IsLocallyControlled())
	{
		SetActorEnableCollision(false);
	}
}


void AMyShooterCharacter::OnEndRewindTime()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		HiddenPlayer = false;
	}

	if (IsLocallyControlled())
	{
		AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
		if (MyPC)
		{
			//enable input
			EnableInput(MyPC);
		}

		SetActorEnableCollision(true);
	}
}


void AMyShooterCharacter::OnRep_HidePlayer()
{
	if (!IsLocallyControlled())
	{
		SetActorHiddenInGame(HiddenPlayer);
		CurrentWeapon->SetActorHiddenInGame(HiddenPlayer);
		SetActorEnableCollision(!HiddenPlayer);
	}
}
