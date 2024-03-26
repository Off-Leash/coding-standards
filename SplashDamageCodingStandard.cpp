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

// @SPLASH_DAMAGE_FILE: <description>

// [markup.engine.file] If this file is being added to an engine location, mark this
// 	with the above comment at the start of the file. This removes the need for the
// 	SPLASH_DAMAGE_CHANGE engine guards around modifications in the file (see [markup.engine])

// [basic.layout] try to limit horizontal space and use vertical layout
//  there is a reason newspapers have columns ;)
//  80 characters is a good guideline to strive for
// --------------------------------------------- 80 char limit --------------->|
// --------------------------------------------- 100 char limit ---------------------------------->|
//  you can add an indicator with visual assist: 
//  Visual Assist Options -> Display -> Display indicator after column n

// [comment.type]
//  always use the C++ style and not the C /**/ style
//  because it shows up in searches
//  Visual Studio shortcut to toggle comments on a block:
//      Edit -> Advanced -> Un/Comment Selection
//      Ctrl+K Ctrl+C / Ctrl+K Ctrl+U

// [header.incl.order.cpp]
//  Generally speaking the include order of files in .cpp's should be
#include "SplashDamageCodingStandard.h"			// 1) the equivalent header file

#include <Components/PrimitiveComponent.h>		// 2) Engine files
#include <UnrealNetwork.h>

//#include <SDWidget.h>							// 3) SD Core files

//#include "Features/Awesome/Foo.h"				// 4) Shared or Feature files
//#include "Shared/Types.h"

//#include "ViewModel.h"						// 5) Local files

// [cpp.namespace.private] use a namespace to wrap translation-unit local free functions 
//  defined only in cpp files. Also mark them as static to enforce internal only linkage.
namespace SDCodingStandardHelpers
{
	static void PrivateHelper(const USDCodingStandardExampleComponent& Object)
	{
	}
}

// [basic.order] respect the order of declarations in the .h header
//  when you write the definitions here

// [basic.rule.brace] FOLLOW UE4 coding style i.e. Allman style aka one every line
void BraceStyle()
{
	const bool FailCondition = false, TrueCondition = true,
		SomethingElse = true, Contract = true, Binding = true;

	if (FailCondition)
	{
		return; // even for one liners
	}

	if (TrueCondition)
	{
		// ...
	}
	else if (SomethingElse)
	{
		// ...
	}
	else
	{
		// ...
	}

	// for `switch` statements follow Unreal's guideline
	// https://docs.unrealengine.com/latest/INT/Programming/Development/CodingStandard/#switchstatements

	// [basic.rule.parens] parenthesis link with the expression not the keyword
	for (;/*...*/;)
	{
	}

	// [cpp.return.early] use early returns to avoid excessive nesting
	//  especially for pre-conditions / contracts
	//  one exception is logic flow where too many early returns would hurt readability
	if (!Contract && !Binding)
	{
		return;
	}
	// versus
	if (Contract)
	{
		if (Binding)
		{
		}
	}

	// [cpp.if.init] use the if-with-initializer idiom
	if (const bool IsGame = FApp::IsGame())
	{
		// ...
	}
}

// [globals.no] avoid globals unless they're POD (plain-old-data)
//  they don't play well with Hot Reload and their order of initialization
//  is undefined. Epic loves them but we don't!
FIntPoint CachedCoordinates; // PASSABLE
class MyBigObject
{
	// ...
};
MyBigObject Cache1; // BAD
MyBigObject Cache2; // BAD - maybe this is started first, not Cache1

// [cpp.return] consider using TOptional for returns that can fail
//  instead of using C style pass by reference
TOptional<FIntRect> IntersectTest(const FIntPoint& min, const FIntPoint& max)
{
	// ...
	return (min.X > max.X || min.Y > max.Y) ? TOptional<FIntRect>() : FIntRect(min, max);
}

// [ue.gen.struct] if Blueprint variables are extracted in separate structures
//  it is possible to pass them around, thus not having to expose all functions as
//  methods in a class, thus leading to less coupling and faster compilation
float DoPassBlueprintVarStructs(const FSDCodingStandardBlueprintVarGroup& vars)
{
	return vars.CameraTraceVolumeWidth / 2.f;
}

