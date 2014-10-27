//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvDLLUtilDefines.h
//!  \brief     Public header Civ5 definitions that are external to the DLL.
//!
//!		This header includes all macros and defines that are external to the DLL
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

#define RANDPLOT_LAND										(0x00000001)
#define RANDPLOT_UNOWNED									(0x00000002)
#define RANDPLOT_ADJACENT_UNOWNED							(0x00000004)
#define RANDPLOT_ADJACENT_LAND								(0x00000008)
#define RANDPLOT_PASSIBLE									(0x00000010)
#define RANDPLOT_NOT_VISIBLE_TO_CIV							(0x00000020)
#define RANDPLOT_NOT_CITY									(0x00000040)

// do not change this or have MAX_PLAYERS exceed it - this will require the engine to be rebuilt
#define REALLY_MAX_PLAYERS (80)
#define REALLY_MAX_TEAMS (80)

#define MAX_CIV_PLAYERS										(63)
#define MAX_CIV_TEAMS										(MAX_CIV_PLAYERS)

#define MAX_MAJOR_CIVS										(22)
#define MAX_MINOR_CIVS										(MAX_CIV_PLAYERS - MAX_MAJOR_CIVS)

#define MAX_PLAYERS											(MAX_CIV_PLAYERS + 1)
#define MAX_TEAMS											(MAX_PLAYERS)
#define BARBARIAN_PLAYER									((PlayerTypes)MAX_CIV_PLAYERS)
#define BARBARIAN_TEAM										((TeamTypes)MAX_CIV_TEAMS)
#define OBSERVER_TEAM											((TeamTypes)(MAX_MAJOR_CIVS-1))		//This only works because observer mode is multiplayer/debug only																																		//and multiplayer only supports 12 max players 																																	//(due to map player restrictions)

#define MAX_DAMAGE_MEMBER_COUNT								32
#define CVHEADER_VERSIONING_STARTED							7		/// The version of CvSavedGameHeader where we added versions to CvPreGame data.

// don't use -1 since that is a valid wrap coordinate
#define INVALID_PLOT_COORD									(-(INT_MAX))

// DLC & Mod IDs
const char* const CIV5_TUTORIAL_MODID = "df3333a4-44be-4fc3-9143-21706ff451d5";
const char* const CIV5_DLC_01_SCENARIO_MODID = "d565342c-f7b5-495e-9c1d-c05c2d55c8a3";
const char* const CIV5_DLC_02_SCENARIO_MODID = "02453135-2694-47f8-9690-29cca6cce199";
const char* const CIV5_DLC_03_SCENARIO_MODID = "a028d675-1f4d-45b5-bd6e-869b77079405";
const char* const CIV5_DLC_04_SCENARIO_MODID = "daf6f887-3614-4169-a0e5-3ac9bd4ef82b";
const char* const CIV5_DLC_05_SCENARIO_MODID = "24081928-4703-4845-8b9c-c1468a8cacf2";
const char* const CIV5_DLC_06_SCENARIO_MODID = "2968ab37-29a3-4de7-8b84-ece688231e40";

const char* const CIV5_XP1_SCENARIO1_MODID = "8f9d820d-e336-4208-891d-50fdd968cc40";	//Into the Renaissance
const char* const CIV5_XP1_SCENARIO2_MODID = "76d702e9-8b28-429b-a97d-f84388f548c5";	//Fall of Rome
const char* const CIV5_XP1_SCENARIO3_MODID = "b103d2e0-3aab-40d7-a2f3-375d2eabfcaf";	//Empires of the Smoky Skies

const char* const CIV5_DLC_06_PACKAGEID = "BBB0D085-A0B1-4475-B007-3E549CF3ADC3";

const char* const CIV5_XP2_SCENARIO1_MODID = "55fd836a-72e5-4aba-bbcf-3395b05a0e70";	//American Civil War
const char* const CIV5_XP2_SCENARIO2_MODID = "f2eb1642-3334-4d2c-8db1-08e6aede3532";	//Scramble for Africa

const char* const CIV5_COMPLETE_SCENARIO1_MODID = "34fb6c19-10dd-4b65-b143-fd00b2c0826f"; //Deluxe Conquest of the New World