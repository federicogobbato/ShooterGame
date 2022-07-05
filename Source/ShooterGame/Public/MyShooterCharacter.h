// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShooterCharacter.h"
#include "MyShooterCharacter.generated.h"



UCLASS()
class SHOOTERGAME_API AMyShooterCharacter : public AShooterCharacter
{
	GENERATED_UCLASS_BODY()

//==================//
//Fields
//==================//

protected:

	UPROPERTY(Replicated)
	bool bPlayerShrinked = false;

	/* Used to reset the original scale of the player */
	FTimerHandle ShrinkTimer;

	/* Scale of the player before is shrinked*/
	FVector ScaleBeforeShrink;

	UPROPERTY(EditDefaultsOnly, Category = CustomAbilities)
	float ShrinkEffectDuration = 10.0f;

public:

	uint32 bPressedTeleport : 1;

	uint32 bPressedRewindTime:1;

	/* Identify when the ability rewind time is charging, if it's charging can't be used */
	bool bRewindCharging = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerHidden)
	/* Set when the player is not visible to the other players */
	bool bPlayerHidden = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerShrinkedScale)
	/* The scale of the player when is shrinked */
	FVector PlayerShrinkedScale;


//==================//
//Overridden methods
//==================//

public:

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/* Detect when a damage is inflicted by a gun that has the ability to shrink a player.
	*  Detect when a ShooterCharacter stomps the current player, when is shrinked. */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	/* If the player is shrinked we reset the original scale before die */
	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) override;

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
	   The player is not visible to the other players, when the RewindTime ability is played.*/
	void OnRep_PlayerHidden();


	void OnStartShrink();

	void OnEndShrink();

	UFUNCTION()
	/* Detect if this player collide with a shrinked player. */
	void OnCharacterCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	/* Called when the player must be scaled */
	void OnRep_PlayerShrinkedScale();

	FORCEINLINE bool GetPlayerShrinked() const { return bPlayerShrinked; };
};
