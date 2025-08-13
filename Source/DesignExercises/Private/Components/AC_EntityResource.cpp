#include "Components/AC_EntityResource.h"

// Sets default values for this component's properties
UAC_EntityResource::UAC_EntityResource()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}


// Called when the game starts
void UAC_EntityResource::BeginPlay()
{
	Super::BeginPlay();

	CurrValue = InitialValue;
	AutoDecreaseStep = 1/AutoDecreaseRate;
	AutoIncreaseStep = 1/AutoIncreaseRate;
}


// Called every frame
void UAC_EntityResource::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Auto 
	if (bAutoDecreaseEnabled && CurrValue > MinThreshold)
	{
		AutoDecreaseAccum += DeltaTime;
		if(AutoDecreaseAccum >= AutoDecreaseRate)
		{
			AutoDecreaseAccum -= AutoDecreaseRate;
			DecreaseValue(1);

			/*
			if (CurrValue < MinThreshold)
			{
				CurrValue = MinThreshold;
			}
			*/
		}
	}

	if (bAutoIncreaseEnabled && CurrValue < MaxThreshold)
	{
		IncreaseValue(AutoIncreaseRate * DeltaTime);
		if (CurrValue > MaxThreshold)
			CurrValue = MaxThreshold;
	}

	// OverTimeEffects
	for (int i = 0; i < OverTimeEffects.Num(); i++)
	{
		float ValueChange = OverTimeEffects[i].ChangeRatePerSecond * DeltaTime;
		OverTimeEffects[i].AppliedChange += ValueChange;
		if (OverTimeEffects[i].AppliedChange >= OverTimeEffects[i].TotalChange)
		{
			//SetCurrValue();
		}

		if (OverTimeEffects[i].ChangeRatePerSecond > 0)
			IncreaseValue(OverTimeEffects[i].ChangeRatePerSecond * DeltaTime, OverTimeEffects[i].bAllowOverflow);
		else
			DecreaseValue(-OverTimeEffects[i].ChangeRatePerSecond * DeltaTime);
	}
}

int UAC_EntityResource::IncreaseValue(const float Amount, const bool bAllowOverflow)
{
	if (bAllowOverflow)
		CurrValue += Amount;
	else if (CurrValue < MaxThreshold)
		CurrValue = FMath::Min(CurrValue + FMath::Abs(Amount), MaxValue);

	OnValueIncreasedDelegate.Broadcast(CurrValue);

	return CurrValue;
}

int UAC_EntityResource::DecreaseValue(const float Amount)
{
	CurrValue = FMath::Max(CurrValue - FMath::Abs(Amount), 0.0f);

	OnValueDecreasedDelegate.Broadcast(CurrValue);
	if (CurrValue <= 0.0f)
		OnValueZeroDelegate.Broadcast();

	return CurrValue;
}

int UAC_EntityResource::IncreaseMaxValue(const float Amount)
{
	MaxValue += FMath::Abs(Amount);
	return MaxValue;
}

int UAC_EntityResource::DecreaseMaxValue(const float Amount)
{
	MaxValue -= FMath::Abs(Amount);
	CurrValue = FMath::Clamp(CurrValue, 0.0f, MaxValue);
	return MaxValue;
}

int UAC_EntityResource::SetCurrValue(const float NewValue)
{
	float NewVal = FMath::Clamp(NewValue, 0.0f, MaxValue);
	if (NewVal > CurrValue)
		OnValueIncreasedDelegate.Broadcast(NewValue);
	else
		OnValueDecreasedDelegate.Broadcast(NewValue);
	CurrValue = NewValue;

	if (CurrValue <= 0.0f)
		OnValueZeroDelegate.Broadcast();

	return CurrValue;
}

void UAC_EntityResource::AddOverTimeEffect(FOverTimeEffect OverTimeEffect)
{
	OverTimeEffect.TotalChange = OverTimeEffect.ChangeRatePerSecond * OverTimeEffect.Duration;
	OverTimeEffects.Add(OverTimeEffect);
}
