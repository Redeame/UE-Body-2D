#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rigidbody2D.generated.h"

UENUM(BlueprintType)
enum class EForceMode : uint8
{
	// velocity += force / mass * deltaTime;
	Force,
	// velocity += force * deltaTime;
	Acceleration,
	// velocity += force / mass;
	Impulse,
	// velocity += force;
	VelocityChange 
};

USTRUCT(BlueprintType)
struct FRigidbody2DConstraints
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Constraints")
	bool PosX = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Constraints")
	bool PosY = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Constraints")
	float MaxX = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Constraints")
	float MaxY = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Constraints")
	float MaxSlopeAngle  = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Constraints")
	float MaxCollisionImpulse  = 10000.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST2D_API URigidbody2D : public UActorComponent
{
	GENERATED_BODY()

public:	
	URigidbody2D();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, Category="RigidBody|Property")
	float Mass = 75.0f;
	UPROPERTY(EditAnywhere, Category="RigidBody|Property")
	float Drag = 1.0f;
	UPROPERTY(EditAnywhere, Category="RigidBody|Property")
	bool UseGravity = true;
	UPROPERTY(EditAnywhere, Category="RigidBody|Property")
	float Gravity = 980.7f;
	UPROPERTY(EditAnywhere, Category="RigidBody|Property")
	float SkinWidth  = 0.01f;
	UPROPERTY(EditAnywhere, Category="RigidBody|Property")
	FRigidbody2DConstraints Constraints;

	FVector OnGroundLastFrameNormal = FVector::ZeroVector;
private:
	UPROPERTY(VisibleAnywhere, Category="RigidBody|Info")
	float Speed = 0.0f;
	UPROPERTY(VisibleAnywhere, Category="RigidBody|Info")
	FVector2D _Velocity = {0.0f,0.0f};
	UPROPERTY(VisibleAnywhere, Category="RigidBody|Info")
	bool OnGround = false;

public:
	UFUNCTION(BlueprintCallable)
	void AddForce(FVector2D Force, EForceMode ForceMode);
	UFUNCTION(BlueprintCallable)
	void AddForceAtPoint(float Force, FVector2D Point, EForceMode ForceMode);
	UFUNCTION(BlueprintCallable)
	void AddForceX(float Force, EForceMode ForceMode);
	UFUNCTION(BlueprintCallable)
	void AddForceY(float Force, EForceMode ForceMode);
	UFUNCTION(BlueprintCallable)
	void SetVelocity(FVector2D Velocity);

	// Getters
	UFUNCTION(BlueprintCallable)
	bool IsOnGround();
	UFUNCTION(BlueprintCallable)
	float GetSpeed();
	UFUNCTION(BlueprintCallable)
	FVector2D GetVelocity();
	
};
