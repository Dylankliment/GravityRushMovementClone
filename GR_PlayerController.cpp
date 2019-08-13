// Fill out your copyright notice in the Description page of Project Settings.

#include "GR_PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"


// Sets default values
AGR_PlayerController::AGR_PlayerController()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = GetCapsuleComponent();

	DumbSpeghet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	DumbSpeghet->SetupAttachment(RootComponent);

	AzimuthGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("Azimuth Gimbal"));
	AzimuthGimbal->SetupAttachment(RootComponent);

	VerticalGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("Vertical Gimbal"));
	VerticalGimbal->SetupAttachment(AzimuthGimbal);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(VerticalGimbal);

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCameraComponent"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArm);

	//TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	////RootComponent = TriggerVolume;
	//TriggerVolume->SetupAttachment(RootComponent);
	//
	//TriggerVolume->Deactivate();	
	//TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AGR_PlayerController::OnAerialBeginOverlap);

}

// Called when the game starts or when spawned
void AGR_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (DumbSpeghet == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No mesh found"));
	}
}

// Called every frame
void AGR_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementState == EMovementState::Aerial)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CammeraForward %s"), *CameraForward.ToString());
		//this->GetTransform()->postion

		FHitResult OutSweepHit;
		AddActorWorldOffset(CameraForward * 500.0 * DeltaTime, true, &OutSweepHit);
		if (OutSweepHit.Actor != nullptr)
		{
			GetCharacterMovement()->MovementMode = MOVE_Walking;
			MovementState = EMovementState::Walking;
			SetGravity(1.0f);
			TestLineTrace();
		}
		//CameraForward = CameraForward + 10.0 * DeltaTime;
	}
}

// Called to bind functionality to input
void AGR_PlayerController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGR_PlayerController::MoveForward);
	PlayerInputComponent->BindAxis("TurnAzimuth", this, &AGR_PlayerController::TurnAzimuth);
	PlayerInputComponent->BindAxis("TurnVertical", this, &AGR_PlayerController::TurnVertical);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGR_PlayerController::MoveRight);


	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGR_PlayerController::TestLineTrace);

	PlayerInputComponent->BindAction("ActionKey", IE_Pressed, this, &AGR_PlayerController::InputKeyPress);
	PlayerInputComponent->BindAction("ConfirmAerialMovement", IE_Pressed, this, &AGR_PlayerController::AerialMovement);

}

void AGR_PlayerController::MoveForward(float val)
{
	if (val != 0.0f && MovementState == EMovementState::Walking)
	{
		// add movement in that direction
		AddMovementInput(ThirdPersonCameraComponent->GetForwardVector(), val);
	}
}

void AGR_PlayerController::InputKeyPress()
{
	//TODO Expand aerial movement functionality

	switch (MovementState) {
	case EMovementState::Walking: UE_LOG(LogTemp, Warning, TEXT("Switch Walking Statement working, Switching to Floating"));
		MovementState = EMovementState::Floating;
		//TODO Create and call function for floating
		SetGravity(0.0f);
		break;
	case EMovementState::Floating: UE_LOG(LogTemp, Warning, TEXT("Switch Floating Statement working, Switching to Walking"));
		GetCharacterMovement()->MovementMode = MOVE_Walking;
		MovementState = EMovementState::Walking;
		SetGravity(1.0f);
		break;
	case EMovementState::Aerial: UE_LOG(LogTemp, Warning, TEXT("Switch Aerial Statement working, Switching to Floating"));
		MovementState = EMovementState::Floating;
		GetCharacterMovement()->MovementMode = MOVE_Flying;
		SetGravity(0.0f);
		//TriggerVolume->Activate();
		break;
	default: UE_LOG(LogTemp, Error, TEXT("Switch Statement Not Working"));
		break;
	}
}



void AGR_PlayerController::AerialMovement()
{
	if (MovementState == EMovementState::Floating)
	{
		//TODO Get camera forward and move player in that direction
		CameraForward = ThirdPersonCameraComponent->GetForwardVector();
		//Currently being done in tick
		MovementState = EMovementState::Aerial;
		GetCharacterMovement()->MovementMode = MOVE_Flying;
	}
}

void AGR_PlayerController::SetGravity(float Value)
{
	UCharacterMovementComponent* MyMovementComponent = GetCharacterMovement();
	if (MyMovementComponent != NULL)
	{
		MyMovementComponent->GravityScale = Value;
	}
	else { UE_LOG(LogTemp, Warning, TEXT("MyMovementComponent Was Null")); }
}

void AGR_PlayerController::TurnAzimuth(float Value)
{
	AzimuthGimbal->AddLocalRotation(FRotator(0, Value, 0));
}

void AGR_PlayerController::TurnVertical(float Value)
{
	VerticalGimbal->AddLocalRotation(FRotator(Value, 0, 0));
}

void AGR_PlayerController::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		//// find out which way is right
		//const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		//// get right vector 
		//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(ThirdPersonCameraComponent->GetRightVector(), Value);
	}
}

