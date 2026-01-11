#pragma once
#include "ZeroGUI.h"

// Namespace pour les variables globales du menu
namespace MenuGlobals {
    extern bool bMenuOpen;
    extern int hookCallCount;
}

// Ajoutez ceci dans votre fichier (Menu.h ou au début de votre fichier principal)

namespace Menu {
    inline bool bAimbot = false;
    inline bool bVisibleCheck = false;
    inline bool bAimKey = false;
    inline int bKey = VK_RBUTTON;
    inline bool drawFov = false;
    inline float fov = 150.0f;
    inline float maxDistance = 10000.0f;
    inline bool bSilent = false;

    inline bool bESP = false;
    inline bool bCorner = false;
    inline bool bTracerLine = false;
    inline bool bSnapLine = false;

    inline FLinearColor test_color{ 1.0f, 0.0f, 0.0f, 1.0f }; // Rouge
    inline FLinearColor a_color{ 1.0f, 1.0f, 0.0f, 1.0f };    // Jaune
    inline FLinearColor b_color{ 1.0f, 0.0f, 1.0f, 1.0f };    // Magenta
    inline FLinearColor c_color{ 0.0f, 0.0f, 1.0f, 1.0f };    // Bleu

    inline bool bBulletTracers = false;
    inline FLinearColor tracerColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    inline float tracerThickness = 2.0f;
    inline float tracerDuration = 2.0f;

    inline bool bNoRecoil = false;
    inline bool bNoSpread = false;
    inline bool bMagicBullet = false;
    inline bool bSpeed = false;
    inline float bSpeedValue = 100.0f;
    inline bool bWireFrame = false;
    inline bool bInstantReload = false;
    inline bool bProjTP = false;
    inline bool bNoCol = false;
    inline bool bFire = false;
    inline float bFireValue = 20.0f;
    inline bool bFOV = false;
    inline float bFOVValue = 90.0f;
    inline bool bfly = false;
    inline float bflyspeed = 30.0f;
    inline bool bflyv2 = false;
    inline float bflyv2speed = 30.0f;
    inline bool bBringAllPlayer = false;
    inline float bBringDistance = 100.0f;
    inline bool bGod = false;
    inline bool bDammage = false;
    inline float bDamValue = 15.0f;
    inline float bulletSpeed = 15.0f;

    inline bool bHitboxChanger = false;
    inline float hitboxSizeMultiplier = 5.0f;

    inline bool bSpinBot = false;
    inline float spinSpeed = 360.0f;
    inline int spinAxis = 0; // 0=Yaw, 1=Pitch, 2=Roll, 3=All
    inline bool bSpinBotAutoFire = false;

    inline bool bThirdPerson = false;
    inline float thirdPersonDistance = 300.0f;
    inline float thirdPersonHeight = 50.0f;
    inline float thirdPersonSideOffset = 0.0f;
    inline bool bThirdPersonCustomFOV = false;
    inline float thirdPersonFOV = 90.0f;
    inline bool bThirdPersonSmooth = true;
    inline bool bThirdPersonOrbital = false;
    inline float thirdPersonOrbitalSpeed = 45.0f;

    inline bool Tpbehindrandomplayer = false;
    inline bool UEConsoleCheats = false;
    inline int UEConsoleOpenBtn = VK_F8;
    inline bool bAirStuck = false;
    inline bool bAirStuckFreezeRotation = false;

    inline bool bChams = false;
    inline bool bChamsEnemyOnly = true;
    inline bool bChamsVisibilityCheck = true;
    inline bool bChamsThroughWalls = true;
    inline bool bChamsNoShadow = true;
    inline int bChamsMethod = 0;

    inline SDK::FLinearColor chamsEnemyColor = SDK::FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    inline SDK::FLinearColor chamsTeamColor = SDK::FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    inline SDK::FLinearColor chamsEnemyVisibleColor = SDK::FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);
    inline SDK::FLinearColor chamsEnemyHiddenColor = SDK::FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    inline SDK::FLinearColor chamsTeamVisibleColor = SDK::FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    inline SDK::FLinearColor chamsTeamHiddenColor = SDK::FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);

    inline bool bMenu = true;
    inline int MenuKey = VK_RSHIFT;
    inline unsigned int currTab = 0;

    inline std::vector<bool*> toggles;
}

extern void Tick();