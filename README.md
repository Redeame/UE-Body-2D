[README.md](https://github.com/user-attachments/files/23291114/README.md)
# UE-Body-2D
Minimalistic rigidbody 2D physics implementation for unreal engine
## Features

- Gravity Simulation
- Drag / Air Resistance
- Axis and Velocity Constraints
- Collision Detection and Response
- Ground Detection and Slope Handling
- Velocity Correction and Stability
- Physical Properties
- Tick-Based Simulation



## Usage/Examples
The following examples show how to use the URigidbody2D component to apply forces and control 2D movement.

### Basic Movement

Applies horizontal and vertical forces for character movement.
Assumes you have custom input functions and a `URigidbody2D` attached to your Actor.
```c++
// Moving left or right
if(float MoveDirection = GetMoveDirection()){
    RigidbodyComponent.AddForceX(MoveSpeed * MoveDirection, EForceMode::Acceleration)
}

// Jumping
if(ShouldJump() && RigidbodyComponent.IsOnGround()){
    RigidbodyComponent.AddForceY(JumpHeight, EForceMode::Impulse)
}
```

### Applying Force at a Specific Point (Explosions)

Applies force from a world-space point to simulate explosions or impacts.
```c++
struct FExplosion {
    FVector2D Location;
    float Force;
};

TArray<FExplosion> Explosions = {
    { FVector2D(100, 200), 2000.0f },
    { FVector2D(-150, 50), 1500.0f }
};

for (const FExplosion& Explosion : Explosions) {
    RigidbodyComponent.AddForceAtPoint(Explosion.Force, Explosion.Location, EForceMode::Impulse);
}

```

### AddForce and SetVelocity

Use `AddForce` for ongoing acceleration or impulses, and `SetVelocity` for instant motion.

```c++
RigidbodyComponent.AddForce(FVector2D(100.0f, 0.0f), EForceMode::Force); // Continuous force
RigidbodyComponent.SetVelocity(FVector2D(300.0f, 0.0f)); // Instant velocity change
```

### Constraints Example

Control position, slope, or collision limits through `FRigidbody2DConstraints`.

```c++
FRigidbody2DConstraints Constraints;
Constraints.PosX = true;                 // Lock X-axis
Constraints.MaxY = 500.0f;               // Limit vertical movement
Constraints.MaxSlopeAngle = 45.0f;       // Max walkable slope
Constraints.MaxCollisionImpulse = 8000.0f; // Cap collision impact

RigidbodyComponent.Constraints = Constraints;
```

### Force Modes
`EForceMode` changes how the applied force affects velocity:
|Mode|Formula|Common Use
|-|-|-|
|Force|`velocity += force / mass * deltaTime`|Continuous push or wind|
|Acceleration|`velocity += force * deltaTime`|Input-based motion|
|Impulse|`velocity += force / mass`|Jumps or explosions|
|VelocityChange|`velocity += force`|Instant velocity shift|

## Code Demo
### Movement Code Bluepint Implementation Demo
![Demo Movement](https://github.com/Redeame/UE-Body-2D/blob/main/Demo/Demo.png?raw=true)
### Grappling Hook Bluepint Implementation Demo
![Demo Movement](https://github.com/Redeame/UE-Body-2D/blob/main/Demo/DemoGrapple.png?raw=true)

## Gameplay Demo

https://github.com/user-attachments/assets/75d989af-618d-4eb9-b599-c68b223eb25e

## Acknowledgements

 - [Unity Rigidbody2D System](https://docs.unity3d.com/ScriptReference/Rigidbody2D.html)
 - [Readme Editor](https://readme.so/editor)
 - [Cobra Code](https://www.youtube.com/@CobraCode)
 - [Grappling Hook Tutorial I Used](https://www.youtube.com/watch?v=4SHDo8qkoco)
