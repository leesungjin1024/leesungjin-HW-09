// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HWGameModeBase.h"
#include "HWGameStateBase.h"
#include "Player/HWPlayerController.h"
#include "EngineUtils.h"
#include "Player/HWPlayerState.h"


void AHWGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void AHWGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	/*
	AHWGameStateBase* HWGameStateBase = GetGameState<AHWGameStateBase>();
	if (IsValid(HWGameStateBase) == true)
	{
		HWGameStateBase->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	}

	AHWPlayerController* HWPlayerController = Cast<AHWPlayerController>(NewPlayer);
	if (IsValid(HWPlayerController) == true)
	{
		AllPlayerControllers.Add(HWPlayerController);
	}*/

	AHWPlayerController* HWPlayerController = Cast<AHWPlayerController>(NewPlayer);
	if (IsValid(HWPlayerController) == true)
	{
		HWPlayerController->NotificationText = FText::FromString(TEXT("게임 서버에 연결되었습니다."));

		AllPlayerControllers.Add(HWPlayerController);

		AHWPlayerState* HWPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
			HWPS->PlayerNameString = TEXT("플레이어") + FString::FromInt(AllPlayerControllers.Num());
			HWPS->CurrentGuessCount = 0;
		}

		AHWGameStateBase* HWGameStateBase = GetGameState<AHWGameStateBase>();
		if (IsValid(HWGameStateBase) == true)
		{
			HWGameStateBase->MulticastRPCBroadcastLoginMessage(HWPS->PlayerNameString);
		}
	}
}

FString AHWGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	UE_LOG(LogTemp, Log, TEXT("정답 숫자: %s"), *Result);
	return Result;
}


bool AHWGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{

			break;
		}
		//중복 숫자 검사 
		if (UniqueDigits.Num() != 3)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}


FString AHWGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AHWGameModeBase::PrintChatMessageString(AHWPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString CleanedInput = InChatMessageString.TrimStartAndEnd();
	FString GuessNumberString = CleanedInput.Right(3);
	//기회를 다 쓴 유저는 입력 불가
	AHWPlayerState* ChattingHW = InChattingPlayerController ? InChattingPlayerController->GetPlayerState<AHWPlayerState>() : nullptr;
	if (ChattingHW != nullptr && IsValid(ChattingHW) == true)
	{
		if (ChattingHW->CurrentGuessCount >= ChattingHW->MaxGuessCount)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("기회를 모두 소진하여 참여할 수 없습니다."));
			return;
		}
	}

	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);
		for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
		{
			AHWPlayerController* HWPlayerController = *It;
			if (IsValid(HWPlayerController) == true)
			{
				FString CleanInfoPrefix = IsValid(ChattingHW) ? FString::Printf(TEXT("[%s] "), *ChattingHW->GetPlayerInfoString()) : TEXT("");

				FString CombinedMessageString = CleanInfoPrefix + GuessNumberString + TEXT(" -> ") + JudgeResultString;
				HWPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
	}

	else
	{
		//오류 시 안내 출력
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("규칙에 맞지 않습니다. 다시 입력하세요."));
		for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
		{
			AHWPlayerController* HWPlayerController = *It;
			if (IsValid(HWPlayerController) == true && HWPlayerController != InChattingPlayerController)
			{
				HWPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void AHWGameModeBase::IncreaseGuessCount(AHWPlayerController* InChattingPlayerController)
{
	AHWPlayerState* HWPS = InChattingPlayerController->GetPlayerState<AHWPlayerState>();
	if (IsValid(HWPS) == true)
	{
		HWPS->CurrentGuessCount++;
	}
}

void AHWGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& HWPlayerController : AllPlayerControllers)
	{
		AHWPlayerState* HWPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
			HWPS->CurrentGuessCount = 0;
		}
		HWPlayerController->ClientRPCPrintChatMessageString(TEXT("게임이 리셋되었습니다!"));
	}
}

void AHWGameModeBase::JudgeGame(AHWPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AHWPlayerState* HWPS = InChattingPlayerController->GetPlayerState<AHWPlayerState>();

		for (const auto& HWPlayerController : AllPlayerControllers)
		{
			if (IsValid(HWPlayerController) && IsValid(HWPS))
			{
				FString CombinedMessageString = HWPS->PlayerNameString + TEXT(" 님이 게임에서 승리했습니다.");

				HWPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				HWPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
		ResetGame();

	}
	else
	{
		bool bIsDraw = true;
		for (const auto& HWPlayerController : AllPlayerControllers)
		{
			AHWPlayerState* HWPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
			if (IsValid(HWPS) == true)
			{
				if (HWPS->CurrentGuessCount < HWPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& HWPlayerController : AllPlayerControllers)
			{
				if (IsValid(HWPlayerController))
				{
					
					HWPlayerController->NotificationText = FText::FromString(TEXT("무승부..."));

					FString DrawMessage = FString::Printf(TEXT("무승부! 모든 기회를 소진했습니다. 정답은 [%s] 였습니다."), *SecretNumberString);
					HWPlayerController->ClientRPCPrintChatMessageString(DrawMessage);
				}
			}

			ResetGame();
		}
	}
}

