#pragma once

#include "CoreMinimal.h"
#include "AC_EntityResource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueIncreasedDelegate, float, CurrValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueDecreasedDelegate, float, CurrValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValueZeroDelegate);

UENUM(BlueprintType)
enum class EOverTimeEffectType : uint8
{
	None,
	Poison,
	Healing
};

USTRUCT(BlueprintType)
struct FOverTimeEffect
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChangeRatePerSecond;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="If <0: infinite."))
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowOverflow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOverTimeEffectType Type;

	float AppliedChange;
	float TotalChange;

	FOverTimeEffect()
		: ChangeRatePerSecond(0.0f),
		  Duration(0.0f),
		  bAllowOverflow(false),
		  Type(EOverTimeEffectType::None),
		  AppliedChange(0.0f),
		  TotalChange(0.0f)
	{
	}

	FOverTimeEffect(const float ChangeRatePerSecond, const float Duration, const bool bAllowOverflow,
	                const EOverTimeEffectType Type)
		: ChangeRatePerSecond(ChangeRatePerSecond),
		  Duration(Duration),
		  bAllowOverflow(bAllowOverflow),
		  Type(Type),
		  AppliedChange(0.0f),
		  TotalChange(ChangeRatePerSecond * Duration)
	{
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DESIGNEXERCISES_API UAC_EntityResource : public UActorComponent
{
	GENERATED_BODY()

public:
	// VARIABLES
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	int InitialValue = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	int MaxValue = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	bool bAutoDecreaseEnabled = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(EditCondition="bAutoDecreaseEnabled"))
	int MinThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(EditCondition="bAutoDecreaseEnabled"))
	float AutoDecreaseRate = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	bool bAutoIncreaseEnabled = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(EditCondition="bAutoIncreaseEnabled"))
	int MaxThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(EditCondition="bAutoIncreaseEnabled"))
	float AutoIncreaseRate = 1.0f;

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
	int GetCurrValue() const { return CurrValue; };
	UFUNCTION(BlueprintCallable)
	int IncreaseValue(const float Amount, const bool bAllowOverflow = false);
	UFUNCTION(BlueprintCallable)
	int DecreaseValue(const float Amount);
	UFUNCTION(BlueprintCallable)
	int IncreaseMaxValue(const float Amount);
	UFUNCTION(BlueprintCallable)
	int DecreaseMaxValue(const float Amount);
	/**
	 * Set a new value for CurrValue, if possible, otherwise it clamps the result.
	 *
	 * @param	NewValue		The new CurrValue.
	 * @return	The new CurrValue (useful to check if result was clamped).
	 */
	UFUNCTION(BlueprintCallable)
	int SetCurrValue(const float NewValue);

	UFUNCTION(BlueprintCallable)
	void AddOverTimeEffect(FOverTimeEffect OverTimeEffect);

protected:
	// VARIABLES
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrValue = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FOverTimeEffect> OverTimeEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AutoDecreaseStep;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AutoIncreaseStep;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AutoDecreaseAccum = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AutoIncreaseAccum = 0.0f;

	// FUNCTIONS
	virtual void BeginPlay() override;
};
