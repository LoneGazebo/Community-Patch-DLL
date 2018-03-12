#pragma once

#ifndef HH_MODS_H
#define HH_MODS_H

//Bonus happiness from discovering natural wonders as a policy value under column "ExtraNaturalWonderHappiness"
#define HH_MOD_NATURAL_WONDER_MODULARITY
//see: CvPolicyClasses@ CvPolicyEntry; CvPlayer@ CvPlayer

//Modifiers for each yield of international trade routes; secondary table Policy_InternationalRouteYieldModifiers (schema same as Policy_YieldFromBarbarianKills)
// -- FOOD and PRODUCTION not supported
#define HH_MOD_API_TRADEROUTE_MODIFIERS
//see: CvPolicyClasses@ CvPolicyEntry, CvPlayerPolicies; CvTradeClasses@ CvPlayerTrade

//Building quality - block benefit from pillaging inside owned territory
#define HH_MOD_BUILDINGS_FRUITLESS_PILLAGE
//see: CvUnit @ CvUnit::pillage; CvPlayer@ CvPlayer; CvCity@ CvCity; CvBuildingClasses@ CvBuildingEntry

#endif