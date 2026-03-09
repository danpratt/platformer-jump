// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UPlatformerMovementComponent.generated.h"

UCLASS()
class PLATFORMER_JUMP_API UUPlatformerMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    // --- Coyote Time ---
    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
    UPROPERTY(EditAnywhere, Category = "Platformer | Coyote Time")
    float CoyoteTimeDuration = 0.15f;

    float LastGroundedTime = -100.0f;

    bool bIsCoyoteTimeWindowActive = false;

    // --- Jump Buffering & Timing ---
    UPROPERTY(EditAnywhere, Category = "Platformer | Jump")
    float JumpBufferDuration = 0.2f;

    float LastJumpRequestTime = -100.0f;

    // The timestamp of the last successful jump execution to prevent "machine gun" firing
    float LastJumpTime = -100.0f;

    // --- Jump Charge (Golf Swing) ---
    UPROPERTY(BlueprintReadOnly, Category = "Platformer | Jump")
    float CurrentJumpCharge = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Platformer | Jump")
    float ChargeSpeed = 1.5f;

    // --- Public Interface ---
    // Called by the Character when Jump is pressed
    void RequestJump();

    // Called by the Character when Jump is released
    void ExecuteJump();

protected:
    // We'll need Tick to update the charge meter while holding the button
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    bool bIsChargingJump = false;
    bool bIsJumping = false;
};