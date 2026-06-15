// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HWPlayerController.h"

#include "UI/HWChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HW09.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/HWGameModeBase.h"
#include "HWPlayerState.h"
#include "Net/UnrealNetwork.h"

AHWPlayerController::AHWPlayerController()
{
	bReplicates = true;
}

void AHWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UHWChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void AHWPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	//PrintChatMessageString(ChatMessageString);
	if (IsLocalController() == true)
	{
		//ServerRPCPrintChatMessageString(InChatMessageString);
		AHWPlayerState* HWPS = GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
			//FString CombinedMessageString = HWPS->PlayerNameString + TEXT(": ") + InChatMessageString;
			FString CombinedMessageString = HWPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
	
}

void AHWPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	/*
	UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);
	FString NetModeString = HW09FunctionLibrary::GetNetModeString(this);
	FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	HW09FunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);
	*/

	HW09FunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AHWPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void AHWPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void AHWPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	/*
	for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
	{
		AHWPlayerController* HWPlayerController = *It;
		if (IsValid(HWPlayerController) == true)
		{
			HWPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}
	*/
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AHWGameModeBase* HWGM = Cast<AHWGameModeBase>(GM);
		if (IsValid(HWGM) == true)
		{
			HWGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}






