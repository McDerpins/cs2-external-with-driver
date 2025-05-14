#pragma once
#include <iostream>

namespace offsets {

	// offsets.hpp
    constexpr std::ptrdiff_t dwCSGOInput = 0x1A69E40;
    constexpr std::ptrdiff_t dwEntityList = 0x19F6FA0;
    constexpr std::ptrdiff_t dwGameEntitySystem = 0x1B1A668;
    constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x20F0;
    constexpr std::ptrdiff_t dwGameRules = 0x1A5B878;
    constexpr std::ptrdiff_t dwGlobalVars = 0x183F128;
    constexpr std::ptrdiff_t dwGlowManager = 0x1A5AFC0;
    constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A45A80;
    constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x184B0B0;
    constexpr std::ptrdiff_t dwPlantedC4 = 0x1A66008;
    constexpr std::ptrdiff_t dwPrediction = 0x184AF30;
    constexpr std::ptrdiff_t dwSensitivity = 0x1A5C598;
    constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x40;
    constexpr std::ptrdiff_t dwViewAngles = 0x1A6A210;
    constexpr std::ptrdiff_t dwViewMatrix = 0x1A5FF70;
    constexpr std::ptrdiff_t dwViewRender = 0x1A608E8;
    constexpr std::ptrdiff_t dwWeaponC4 = 0x19F9470;
	// client.dll.hpp
	constexpr std::ptrdiff_t m_vOldOrigin = 0x1324; // Vector
	constexpr std::ptrdiff_t m_iTeamNum = 0x3E3; // uint8
	constexpr std::ptrdiff_t m_lifeState = 0x348; // uint8
	constexpr std::ptrdiff_t m_hPlayerPawn = 0x824; // CHandle<C_CSPlayerPawn>
	constexpr std::ptrdiff_t m_vecViewOffset = 0xCB0; // CNetworkViewOffsetVector
	constexpr std::ptrdiff_t m_iHealth = 0X344; // int32
    constexpr std::ptrdiff_t m_pGameSceneNode = 0x328;
    constexpr std::ptrdiff_t m_skeletonInstance = 0x50;
    constexpr std::ptrdiff_t m_modelState = 0x170;

}