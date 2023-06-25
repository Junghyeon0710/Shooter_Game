#pragma once
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(Display = "9mm"),
	EAT_AR UMETA(Display = "Assalult Rifle"),

	EAT_MAX UMETA(Display = "DefaultMax")
};
