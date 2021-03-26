// Fill out your copyright notice in the Description page of Project Settings.


#include "GSSoldierCharacter.h"

// Sets default values
AGSSoldierCharacter::AGSSoldierCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGSSoldierCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGSSoldierCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGSSoldierCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

