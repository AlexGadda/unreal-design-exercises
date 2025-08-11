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
}


// Called every frame
void UAC_EntityResource::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UAC_EntityResource::IncreaseValue(const float Amount, const bool bAllowOverflow)
{
	if(bAllowOverflow)
		CurrValue += Amount;
	else
		CurrValue = FMath::Min(CurrValue + FMath::Abs(Amount), MaxValue);
	OnValueIncreasedDelegate.Broadcast(CurrValue);
	return CurrValue;
}

float UAC_EntityResource::DecreaseValue(const float Amount)
{
	CurrValue = FMath::Max(CurrValue - FMath::Abs(Amount), 0.0f);
	OnValueDecreasedDelegate.Broadcast(CurrValue);
	if(CurrValue <= 0.0f)
		OnValueZeroDelegate.Broadcast();
	return CurrValue;
}

