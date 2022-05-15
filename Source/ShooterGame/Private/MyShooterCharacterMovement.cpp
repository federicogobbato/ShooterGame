// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "MyShooterCharacterMovement.h"


UMyShooterCharacterMovement::UMyShooterCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UMyShooterCharacterMovement::DoTeleport()
{
	AShooterCharacter* myCharacterOwner = Cast<AShooterCharacter>(CharacterOwner);

	if (myCharacterOwner && myCharacterOwner->bPressedTeleport)
	{
		myCharacterOwner->bPressedTeleport = false;

		TeleportDestination = CharacterOwner->GetActorLocation() + CharacterOwner->GetActorForwardVector() * TeleportDistance;
		CharacterOwner->SetActorLocation(TeleportDestination, true);

		if (CharacterOwner->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
		{
			ServerDoTeleport(TeleportDestination);
		}
	}
}


void UMyShooterCharacterMovement::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
	Super::ControlledCharacterMove(InputVector, DeltaSeconds);

	DoTeleport();
}


void UMyShooterCharacterMovement::ServerDoTeleport_Implementation(FVector TeleportLocation)
{
	TeleportDestination = TeleportLocation;

	DoTeleport();
}


FNetworkPredictionData_Client* UMyShooterCharacterMovement::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UMyShooterCharacterMovement* MutableThis = const_cast<UMyShooterCharacterMovement*>(this);
		MutableThis->ClientPredictionData = new FMyNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}


void UMyShooterCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Cast<AShooterCharacter>(CharacterOwner)->bPressedTeleport = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
}


//======================================//
//NetworkPredictionData_Client_Character
//======================================//

FMyNetworkPredictionData_Client_Character::FMyNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{
}


FSavedMovePtr FMyNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FMySavedMove_Character());
}


//======================================//
//SavedMove_Character
//======================================//

uint8 FMySavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (bPressedTeleportSaved)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}


void FMySavedMove_Character::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	bPressedTeleportSaved = Cast<AShooterCharacter>(Character)->bPressedTeleport;
}


void FMySavedMove_Character::Clear()
{
	FSavedMove_Character::Clear();

	bPressedTeleportSaved = false;
}