// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShooterCharacterMovement.h"
#include "MyShooterCharacterMovement.generated.h"

/**
 * 
 */

UCLASS()
class SHOOTERGAME_API UMyShooterCharacterMovement : public UShooterCharacterMovement
{
	GENERATED_UCLASS_BODY()
	
public:

	virtual void DoTeleport();

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	FORCEINLINE FVector GetTeleportLocation() const { return TeleportDestination; }

	FORCEINLINE void SetTeleportLocation(FVector location) { TeleportDestination = location; }

protected:

	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

private:

	FVector TeleportDestination;

	UPROPERTY(EditDefaultsOnly)
	float TeleportDistance = 1000.0f;

	//Teleport destination is sent to the server to update the others client and convalidate the teleport
	UFUNCTION(reliable, server)
	void ServerDoTeleport(FVector TeleportLocation);

};


class FMyNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{

public:

	FMyNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};


class FMySavedMove_Character : public FSavedMove_Character
{

public:

	uint32 bPressedTeleportSaved : 1;

	virtual uint8 GetCompressedFlags() const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void Clear() override;
};