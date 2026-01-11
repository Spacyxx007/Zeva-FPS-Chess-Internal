#include "../FPS Chess/SDK/Basic.cpp"
#include "../FPS Chess/SDK/CoreUObject_functions.cpp"
#include "../FPS Chess//SDK/Engine_functions.cpp"
#include "../FPS Chess/SDK/BP_Player_classes.hpp"

#include "Math.h"
#include "Menu.h"
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <chrono>

SDK::UWorld* world{ nullptr };
SDK::ULevel* persistentLevel{ nullptr };
SDK::APlayerController* localPlayerController{ nullptr };
SDK::UGameViewportClient* gameViewPortClient{ nullptr };

bool bHooked{ false };
int postRenderIndex{ 0x63 };
uintptr_t* vTable{ nullptr };
static uintptr_t vTableHooked[4096];
void (*PostRender)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);

bool bRButtonDown{ false };

int localTeam{ 0 };
SDK::FVector viewPoint;

bool bSilentWasActive = false;
SDK::FRotator silentSavedRot;

namespace Maths
{
    constexpr float PI = 3.14159265358979323846f;

    float ToRad(float Degrees) {
        return Degrees * (Maths::PI / 180.0f);
    }

    float ToDeg(float Radians) {
        return Radians * (180.0f / Maths::PI);
    }

    SDK::FVector GetForwardVector(float Pitch, float Yaw) {
        float radPitch = ToRad(Pitch);
        float radYaw = ToRad(Yaw);

        SDK::FVector forward;
        forward.X = cosf(radPitch) * cosf(radYaw);
        forward.Y = cosf(radPitch) * sinf(radYaw);
        forward.Z = sinf(radPitch);
        return forward;
    }

    SDK::FVector GetRightVector(float Pitch, float Yaw) {
        float radYaw = ToRad(Yaw);

        SDK::FVector right;
        right.X = -sinf(radYaw);
        right.Y = cosf(radYaw);
        right.Z = 0.0f;
        return right;
    }

    SDK::FVector GetUpVector(float Pitch, float Yaw) {
        float radPitch = ToRad(Pitch);
        float radYaw = ToRad(Yaw);

        SDK::FVector up;
        up.X = -sinf(radPitch) * cosf(radYaw);
        up.Y = -sinf(radPitch) * sinf(radYaw);
        up.Z = cosf(radPitch);
        return up;
    }

    SDK::FVector Normalize(const SDK::FVector& v) {
        float length = sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
        if (length < 0.0001f) return SDK::FVector{ 0, 0, 0 };

        SDK::FVector normalized;
        normalized.X = v.X / length;
        normalized.Y = v.Y / length;
        normalized.Z = v.Z / length;
        return normalized;
    }

    SDK::FVector Scale(const SDK::FVector& v, float s) {
        SDK::FVector result;
        result.X = v.X * s;
        result.Y = v.Y * s;
        result.Z = v.Z * s;
        return result;
    }

    SDK::FVector Add(const SDK::FVector& a, const SDK::FVector& b) {
        SDK::FVector result;
        result.X = a.X + b.X;
        result.Y = a.Y + b.Y;
        result.Z = a.Z + b.Z;
        return result;
    }

    SDK::FVector Subtract(const SDK::FVector& a, const SDK::FVector& b) {
        SDK::FVector result;
        result.X = a.X - b.X;
        result.Y = a.Y - b.Y;
        result.Z = a.Z - b.Z;
        return result;
    }

