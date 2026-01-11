#include "pch.h"
#include "ZeroGUI.h"
#include "Menu.h"
#include <iostream>

namespace MenuGlobals {
    bool bMenuOpen = true;
    int hookCallCount = 0;
}

static FVector2D pos = FVector2D(960.0f, 540.0f);
static FVector2D size = FVector2D{ 500.0f, 400.0f };

void Tick()
{
    ZeroGUI::Input::Handle();

    if (GetAsyncKeyState(Menu::MenuKey) & 1) {
        MenuGlobals::bMenuOpen = !MenuGlobals::bMenuOpen;
    }

    if (ZeroGUI::Window((char*)"Zeva - FPS Chess", &pos, size, MenuGlobals::bMenuOpen))
    {
        static int tab = 0;
        if (ZeroGUI::ButtonTab((char*)"Aimbot", FVector2D{ 110, 25 }, tab == 0)) tab = 0;
        if (ZeroGUI::ButtonTab((char*)"Visuals", FVector2D{ 110, 25 }, tab == 1)) tab = 1;
        if (ZeroGUI::ButtonTab((char*)"Exploits", FVector2D{ 110, 25 }, tab == 2)) tab = 2;
        if (ZeroGUI::ButtonTab((char*)"Settings", FVector2D{ 110, 25 }, tab == 3)) tab = 3;

        ZeroGUI::NextColumn(130.0f);

        if (tab == 0)
        {
            ZeroGUI::Checkbox((char*)"Aimbot", &Menu::bAimbot);

            if (Menu::bAimbot) {
                ZeroGUI::SameLine();
                ZeroGUI::Hotkey((char*)"Key", FVector2D{ 80, 25 }, &Menu::bKey);

                ZeroGUI::Checkbox((char*)"Draw FOV", &Menu::drawFov);
                if (Menu::drawFov) {
                    ZeroGUI::SliderFloat((char*)"FOV", &Menu::fov, 10.0f, 500.0f, "%.1f");
                }

                ZeroGUI::SliderFloat((char*)"Max Distance", &Menu::maxDistance, 1000.0f, 50000.0f, "%.0f");
                ZeroGUI::Checkbox((char*)"Silent Aim", &Menu::bSilent);
                ZeroGUI::Checkbox((char*)"Visibility Check", &Menu::bVisibleCheck);
            }

            ZeroGUI::Checkbox((char*)"Target Line", &Menu::bSnapLine);
            if (Menu::bSnapLine) {
                ZeroGUI::SameLine();
                ZeroGUI::ColorPicker((char*)"Color", &Menu::test_color);
            }
        }

        else if (tab == 1)
        {
            ZeroGUI::Checkbox((char*)"Skeleton", &Menu::bESP);
            if (Menu::bESP) {
                ZeroGUI::SameLine();
                ZeroGUI::ColorPicker((char*)"Color", &Menu::a_color);
            }

            ZeroGUI::Checkbox((char*)"Snaplines", &Menu::bTracerLine);
            if (Menu::bTracerLine) {
                ZeroGUI::SameLine();
                ZeroGUI::ColorPicker((char*)"Color", &Menu::b_color);
            }

            ZeroGUI::Checkbox((char*)"Boxes", &Menu::bCorner);
        }

        else if (tab == 2)
        {
            ZeroGUI::Checkbox((char*)"No Recoil", &Menu::bNoRecoil);
            ZeroGUI::Checkbox((char*)"No Spread", &Menu::bNoSpread);

            ZeroGUI::Checkbox((char*)"FOV Changer", &Menu::bFOV);
            if (Menu::bFOV) {
                ZeroGUI::SliderFloat((char*)"FOV", &Menu::bFOVValue, 10.0f, 150.0f, "%.0f");
            }

            ZeroGUI::Checkbox((char*)"SpeedHack", &Menu::bSpeed);
            if (Menu::bFOV) {
                ZeroGUI::SliderFloat((char*)"Speed", &Menu::bSpeedValue, 10.0f, 150.0f, "%.0f");
            }
        }

        else if (tab == 3)
        {
            ZeroGUI::Text((char*)"Controls", true, true);
            ZeroGUI::Hotkey((char*)"Menu Key", FVector2D{ 100, 25 }, &Menu::MenuKey);

            ZeroGUI::PushNextElementY(15.0f);
            ZeroGUI::Text((char*)"Press END to unload", false, false);

            ZeroGUI::PushNextElementY(20.0f);
            ZeroGUI::Text((char*)"Made by @spacyxx007", false, false);
        }
    }

    ZeroGUI::Render();
    ZeroGUI::Draw_Cursor(MenuGlobals::bMenuOpen);
}