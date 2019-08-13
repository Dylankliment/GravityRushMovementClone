// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GR_Pawn.generated.h"

UENUM()
	enum class EPawnMovementState : uint8
{
	Walking,
	Floating,
	Aerial
};

UCLASS()
	class CLONE_GAME_API AGR_Pawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UCapsuleComponent* MyCapsuleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* AzimuthGimbal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VerticalGimbal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	class UCharacterMovementComponent* CharacterMovementComponent;

public:
	// Sets default values for this character's properties
	AGR_Pawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = State)
	EPawnMovementState MovementState = EPawnMovementState::Walking;

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

	FVector CameraForward;
};
