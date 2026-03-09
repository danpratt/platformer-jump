// Fill out your copyright notice in the Description page of Project Settings.

#include "UPlatformerMovementComponent.h"
#include "GameFramework/Character.h"

void UUPlatformerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    if (MovementMode == MOVE_Falling && PreviousMovementMode == MOVE_Walking)
    {
        // If we are NOT already in a jumping state, this must be a ledge fall
        if (!bIsJumping)
        {
            LastGroundedTime = GetWorld()->GetTimeSeconds();
            UE_LOG(LogTemp, Warning, TEXT("LEFT GROUND (Ledge Fall): Coyote Window Started at %f"), LastGroundedTime);
            bIsCoyoteTimeWindowActive = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LEFT GROUND (Jump): Coyote Window Ignored"));
            bIsCoyoteTimeWindowActive = false;
        }
    }
    else if (MovementMode == MOVE_Walking)
    {
        // We landed, so we are definitely no longer "Jumping"
        bIsJumping = false;
        bIsCoyoteTimeWindowActive = false;
        UE_LOG(LogTemp, Warning, TEXT("TOUCHED GROUND: bIsJumping reset to false"));
    }
}

void UUPlatformerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (IsWalking())
    {
        // 1. Increment charge while holding the button
        if (bIsChargingJump)
        {
            CurrentJumpCharge = FMath::Clamp(CurrentJumpCharge + (DeltaTime * ChargeSpeed), 0.0f, 1.0f);
        }

        // 2. Jump Buffer: If we hit space right before landing, jump now
        float TimeSinceRequest = GetWorld()->GetTimeSeconds() - LastJumpRequestTime;
        if (TimeSinceRequest <= JumpBufferDuration)
        {
            // Landing from a fall; execute a quick tap jump
            bIsChargingJump = true;
            CurrentJumpCharge = 0.0f;
            ExecuteJump();
            LastJumpRequestTime = -100.0f; // Clear the buffer
        }
    }
    else if (bIsCoyoteTimeWindowActive)
    {
        float coyoteElapsedTime = GetWorld()->GetTimeSeconds() - LastGroundedTime;
        if (coyoteElapsedTime > CoyoteTimeDuration) { 
            bIsCoyoteTimeWindowActive = false;
            UE_LOG(LogTemp, Warning, TEXT("Coyote Jump Window Expired"));
        }
    }
}

void UUPlatformerMovementComponent::RequestJump()
{
    UE_LOG(LogTemp, Warning, TEXT("Jump requested"));
    // If we are already charging, just exit. This stops the "immediate jump" 
    // because RequestJump is now being called every frame while holding.
    if (bIsChargingJump) return;

    if (IsWalking())
    {
        UE_LOG(LogTemp, Warning, TEXT("CHARGE STARTED"));
        bIsChargingJump = true;
        CurrentJumpCharge = 0.0f;
    }
    else if (bIsCoyoteTimeWindowActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Perform Coyote Jump"));

        // 1. Mark as jumping so the mode change doesn't try to start a new window
        bIsJumping = true;

        // 2. Kill the window immediately so they can't double-jump in mid-air
        bIsCoyoteTimeWindowActive = false;

        // 3. Apply the jump force. 
        // Usually, we use the full JumpZVelocity here to give the player a fair recovery.
        Velocity.Z = JumpZVelocity;

        // 4. Ensure we are in Falling mode (though we already are, this ensures physics update)
        SetMovementMode(MOVE_Falling);

        // 5. Clean up the ground-charge state just in case they were holding space while falling
        bIsChargingJump = false;
        CurrentJumpCharge = 0.0f;
    }
}

void UUPlatformerMovementComponent::ExecuteJump()
{
    if (bIsChargingJump)
    {
        UE_LOG(LogTemp, Warning, TEXT("LAUNCH EXECUTED with Charge: %f"), CurrentJumpCharge);

        if (IsWalking())
        {
            // Mark that we are now jumping on purpose
            bIsJumping = true;

            float FinalJumpForce = JumpZVelocity * (0.5f + (CurrentJumpCharge * 0.5f));
            Velocity.Z = FinalJumpForce;
            SetMovementMode(MOVE_Falling);

            UE_LOG(LogTemp, Warning, TEXT("LAUNCH EXECUTED: bIsJumping is now true"));
        }

        bIsChargingJump = false;
        CurrentJumpCharge = 0.0f;
    }
}