void AGR_PlayerController::TestLineTrace()
{
	TestArrayStruct NearestHitResult;
	NearestHitResult.bIsEmpty = true;
	NearestHitResult.Distance = 1000000.0f;

	LineTraceLooper LineTraceTest;
	LineTraceTest.NearestHit = NearestHitResult;
	LineTraceTest.Name = EHitResult::OutHitForward;
	LineTraceTest.Start = DumbSpeghet->GetComponentLocation();
	LineTraceTest.End = ((DumbSpeghet->GetForwardVector() * 100.f) + DumbSpeghet->GetComponentLocation());
	NearestHitResult = LineTrace(LineTraceTest);

	LineTraceTest.NearestHit = NearestHitResult;
	LineTraceTest.Name = EHitResult::OutHitBackward;
	LineTraceTest.End = ((DumbSpeghet->GetForwardVector() * -100.f) + DumbSpeghet->GetComponentLocation());
	NearestHitResult = LineTrace(LineTraceTest);

	LineTraceTest.NearestHit = NearestHitResult;
	LineTraceTest.Name = EHitResult::OutHitRight;
	LineTraceTest.Start = DumbSpeghet->GetComponentLocation();
	LineTraceTest.End = ((DumbSpeghet->GetRightVector() * 100.f) + DumbSpeghet->GetComponentLocation());
	NearestHitResult = LineTrace(LineTraceTest);

	LineTraceTest.NearestHit = NearestHitResult;
	LineTraceTest.Name = EHitResult::OutHitLeft;
	LineTraceTest.End = ((DumbSpeghet->GetRightVector() * -100.f) + DumbSpeghet->GetComponentLocation());
	NearestHitResult = LineTrace(LineTraceTest);

	LineTraceTest.NearestHit = NearestHitResult;
	LineTraceTest.Name = EHitResult::OutHitUp;
	LineTraceTest.Start = DumbSpeghet->GetComponentLocation();
	LineTraceTest.End = ((DumbSpeghet->GetUpVector() * 100.f) + DumbSpeghet->GetComponentLocation());
	NearestHitResult = LineTrace(LineTraceTest);

	LineTraceTest.NearestHit = NearestHitResult;
	LineTraceTest.Name = EHitResult::OutHitDown;
	LineTraceTest.End = ((DumbSpeghet->GetUpVector() * -100.f) + DumbSpeghet->GetComponentLocation());
	NearestHitResult = LineTrace(LineTraceTest);

	if (!NearestHitResult.bIsEmpty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pre Rot %s"), *GetActorRotation().ToString());

		SetActorRotation(UKismetMathLibrary::MakeRotFromZY(NearestHitResult.ThisResult.Normal,GetActorRightVector()));
		UE_LOG(LogTemp, Warning, TEXT("Post Rot %s"), *GetActorRotation().ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("Nearest %f"), NearestHitResult.Distance);
}

TestArrayStruct AGR_PlayerController::LineTrace(LineTraceLooper InStructTrace)
{
	FHitResult OutResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);


	GetWorld()->LineTraceSingleByChannel(
		OutResult,
		InStructTrace.Start,
		InStructTrace.End,
		ECollisionChannel::ECC_WorldStatic,
		CollisionQueryParams);

	if (OutResult.Actor == NULL || InStructTrace.NearestHit.Distance < OutResult.Distance)
	{
		return InStructTrace.NearestHit;
	}
	else if (InStructTrace.NearestHit.Distance > OutResult.Distance)
	{

		UE_LOG(LogTemp, Warning, TEXT("LineTrace is nearer"));


		DrawDebugLine(
			GetWorld(),
			InStructTrace.Start,
			InStructTrace.End,
			FColor(255, 0, 0),
			false,
			3.0f,
			0,
			13.0f);

		TestArrayStruct ReturnHit;
		ReturnHit.Name = InStructTrace.Name;
		ReturnHit.Distance = OutResult.Distance;
		ReturnHit.ImpactNormal = OutResult.ImpactNormal;
		ReturnHit.ThisResult = OutResult;
		ReturnHit.ImpactPoint = OutResult.ImpactPoint;
		ReturnHit.bIsEmpty = false;
		//insert already written code for Debug linetrace.
		return ReturnHit;
	}
	else
	{
		DrawDebugLine(
			GetWorld(),
			InStructTrace.Start,
			InStructTrace.End,
			FColor(255, 0, 0),
			false,
			3.0f,
			0,
			13.0f);

		TestArrayStruct NearestHitResult;
		NearestHitResult.bIsEmpty = true;
		NearestHitResult.Distance = 1000000.0f;
		UE_LOG(LogTemp, Error, TEXT("Unknown Error in LineTrace"));

		return NearestHitResult;
	}
}

//void AGR_PlayerController::OnAerialBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//
//	UE_LOG(LogTemp, Error, TEXT("Triggered"));
//
//	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
//	{
//		UE_LOG(LogTemp, Error, TEXT("Triggered Again"));
//
//		TriggerVolume->Deactivate();
//		MovementState = EMovementState::Walking;
//		SetGravity(1.0f);
//		TestLineTrace();
//	}
//	
//}
