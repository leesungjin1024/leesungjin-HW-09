// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HWPlayerState.h"

#include "Net/UnrealNetwork.h"

AHWPlayerState::AHWPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)

{
	bReplicates = true;
}

void AHWPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString AHWPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = FString::Printf(TEXT("%s [%d / %d]"),
		*PlayerNameString, CurrentGuessCount, MaxGuessCount);

	return PlayerInfoString;
}
