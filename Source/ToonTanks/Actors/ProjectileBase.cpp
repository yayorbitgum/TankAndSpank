// -------------------------------------------------------------------------------------------
/* Dynamic Delegates are events you can set to be called and responded to,
*   so that every time this event is fired, anything that's listening to
*   this event will receive it, and then be able to take action or call
*   their own function based on that event.
*
*   Example:    ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
*
*   - Makes "OnHit" a Delegate, that is called when an OnComponentHit Event is fired off.
*
*   - To do this, our OnHit() has to be declared a UFUNCTION first, and
*		we have to give it enough parameters to make use of the info we'll get from the OnComponentHit Event.
*
*   - We can find what parameters we need by ctrl-clicking into OnComponentHit,
*		and searching upwards to where the delegate parameters are declared.
*
*
*  All Dynamic Delegates need to be created as UFUNCTION.
*
* UFUNCTION is the same as C++ functions, with a few differences.
*	- Can be called or overridden from within Blueprints Visual Scripting,
*		/w parameters: BlueprintCallable, BlueprintImplementableEvent, BlueprintPure.
*
*	- Can be assigned as delegates within the default properties of a class.
*		This is common for input action or axes binds to functions in Input classes.
*
*	- Replication callbacks: UFunction will be called when the variable changes that
*		it's associated with, and is replicated across the network.
*
*	- The only type of function that can be declared as an exec function, allowing them
*		to be called by the player from the in-game console.
*
*/
// -------------------------------------------------------------------------------------------

#include "ProjectileBase.h"

#include "Projects.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	// Since UProjectileMovementComponent isn't a tangible thing to be part of the scene,
	// it doesn't need to attach to anything like the root component, like other solids do.
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = MoveSpeedStart;
	ProjectileMovement->MaxSpeed = MoveSpeedMax;
	InitialLifeSpan = LifeSpan;

	// This binds "OnComponentHit" event to "OnHit()" function, so OnHit is called any time this component is hit.
	// AddDynamic() is a helper macro that binds the event to the object and method we want to call.
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

}

// *** See comments at top of file. ***
/// When this actor hits another actor. Handles damage, type, etc.
void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();

	if (!MyOwner) {
		return;
	}

	// If we hit a valid actor that isn't itself nor the owning pawn:
	if (OtherActor && OtherActor != this && OtherActor != MyOwner) {
		AController* InstigatorController = MyOwner->GetInstigatorController();

		// Generate and apply the damage.
		UGameplayStatics::ApplyDamage(
			OtherActor,				// Actor that will be damaged.
			Damage,					// Damage amount.
			InstigatorController,	// Which player instigated it.
			this,					// What actor caused the damage.
			DamageType				// Type of damage done.
			);
	}

	// Then destroy the projectile after LifeSpan seconds, via a Timer (aka Delay :D).
	FTimerManager& DestroyTimer = GetWorld()->GetTimerManager();

	DestroyTimer.SetTimer(
		OUT ExplosionTimerHandle,
		this,
		&AProjectileBase::DestroyProjectile,
		LifeSpan,
		false
		);

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileBase::DestroyProjectile()
{
	Destroy();
}