// Fill out your copyright notice in the Description page of Project Settings.

#include "APlatformerBaseCharacter.h"
#include "UPlatformerMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Updated Constructor to use ObjectInitializer
AAPlatformerBaseCharacter::AAPlatformerBaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UUPlatformerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    bPressedJump = false;
}

void AAPlatformerBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Add the Mapping Context to the local player so the Jump Action works
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

	// Create a dynamic material instance for the glow effect
    // Get the mesh, get the material at slot 0, and make it dynamic
    UMaterialInterface* CurrentMat = GetMesh()->GetMaterial(MaterialSlotIndex);
    if (CurrentMat)
    {
        DynamicGlowMaterial = GetMesh()->CreateDynamicMaterialInstance(MaterialSlotIndex, CurrentMat);
    }
}

void AAPlatformerBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (UUPlatformerMovementComponent* CustomMoveComp = Cast<UUPlatformerMovementComponent>(GetCharacterMovement()))
    {
        UpdateGlow(CustomMoveComp->CurrentJumpCharge);
    }
    
}

// Glow updates based on the charge percent from the movement component
void AAPlatformerBaseCharacter::UpdateGlow(float ChargePercent)
{
    if (DynamicGlowMaterial)
    {
        // "GlowIntensity" must match the name of the Scalar Parameter in your Shader
        float FinalIntensity = ChargePercent * 10.0f; // Multiplier for extra "pop"
        DynamicGlowMaterial->SetScalarParameterValue(FName("GlowIntensity"), FinalIntensity);

        // Optional: Change the color to a "Warning Red" as it gets full
        FLinearColor GlowColor = FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, ChargePercent);
        DynamicGlowMaterial->SetVectorParameterValue(FName("GlowColor"), GlowColor);
    }
}

// Binding the Jump input using the Enhanced Input System
void AAPlatformerBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (JumpAction)
        {
            // Use TRIGGERED for the press and hold
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAPlatformerBaseCharacter::StartJump);

            // Use COMPLETED for the release
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAPlatformerBaseCharacter::StopJump);
        }
    }
}

void AAPlatformerBaseCharacter::StartJump(const FInputActionValue& Value)
{
    // Access your custom component and tell it to start the "Golf Swing" or Jump Buffer
    if (UUPlatformerMovementComponent* CustomMoveComp = Cast<UUPlatformerMovementComponent>(GetCharacterMovement()))
    {
        CustomMoveComp->RequestJump();
    }
}

void AAPlatformerBaseCharacter::StopJump(const FInputActionValue& Value)
{
    // Tell the component to execute the jump based on the charge meter
    if (UUPlatformerMovementComponent* CustomMoveComp = Cast<UUPlatformerMovementComponent>(GetCharacterMovement()))
    {
        CustomMoveComp->ExecuteJump();
    }
}

bool AAPlatformerBaseCharacter::CanJumpInternal_Implementation() const
{
    // Returning false here kills the default ACharacter jump logic
    return false;
}