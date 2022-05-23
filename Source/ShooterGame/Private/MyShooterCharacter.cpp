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


void AMyShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	//Replicate HiddenPlayer.
	DOREPLIFETIME(AMyShooterCharacter, bPlayerHidden);
	DOREPLIFETIME(AMyShooterCharacter, PlayerShrinkedScale);
}


float AMyShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (!bPlayerShrinked)
	{
		AShooterWeapon_Instant* weapon = Cast<AShooterWeapon_Instant>(DamageCauser);

		if (weapon && weapon->bCanShrink)
		{
			bPlayerShrinked = true;
			ScaleBeforeShrink = GetActorScale();
			PlayerShrinkedScale = ScaleBeforeShrink * ShrinkMultiplier;

			GetWorldTimerManager().SetTimer(ShrinkTimer, this, &AMyShooterCharacter::OnResetShrink, ShrinkDuration, false);
		}
	}
	else
	{
		AMyShooterCharacter* character = Cast<AMyShooterCharacter>(DamageCauser);
		if (character)
		{
			bPlayerShrinked = false;
			PlayerShrinkedScale = ScaleBeforeShrink;

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
			//enable input
			EnableInput(MyPC);
		}

		SetActorEnableCollision(true);
	}
}


void AMyShooterCharacter::OnRep_PlayerHidden()
{
	if (!IsLocallyControlled())
	{
		SetActorHiddenInGame(bPlayerHidden);
		CurrentWeapon->SetActorHiddenInGame(bPlayerHidden);
		SetActorEnableCollision(!bPlayerHidden);
	}
}


void AMyShooterCharacter::OnResetShrink()
{
	bPlayerShrinked = false;

	if (GetLocalRole() == ROLE_Authority)
	{
		PlayerShrinkedScale = ScaleBeforeShrink;
	}
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


void AMyShooterCharacter::OnRep_PlayerShrinkedScale()
{
	SetActorScale3D(PlayerShrinkedScale);
}

