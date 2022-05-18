// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "MyShooterCharacterMovement.h"


//==================//
//OVERRIDDEN METHODS
//==================//

UMyShooterCharacterMovement::UMyShooterCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UMyShooterCharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	MyCharacterOwner = Cast<AMyShooterCharacter>(CharacterOwner);

	DelayBetweenFrames = RewindTimeDuration / (1.0f / RewindTimeSpeedUp);
}


//void UMyShooterCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}


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

	MyCharacterOwner->bPressedTeleport = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);

	MyCharacterOwner->bPressedRewindTime = ((Flags & FSavedMove_Character::FLAG_Custom_1) != 0);
}


void UMyShooterCharacterMovement::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	DoTeleport();

	DoRewind();
}


//==================//
//TELEPORT
//==================//

void UMyShooterCharacterMovement::DoTeleport()
{
	if (MyCharacterOwner && MyCharacterOwner->bPressedTeleport)
	{
		MyCharacterOwner->bPressedTeleport = false;

		FVector teleportDestination = CharacterOwner->GetActorLocation() + CharacterOwner->GetActorForwardVector() * TeleportDistance;
		
		CharacterOwner->SetActorLocation(teleportDestination, true);
		//CharacterOwner->TeleportTo(teleportDestination, CharacterOwner->GetActorRotation());
	}
}

//==================//
//REWIND
//==================//

void UMyShooterCharacterMovement::DoRewind()
{
	if (MyCharacterOwner)
	{
		if (MyCharacterOwner->bPressedRewindTime)
		{
			if (!MyCharacterOwner->bRewindTimeRunning) 
			{
				MyCharacterOwner->bRewindTimeRunning = true;
				MyCharacterOwner->OnStartRewindTime();
			}

			if (RewindFrames.Num() > 0)
			{
				//Set next rewind position
				FRewindData& nextRewindFrame = RewindFrames.Last();
				RewindFrames.RemoveSingle(nextRewindFrame);

				//Move the player
				MyCharacterOwner->SetActorLocationAndRotation(nextRewindFrame.Position, nextRewindFrame.Rotation);
			}
			else 
			{
				MyCharacterOwner->bPressedRewindTime = false;
			}
		}
		else
		{
			if (MyCharacterOwner->bRewindTimeRunning)
			{
				MyCharacterOwner->bRewindTimeRunning = false;
				MyCharacterOwner->bRewindCharging = true;
				MyCharacterOwner->OnEndRewindTime();

				FTimerHandle rechargeAbilityTimer;
				FTimerDelegate rechargeAbilityDelegate;
				rechargeAbilityDelegate.BindLambda([&]() { MyCharacterOwner->bRewindCharging = false; });

				GetWorld()->GetTimerManager().SetTimer(rechargeAbilityTimer, rechargeAbilityDelegate, RewindTimeDuration, false);
			}
			else
			{
				RechargeRewind();
			}
		}
	}
}


void UMyShooterCharacterMovement::RechargeRewind() 
{
	//Populate the RewindFrames Queue
	FRewindData newData;
	newData.CaptureTime = GetWorld()->GetTimeSeconds();
	newData.Position = MyCharacterOwner->GetActorLocation();
	newData.Rotation = MyCharacterOwner->GetActorRotation();

	if (RewindFrames.Num() > 0)
	{
		FRewindData& head = RewindFrames[0];
		// Remove a Frame from the queue
		if (newData.CaptureTime - head.CaptureTime >= RewindTimeDuration)
		{
			RewindFrames.RemoveSingle(head);
		}
	}

	RewindFrames.Add(newData);
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

	if (bPressedRewindTimeSaved)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}


void FMySavedMove_Character::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	bPressedTeleportSaved = Cast<AMyShooterCharacter>(Character)->bPressedTeleport;

	bPressedRewindTimeSaved = Cast<AMyShooterCharacter>(Character)->bRewindTimeRunning;
}


void FMySavedMove_Character::Clear()
{
	FSavedMove_Character::Clear();

	bPressedTeleportSaved = false;

	bPressedRewindTimeSaved = false;
}