    float Dot(const SDK::FVector& a, const SDK::FVector& b) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }

    SDK::FVector Cross(const SDK::FVector& a, const SDK::FVector& b) {
        SDK::FVector result;
        result.X = a.Y * b.Z - a.Z * b.Y;
        result.Y = a.Z * b.X - a.X * b.Z;
        result.Z = a.X * b.Y - a.Y * b.X;
        return result;
    }

    float Length(const SDK::FVector& v) {
        return sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
    }

    float Distance(const SDK::FVector& a, const SDK::FVector& b) {
        return Length(Subtract(a, b));
    }

    SDK::FVector GetForwardVector(const SDK::FRotator& Rot) {
        return GetForwardVector(Rot.Pitch, Rot.Yaw);
    }

    SDK::FVector GetRightVector(const SDK::FRotator& Rot) {
        return GetRightVector(Rot.Pitch, Rot.Yaw);
    }

    SDK::FVector GetUpVector(const SDK::FRotator& Rot) {
        return GetUpVector(Rot.Pitch, Rot.Yaw);
    }

    SDK::FRotator LookAt(const SDK::FVector& Start, const SDK::FVector& Target) {
        SDK::FVector dir = Subtract(Target, Start);
        float length = Length(dir);

        if (length < 0.0001f) return SDK::FRotator{ 0, 0, 0 };

        SDK::FRotator result;
        result.Pitch = ToDeg(asinf(dir.Z / length));
        result.Yaw = ToDeg(atan2f(dir.Y, dir.X));
        result.Roll = 0.0f;
        return result;
    }

    bool WorldToScreen(const SDK::FVector& WorldLocation, SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize) {
        return false;
    }

    float Lerp(float A, float B, float Alpha) {
        return A + (B - A) * Alpha;
    }

    SDK::FVector Lerp(const SDK::FVector& A, const SDK::FVector& B, float Alpha) {
        SDK::FVector result;
        result.X = Lerp(A.X, B.X, Alpha);
        result.Y = Lerp(A.Y, B.Y, Alpha);
        result.Z = Lerp(A.Z, B.Z, Alpha);
        return result;
    }

    float SmoothStep(float t) {
        return t * t * (3.0f - 2.0f * t);
    }

    SDK::FVector SmoothDamp(const SDK::FVector& current, const SDK::FVector& target,
        SDK::FVector& currentVelocity, float smoothTime,
        float maxSpeed, float deltaTime) {
        // Implémentation simplifiée
        SDK::FVector diff = Subtract(target, current);
        float distance = Length(diff);

        if (distance < 0.0001f) return target;

        SDK::FVector direction = Normalize(diff);
        float speed = distance / (smoothTime + 0.0001f);
        speed = (speed > maxSpeed) ? maxSpeed : speed;

        return Add(current, Scale(direction, speed * deltaTime));
    }
}

namespace CheatVars {
    SDK::AActor* currentTarget = nullptr;
    SDK::FVector lastTargetLocation;
    bool hasTarget = false;
}

bool IsValidActor(SDK::AActor* actor) {
    if (!actor) return false;
    if (!actor->RootComponent) return false;
    return true;
}

bool IsPlayerPawn(SDK::AActor* actor) {
    if (!IsValidActor(actor)) return false;

    auto pawn = static_cast<SDK::APawn*>(actor);
    if (!pawn) return false;

    if (localPlayerController && localPlayerController->AcknowledgedPawn == pawn)
        return false;

    return true;
}

SDK::FVector GetActorLocation(SDK::AActor* actor) {
    if (!IsValidActor(actor)) return SDK::FVector{ 0, 0, 0 };
    return actor->RootComponent->RelativeLocation;
}

bool WorldToScreen(SDK::UCanvas* canvas, const SDK::FVector& worldPos, SDK::FVector2D& screenPos) {
    if (!canvas || !localPlayerController) return false;

    SDK::FVector2D outScreen;
    bool result = localPlayerController->ProjectWorldLocationToScreen(worldPos, &outScreen, false);

    if (result) {
        screenPos = outScreen;

        // Vérifier si à l'écran
        if (screenPos.X < 0 || screenPos.X > canvas->ClipX) return false;
        if (screenPos.Y < 0 || screenPos.Y > canvas->ClipY) return false;

        return true;
    }

    return false;
}

struct AimBot
{
    SDK::AActor* GetClosestTarget() {
        if (!localPlayerController || !localPlayerController->PlayerCameraManager)
            return nullptr;

        SDK::FVector cameraLoc = localPlayerController->PlayerCameraManager->GetCameraLocation();
        SDK::FRotator cameraRot = localPlayerController->PlayerCameraManager->GetCameraRotation();

        float closestDistance = Menu::maxDistance;
        SDK::AActor* closestActor = nullptr;

        if (!persistentLevel) return nullptr;

        // Parcourir tous les acteurs
        for (int i = 0; i < persistentLevel->Actors.Num(); i++) {
            auto actor = persistentLevel->Actors[i];
            if (!IsPlayerPawn(actor)) continue;

            SDK::FVector actorLoc = GetActorLocation(actor);
            float distance = Maths::Distance(cameraLoc, actorLoc);

            if (distance > Menu::maxDistance) continue;

            // Vérifier FOV
            SDK::FRotator lookAt = Maths::LookAt(cameraLoc, actorLoc);
            float yawDiff = abs(lookAt.Yaw - cameraRot.Yaw);
            float pitchDiff = abs(lookAt.Pitch - cameraRot.Pitch);

            if (yawDiff > Menu::fov || pitchDiff > Menu::fov) continue;

            // Vérifier visibilité si activé
            if (Menu::bVisibleCheck) {
                SDK::FHitResult hit;
                if (!localPlayerController->LineOfSightTo(actor, cameraLoc, false))
                    continue;
            }

            if (distance < closestDistance) {
                closestDistance = distance;
                closestActor = actor;
            }
        }

        return closestActor;
    }

