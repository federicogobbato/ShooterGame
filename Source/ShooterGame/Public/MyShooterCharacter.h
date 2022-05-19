// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShooterCharacter.h"
#include "MyShooterCharacter.generated.h"



UCLASS()
class SHOOTERGAME_API AMyShooterCharacter : public AShooterCharacter
{
	GENERATED_UCLASS_BODY()
	

public:

	UPROPERTY(BlueprintReadOnly, Category = Custom)
	uint32 bPressedTeleport:1;

	UPROPERTY(BlueprintReadOnly, Category = Custom)
	bool bPressedRewindTime = false;

	UPROPERTY(BlueprintReadOnly, Category = Custom)
	bool bRewindCharging = false;

	UPROPERTY(BlueprintReadOnly, Category = Custom)
	uint32 bRewindTimeRunning : 1;


	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void OnTeleport();

	void OnRewindTime();



	void OnStartRewindTime();

	void OnEndRewindTime();

	UPROPERTY(ReplicatedUsing = OnRep_HidePlayer)
	bool HiddenPlayer;

	UFUNCTION()
	void OnRep_HidePlayer();
};
