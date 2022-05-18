// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShooterCharacterMovement.h"
#include "MyShooterCharacterMovement.generated.h"


USTRUCT()
struct FRewindData
{
	GENERATED_BODY()

	float CaptureTime;
	FVector Position;
	FRotator Rotation;

	inline bool operator == (const FRewindData& other) const
	{
		return ((other.CaptureTime == CaptureTime) &&
				(other.Position == Position) &&
				(other.Rotation == Rotation));
	}
};


UCLASS()
class SHOOTERGAME_API UMyShooterCharacterMovement : public UShooterCharacterMovement
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;

	//virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:

	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void DoTeleport();

	virtual void DoRewind();

	virtual void RechargeRewind();

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

private:

	AMyShooterCharacter* MyCharacterOwner;

	////FVector TeleportDestination;

	////FRewindData NextRewindPosition;

	TArray<FRewindData> RewindFrames;

	FVector LastRewindPosition;

	float DelayBetweenFrames = 0;

	UPROPERTY(EditDefaultsOnly)
	float TeleportDistance = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float RewindTimeDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float RewindTimeSpeedUp = 10.0f;

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

	uint32 bPressedTeleportSaved:1;

	uint32 bPressedRewindTimeSaved:1;

	virtual uint8 GetCompressedFlags() const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void Clear() override;
};