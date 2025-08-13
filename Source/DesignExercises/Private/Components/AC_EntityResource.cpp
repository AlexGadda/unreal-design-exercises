#include "Components/AC_EntityResource.h"

#include "MathUtil.h"

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
}


// Called every frame
void UAC_EntityResource::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Auto 
	if (bAutoDecreaseEnabled && CurrValue > MinThreshold)
	{
		AutoDecreaseAccum += AutoDecreaseRate * DeltaTime;
		if(AutoDecreaseAccum >= 1.0f)
		{
			AutoDecreaseAccum -= 1.0f;
			DecreaseValue(1);
		}
	}

	if (bAutoIncreaseEnabled && CurrValue < MaxThreshold)
	{
		AutoIncreaseAccum += AutoIncreaseRate * DeltaTime;
		if(AutoIncreaseAccum >= 1.0f)
		{
			AutoIncreaseAccum -= 1.0f;
			IncreaseValue(1);
		}
	}

	// OverTimeEffects
	for (int i = OverTimeEffects.Num()-1; i >= 0; --i)
	{
		OverTimeEffects[i].ElapsedTime += DeltaTime;
		if(OverTimeEffects[i].ElapsedTime > OverTimeEffects[i].Duration)
		{
			if(OverTimeEffects[i].AppliedChange != OverTimeEffects[i].TotalChange)
			{
				int Difference = OverTimeEffects[i].TotalChange - OverTimeEffects[i].AppliedChange;
				if(Difference > 0)
					IncreaseValue(Difference);
				else
					DecreaseValue(Difference);
			}
			OverTimeEffects.RemoveAt(i);
			continue;
		}
		
		float ValueChange = OverTimeEffects[i].ChangeRatePerSecond * DeltaTime;
		OverTimeEffects[i].TotalChange += ValueChange;
		OverTimeEffects[i].ChangeAccum += FMath::Abs(ValueChange);
		while(OverTimeEffects[i].ChangeAccum >= 1.0f) // For accounting lag spikes
		{
			OverTimeEffects[i].ChangeAccum -= 1.0f;

			if (OverTimeEffects[i].ChangeRatePerSecond > 0)
			{
				IncreaseValue(1.0f);
				OverTimeEffects[i].AppliedChange += 1.0f;
			}
			else
			{
				DecreaseValue(1.0f);
				OverTimeEffects[i].AppliedChange -= 1.0f;
			}
		}
	}
}

int UAC_EntityResource::IncreaseValue(const float Amount, const bool bAllowOverflow)
{
	UE_LOG(LogTemp, Warning, TEXT("Hello World"));
	if (bAllowOverflow)
		CurrValue += Amount;
	else if (CurrValue < MaxValue)
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
	OverTimeEffect.TotalChange = FMath::RoundToInt(OverTimeEffect.ChangeRatePerSecond * OverTimeEffect.Duration);
	OverTimeEffects.Add(OverTimeEffect);
}
