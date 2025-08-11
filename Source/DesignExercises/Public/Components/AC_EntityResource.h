#pragma once

#include "CoreMinimal.h"
#include "AC_EntityResource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueIncreasedDelegate, float, CurrValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueDecreasedDelegate, float, CurrValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValueZeroDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DESIGNEXERCISES_API UAC_EntityResource : public UActorComponent
{
	GENERATED_BODY()

public:
	// VARIABLES
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialValue = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxValue = 1.0f;
	UPROPERTY(BlueprintAssignable)
	FOnValueIncreasedDelegate OnValueIncreasedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnValueDecreasedDelegate OnValueDecreasedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnValueZeroDelegate OnValueZeroDelegate;

	// FUNCTIONS
	UAC_EntityResource();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintPure)
	float GetCurrValue() const { return CurrValue; };
	UFUNCTION(BlueprintCallable)
	float IncreaseValue(const float Amount, const bool bAllowOverflow);
	UFUNCTION(BlueprintCallable)
	float DecreaseValue(const float Amount);

protected:
	// VARIABLES
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrValue = 0.0f;

	// FUNCTIONS
	virtual void BeginPlay() override;
};
