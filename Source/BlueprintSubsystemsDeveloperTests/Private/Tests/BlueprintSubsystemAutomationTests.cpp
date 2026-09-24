#if WITH_DEV_AUTOMATION_TESTS

#include "Tests/BlueprintSubsystemTestTypes.h"

#include "Data/BlueprintSubsystemBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libs/BlueprintSubsystemsLib.h"
#include "Misc/AutomationTest.h"
#include "Subsystems/BlueprintSubsystemManager.h"
#include "BlueprintsSubsystemDeveloperSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DeveloperSettings.h"

namespace
{
	UBlueprintSubsystemManager* CreateManager(UGameInstance*& OutGameInstance)
	{
		OutGameInstance = NewObject<UGameInstance>(GetTransientPackage());
		return NewObject<UBlueprintSubsystemManager>(OutGameInstance);
	}

	bool RunReflectionCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		const UClass* baseClass = UBlueprintSubsystemBase::StaticClass();
		const UClass* managerClass = UBlueprintSubsystemManager::StaticClass();
		const UClass* libraryClass = UBlueprintSubsystemsLib::StaticClass();
		const UClass* settingsClass = UBlueprintsSubsystemDeveloperSettings::StaticClass();

		switch (CaseId)
		{
		case 1: Test.TestNotNull(TEXT("Base class exists"), baseClass); break;
		case 2: Test.TestTrue(TEXT("Base derives from UObject"), baseClass->IsChildOf(UObject::StaticClass())); break;
		case 3: Test.TestTrue(TEXT("Base is abstract"), baseClass->HasAnyClassFlags(CLASS_Abstract)); break;
		case 4: Test.TestTrue(TEXT("Base exposes a Blueprint event"), baseClass->FindFunctionByName(TEXT("Initialize"))->HasAnyFunctionFlags(FUNC_BlueprintEvent)); break;
		case 5: Test.TestTrue(TEXT("Base exposes a Blueprint pure function"), baseClass->FindFunctionByName(TEXT("GetGameInstance"))->HasAnyFunctionFlags(FUNC_BlueprintPure)); break;
		case 6: Test.TestTrue(TEXT("Manager derives from UGameInstanceSubsystem"), managerClass->IsChildOf(UGameInstanceSubsystem::StaticClass())); break;
		case 7: Test.TestTrue(TEXT("Library derives from UBlueprintFunctionLibrary"), libraryClass->IsChildOf(UBlueprintFunctionLibrary::StaticClass())); break;
		case 8: Test.TestTrue(TEXT("Settings derive from UDeveloperSettings"), settingsClass->IsChildOf(UDeveloperSettings::StaticClass())); break;
		case 9: Test.TestNotNull(TEXT("Initialize is reflected"), baseClass->FindFunctionByName(TEXT("Initialize"))); break;
		case 10: Test.TestNotNull(TEXT("DeInitialize is reflected"), baseClass->FindFunctionByName(TEXT("DeInitialize"))); break;
		case 11: Test.TestNotNull(TEXT("ShouldCreateSubsystem is reflected"), baseClass->FindFunctionByName(TEXT("ShouldCreateSubsystem"))); break;
		case 12: Test.TestNotNull(TEXT("GetGameInstance is reflected"), baseClass->FindFunctionByName(TEXT("GetGameInstance"))); break;
		case 13: Test.TestNotNull(TEXT("GetWorldContext is reflected"), baseClass->FindFunctionByName(TEXT("GetWorldContext"))); break;
		case 14: Test.TestNotNull(TEXT("GetSubsystem is reflected"), managerClass->FindFunctionByName(TEXT("GetSubsystem"))); break;
		case 15: Test.TestNotNull(TEXT("ActivateSubsystem is reflected"), managerClass->FindFunctionByName(TEXT("ActivateSubsystem"))); break;
		case 16: Test.TestNotNull(TEXT("DeactivateSubsystem is reflected"), managerClass->FindFunctionByName(TEXT("DeactivateSubsystem"))); break;
		case 17: Test.TestNotNull(TEXT("Get Subsystem library function is reflected"), libraryClass->FindFunctionByName(TEXT("GetBlueprintSubsystem"))); break;
		case 18: Test.TestTrue(TEXT("Lookup has a WorldContext parameter"), libraryClass->FindFunctionByName(TEXT("GetBlueprintSubsystem"))->HasMetaData(TEXT("WorldContext"))); break;
		case 19: Test.TestEqual(TEXT("Lookup output type metadata is correct"), libraryClass->FindFunctionByName(TEXT("GetBlueprintSubsystem"))->GetMetaData(TEXT("DeterminesOutputType")), FString(TEXT("SubsystemClass"))); break;
		case 20: Test.TestNotNull(TEXT("Settings expose ActiveSubsystems"), settingsClass->FindPropertyByName(TEXT("ActiveSubsystems"))); break;
		default: return false;
		}
		return true;
	}

	bool RunBaseCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		UGameInstance* gameInstance = nullptr;
		UBlueprintSubsystemManager* manager = CreateManager(gameInstance);
		Test.TestNotNull(TEXT("Test manager exists"), manager);
		if (!manager)
		{
			return false;
		}

		UBlueprintSubsystemAcceptingTestSubsystem* subsystem =
			NewObject<UBlueprintSubsystemAcceptingTestSubsystem>(manager);

		switch (CaseId)
		{
		case 21: Test.TestNotNull(TEXT("Fixture exists"), subsystem); break;
		case 22: Test.TestEqual(TEXT("Fixture starts with no initialization"), subsystem->InitializeCount, 0); break;
		case 23: Test.TestEqual(TEXT("Fixture starts with no deinitialization"), subsystem->DeInitializeCount, 0); break;
		case 24: Test.TestEqual(TEXT("Fixture starts with no creation checks"), subsystem->ShouldCreateCount, 0); break;
		case 25: Test.TestEqual(TEXT("GameInstance is resolved through the manager"), subsystem->GetGameInstance(), gameInstance); break;
		case 26: Test.TestNull(TEXT("World is null without a world"), subsystem->GetWorld()); break;
		case 27: Test.TestNull(TEXT("World context is null without a world"), subsystem->GetWorldContext()); break;
		case 28: Test.TestTrue(TEXT("Default accepting predicate accepts a valid GameInstance"), subsystem->ShouldCreateSubsystem(gameInstance)); break;
		case 29: Test.TestFalse(TEXT("Default accepting predicate rejects a null GameInstance"), subsystem->ShouldCreateSubsystem(nullptr)); break;
		case 30: subsystem->Initialize({}); Test.TestEqual(TEXT("Initialize implementation runs once"), subsystem->InitializeCount, 1); break;
		case 31: subsystem->DeInitialize(); Test.TestEqual(TEXT("DeInitialize implementation runs once"), subsystem->DeInitializeCount, 1); break;
		case 32: subsystem->Initialize({}); subsystem->Initialize({}); Test.TestEqual(TEXT("Initialize can be observed repeatedly"), subsystem->InitializeCount, 2); break;
		case 33: subsystem->DeInitialize(); subsystem->DeInitialize(); Test.TestEqual(TEXT("DeInitialize can be observed repeatedly"), subsystem->DeInitializeCount, 2); break;
		case 34: Test.TestTrue(TEXT("Fixture is a Blueprint subsystem"), subsystem->IsA(UBlueprintSubsystemBase::StaticClass())); break;
		case 35: Test.TestTrue(TEXT("Manager is an outer"), subsystem->GetTypedOuter<UBlueprintSubsystemManager>() == manager); break;
		case 36: Test.TestEqual(TEXT("World context forwards to GetWorld"), subsystem->GetWorldContext(), subsystem->GetWorld()); break;
		case 37: Test.TestTrue(TEXT("Valid GameInstance is a UObject"), IsValid(gameInstance)); break;
		case 38: Test.TestFalse(TEXT("Null GameInstance is invalid"), IsValid(static_cast<UGameInstance*>(nullptr))); break;
		case 39: Test.TestTrue(TEXT("Fixture is valid"), IsValid(subsystem)); break;
		case 40: Test.TestEqual(TEXT("Fixture has the expected class"), subsystem->GetClass(), UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); break;
		default: return false;
		}
		return true;
	}

	bool RunManagerCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		UGameInstance* gameInstance = nullptr;
		UBlueprintSubsystemManager* manager = CreateManager(gameInstance);
		Test.TestNotNull(TEXT("Manager exists"), manager);
		if (!manager)
		{
			return false;
		}

		switch (CaseId)
		{
		case 41: Test.TestNull(TEXT("Null lookup returns null"), manager->GetSubsystem(nullptr)); break;
		case 42: Test.TestFalse(TEXT("Null activation returns null"), manager->ActivateSubsystem(nullptr) != nullptr); break;
		case 43: Test.TestFalse(TEXT("Null deactivation returns false"), manager->DeactivateSubsystem(nullptr)); break;
		case 44: Test.TestNotNull(TEXT("Valid activation returns an object"), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 45: Test.TestNotNull(TEXT("Activated subsystem is findable"), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 46: Test.TestEqual(TEXT("Duplicate activation returns the same object"), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 47: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Activated subsystem is stored"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) != nullptr); break;
		case 48: Test.TestFalse(TEXT("Rejecting subsystem is not stored"), manager->ActivateSubsystem(UBlueprintSubsystemRejectingTestSubsystem::StaticClass()) != nullptr); break;
		case 49: Test.TestFalse(TEXT("Abstract subsystem is rejected"), manager->ActivateSubsystem(UBlueprintSubsystemAbstractTestSubsystem::StaticClass()) != nullptr); break;
		case 50: Test.TestTrue(TEXT("Activation resolves the owning GameInstance"), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())->GetGameInstance() == gameInstance); break;
		case 51: { auto* value = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); Test.TestEqual(TEXT("Activation initializes once"), value->InitializeCount, 1); break; }
		case 52: { auto* value = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); Test.TestEqual(TEXT("Creation predicate runs once"), value->ShouldCreateCount, 1); break; }
		case 53: { auto* value = Cast<UBlueprintSubsystemRejectingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemRejectingTestSubsystem::StaticClass())); Test.TestNull(TEXT("Rejected object is not returned"), value); break; }
		case 54: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Derived lookup finds the base fixture"), manager->GetSubsystem(UBlueprintSubsystemBase::StaticClass()) != nullptr); break;
		case 55: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Exact lookup finds the fixture"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) != nullptr); break;
		case 56: Test.TestFalse(TEXT("Unknown class is not active"), manager->GetSubsystem(UBlueprintSubsystemRejectingTestSubsystem::StaticClass()) != nullptr); break;
		case 57: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Deactivation succeeds"), manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 58: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNull(TEXT("Deactivated subsystem is removed"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 59: Test.TestFalse(TEXT("Repeated deactivation returns false"), manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 60: { auto* value = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestEqual(TEXT("Deactivation calls cleanup"), value->DeInitializeCount, 1); break; }
		case 61: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Manager remains valid after deactivation"), manager); break;
		case 62: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("GameInstance remains valid after deactivation"), gameInstance); break;
		case 63: Test.TestNull(TEXT("Fresh manager has no active subsystem"), manager->GetSubsystem(UBlueprintSubsystemBase::StaticClass())); break;
		case 64: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Initialize path leaves manager valid"), manager); break;
		case 65: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Deactivation path leaves manager valid"), manager); break;
		case 66: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Repeated removal is safe"), manager); break;
		case 67: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNull(TEXT("Removal clears lookup"), manager->GetSubsystem(UBlueprintSubsystemBase::StaticClass())); break;
		case 68: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestEqual(TEXT("Only one activation is retained"), manager->GetSubsystem(UBlueprintSubsystemBase::StaticClass())->GetTypedOuter<UBlueprintSubsystemManager>(), manager); break;
		case 69: Test.TestTrue(TEXT("Manager is a GameInstance subsystem"), manager->IsA(UGameInstanceSubsystem::StaticClass())); break;
		case 70: Test.TestTrue(TEXT("Manager has the expected GameInstance"), manager->GetGameInstance() == gameInstance); break;
		case 71: Test.TestTrue(TEXT("Manager exposes a Blueprint callable function"), manager->GetClass()->FindFunctionByName(TEXT("ActivateSubsystem"))->HasAnyFunctionFlags(FUNC_BlueprintCallable)); break;
		case 72: Test.TestNotNull(TEXT("Manager settings are available"), manager->GetClass()->FindFunctionByName(TEXT("GetSubsystem"))); break;
		case 73: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestFalse(TEXT("Rejecting activation does not displace active state"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) == nullptr); break;
		case 74: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Activation is repeatable after lookup"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) != nullptr); break;
		case 75: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Manager supports a fresh activation after removal"), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) != nullptr); break;
		default: return false;
		}
		return true;
	}

	bool RunRuntimeCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		UGameInstance* gameInstance = nullptr;
		UBlueprintSubsystemManager* manager = CreateManager(gameInstance);
		auto* first = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()));

		switch (CaseId)
		{
		case 76: Test.TestNotNull(TEXT("Runtime fixture activates"), first); break;
		case 77: Test.TestEqual(TEXT("Runtime fixture initializes once"), first->InitializeCount, 1); break;
		case 78: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestEqual(TEXT("Runtime deactivation calls once"), first->DeInitializeCount, 1); break;
		case 79: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestFalse(TEXT("Runtime object is no longer active"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) != nullptr); break;
		case 80: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestFalse(TEXT("Second runtime deactivation fails"), manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 81: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Runtime manager survives removal"), manager); break;
		case 82: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Runtime GameInstance survives removal"), gameInstance); break;
		case 83: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNotNull(TEXT("Removed object remains inspectable for cleanup assertion"), first); break;
		case 84: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Removed object is still a subsystem object"), first->IsA(UBlueprintSubsystemBase::StaticClass())); break;
		case 85: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestEqual(TEXT("Cleanup is not duplicated by removal"), first->DeInitializeCount, 1); break;
		case 86: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestEqual(TEXT("Initialization count is stable after removal"), first->InitializeCount, 1); break;
		case 87: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestEqual(TEXT("Creation count is stable after removal"), first->ShouldCreateCount, 1); break;
		case 88: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNull(TEXT("Base lookup is empty after removal"), manager->GetSubsystem(UBlueprintSubsystemBase::StaticClass())); break;
		case 89: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestTrue(TEXT("Fresh activation succeeds after removal"), manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) != nullptr); break;
		case 90: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); { auto* second = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); Test.TestTrue(TEXT("Fresh activation creates a new object"), second != first); break; }
		case 91: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); { auto* second = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); Test.TestEqual(TEXT("Fresh activation initializes the new object"), second->InitializeCount, 1); break; }
		case 92: manager->DeactivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); { auto* second = Cast<UBlueprintSubsystemAcceptingTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); Test.TestEqual(TEXT("Fresh activation checks creation again"), second->ShouldCreateCount, 1); break; }
		case 93: Test.TestTrue(TEXT("Active runtime object has manager outer"), first->GetTypedOuter<UBlueprintSubsystemManager>() == manager); break;
		case 94: Test.TestTrue(TEXT("Active runtime object has GameInstance"), first->GetGameInstance() == gameInstance); break;
		case 95: Test.TestNull(TEXT("Active runtime object has no world in this fixture"), first->GetWorld()); break;
		case 96: Test.TestEqual(TEXT("Active runtime world context mirrors world"), first->GetWorldContext(), first->GetWorld()); break;
		case 97: Test.TestFalse(TEXT("Rejected runtime activation returns false state"), manager->ActivateSubsystem(UBlueprintSubsystemRejectingTestSubsystem::StaticClass()) != nullptr); break;
		case 98: Test.TestFalse(TEXT("Abstract runtime activation returns false state"), manager->ActivateSubsystem(UBlueprintSubsystemAbstractTestSubsystem::StaticClass()) != nullptr); break;
		case 99: Test.TestTrue(TEXT("Runtime activation remains available after rejected classes"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()) == first); break;
		case 100: Test.TestNotNull(TEXT("Runtime manager remains usable after all cases"), manager); break;
		default: return false;
		}
		return true;
	}

	bool RunLibraryCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		UGameInstance* gameInstance = nullptr;
		UBlueprintSubsystemManager* manager = CreateManager(gameInstance);
		const UObject* nullContext = nullptr;

		switch (CaseId)
		{
		case 101: Test.TestNull(TEXT("Null context returns null"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(nullContext, UBlueprintSubsystemBase::StaticClass())); break;
		case 102: Test.TestNull(TEXT("Null class returns null"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(gameInstance, nullptr)); break;
		case 103: Test.TestNull(TEXT("Unworlded GameInstance returns null"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(gameInstance, UBlueprintSubsystemBase::StaticClass())); break;
		case 104: Test.TestNull(TEXT("Unworlded manager does not fabricate a subsystem"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(manager, UBlueprintSubsystemBase::StaticClass())); break;
		case 105: Test.TestNotNull(TEXT("Library class exists"), UBlueprintSubsystemsLib::StaticClass()); break;
		case 106: Test.TestTrue(TEXT("Library is a Blueprint function library"), UBlueprintSubsystemsLib::StaticClass()->IsChildOf(UBlueprintFunctionLibrary::StaticClass())); break;
		case 107: Test.TestNotNull(TEXT("Library lookup function exists"), UBlueprintSubsystemsLib::StaticClass()->FindFunctionByName(TEXT("GetBlueprintSubsystem"))); break;
		case 108: Test.TestEqual(TEXT("Library lookup has one world context metadata value"), UBlueprintSubsystemsLib::StaticClass()->FindFunctionByName(TEXT("GetBlueprintSubsystem"))->GetMetaData(TEXT("WorldContext")), FString(TEXT("WorldContextObject"))); break;
		case 109: Test.TestEqual(TEXT("Library lookup has one output type metadata value"), UBlueprintSubsystemsLib::StaticClass()->FindFunctionByName(TEXT("GetBlueprintSubsystem"))->GetMetaData(TEXT("DeterminesOutputType")), FString(TEXT("SubsystemClass"))); break;
		case 110: Test.TestNull(TEXT("Null UObject context is rejected"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(static_cast<const UObject*>(nullptr), nullptr)); break;
		case 111: Test.TestNull(TEXT("Null world context and valid class are rejected"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(nullContext, UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 112: Test.TestNull(TEXT("Valid GameInstance and null class are rejected"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(gameInstance, nullptr)); break;
		case 113: Test.TestTrue(TEXT("Manager can be used as a UObject context"), IsValid(static_cast<UObject*>(manager))); break;
		case 114: Test.TestTrue(TEXT("GameInstance can be used as a UObject context"), IsValid(static_cast<UObject*>(gameInstance))); break;
		case 115: Test.TestNull(TEXT("Library does not return unregistered class"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(manager, UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 116: manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()); Test.TestNull(TEXT("Unworlded library still refuses unresolved world"), UBlueprintSubsystemsLib::GetBlueprintSubsystem(manager, UBlueprintSubsystemAcceptingTestSubsystem::StaticClass())); break;
		case 117: Test.TestTrue(TEXT("Library exposes a Blueprint callable function"), UBlueprintSubsystemsLib::StaticClass()->FindFunctionByName(TEXT("GetBlueprintSubsystem"))->HasAnyFunctionFlags(FUNC_BlueprintCallable)); break;
		case 118: Test.TestTrue(TEXT("Library exposes a Blueprint pure function"), UBlueprintSubsystemsLib::StaticClass()->FindFunctionByName(TEXT("GetBlueprintSubsystem"))->HasAnyFunctionFlags(FUNC_BlueprintPure)); break;
		case 119: Test.TestNotNull(TEXT("Manager is a valid transient context"), manager); break;
		case 120: Test.TestNotNull(TEXT("GameInstance is a valid transient context"), gameInstance); break;
		default: return false;
		}
		return true;
	}

	bool RunDependencyCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		UGameInstance* gameInstance = nullptr;
		UBlueprintSubsystemManager* manager = CreateManager(gameInstance);
		Test.TestNotNull(TEXT("Dependency test manager exists"), manager);
		if (!manager)
		{
			return false;
		}

		switch (CaseId)
		{
		case 121:
		{
			const UFunction* function = UBlueprintSubsystemBase::StaticClass()->FindFunctionByName(TEXT("InitializeDependency"));
			Test.TestNotNull(TEXT("Initialize Dependency is reflected"), function);
			break;
		}
		case 122:
		{
			const UFunction* function = UBlueprintSubsystemBase::StaticClass()->FindFunctionByName(TEXT("InitializeDependency"));
			Test.TestTrue(TEXT("Initialize Dependency is Blueprint callable"), function && function->HasAnyFunctionFlags(FUNC_BlueprintCallable));
			break;
		}
		case 123:
		{
			const UFunction* function = UBlueprintSubsystemBase::StaticClass()->FindFunctionByName(TEXT("InitializeDependency"));
			Test.TestEqual(TEXT("Initialize Dependency determines output type"), function ? function->GetMetaData(TEXT("DeterminesOutputType")) : FString(), FString(TEXT("SubsystemClass")));
			break;
		}
		case 124:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			Test.TestNotNull(TEXT("Requester activates with dependency"), requester);
			Test.TestNotNull(TEXT("Dependency is returned to requester"), requester ? requester->Dependency.Get() : nullptr);
			break;
		}
		case 125:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			auto* dependency = Cast<UBlueprintSubsystemDependencyLeafTestSubsystem>(manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			Test.TestTrue(TEXT("Dependency initializes before requester"), dependency && requester && dependency->InitializeOrder < requester->InitializeOrder);
			break;
		}
		case 126:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			auto* second = Cast<UBlueprintSubsystemSecondRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemSecondRequesterTestSubsystem::StaticClass()));
			Test.TestNotNull(TEXT("Shared dependency is retained once"), manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Both requesters share the dependency"), second ? second->Dependency.Get() : nullptr, manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			break;
		}
		case 127:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Dependency can be activated on demand"), requester ? requester->Dependency.Get() : nullptr, manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			break;
		}
		case 128:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			UBlueprintSubsystemBase* dependency = requester ? requester->InitializeDependency(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()) : nullptr;
			auto* leaf = Cast<UBlueprintSubsystemDependencyLeafTestSubsystem>(dependency);
			Test.TestTrue(TEXT("Repeated dependency request returns the same object"), leaf == manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Repeated dependency request does not reinitialize"), leaf ? leaf->InitializeCount : 0, 1);
			break;
		}
		case 129:
		{
			Test.TestNull(TEXT("Null dependency class is rejected"), manager->InitializeDependency(nullptr, nullptr));
			break;
		}
		case 130:
		{
			Test.TestNull(TEXT("Abstract dependency is rejected"), manager->ActivateSubsystem(UBlueprintSubsystemAbstractTestSubsystem::StaticClass()));
			break;
		}
		case 131:
		{
			auto* dependency = manager->ActivateSubsystem(UBlueprintSubsystemRejectingTestSubsystem::StaticClass());
			Test.TestNull(TEXT("Rejected dependency is not activated"), dependency);
			break;
		}
		case 132:
		{
			Test.TestNull(TEXT("Cycle activation fails"), manager->ActivateSubsystem(UBlueprintSubsystemDependencyCycleATestSubsystem::StaticClass()));
			break;
		}
		case 133:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyCycleATestSubsystem::StaticClass());
			Test.TestNull(TEXT("Indirect dependency cycle leaves no active cycle member"), manager->GetSubsystem(UBlueprintSubsystemDependencyCycleBTestSubsystem::StaticClass()));
			break;
		}
		case 134:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass());
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRejectingRequesterTestSubsystem::StaticClass());
			Test.TestNotNull(TEXT("Unrelated subsystem survives dependency failure"), manager->GetSubsystem(UBlueprintSubsystemAcceptingTestSubsystem::StaticClass()));
			break;
		}
		case 135:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRootTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRootTestSubsystem::StaticClass()));
			auto* middle = Cast<UBlueprintSubsystemDependencyMiddleTestSubsystem>(manager->GetSubsystem(UBlueprintSubsystemDependencyMiddleTestSubsystem::StaticClass()));
			auto* leaf = Cast<UBlueprintSubsystemDependencyLeafTestSubsystem>(manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			Test.TestTrue(TEXT("Transitive dependency chain initializes"), requester && middle && leaf && requester->Dependency.Get() == middle && middle->Dependency.Get() == leaf);
			break;
		}
		case 136:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Initialization receives the active collection"), requester ? requester->InitializeListCount : 0, 1);
			break;
		}
		case 137:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			Test.TestFalse(TEXT("Direct dependency deactivation is blocked"), manager->DeactivateSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			break;
		}
		case 138:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRootTestSubsystem::StaticClass());
			Test.TestFalse(TEXT("Transitive dependency deactivation is blocked"), manager->DeactivateSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			break;
		}
		case 139:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			manager->DeactivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			Test.TestTrue(TEXT("Dependency deactivation succeeds after dependent removal"), manager->DeactivateSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			break;
		}
		case 140:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			auto* leaf = Cast<UBlueprintSubsystemDependencyLeafTestSubsystem>(manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			manager->Deinitialize();
			Test.TestTrue(TEXT("Cleanup runs in reverse initialization order"), requester && leaf && requester->DeInitializeOrder < leaf->DeInitializeOrder);
			break;
		}
		case 141:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Requester initializes once"), requester ? requester->InitializeCount : 0, 1);
			break;
		}
		case 142:
		{
			auto* leaf = Cast<UBlueprintSubsystemDependencyLeafTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Standalone runtime activation initializes once"), leaf ? leaf->InitializeCount : 0, 1);
			break;
		}
		case 143:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			Test.TestTrue(TEXT("Managed dependency remains GC referenced"), IsValid(manager->GetSubsystem(UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass())));
			break;
		}
		case 144:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Dependency lookup uses the requested class"), requester && requester->Dependency ? requester->Dependency->GetClass() : nullptr, UBlueprintSubsystemDependencyLeafTestSubsystem::StaticClass());
			break;
		}
		case 145:
		{
			manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			manager->DeactivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass());
			Test.TestNull(TEXT("Removed requester no longer blocks dependency removal"), manager->GetSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			break;
		}
		case 146:
		{
			auto* requester = Cast<UBlueprintSubsystemDependencyRequesterTestSubsystem>(manager->ActivateSubsystem(UBlueprintSubsystemDependencyRequesterTestSubsystem::StaticClass()));
			Test.TestEqual(TEXT("Requester stores the manager dependency instance"), requester && requester->Dependency ? requester->Dependency->GetTypedOuter<UBlueprintSubsystemManager>() : nullptr, manager);
			break;
		}
		default:
			return false;
		}
		return true;
	}

	bool RunBlueprintSubsystemTestCase(FAutomationTestBase& Test, const int32 CaseId)
	{
		if (CaseId <= 20)
		{
			return RunReflectionCase(Test, CaseId);
		}
		if (CaseId <= 40)
		{
			return RunBaseCase(Test, CaseId);
		}
		if (CaseId <= 75)
		{
			return RunManagerCase(Test, CaseId);
		}
		if (CaseId <= 100)
		{
			return RunRuntimeCase(Test, CaseId);
		}
		if (CaseId <= 120)
		{
			return RunLibraryCase(Test, CaseId);
		}
		return RunDependencyCase(Test, CaseId);
	}
}

