// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "MyShooterCharacter.h"
#include "Weapons/ShooterWeapon.h"
#include <Weapons/ShooterWeapon_Instant.h>


//==================//
//Overridden methods
//==================//

AMyShooterCharacter::AMyShooterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMyShooterCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
}


void AMyShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyShooterCharacter::OnCharacterCollide);
}


bool AMyShooterCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (bPlayerShrinked)
	{
		OnEndShrink();
	}

	return Super::Die(KillingDamage, DamageEvent, Killer, DamageCauser);
}


void AMyShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMyShooterCharacter::OnTeleport);
	PlayerInputComponent->BindAction("Rewind", IE_Pressed, this, &AMyShooterCharacter::OnRewindTime);
}


void AMyShooterCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyShooterCharacter, bPlayerHidden);
	DOREPLIFETIME(AMyShooterCharacter, bPlayerShrinked);
	DOREPLIFETIME(AMyShooterCharacter, PlayerShrinkedScale);
}


float AMyShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (!bPlayerShrinked)
	{
		// Checks if the weapon can shrink a player.
		AShooterWeapon_Instant* weapon = Cast<AShooterWeapon_Instant>(DamageCauser);
		if (weapon && weapon->bCanShrink)
		{
			OnStartShrink();
		}
	}
	else
	{
		// Check if the damage come from another player that stomps it.
		AMyShooterCharacter* character = Cast<AMyShooterCharacter>(DamageCauser);
		if (character)
		{
			Die(Damage, DamageEvent, EventInstigator, DamageCauser);
			return Damage;
		}
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}


//==================//
//Ability methods
//==================//

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
		bPlayerHidden = true;
	}
}


void AMyShooterCharacter::OnEndRewindTime()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bPlayerHidden = false;
	}
}


void AMyShooterCharacter::OnRep_PlayerHidden()
{
	if (!IsLocallyControlled())
	{
		// The player is using the rewind time ability is not visible to other players.
		SetActorHiddenInGame(bPlayerHidden);
		CurrentWeapon->SetActorHiddenInGame(bPlayerHidden);
		SetActorEnableCollision(!bPlayerHidden);
	}
	else 
	{
		if (bPlayerHidden)
		{
			// Locally we disable only the collision during the rewind ability.
			SetActorEnableCollision(false);
		}
		else 
		{
			AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
			if (MyPC) EnableInput(MyPC);

			SetActorEnableCollision(true);
		}
	}
}


void AMyShooterCharacter::OnStartShrink() 
{
	ScaleBeforeShrink = GetActorScale();

	if (GetLocalRole() == ROLE_Authority)
	{
		bPlayerShrinked = true;
		PlayerShrinkedScale = ScaleBeforeShrink * 0.5f;
	}

	// Run a Timer that will resize the player after an amount of time to the original scale
	GetWorldTimerManager().SetTimer(ShrinkTimer, this, &AMyShooterCharacter::OnEndShrink, ShrinkEffectDuration, false);
}


void AMyShooterCharacter::OnEndShrink()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bPlayerShrinked = false;
		PlayerShrinkedScale = ScaleBeforeShrink;
	}

	GetWorldTimerManager().ClearTimer(ShrinkTimer);
}


void AMyShooterCharacter::OnRep_PlayerShrinkedScale()
{
	SetActorScale3D(PlayerShrinkedScale);
}


void AMyShooterCharacter::OnCharacterCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bPlayerShrinked)
	{
		AMyShooterCharacter* otherCharacter = Cast<AMyShooterCharacter>(OtherActor);

		if (otherCharacter && otherCharacter->bPlayerShrinked)
		{
			FPointDamageEvent PointDmg;
			PointDmg.Damage = 10000;

			otherCharacter->TakeDamage(PointDmg.Damage, PointDmg, GetController(), this);
		}
	}
}




