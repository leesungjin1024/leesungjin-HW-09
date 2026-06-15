// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HWPawn.h"
#include "HW09.h"


void AHWPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = HW09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("HWPawn::BeginPlay() %s [%s]"), *HW09FunctionLibrary::GetNetModeString(this), *NetRoleString);
	HW09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void AHWPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = HW09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("HWPawn::PossessedBy() %s [%s]"), *HW09FunctionLibrary::GetNetModeString(this), *NetRoleString);
	HW09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
