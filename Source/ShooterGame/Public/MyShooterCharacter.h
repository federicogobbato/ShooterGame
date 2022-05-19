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

	//==================//
	//Fields
	//==================//

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	uint32 bPressedTeleport:1;

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	uint32 bPressedRewindTime:1;

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	bool bRewindCharging = false;

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	bool bRewindTimeRunning = false;

	UPROPERTY(ReplicatedUsing = OnRep_HidePlayer)
	bool HiddenPlayer;


	//==================//
	//Overridden methods
	//==================//

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	//==================//
	//Ability methods
	//==================//

protected:

	void OnTeleport();

	void OnRewindTime();

public:

	void OnStartRewindTime();

	void OnEndRewindTime();

	UFUNCTION()
	/* Called when the variable HiddenPlayer change.
	   The player is hidden during the time the RewindTime ability is played.*/
	void OnRep_HidePlayer();
};
