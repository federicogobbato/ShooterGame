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

	virtual void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) override;

	virtual void AddControllerPitchInput(float Val) override;

	virtual void AddControllerYawInput(float Val) override;


	void OnTeleport();

	void OnRewindTime();

	void OnStartRewindTime();

	void OnEndRewindTime();

};