    void DrawFOV(SDK::UCanvas* canvas) {
        if (!Menu::drawFov || !canvas) return;

        SDK::FVector2D center;
        center.X = canvas->ClipX / 2.0f;
        center.Y = canvas->ClipY / 2.0f;

        float radius = Menu::fov * 2.0f;

        // Dessiner le cercle FOV
        int segments = 64;
        for (int i = 0; i < segments; i++) {
            float angle1 = (i * 360.0f / segments) * (3.14159f / 180.0f);
            float angle2 = ((i + 1) * 360.0f / segments) * (3.14159f / 180.0f);

            SDK::FVector2D p1, p2;
            p1.X = center.X + cos(angle1) * radius;
            p1.Y = center.Y + sin(angle1) * radius;
            p2.X = center.X + cos(angle2) * radius;
            p2.Y = center.Y + sin(angle2) * radius;

            canvas->K2_DrawLine(p1, p2, 1.0f, SDK::FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f });
        }
    }

    void TargetLine(SDK::UCanvas* canvas) {
        if (!Menu::bSnapLine || !CheatVars::currentTarget) return;

        SDK::FVector targetLoc = GetActorLocation(CheatVars::currentTarget);
        SDK::FVector2D screenPos;

        if (WorldToScreen(canvas, targetLoc, screenPos)) {
            SDK::FVector2D center;
            center.X = canvas->ClipX / 2.0f;
            center.Y = canvas->ClipY / 2.0f;

            canvas->K2_DrawLine(center, screenPos, 2.0f, Menu::test_color);
        }
    }

    void Aimbot() {
        if (!Menu::bAimbot) {
            CheatVars::hasTarget = false;
            CheatVars::currentTarget = nullptr;
            return;
        }

        bool keyPressed = GetAsyncKeyState(Menu::bKey) & 0x8000;
        if (!keyPressed) {
            CheatVars::hasTarget = false;
            CheatVars::currentTarget = nullptr;
            return;
        }

        if (!localPlayerController || !localPlayerController->PlayerCameraManager)
            return;

        CheatVars::currentTarget = GetClosestTarget();
        CheatVars::hasTarget = (CheatVars::currentTarget != nullptr);

        if (!CheatVars::hasTarget) return;

        SDK::FVector cameraLoc = localPlayerController->PlayerCameraManager->GetCameraLocation();
        SDK::FVector targetLoc = GetActorLocation(CheatVars::currentTarget);

        SDK::FRotator targetRot = Maths::LookAt(cameraLoc, targetLoc);

        localPlayerController->SetControlRotation(targetRot);
    }

    void SilentAim() {
        if (!Menu::bSilent) {
            if (bSilentWasActive && localPlayerController) {
                localPlayerController->SetControlRotation(silentSavedRot);
                bSilentWasActive = false;
            }
            return;
        }

        bool keyPressed = GetAsyncKeyState(Menu::bKey) & 0x8000;
        if (!keyPressed) {
            if (bSilentWasActive && localPlayerController) {
                localPlayerController->SetControlRotation(silentSavedRot);
                bSilentWasActive = false;
            }
            return;
        }

        if (!localPlayerController || !localPlayerController->PlayerCameraManager)
            return;

        auto target = GetClosestTarget();
        if (!target) return;

        // Sauvegarder la rotation actuelle
        if (!bSilentWasActive) {
            silentSavedRot = localPlayerController->GetControlRotation();
            bSilentWasActive = true;
        }

        SDK::FVector cameraLoc = localPlayerController->PlayerCameraManager->GetCameraLocation();
        SDK::FVector targetLoc = GetActorLocation(target);

        SDK::FRotator targetRot = Maths::LookAt(cameraLoc, targetLoc);

        localPlayerController->SetControlRotation(targetRot);
    }

} Aimbot;

