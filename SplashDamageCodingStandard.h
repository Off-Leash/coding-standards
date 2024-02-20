// BSD 3-Clause License
//
// Copyright (c) 2019, Splash Damage
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// [header.guard] include guard
#pragma once

// [basic.rule] FOLLOW UE4 coding style! 
//  https://docs.unrealengine.com/latest/INT/Programming/Development/CodingStandard/
//  especially here in the header/interface
//  in the implementation (.cpp) you are allowed to diverge a bit for personal taste
//  but NEVER when it comes to things that will be visible from outside
//  aka public variables, functions, names etc

// [header.iwyu] we shall use IWYU 
//  https://docs.unrealengine.com/latest/INT/Programming/UnrealBuildSystem/IWYUReferenceGuide/index.html
#include <CoreMinimal.h>

// [header.engine.incl] list all the Engine dependencies with angle brackets includes
#include <GameFramework/Character.h>
#include <GameFramework/Pawn.h>
//  group and separate them by shared logic or purpose
//  use full relative path (in relation to Engine/Classes folders)
#include <GameFramework/HUD.h>
#include <GameFramework/HUDHitBox.h>

// [header.incl.order]
//  Generally speaking the include order of files should be:
//  1) CoreMinimal
//  2) Other Engine files
//  3) SD Core files
//  4) local includes - with "" style
//  5) this file .generate.h - with "" style

// [header.incl.privatepath]
//  Never include private paths in a public header file. This can cause hard to track linker
//  or compiler errors at a later date. Either re-factor your code so that you can use a forward
//  declare or, if necessary, make the file you are trying to include public.

// [header.incl.gen] always have the .generated files last
#include "SplashDamageCodingStandard.generated.h"

// [header.rule.fwd] list as many forward declaration as you can
//  it's also acceptable to declare them at the usage site, but if they recur, bring them up here
class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;

UCLASS()
// [class.name] embed the agreed project codename while following UE4 naming rules. 
//  - See [module.naming.class]
class AVallhundCodingStandardExampleActor : public ACharacter
{
	GENERATED_BODY() // GENERATED_UCLASS_BODY is deprecated

	// [ue.gen.access] follow up the reflection macros with access level specifiers 
	//  because they don't modify them anymore (used to be the case)
	//  structs do not need to specify public, unless they also specify protected/private (see [ue.gen.struct])
public:

	// [class.ctor.default] don't write an empty one, remove it
	//  due to the way GENERATED_BODY works, `= default`-ing the constructor can lead to crashes

	// [class.dtor] don't write empty one, default or remove it
	//  respect the rule of 3/5/0 http://en.cppreference.com/w/cpp/language/rule_of_three
	~AVallhundCodingStandardExampleActor() = default; // or just remove

	// [class.virtual] explicitly mark up virtual methods
	//  - always use the `override` specifier
	//  - use the `final` specifier with care as it can have large ramifications on downstream classes.
	//  - group overridden functions by the class that first defined, them using begin/end comments
	// Begin AActor override
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// End AActor override

	// [class.same-line] DON'T write definitions on the same line as declarations!
	//  it makes debugging impossible
	// [class.inline.bad] NEVER USE `inline or `FORCEINLINE` inside a class declaration
	//  - it's useless and leads to linking errors
	//  - definitions(bodies) inside a class/struct are implicitly inline!
	// [comment.useless] DON'T write meaningless comments!
	//  they should always reflect bigger purpose or reveal hidden details
	// Returns Mesh subobject
	/* BAD -> */ FORCEINLINE const USkeletalMeshComponent* GetMesh() const { return MyMesh; } /* <- BAD */

	// [class.inline.good] Move the definitions of inline function outside the class
	const USkeletalMeshComponent* GoodExampleOfInline() const;

protected:
	// [class.order] Do not alternate between functions and variables in the class declaration
	//  Put all functions first, all variables last

	// [ue.ecs.split] Split functionality into components
	//  avoid creating monolithic giant classes!

	// [ue.ecs.gc] never use naked pointers to UObject's, always have UPROPERTY or UE smart ptr
	//  Generally, for storing pointers to classes you don't own, use TWeakObjectPtr.
	TWeakObjectPtr<const USkeletalMeshComponent> OtherMesh = nullptr;
	//  Generally, for storing pointers to classes you do own, use UPROPERTY().
	UPROPERTY(BlueprintReadOnly, Category = Mesh)
	const USkeletalMeshComponent* MyMesh = nullptr;
	//  For more information on other forms of UE4 smart pointers see
	//  https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/SmartPointerLibrary/ 

	// [class.order.replication] As an exception to [class.ordering], declare replication functions
	//  next to the variable that used them to avoid cluttering the interface with these functions 
	//  that are not called by client code.
	UPROPERTY(Transient, ReplicatedUsing = "OnRep_WantsToSprint")
	bool WantsToSprint = false;
	UFUNCTION()
	void OnRep_WantsToSprint();
};

