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
	
	if(IsLocallyControlled())
	{
		// Locally we disable only the collision during the rewind ability.
		SetActorEnableCollision(false);
	}
}


void AMyShooterCharacter::OnEndRewindTime()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bPlayerHidden = false;
	}

	if (IsLocallyControlled())
	{
		AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
		if (MyPC)
		{
			EnableInput(MyPC);
		}

		SetActorEnableCollision(true);
	}
}


void AMyShooterCharacter::OnRep_PlayerHidden()
{
	// The player is playing the rewind time ability is not visible to other players.
	if (!IsLocallyControlled())
	{
		SetActorHiddenInGame(bPlayerHidden);
		CurrentWeapon->SetActorHiddenInGame(bPlayerHidden);
		SetActorEnableCollision(!bPlayerHidden);
	}
}


void AMyShooterCharacter::OnStartShrink() 
{
	bPlayerShrinked = true;
	ScaleBeforeShrink = GetActorScale();
	// Run a Timer that will resize the player after an amount of time to the original scale
	GetWorldTimerManager().SetTimer(ShrinkTimer, this, &AMyShooterCharacter::OnEndShrink, ShrinkEffectDuration, false);

	if (GetLocalRole() == ROLE_Authority)
	{
		PlayerShrinkedScale = ScaleBeforeShrink * 0.5f;
	}
}


void AMyShooterCharacter::OnEndShrink()
{
	bPlayerShrinked = false;
	GetWorldTimerManager().ClearTimer(ShrinkTimer);

	if (GetLocalRole() == ROLE_Authority)
	{
		PlayerShrinkedScale = ScaleBeforeShrink;
	}
}


void AMyShooterCharacter::OnRep_PlayerShrinkedScale()
{
	SetActorScale3D(PlayerShrinkedScale);
}


void AMyShooterCharacter::OnCharacterCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AMyShooterCharacter* character = Cast<AMyShooterCharacter>(OtherActor);

	if (character && character->bPlayerShrinked)
	{
		FPointDamageEvent PointDmg;
		PointDmg.Damage = 10000;

		character->TakeDamage(PointDmg.Damage, PointDmg, GetController(), this);
	}
}