void DontWasteMemory(const AActor& Actor)
{
	// [ue.container] Mind your allocations!
	//  don't go to the heap, go to the stack!
	TInlineComponentArray<UPrimitiveComponent*> PrimComponents; // 24 item reserved by default
	Actor.GetComponents(PrimComponents);

	// [ue.container] [ue.ecs.get] Customize the get-ers for this purpose!
	using TCustomAlloc = TInlineAllocator<32>;
	TArray<UActorComponent *, TCustomAlloc> LocalItems;
	Actor.GetComponents<UActorComponent, TCustomAlloc>(LocalItems);

	// [ue.container.reserve] Prepare upfront the containers
	//  cut down on the need to allocate per-item
	PrimComponents.Reserve(64);
	PrimComponents.Init(nullptr, 64);

	// [ue.container.reset] Don't empty, just reset!
	PrimComponents.Empty(); // BAD - deallocates for new 0 size
	PrimComponents.Reset(); // GOOD - same effect, but instant, no realloc
	PrimComponents.Empty(64); // PASSABLE - empty with slack

	// [hardware.cache] be mindful of cache access and plan your memory access accordingly
	//
	//  1 CPU cycle
	//  o
	//
	//  L1 cache access
	//  ooo
	//
	//  L2 cache access
	//  ooooooooo
	//
	//  L3 cache access
	//  oooooooooooooooooooooooooooooooooooooooooo
	//
	//  Main memory access
	//  oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
	//  oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
	//  oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
	//  oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
	//
	//  from https://twitter.com/srigi/status/917998817051541504
	//
	//  Some general tips
	//  - prefer linear data structures, don't jump via pointers too much etc
	//  - all cache access is through lines 64 bytes long, have that in mind!
	//  - in classes put related data together, group them by algorithm/logic access
	//  - be mindful of the invisible packing the compiler will do behind your back
	//      don't intermingle bool's or small types willy nilly with bigger ones etc
}

bool GameWithEditorChanges(const TArray<int>& Widgets)
{
	// [markup.editor] isolate Editor specific changes in game code
#if WITH_EDITOR
	// ...

	// [assert.editor] never assert in Editor code - try to recover to your best effort!
	check(Widgets.Num()); // <- BAD, will force-crash and potentially destroy work
	ensureMsgf(Widgets.Num(), TEXT("Must have widgets selected!")); // <- GOOD, doesn't force-crash

	// BETTER, doesn't force-crash & prevents potential errors
	if (!ensureMsgf(Widgets.Num(), TEXT("Must have widgets selected!")))
	{
		return false;
	}
#endif
	return true;
}

// [cpp.auto] avoid auto as often as possible

void NumericLimits()
{
	// [cpp.numericlimits] Use TNumericLimits instead of #defines such as FLT_MAX
	//  See http://api.unrealengine.com/INT/API/Runtime/Core/Math/TNumericLimits/

	// E.g. For all floating point types
	const float MaxPositiveFloatValue = TNumericLimits<float>::Max();
	const float MinPositiveFloatValue = TNumericLimits<float>::Min();
	const float MinNegativeFloatValue = TNumericLimits<float>::Lowest();

	// E.g. For integral types
	const int32 MaxPositiveIntValue = TNumericLimits<int32>::Max();
	// This is the same as Lowest() for all integral types.
	const int32 MinNegativeIntValue = TNumericLimits<int32>::Min();
}

void ASDCodingStandardExampleActor::BeginPlay()
{
	// [ue.ecs.super] always call Super:: method for Actor/Component tickable overridden functions
	//  other regular methods don't necessary need to do this
	Super::BeginPlay();
}

void ASDCodingStandardExampleActor::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDCodingStandardExampleActor, WantsToSprint);
}

void ASDCodingStandardExampleActor::OnRep_WantsToSprint()
{
}

void USDCodingStandardExampleComponent::LambdaStyle(const AActor* ExternalEntity) const
{
	// [cpp.lambda.general] use lambda's to your advantage
	//  especially when they will isolate work in the implementation
	//  rather than pollute the interface with helper methods
	//
	//  but don't abuse them - if their body becomes complex enough
	//  extract into separate function/method

	// [cpp.lambda.dangling] biggest problem in production is creating dangling references
	//  by capturing objects by reference that die before the lambda gets called
	auto lambda_dangling = [ExternalEntity]()
	{
		// will ExternalEntity still be valid at this point?
	};

	// [cpp.lambda.this] don't capture `this`!
	//  instead use the named captures to cherry pick
	auto lambda_this = [LocalCopy = this->BlueprintGroup.ShowCameraWidget]()
	{
		// LocalCopy available irregardless of the fate of parent
	};

	// [cpp.lambda.=&] avoid capturing everything by value or worse, by reference!
	auto lambda_avoid = [&]()
	{
		// HERE BE DRAGONS!
	};

	// [cpp.lambda.auto] the type deduction for the captures are quite convoluted
	//  - by reference: uses template rules; will preserve `const` and `&`
	//  - by value: uses template rules; will preserve `const`
	//  - init capture: uses `auto` rules; WILL MESS UP `const` and `&` (add them back manually)
	const int Original = 0;
	const int &Reference = Original;
	auto lambda_auto =
		[Original, Duplicate = Original, &RefDuplicate = Original, NotReference = Reference]()
	{
		// Original => `const int`
		// Duplicate => `int`
		// RefDuplicate => `const int &`
		// NotReference => `int`
	};

	// [cpp.lambda.shared]
	// Consider using the new shared pointer delegate:
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		AssetRegistryModule.Get().OnFilesLoaded().AddSP(this,
			&SDiscoveringAssetsDialog::AssetRegistryLoadComplete);
	}
}

void SDCodingStandardHelpers::PublicHelper(const USDCodingStandardExampleComponent& Object)
{
}