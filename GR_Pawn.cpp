// Fill out your copyright notice in the Description page of Project Settings.

#include "GR_Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

// Sets default values
AGR_Pawn::AGR_Pawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	RootComponent = MyCapsuleComponent;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh3P->SetupAttachment(RootComponent);

	AzimuthGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("Azimuth Gimbal"));
	AzimuthGimbal->SetupAttachment(RootComponent);

	VerticalGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("Vertical Gimbal"));
	VerticalGimbal->SetupAttachment(AzimuthGimbal);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(VerticalGimbal);

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Third Person Camera Component"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArm);

	CharacterMovementComponent = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Character Movement Component"));

}

// Called when the game starts or when spawned
void AGR_Pawn::BeginPlay()
{

	Super::BeginPlay();

}

// Called every frame
void AGR_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementState == EPawnMovementState::Aerial)
	{

		UE_LOG(LogTemp, Warning, TEXT("CammeraForward %s"), *CameraForward.ToString());
		//this->GetTransform()->postion
		AddActorWorldOffset(CameraForward * 500.0 * DeltaTime);

		//CameraForward = CameraForward + 10.0 * DeltaTime;
	}
}

// Called to bind functionality to input
void AGR_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGR_Pawn::MoveForward);
	PlayerInputComponent->BindAxis("TurnAzimuth", this, &AGR_Pawn::TurnAzimuth);
	PlayerInputComponent->BindAxis("TurnVertical", this, &AGR_Pawn::TurnVertical);



	PlayerInputComponent->BindAction("ActionKey", IE_Pressed, this, &AGR_Pawn::InputKeyPress);
	PlayerInputComponent->BindAction("ConfirmAerialMovement", IE_Pressed, this, &AGR_Pawn::AerialMovement);

}

void AGR_Pawn::MoveForward(float val)
{

	if (val != 0.0f && MovementState == EPawnMovementState::Walking)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), val);
		//UGameplayStatics::GetPlayerController(GetWorld(),0)->
		UE_LOG(LogTemp, Warning, TEXT("MOVING"));

	}
}

void AGR_Pawn::InputKeyPress()
{
	//TODO Create function and action mapping for doing aerial movement

	switch (MovementState) {
	case EPawnMovementState::Walking: UE_LOG(LogTemp, Warning, TEXT("Switch Walking Statement working, Switching to Floating"));
		MovementState = EPawnMovementState::Floating;
		//TODO Create and call function for floating
		SetGravity(0.0f);
		break;
	case EPawnMovementState::Floating: UE_LOG(LogTemp, Warning, TEXT("Switch Floating Statement working, Switching to Walking"));
		//GetCharacterMovement()->MovementMode = MOVE_Walking;
		MovementState = EPawnMovementState::Walking;
		SetGravity(1.0f);
		break;
	case EPawnMovementState::Aerial: UE_LOG(LogTemp, Warning, TEXT("Switch Aerial Statement working, Switching to Floating"));
		MovementState = EPawnMovementState::Floating;
		//GetCharacterMovement()->MovementMode = MOVE_Flying;
		SetGravity(0.0f);
		break;
	default: UE_LOG(LogTemp, Error, TEXT("Switch Statement Not Working"));
		break;
	}
}

void AGR_Pawn::AerialMovement()
{
	if (MovementState == EPawnMovementState::Floating)
	{
		//TODO Get camera forward and move player in that direction
		CameraForward = ThirdPersonCameraComponent->GetForwardVector();
		//Currently being done in tick
		MovementState = EPawnMovementState::Aerial;
		//GetCharacterMovement()->MovementMode = MOVE_Flying;
	}
}

void AGR_Pawn::SetGravity(float Value)
{
	/*UCharacterMovementComponent* MyMovementComponent = GetCharacterMovement();
	if (MyMovementComponent != NULL)
	{
		MyMovementComponent->GravityScale = Value;
	}
	else { UE_LOG(LogTemp, Warning, TEXT("MyMovementComponent Was Null")); }*/
}

void AGR_Pawn::TurnAzimuth(float Value)
{
	AzimuthGimbal->AddLocalRotation(FRotator(0, Value, 0));
}

void AGR_Pawn::TurnVertical(float Value)
{
	VerticalGimbal->AddLocalRotation(FRotator(Value, 0, 0));

}