struct Visuals
{
    void Box(SDK::UCanvas* canvas, SDK::AActor* actor) {
        if (!Menu::bCorner || !IsValidActor(actor)) return;

        SDK::FVector actorLoc = GetActorLocation(actor);
        SDK::FVector2D screenPos;

        if (!WorldToScreen(canvas, actorLoc, screenPos)) return;

        float boxWidth = 50.0f;
        float boxHeight = 80.0f;

        SDK::FVector2D topLeft = { screenPos.X - boxWidth / 2, screenPos.Y - boxHeight };
        SDK::FVector2D topRight = { screenPos.X + boxWidth / 2, screenPos.Y - boxHeight };
        SDK::FVector2D bottomLeft = { screenPos.X - boxWidth / 2, screenPos.Y };
        SDK::FVector2D bottomRight = { screenPos.X + boxWidth / 2, screenPos.Y };

        SDK::FLinearColor color = { 1.0f, 0.0f, 0.0f, 1.0f };

        canvas->K2_DrawLine(topLeft, topRight, 2.0f, color);
        canvas->K2_DrawLine(topRight, bottomRight, 2.0f, color);
        canvas->K2_DrawLine(bottomRight, bottomLeft, 2.0f, color);
        canvas->K2_DrawLine(bottomLeft, topLeft, 2.0f, color);
    }

    void SnapLine(SDK::UCanvas* canvas, SDK::AActor* actor) {
        if (!Menu::bTracerLine || !IsValidActor(actor)) return;

        SDK::FVector actorLoc = GetActorLocation(actor);
        SDK::FVector2D screenPos;

        if (!WorldToScreen(canvas, actorLoc, screenPos)) return;

        SDK::FVector2D screenCenter = { canvas->ClipX / 2.0f, canvas->ClipY };

        canvas->K2_DrawLine(screenCenter, screenPos, 1.5f, Menu::b_color);
    }

    void Skeleton(SDK::UCanvas* canvas, SDK::AActor* actor) {
        if (!Menu::bESP || !IsValidActor(actor)) return;

        SDK::FVector actorLoc = GetActorLocation(actor);
        SDK::FVector2D screenPos;

        if (!WorldToScreen(canvas, actorLoc, screenPos)) return;

        SDK::FVector2D p1 = { screenPos.X - 2, screenPos.Y - 2 };
        SDK::FVector2D p2 = { screenPos.X + 2, screenPos.Y + 2 };
        canvas->K2_DrawLine(p1, p2, 2.0f, Menu::a_color);
    }

} Visuals;

struct Exploits
{
    void NoRecoil() {
        if (!Menu::bNoRecoil) return;
            // make ur
    }

    void NoSpread() {
        if (!Menu::bNoSpread) return;

        // same
    }

    void MagicBullet() {
        if (!Menu::bMagicBullet) return;

        // Same (bored to make)
    }

    void SpeedHack() {
        // Not work, idk why, fix it

        if (!Menu::bSpeed) return;

        if (!localPlayerController || !localPlayerController->AcknowledgedPawn)
            return;

        auto pawn = localPlayerController->AcknowledgedPawn;
        auto movement = pawn->GetMovementComponent();

        if (movement) {
            auto charMovement = static_cast<SDK::UCharacterMovementComponent*>(movement);
            if (charMovement) {
                charMovement->MaxWalkSpeed = Menu::bSpeedValue;
                charMovement->MaxWalkSpeedCrouched = Menu::bSpeedValue;
                charMovement->MinAnalogWalkSpeed = Menu::bSpeedValue;
            }
        }
    }

    void FOVChanger() {
        if (!Menu::bFOV) return;

        if (!localPlayerController || !localPlayerController->PlayerCameraManager)
            return;

        localPlayerController->PlayerCameraManager->DefaultFOV = Menu::bFOVValue;
    }

} Exploits;

