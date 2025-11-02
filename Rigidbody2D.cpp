#include "Rigidbody2D.h"

// Constructor
URigidbody2D::URigidbody2D()
{
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void URigidbody2D::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void URigidbody2D::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetOwner() || DeltaTime <= 0.f)
        return;

    const float Dt = DeltaTime;

    // --- Apply Gravity ---
    if (UseGravity)
        _Velocity.Y -= Gravity * Dt;

    // --- Apply Drag ---
    if (Drag > 0.f)
    {
        float DragFactor = FMath::Clamp(Dt * Drag, 0.f, 1.f);
        _Velocity *= (1.f - DragFactor);
    }

    // --- Apply Constraints ---
    if (Constraints.PosX) _Velocity.X = 0.f;
    if (Constraints.PosY) _Velocity.Y = 0.f;
    _Velocity.X = FMath::Clamp(_Velocity.X, -Constraints.MaxX, Constraints.MaxX);
    _Velocity.Y = FMath::Clamp(_Velocity.Y, -Constraints.MaxY, Constraints.MaxY);

    // --- Compute Proposed Movement ---
    FVector DeltaMove(_Velocity.X * Dt, 0.f, _Velocity.Y * Dt);
    FVector ActorLoc = GetOwner()->GetActorLocation();
    FHitResult Hit;

    GetOwner()->SetActorRelativeLocation(ActorLoc + DeltaMove, true, &Hit);
    OnGround = false;

    if (Hit.bBlockingHit)
    {
        FVector Normal = Hit.ImpactNormal.GetSafeNormal();

        // --- Correct Position to prevent penetration ---
        FVector Correction = FVector::VectorPlaneProject(DeltaMove, Normal);
        GetOwner()->SetActorLocation(ActorLoc + Correction + Normal * SkinWidth, true);

        FVector Vel3D(_Velocity.X, 0.f, _Velocity.Y);

        // --- Dynamic collision with another Rigidbody2D ---
        if (URigidbody2D* OtherBody = Hit.GetActor()->FindComponentByClass<URigidbody2D>())
        {
            FVector OtherVel3D(OtherBody->GetVelocity().X, 0.f, OtherBody->GetVelocity().Y);
            float RelativeVel = FVector::DotProduct(Vel3D - OtherVel3D, Normal);

            if (RelativeVel < 0.f)
            {
                const float Restitution = 0.8f;
                float Impulse = -(1.f + Restitution) * RelativeVel / (1.f / Mass + 1.f / OtherBody->Mass);
                Impulse = FMath::Clamp(Impulse, -Constraints.MaxCollisionImpulse, Constraints.MaxCollisionImpulse);

                FVector ImpulseVec = Impulse * Normal;
                Vel3D += ImpulseVec / Mass;
                OtherVel3D -= ImpulseVec / OtherBody->Mass;

                _Velocity = FVector2D(Vel3D.X, Vel3D.Z);
                OtherBody->SetVelocity(FVector2D(OtherVel3D.X, OtherVel3D.Z));
            }
        }
        else
        {
            // --- Static collision: remove velocity along collision normal ---
            FVector NormalVel = FVector::DotProduct(Vel3D, Normal) * Normal;
            Vel3D -= NormalVel;

            // Apply position correction with skin width
            GetOwner()->AddActorWorldOffset(Normal * (Hit.PenetrationDepth + SkinWidth), true);
            _Velocity = FVector2D(Vel3D.X, Vel3D.Z);
        }

        // --- Slope detection for ground ---
        float UpDot = FVector::DotProduct(Normal, FVector::UpVector);
        if (UpDot > 0.2f)
        {
            FVector SlideVel = FVector::VectorPlaneProject(Vel3D, Normal);
            _Velocity = FVector2D(SlideVel.X, SlideVel.Z);
            OnGround = true;
        }
    }

    // --- Ground snapping ---
    if (_Velocity.Y <= 0.f)
    {
        FHitResult GroundHit;
        FVector Start = GetOwner()->GetActorLocation();
        FVector DownDir = FVector::DownVector;

        if (OnGroundLastFrameNormal.SizeSquared() > 0.f)
            DownDir = (FVector::DownVector - OnGroundLastFrameNormal * 0.25f).GetSafeNormal();

        const float TraceDistance = 10.f + SkinWidth;
        const float GroundSnapDistance = 2.f + SkinWidth;
        FVector End = Start + DownDir * TraceDistance;

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());
        GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, Params);

        if (GroundHit.bBlockingHit)
        {
            float SlopeAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(GroundHit.ImpactNormal, FVector::UpVector)));
            if (SlopeAngle <= Constraints.MaxSlopeAngle)
            {
                FVector DesiredLoc = GroundHit.ImpactPoint + GroundHit.ImpactNormal * SkinWidth;
                FVector CurrentLoc = GetOwner()->GetActorLocation();

                if ((CurrentLoc - DesiredLoc).SizeSquared() > GroundSnapDistance * GroundSnapDistance)
                {
                    FVector NewLoc = FMath::VInterpTo(CurrentLoc, DesiredLoc, Dt, 20.f);
                    GetOwner()->SetActorLocation(NewLoc);
                }

                OnGroundLastFrameNormal = GroundHit.ImpactNormal;
                OnGround = true;
            }
            else
            {
                OnGroundLastFrameNormal = FVector::ZeroVector;
            }
        }
        else
        {
            OnGroundLastFrameNormal = FVector::ZeroVector;
        }
    }

    // --- Zero out very small velocities to avoid jitter ---
    if (_Velocity.SizeSquared() < SkinWidth * SkinWidth)
        _Velocity = FVector2D::ZeroVector;

    Speed = _Velocity.Size();
}

