// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "APlatformerBaseCharacter.generated.h"

// Forward declarations to keep the compiler happy without explicit includes here
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PLATFORMER_JUMP_API AAPlatformerBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAPlatformerBaseCharacter(const FObjectInitializer& ObjectInitializer);
    virtual bool CanJumpInternal_Implementation() const override;

protected:
    virtual void BeginPlay() override;

    /** * Input Assets to be assigned in BP_Player */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    /** * Input Handlers */
    void StartJump(const FInputActionValue& Value);
    void StopJump(const FInputActionValue& Value);

public:
    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};