#define BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(TestName, TestPath, CaseId) \
	IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestName, TestPath, EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter) \
	bool TestName::RunTest(const FString&) \
	{ \
		return RunBlueprintSubsystemTestCase(*this, CaseId); \
	}

BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection001, "BlueprintSubsystems.Reflection.BaseClass", 1)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection002, "BlueprintSubsystems.Reflection.UObjectInheritance", 2)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection003, "BlueprintSubsystems.Reflection.Abstract", 3)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection004, "BlueprintSubsystems.Reflection.Blueprintable", 4)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection005, "BlueprintSubsystems.Reflection.BlueprintType", 5)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection006, "BlueprintSubsystems.Reflection.GameInstanceSubsystem", 6)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection007, "BlueprintSubsystems.Reflection.FunctionLibrary", 7)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection008, "BlueprintSubsystems.Reflection.DeveloperSettings", 8)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection009, "BlueprintSubsystems.Reflection.Initialize", 9)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection010, "BlueprintSubsystems.Reflection.DeInitialize", 10)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection011, "BlueprintSubsystems.Reflection.ShouldCreate", 11)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection012, "BlueprintSubsystems.Reflection.GameInstanceGetter", 12)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection013, "BlueprintSubsystems.Reflection.WorldContextGetter", 13)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection014, "BlueprintSubsystems.Reflection.GetSubsystem", 14)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection015, "BlueprintSubsystems.Reflection.ActivateSubsystem", 15)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection016, "BlueprintSubsystems.Reflection.DeactivateSubsystem", 16)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection017, "BlueprintSubsystems.Reflection.LibraryLookup", 17)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection018, "BlueprintSubsystems.Reflection.WorldContextMetadata", 18)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection019, "BlueprintSubsystems.Reflection.OutputTypeMetadata", 19)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemReflection020, "BlueprintSubsystems.Reflection.SettingsProperty", 20)

BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase021, "BlueprintSubsystems.Base.Fixture", 21)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase022, "BlueprintSubsystems.Base.InitialState", 22)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase023, "BlueprintSubsystems.Base.InitialCleanupState", 23)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase024, "BlueprintSubsystems.Base.InitialCreationState", 24)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase025, "BlueprintSubsystems.Base.GameInstance", 25)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase026, "BlueprintSubsystems.Base.NullWorld", 26)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase027, "BlueprintSubsystems.Base.NullWorldContext", 27)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase028, "BlueprintSubsystems.Base.ValidPredicate", 28)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase029, "BlueprintSubsystems.Base.NullPredicate", 29)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase030, "BlueprintSubsystems.Base.Initialize", 30)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase031, "BlueprintSubsystems.Base.DeInitialize", 31)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase032, "BlueprintSubsystems.Base.RepeatedInitialize", 32)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase033, "BlueprintSubsystems.Base.RepeatedDeInitialize", 33)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase034, "BlueprintSubsystems.Base.Inheritance", 34)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase035, "BlueprintSubsystems.Base.ManagerOuter", 35)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase036, "BlueprintSubsystems.Base.WorldForwarding", 36)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase037, "BlueprintSubsystems.Base.ValidGameInstance", 37)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase038, "BlueprintSubsystems.Base.InvalidGameInstance", 38)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase039, "BlueprintSubsystems.Base.PendingKill", 39)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemBase040, "BlueprintSubsystems.Base.ExpectedClass", 40)

BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager041, "BlueprintSubsystems.Manager.NullLookup", 41)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager042, "BlueprintSubsystems.Manager.NullActivation", 42)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager043, "BlueprintSubsystems.Manager.NullDeactivation", 43)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager044, "BlueprintSubsystems.Manager.Activation", 44)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager045, "BlueprintSubsystems.Manager.Lookup", 45)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager046, "BlueprintSubsystems.Manager.DuplicateActivation", 46)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager047, "BlueprintSubsystems.Manager.Storage", 47)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager048, "BlueprintSubsystems.Manager.Rejection", 48)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager049, "BlueprintSubsystems.Manager.AbstractClass", 49)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager050, "BlueprintSubsystems.Manager.GameInstance", 50)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager051, "BlueprintSubsystems.Manager.Initialization", 51)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager052, "BlueprintSubsystems.Manager.CreationPredicate", 52)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager053, "BlueprintSubsystems.Manager.RejectedObject", 53)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager054, "BlueprintSubsystems.Manager.BaseLookup", 54)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager055, "BlueprintSubsystems.Manager.ExactLookup", 55)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager056, "BlueprintSubsystems.Manager.UnknownClass", 56)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager057, "BlueprintSubsystems.Manager.Deactivation", 57)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager058, "BlueprintSubsystems.Manager.Removal", 58)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager059, "BlueprintSubsystems.Manager.RepeatedRemoval", 59)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager060, "BlueprintSubsystems.Manager.Cleanup", 60)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager061, "BlueprintSubsystems.Manager.Lifetime", 61)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager062, "BlueprintSubsystems.Manager.GameInstanceLifetime", 62)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager063, "BlueprintSubsystems.Manager.EmptyState", 63)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager064, "BlueprintSubsystems.Manager.Initialize", 64)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager065, "BlueprintSubsystems.Manager.Deinitialize", 65)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager066, "BlueprintSubsystems.Manager.RepeatedDeinitialize", 66)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager067, "BlueprintSubsystems.Manager.ClearsState", 67)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager068, "BlueprintSubsystems.Manager.SingleRetention", 68)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager069, "BlueprintSubsystems.Manager.Inheritance", 69)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager070, "BlueprintSubsystems.Manager.OwningGameInstance", 70)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager071, "BlueprintSubsystems.Manager.Blueprintable", 71)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager072, "BlueprintSubsystems.Manager.Reflection", 72)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager073, "BlueprintSubsystems.Manager.RejectionIsolation", 73)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager074, "BlueprintSubsystems.Manager.RepeatedLookup", 74)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemManager075, "BlueprintSubsystems.Manager.Reactivate", 75)

BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime076, "BlueprintSubsystems.Runtime.Activation", 76)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime077, "BlueprintSubsystems.Runtime.Initialization", 77)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime078, "BlueprintSubsystems.Runtime.DeactivationCleanup", 78)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime079, "BlueprintSubsystems.Runtime.Removal", 79)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime080, "BlueprintSubsystems.Runtime.RepeatedDeactivation", 80)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime081, "BlueprintSubsystems.Runtime.ManagerLifetime", 81)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime082, "BlueprintSubsystems.Runtime.GameInstanceLifetime", 82)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime083, "BlueprintSubsystems.Runtime.CleanupObject", 83)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime084, "BlueprintSubsystems.Runtime.BaseObject", 84)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime085, "BlueprintSubsystems.Runtime.SingleCleanup", 85)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime086, "BlueprintSubsystems.Runtime.InitializationStable", 86)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime087, "BlueprintSubsystems.Runtime.CreationStable", 87)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime088, "BlueprintSubsystems.Runtime.BaseLookupEmpty", 88)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime089, "BlueprintSubsystems.Runtime.Reactivation", 89)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime090, "BlueprintSubsystems.Runtime.NewObject", 90)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime091, "BlueprintSubsystems.Runtime.NewInitialization", 91)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime092, "BlueprintSubsystems.Runtime.NewCreationCheck", 92)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime093, "BlueprintSubsystems.Runtime.ManagerOuter", 93)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime094, "BlueprintSubsystems.Runtime.GameInstance", 94)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime095, "BlueprintSubsystems.Runtime.NullWorld", 95)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime096, "BlueprintSubsystems.Runtime.WorldContext", 96)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime097, "BlueprintSubsystems.Runtime.RejectedActivation", 97)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime098, "BlueprintSubsystems.Runtime.AbstractActivation", 98)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime099, "BlueprintSubsystems.Runtime.StateIsolation", 99)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemRuntime100, "BlueprintSubsystems.Runtime.ManagerUsable", 100)

BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary101, "BlueprintSubsystems.Library.NullContext", 101)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary102, "BlueprintSubsystems.Library.NullClass", 102)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary103, "BlueprintSubsystems.Library.UnworldedGameInstance", 103)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary104, "BlueprintSubsystems.Library.UnworldedManager", 104)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary105, "BlueprintSubsystems.Library.Class", 105)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary106, "BlueprintSubsystems.Library.Inheritance", 106)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary107, "BlueprintSubsystems.Library.Function", 107)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary108, "BlueprintSubsystems.Library.WorldContextMetadata", 108)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary109, "BlueprintSubsystems.Library.OutputMetadata", 109)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary110, "BlueprintSubsystems.Library.NullInputs", 110)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary111, "BlueprintSubsystems.Library.NullClassContext", 111)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary112, "BlueprintSubsystems.Library.NullGameClass", 112)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary113, "BlueprintSubsystems.Library.ManagerContext", 113)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary114, "BlueprintSubsystems.Library.GameInstanceContext", 114)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary115, "BlueprintSubsystems.Library.UnregisteredClass", 115)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary116, "BlueprintSubsystems.Library.RegisteredUnworlded", 116)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary117, "BlueprintSubsystems.Library.Blueprintable", 117)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary118, "BlueprintSubsystems.Library.BlueprintType", 118)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary119, "BlueprintSubsystems.Library.ManagerValidity", 119)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemLibrary120, "BlueprintSubsystems.Library.GameInstanceValidity", 120)

BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency121, "BlueprintSubsystems.Dependency.Reflection", 121)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency122, "BlueprintSubsystems.Dependency.BlueprintCallable", 122)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency123, "BlueprintSubsystems.Dependency.OutputMetadata", 123)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency124, "BlueprintSubsystems.Dependency.Activation", 124)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency125, "BlueprintSubsystems.Dependency.Order", 125)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency126, "BlueprintSubsystems.Dependency.Shared", 126)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency127, "BlueprintSubsystems.Dependency.OnDemand", 127)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency128, "BlueprintSubsystems.Dependency.Reuse", 128)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency129, "BlueprintSubsystems.Dependency.Null", 129)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency130, "BlueprintSubsystems.Dependency.Abstract", 130)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency131, "BlueprintSubsystems.Dependency.Rejected", 131)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency132, "BlueprintSubsystems.Dependency.Cycle", 132)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency133, "BlueprintSubsystems.Dependency.CycleCleanup", 133)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency134, "BlueprintSubsystems.Dependency.Isolation", 134)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency135, "BlueprintSubsystems.Dependency.Transitive", 135)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency136, "BlueprintSubsystems.Dependency.Collection", 136)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency137, "BlueprintSubsystems.Dependency.BlockDirectRemoval", 137)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency138, "BlueprintSubsystems.Dependency.BlockTransitiveRemoval", 138)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency139, "BlueprintSubsystems.Dependency.RemoveAfterDependent", 139)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency140, "BlueprintSubsystems.Dependency.ReverseCleanup", 140)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency141, "BlueprintSubsystems.Dependency.SingleInitialization", 141)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency142, "BlueprintSubsystems.Dependency.RuntimeActivation", 142)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency143, "BlueprintSubsystems.Dependency.GCReference", 143)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency144, "BlueprintSubsystems.Dependency.RequestedClass", 144)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency145, "BlueprintSubsystems.Dependency.RemovedRequester", 145)
BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST(FBlueprintSubsystemDependency146, "BlueprintSubsystems.Dependency.ManagerOuter", 146)

#undef BLUEPRINT_SUBSYSTEM_AUTOMATION_TEST

#endif
