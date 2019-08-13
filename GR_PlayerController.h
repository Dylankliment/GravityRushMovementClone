// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GR_PlayerController.generated.h"


UENUM()
enum class EMovementState : uint8
{
	Walking,
	Floating,
	Aerial
};

UENUM()
enum class EHitResult : uint8
{
	OutHitForward,
	OutHitBackward,
	OutHitRight,
	OutHitLeft,	
	OutHitUp,
	OutHitDown	
};

struct TestArrayStruct
{
	float Distance;
	EHitResult Name;
	FVector_NetQuantizeNormal ImpactNormal;
	FVector_NetQuantize ImpactPoint;
	FHitResult ThisResult;
	bool bIsEmpty;
};

struct LineTraceLooper
{
	TestArrayStruct NearestHit;
	EHitResult Name;
	FVector Start;
	FVector End;
};

UCLASS()
class CLONE_GAME_API AGR_PlayerController : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly)
	class USkeletalMeshComponent* DumbSpeghet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* AzimuthGimbal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VerticalGimbal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerVolume;

public:
	// Sets default values for this character's properties
	AGR_PlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = State)
	EMovementState MovementState = EMovementState::Walking;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	void MoveForward(float val);

	void InputKeyPress();

	void AerialMovement();

	void SetGravity(float Value);

	void TurnAzimuth(float Value);

	void TurnVertical(float Value);

	void MoveRight(float Value);

	void TestLineTrace();

	TestArrayStruct LineTrace(LineTraceLooper InStructTrace);


	/*UFUNCTION()
	void OnAerialBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/


	FVector CameraForward;

	float NearestWall;
};