// [ue.gen.struct] [ue.ecs.group] move groups of Blueprint exposed variables into separate structures
//  this helps refactoring and takes the stress out of interfaces
//  for ex implementation-only functions can just take the group as an argument
//  NOTE: this is not always possible, but highly desirable for "config" like variables
USTRUCT(BlueprintType)
struct FVallhundCodingStandardBlueprintVarGroup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	TArray<int> WidgetCameraLevels; // [class.member.def] Unreal arrays start initialized and empty

	// [class.member.def] always provide defaults for member variables
	//  prefer assigning them here, not in the constructor - that should be reserved
	//  for more complicated init logic / creation 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float CameraTraceVolumeWidth = 96.0f * 5;

	// [class.member.config] member variables that are used as editor config variables
	//  MUST have a comment supplied as it shows up as the tooltip in the Editor.
	//  They should also be marked as EditDefaultsOnly by default.
	//  If you expect them to be read in blueprints then use BlueprintReadOnly.
	//  Only use EditAnywhere, EditInstanceOnly or BlueprintReadWrite if it's necessary for your use case.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float CameraTraceVolumeHeight = 96.0f * 5;

	// [hardware.cache] try to order data members with cache and alignment in mind
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	bool ShowCameraWidget = true;

	// [hardware.cache] for ex grouping similar types like this will minimize the 
	//  internal padding the compiler will add
	//  general rule of thumb: sort in descending order by size 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	bool ShowWeaponWidget = true;
};

// [cpp.enum.strong] use the strongly typed enums rather than old C style + dirty namespace tricks
// [cpp.enum.generated] Use the GenerateStringFuncs parameter if you need string conversion or
//  query the number of values. NOTE: only available on certain projects 
//UENUM(GenerateStringFuncs)
enum class EVallhundCodingStandardEnum // `: uint8` optional underlying type 
{
	ValueA,
	ValueB,
	ValueC,

	// [cpp.enum.generated.count] Don't put a label to represent the number of values in the enum
	//  use EnumAutoGen::GetNumValues<EVallhundCodingStandardEnum>() instead.
	Max // <- BAD
};

UCLASS()
class UVallhundCodingStandardExampleComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// [func.arg.readability] avoid consecutive chains of same type, avoid too many arguments
	/* BAD -> */ void FuncWithTooManyArgs(const FVector& Location, const FVector& Origin,
	const FVector& EndPoint, const FRotator& Rotation, const UPrimitiveComponent& Parent,
	const AActor& Owner) const; /* <- BAD */
	//  try to add a helper structure, or possibly split into more functions that are less complex

	// [cpp.lambda] used later for guidelines
	void LambdaStyle(const AActor* ExternalEntity) const;

	// [cpp.rel_ops] when implementing relation operators, use the binary free form
	//  as it provides the most flexibility with operands order and usage
	//  if it needs to access private members, make it `friend` and respect [class.inline.good]
	//  NOTE: add working functionality only for `==` and `<` everything else can be inferred
	friend bool operator == (
		const USDCodingStandardExampleComponent& lhs,
		const USDCodingStandardExampleComponent& rhs);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSDCodingStandardBlueprintVarGroup BlueprintGroup;

private:
	// [naming.bool] Exception from UE4 coding standard:
	//  DON'T add `b` prefix to bool declaration names!
	//  instead use English Modal Verbs and variations like: Can, Does, Will, Is, Has, Use, etc
	bool bInGame, bAttack, bLog, bCustomStencil; /* <- BAD */
	bool InGame, CanAttack, UseLog, HasCustomStencil; /* <- GOOD */
};

// [class.inline.good]
//  - don't use FORCEINLINE unless you really want to persuade the
//      compiler to inline a complicated function
//      which is NOT guaranteed to work!
//  - 100% for one-liners or very simple functions inline-ing will work regardless
//  - here we have to use the `inline` to allow proper linkage
//      this simple function would have been inlined anyway
//  - having it like this also helps refactoring
//      you can easily move this to the .cpp without messing up the class definition
inline const USkeletalMeshComponent* AVallhundCodingStandardExampleActor::GoodExampleOfInline() const
{
	return OtherMesh.Get();
}

// [cpp.rel_ops] when implementing relation operators, use the binary free form
//  as it provides the most flexibility with operands order and usage
//  if it needs to access private members, make it `friend` and respect [class.inline.good]
//  NOTE: add working functionality only for `==` and `<` everything else can be inferred from them
inline bool operator == (
	const USDCodingStandardExampleComponent& lhs,
	const USDCodingStandardExampleComponent& rhs)
{
	return lhs.InGame == rhs.InGame;
}

// [cpp.namepace.public] Use a namespace to contain free functions.
//  This helps manage potential name clashes and unity build issues.
namespace VallhundCodingStandardHelpers
{
	void PublicHelper(const USDCodingStandardExampleComponent& Object);
}

// [module.naming] when adding new module folders follow a consistent naming convention
//  - Interface modules should be prefixed with Interface.
//  - Game independent modules should be prefixed with Core.
//  - Game specific modules do not have any prefix.

// [module.naming.class] code in modules should also follow our naming convention
//  - Interfaces in Interface modules should be prefixed with IVallhund
//  - Classes, Struct & Enums should be prefixed with Vallhund

// [module.naming.namespace]
//  - Namespaces in Game modules should be prefixed with Vallhund
//  - Namespaces should be named the same as the file they live in
//  - Namespaces may be postfixed with "Helpers" where it prevents ambiguity