void LoopActors(SDK::UCanvas* canvas)
{
    if (!persistentLevel || !canvas) return;

    Aimbot.DrawFOV(canvas);
    Aimbot.Aimbot();
    Aimbot.SilentAim();
    Aimbot.TargetLine(canvas);

    Exploits.NoRecoil();
    Exploits.NoSpread();
    Exploits.FOVChanger();
    Exploits.SpeedHack();

    for (int i = 0; i < persistentLevel->Actors.Num(); i++) {
        auto actor = persistentLevel->Actors[i];
        if (!IsPlayerPawn(actor)) continue;

        Visuals.Box(canvas, actor);
        Visuals.SnapLine(canvas, actor);
        Visuals.Skeleton(canvas, actor);
    }
}

bool UpdateInstance()
{
    world = SDK::UWorld::GetWorld();
    if (world == nullptr)
        return false;

    persistentLevel = world->PersistentLevel;
    if (persistentLevel == nullptr)
        return false;

    if (world->OwningGameInstance == nullptr)
        return false;
    if (world->OwningGameInstance->LocalPlayers[0] == nullptr)
        return false;
    if (world->OwningGameInstance->LocalPlayers[0]->PlayerController == nullptr)
        return false;

    localPlayerController = world->OwningGameInstance->LocalPlayers[0]->PlayerController;

    return true;
}
void UnHook()
{
    *(PVOID*)gameViewPortClient = vTable;
}

void PostRenderHook(SDK::UGameViewportClient* viewport, SDK::UCanvas* canvas)
{
    if (!viewport || !canvas)
        return;

    static bool loggedOnce = false;
    if (!loggedOnce)
    {
        std::cout << "[+] PostRenderHook OK" << std::endl;
        std::cout << "[+] Viewport: 0x" << std::hex << (uintptr_t)viewport << std::endl;
        std::cout << "[+] Canvas: 0x" << std::hex << (uintptr_t)canvas << std::endl;
        loggedOnce = true;
    }

    if (PostRender)
        PostRender(viewport, canvas);

    bRButtonDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);

    ZeroGUI::SetupCanvas(canvas);

    if (UpdateInstance())
    {
        LoopActors(canvas);
    }

    Tick();
}
void InitHook()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    if (!gameViewPortClient)
    {
        std::cout << "[!] ERREUR : gameViewPortClient are NULL!" << std::endl;
        return;
    }

    std::cout << "[+] gameViewPortClient: 0x" << std::hex << (uintptr_t)gameViewPortClient << std::endl;

    bHooked = true;
    vTable = *(uintptr_t**)gameViewPortClient;

    if (!vTable)
    {
        std::cout << "[!] ERREUR : vTable are NULL!" << std::endl;
        return;
    }

    std::cout << "[+] VTable: 0x" << std::hex << (uintptr_t)vTable << std::endl;

    // Charger la police
    Functions::NamesFont = SDK::UObject::FindObject<SDK::UFont>("Font Roboto.Roboto");

    if (!Functions::NamesFont)
    {
        std::cout << "[!] Erreur : Font not found!" << std::endl;
    }
    else
    {
        std::cout << "[+] Font found: 0x" << std::hex << (uintptr_t)Functions::NamesFont << std::endl;
    }

    // VTable Swapping
    memcpy(vTableHooked, vTable, (sizeof(vTableHooked) / sizeof(uintptr_t)));

    vTableHooked[0x63] = (uintptr_t)&PostRenderHook;
    PostRender = (decltype(PostRender))vTable[0x63];

    std::cout << "[+] PostRender: 0x" << std::hex << (uintptr_t)PostRender << std::endl;

    *(PVOID*)gameViewPortClient = vTableHooked;

    std::cout << "[+] Have fun, lamo" << std::endl;
}
DWORD HackThread(HMODULE hModule)
{
    // Get UWorld to obtain viewportclient to hook post render
    do
    {
        world = SDK::UWorld::GetWorld();
        if (!world)
            continue;
        if (!world->OwningGameInstance)
            continue;
        if (!world->OwningGameInstance->LocalPlayers[0])
            continue;
        if (!world->OwningGameInstance->LocalPlayers[0]->ViewportClient)
            continue;

        gameViewPortClient = world->OwningGameInstance->LocalPlayers[0]->ViewportClient;
        InitHook();
    } while (!bHooked);

    while (bHooked)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            UnHook();
            FreeLibraryAndExitThread(hModule, 0);
            break;
        }
        Sleep(5);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}