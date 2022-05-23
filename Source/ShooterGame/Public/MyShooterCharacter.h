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

	bool bPlayerShrinked = false;

	FTimerHandle ShrinkTimer;

	UPROPERTY(EditDefaultsOnly, Category = CustomAbilities)
	float ShrinkMultiplier = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = CustomAbilities)
	float ShrinkDuration = 10.0f;

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	FVector ScaleBeforeShrink;

public:

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	uint32 bPressedTeleport:1;

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	uint32 bPressedRewindTime:1;

	UPROPERTY(BlueprintReadOnly, Category = CustomAbilities)
	bool bRewindCharging = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerHidden)
	bool bPlayerHidden = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerShrinkedScale)
	FVector PlayerShrinkedScale;


//==================//
//Overridden methods
//==================//

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

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


	void OnResetShrink();

	UFUNCTION()
	void OnCharacterCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnRep_PlayerShrinkedScale();
};