// --- Force application functions ---
void URigidbody2D::AddForce(FVector2D Force, EForceMode ForceMode)
{
    float Dt = GetWorld()->GetDeltaSeconds();

    switch (ForceMode)
    {
        case EForceMode::Force:           _Velocity += (Force / Mass) * Dt; break;
        case EForceMode::Acceleration:    _Velocity += Force * Dt; break;
        case EForceMode::Impulse:         _Velocity += Force / Mass; break;
        case EForceMode::VelocityChange:  _Velocity += Force; break;
    }
}

void URigidbody2D::AddForceAtPoint(float Force, FVector2D Point, EForceMode ForceMode)
{
    FVector2D Center = { GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Z };
    FVector2D Direction = Center - Point;
    float Distance = Direction.Size();

    if (Distance == 0.f)
        Direction = FVector2D(0.f, 1.f);
    else
        Direction /= Distance;

    AddForce(Direction * Force, ForceMode);
}

void URigidbody2D::AddForceX(float Force, EForceMode ForceMode)
{
    float Dt = GetWorld()->GetDeltaSeconds();
    switch (ForceMode)
    {
        case EForceMode::Force:           _Velocity.X += (Force / Mass) * Dt; break;
        case EForceMode::Acceleration:    _Velocity.X += Force * Dt; break;
        case EForceMode::Impulse:         _Velocity.X += Force / Mass; break;
        case EForceMode::VelocityChange:  _Velocity.X += Force; break;
    }
}

void URigidbody2D::AddForceY(float Force, EForceMode ForceMode)
{
    float Dt = GetWorld()->GetDeltaSeconds();
    switch (ForceMode)
    {
        case EForceMode::Force:           _Velocity.Y += (Force / Mass) * Dt; break;
        case EForceMode::Acceleration:    _Velocity.Y += Force * Dt; break;
        case EForceMode::Impulse:         _Velocity.Y += Force / Mass; break;
        case EForceMode::VelocityChange:  _Velocity.Y += Force; break;
    }
}

// --- Getters and Setters ---
FVector2D URigidbody2D::GetVelocity() { return _Velocity; }
void URigidbody2D::SetVelocity(FVector2D Velocity) { _Velocity = Velocity; }
bool URigidbody2D::IsOnGround() { return OnGround; }
float URigidbody2D::GetSpeed() { return Speed; }
