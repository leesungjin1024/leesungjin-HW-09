
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HWGameModeBase.generated.h"

class AHWPlayerController;

UCLASS()
class HW09_API AHWGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	//숫자 출력
	void PrintChatMessageString(AHWPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	//횟수 증가
	void IncreaseGuessCount(AHWPlayerController* InChattingPlayerController);

	//게임 재시작
	void ResetGame();

	//숫자 판정
	void JudgeGame(AHWPlayerController* InChattingPlayerController, int InStrikeCount);

	//숫자 검증
	bool IsGuessNumberString(const FString& InNumberString);

	//랜덤 숫자 생성
	FString GenerateSecretNumber();

	//판정 결과 계산
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

protected:
	FString SecretNumberString; //정답 저장

	TArray<TObjectPtr<AHWPlayerController>> AllPlayerControllers; //플레이어 컨트롤러 관리
};
