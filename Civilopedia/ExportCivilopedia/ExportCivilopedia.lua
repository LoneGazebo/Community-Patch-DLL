-- ExportCivilopedia.lua
-- Exports all Civilopedia data to JSON format for generating an online HTML version

print("Loading ExportCivilopedia.lua")

include("InfoTooltipInclude")
include( "TechHelpInclude" )

-- Simple JSON encoder
local JSONEncoder = {}

function JSONEncoder.encode(val)
    local function escapeString(str)
        if str == nil then return "null" end
        str = tostring(str)
        str = string.gsub(str, "\\", "\\\\")
        str = string.gsub(str, '"', '\\"')
        str = string.gsub(str, "\n", "\\n")
        str = string.gsub(str, "\r", "\\r")
        str = string.gsub(str, "\t", "\\t")
        return '"' .. str .. '"'
    end

    local function encodeValue(v, indent)
        local indent = indent or ""
        local nextIndent = indent .. "  "

        if v == nil then
            return "null"
        elseif type(v) == "boolean" then
            return tostring(v)
        elseif type(v) == "number" then
            return tostring(v)
        elseif type(v) == "string" then
            return escapeString(v)
        elseif type(v) == "table" then
            -- Check if it's an array
            local isArray = true
            local count = 0
            for k, _ in pairs(v) do
                count = count + 1
                if type(k) ~= "number" or k ~= count then
                    isArray = false
                    break
                end
            end

            if isArray and count > 0 then
                -- Encode as array
                local parts = {}
                for i = 1, count do
                    table.insert(parts, encodeValue(v[i], nextIndent))
                end
                if count > 3 then
                    return "[\n" .. nextIndent .. table.concat(parts, ",\n" .. nextIndent) .. "\n" .. indent .. "]"
                else
                    return "[" .. table.concat(parts, ", ") .. "]"
                end
            else
                -- Encode as object
                local parts = {}
                for k, val in pairs(v) do
                    local key = type(k) == "string" and escapeString(k) or escapeString(tostring(k))
                    table.insert(parts, key .. ": " .. encodeValue(val, nextIndent))
                end
                if #parts > 0 then
                    return "{\n" .. nextIndent .. table.concat(parts, ",\n" .. nextIndent) .. "\n" .. indent .. "}"
                else
                    return "{}"
                end
            end
        else
            return "null"
        end
    end

    return encodeValue(val, "")
end

-- Helper function to check if a text key exists
function TagExists(tag)
    local result = Locale.ConvertTextKey(tag)
    return result ~= tag
end

-- Get all text sections for a given tag (like TXT_KEY_CIV5_AMERICA)
function GetAllTextSections(baseTag)
    local sections = {}

    if not baseTag then
        return sections
    end

    -- Get numbered sections (HEADING_1, TEXT_1, etc.)
    local headerString = baseTag .. "_HEADING_"
    local bodyString = baseTag .. "_TEXT_"
    local i = 1

    while true do
        local headerTag = headerString .. tostring(i)
        local bodyTag = bodyString .. tostring(i)

        if TagExists(headerTag) and TagExists(bodyTag) then
            table.insert(sections, {
                heading = Locale.ConvertTextKey(headerTag),
                text = Locale.ConvertTextKey(bodyTag)
            })
            i = i + 1
        else
            break
        end
    end

    -- Get factoid if it exists
    local factoidHeaderTag = baseTag .. "_FACTOID_HEADING"
    local factoidBodyTag = baseTag .. "_FACTOID_TEXT"

    if TagExists(factoidHeaderTag) and TagExists(factoidBodyTag) then
        table.insert(sections, {
            heading = Locale.ConvertTextKey(factoidHeaderTag),
            text = Locale.ConvertTextKey(factoidBodyTag),
            isFactoid = true
        })
    end

    return sections
end

-- Helper function to safely get field values (handles nil)
local function safeGet(obj, field, default)
    if obj and obj[field] ~= nil then
        return obj[field]
    end
    return default or nil
end

-- Helper function to clean worker action descriptions
-- Removes [LINK=XXX]text[\LINK] and the duplicate text that follows
local function cleanWorkerActionText(text)
    if not text then return text end

    -- Simply remove LINK tags from worker action descriptions
    -- The game data has: "Construct an [LINK=IMPROVEMENT_EKI]Eki[\LINK]"
    -- We want: "Construct an Eki"
    local cleaned = text:gsub("%[LINK=[^%]]+%]([^%[]+)%[\\LINK%]", "%1")

    return cleaned
end

-- Helper function to build civilization relationship tables
-- This matches the logic in CivilopediaScreen.lua AppendCivRelationshipTable
local function AppendCivRelationshipTable(relationshipTable, searchTable, typeColumnName, civColumnName)
    for row in searchTable do
        if row[typeColumnName] ~= nil and row[civColumnName] ~= nil then
            relationshipTable[row[civColumnName]] = row[typeColumnName]
        end
    end
    return relationshipTable
end

-- Helper function to check if a building should be added to "leads to" list
-- This matches the logic in CivilopediaScreen.lua AddLeadsToBuildingInstance
local function ShouldAddLeadsToBuilding(thisBuildingTable, thisRelatedBuildingTable, nextBuildingType)
    local bOtherBuildingOverride = false
    local bAddBuilding = false

    local nextBuildingTable = AppendCivRelationshipTable({}, GameInfo.Buildings("Type = '" .. nextBuildingType .. "'"), "Type", "CivilizationRequired")
    nextBuildingTable = AppendCivRelationshipTable(nextBuildingTable, GameInfo.Civilization_BuildingClassOverrides("BuildingType = '" .. nextBuildingType .. "'"), "BuildingType", "CivilizationType")

    local nextBuilding = GameInfo.Buildings[nextBuildingType]
    if not nextBuilding then
        return false
    end

    local nextRelatedBuildingTable = AppendCivRelationshipTable({}, GameInfo.Buildings("BuildingClass = '" .. nextBuilding.BuildingClass .. "'"), "Type", "CivilizationRequired")
    nextRelatedBuildingTable = AppendCivRelationshipTable(nextRelatedBuildingTable, GameInfo.Civilization_BuildingClassOverrides("BuildingClassType = '" .. nextBuilding.BuildingClass .. "'"), "BuildingType", "CivilizationType")

    -- If the potential next building matches civilizations with this building, add it
    for thisCivilization, _ in pairs(thisBuildingTable) do
        for nextCivilization, _ in pairs(nextBuildingTable) do
            if nextCivilization == thisCivilization then
                bAddBuilding = true
            end
        end
    end

    -- If the potential NEXT building is default and there are no other buildings in the NEXT building's class that match civilizations with this building, add the potential next building
    local nextTableEmpty = true
    for _ in pairs(nextBuildingTable) do
        nextTableEmpty = false
        break
    end

    if nextTableEmpty then
        for thisCivilization, _ in pairs(thisBuildingTable) do
            for nextRelatedCivilization, _ in pairs(nextRelatedBuildingTable) do
                if thisCivilization == nextRelatedCivilization then
                    bOtherBuildingOverride = true
                end
            end
        end
        if not bOtherBuildingOverride then
            bAddBuilding = true
        end
    else
        -- If THIS building is default and there are no other buildings in THIS building's Class that match civilizations with the potential next building, then add the potential next building
        local thisTableEmpty = true
        for _ in pairs(thisBuildingTable) do
            thisTableEmpty = false
            break
        end

        if thisTableEmpty then
            for thisRelatedCivilization, _ in pairs(thisRelatedBuildingTable) do
                for nextCivilization, _ in pairs(nextBuildingTable) do
                    if nextCivilization == thisRelatedCivilization then
                        bOtherBuildingOverride = true
                    end
                end
            end
            if not bOtherBuildingOverride then
                bAddBuilding = true
            end
        end
    end

    return bAddBuilding
end

-- Helper function to get all buildings that require this building (leads to)
-- This matches the logic in CivilopediaScreen.lua AddLeadsToBuildingFrame
local function GetLeadsToBuildings(thisBuilding)
    local thisBuildingTable = AppendCivRelationshipTable({}, GameInfo.Buildings("Type = '" .. thisBuilding.Type .. "'"), "Type", "CivilizationRequired")
    thisBuildingTable = AppendCivRelationshipTable(thisBuildingTable, GameInfo.Civilization_BuildingClassOverrides("BuildingType = '" .. thisBuilding.Type .. "'"), "BuildingType", "CivilizationType")

    local thisRelatedBuildingTable = AppendCivRelationshipTable({}, GameInfo.Buildings("BuildingClass = '" .. thisBuilding.BuildingClass .. "'"), "Type", "CivilizationRequired")
    thisRelatedBuildingTable = AppendCivRelationshipTable(thisRelatedBuildingTable, GameInfo.Civilization_BuildingClassOverrides("BuildingClassType = '" .. thisBuilding.BuildingClass .. "'"), "BuildingType", "CivilizationType")

    local leadsToBuildings = {}
    local thisBuildingClassCondition = "BuildingClassType = '" .. thisBuilding.BuildingClass .. "'"

    -- Check Building_ClassesNeededInCity
    for nextBuildingRow in GameInfo.Building_ClassesNeededInCity(thisBuildingClassCondition) do
        if ShouldAddLeadsToBuilding(thisBuildingTable, thisRelatedBuildingTable, nextBuildingRow.BuildingType) then
            local nextBuilding = GameInfo.Buildings[nextBuildingRow.BuildingType]
            if nextBuilding then
                -- Check if this building is a wonder
                local nextBuildingClass = GameInfo.BuildingClasses[nextBuilding.BuildingClass]
                local isWonder = false
                if nextBuildingClass then
                    if nextBuildingClass.MaxGlobalInstances > 0 then
                        isWonder = true
                    elseif nextBuildingClass.MaxPlayerInstances == 1 and safeGet(nextBuilding, "SpecialistCount", 0) == 0 then
                        isWonder = true
                    end
                end

                table.insert(leadsToBuildings, {
                    Type = nextBuilding.Type,
                    Name = Locale.ConvertTextKey(nextBuilding.Description),
                    IsWonder = isWonder
                })
            end
        end
    end

    -- Check Building_ClassNeededAnywhere
    for nextBuildingRow in GameInfo.Building_ClassNeededAnywhere(thisBuildingClassCondition) do
        if ShouldAddLeadsToBuilding(thisBuildingTable, thisRelatedBuildingTable, nextBuildingRow.BuildingType) then
            local nextBuilding = GameInfo.Buildings[nextBuildingRow.BuildingType]
            if nextBuilding then
                -- Check if already added
                local alreadyAdded = false
                for _, existing in ipairs(leadsToBuildings) do
                    if existing.Type == nextBuilding.Type then
                        alreadyAdded = true
                        break
                    end
                end

                if not alreadyAdded then
                    -- Check if this building is a wonder
                    local nextBuildingClass = GameInfo.BuildingClasses[nextBuilding.BuildingClass]
                    local isWonder = false
                    if nextBuildingClass then
                        if nextBuildingClass.MaxGlobalInstances > 0 then
                            isWonder = true
                        elseif nextBuildingClass.MaxPlayerInstances == 1 and safeGet(nextBuilding, "SpecialistCount", 0) == 0 then
                            isWonder = true
                        end
                    end

                    table.insert(leadsToBuildings, {
                        Type = nextBuilding.Type,
                        Name = Locale.ConvertTextKey(nextBuilding.Description),
                        IsWonder = isWonder
                    })
                end
            end
        end
    end

    return leadsToBuildings
end

-- Export Leader data
function ExportLeaders()
    print("Exporting leaders...")
    local leaders = {}

    for row in GameInfo.Leaders() do
        local leaderData = {
            Type = row.Type,
            ID = safeGet(row, "ID"),
            Name = Locale.ConvertTextKey(row.Description),
            CivilopediaTag = safeGet(row, "CivilopediaTag"),
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),

            -- Get civilization for this leader
            Civilizations = {},

            -- Get leader traits
            Traits = {},

            -- Get civilopedia text sections
            TextSections = GetAllTextSections(row.CivilopediaTag)
        }

        if leaderData.Type ~= "LEADER_BARBARIAN" then
            -- Get associated civilization(s)
            for civRow in GameInfo.Civilization_Leaders("LeaderheadType = '" .. row.Type .. "'") do
                local civ = GameInfo.Civilizations[civRow.CivilizationType]
                if civ and civ.Type ~= "CIVILIZATION_MINOR" and civ.Type ~= "CIVILIZATION_BARBARIAN" then
                    table.insert(leaderData.Civilizations, {
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    })
                end
            end

            -- Get leader traits
            for traitRow in GameInfo.Leader_Traits("LeaderType = '" .. row.Type .. "'") do
                local trait = GameInfo.Traits[traitRow.TraitType]
                if trait then
                    table.insert(leaderData.Traits, {
                        Type = trait.Type,
                        Name = Locale.ConvertTextKey(trait.ShortDescription),
                        Description = Locale.ConvertTextKey(trait.Description)
                    })
                end
            end

            table.insert(leaders, leaderData)
        end
    end

    print("Exported " .. #leaders .. " leaders")
    return leaders
end

-- Export Civilization data
function ExportCivilizations()
    print("Exporting civilizations...")
    local civs = {}

    for row in GameInfo.Civilizations() do
        if row.Type ~= "CIVILIZATION_MINOR" and row.Type ~= "CIVILIZATION_BARBARIAN" then
            local civData = {
                Type = row.Type,
                ID = row.ID,
                Name = Locale.ConvertTextKey(row.ShortDescription),
                Adjective = Locale.ConvertTextKey(row.Adjective),
                CivilopediaTag = safeGet(row, "CivilopediaTag"),
                IconAtlas = safeGet(row, "IconAtlas"),
                PortraitIndex = safeGet(row, "PortraitIndex"),
                AlphaIconAtlas = safeGet(row, "AlphaIconAtlas"),

                -- Get leader info
                Leaders = {},

                -- Get unique units
                UniqueUnits = {},

                -- Get unique buildings
                UniqueBuildings = {},

                -- Get unique improvements
                UniqueImprovements = {},

                -- Get start biases
                StartBiases = {},

                -- Get civilopedia text sections
                TextSections = GetAllTextSections(row.CivilopediaTag)
            }

            -- Get leader(s)
            for leaderRow in GameInfo.Civilization_Leaders("CivilizationType = '" .. row.Type .. "'") do
                local leader = GameInfo.Leaders[leaderRow.LeaderheadType]
                if leader then
                    table.insert(civData.Leaders, {
                        Type = leader.Type,
                        Name = Locale.ConvertTextKey(leader.Description),
                        IconAtlas = safeGet(leader, "IconAtlas"),
                        PortraitIndex = safeGet(leader, "PortraitIndex")
                    })
                end
            end

            -- Get unique units
            for override in GameInfo.Civilization_UnitClassOverrides("CivilizationType = '" .. row.Type .. "'") do
                if override.UnitType then
                    local unit = GameInfo.Units[override.UnitType]
                    if unit then
                        table.insert(civData.UniqueUnits, {
                            Type = unit.Type,
                            Name = Locale.ConvertTextKey(unit.Description),
                            IconAtlas = safeGet(unit, "IconAtlas"),
                            PortraitIndex = safeGet(unit, "PortraitIndex")
                        })
                    end
                end
            end

            -- Get unique buildings
            for override in GameInfo.Civilization_BuildingClassOverrides("CivilizationType = '" .. row.Type .. "'") do
                if override.BuildingType then
                    local building = GameInfo.Buildings[override.BuildingType]
                    if building then
                        table.insert(civData.UniqueBuildings, {
                            Type = building.Type,
                            Name = Locale.ConvertTextKey(building.Description),
                            IconAtlas = safeGet(building, "IconAtlas"),
                            PortraitIndex = safeGet(building, "PortraitIndex")
                        })
                    end
                end
            end

            -- Get unique improvements
            for improvement in GameInfo.Improvements("CivilizationType = '" .. row.Type .. "'") do
                table.insert(civData.UniqueImprovements, {
                    Type = improvement.Type,
                    Name = Locale.ConvertTextKey(improvement.Description),
                    IconAtlas = safeGet(improvement, "IconAtlas"),
                    PortraitIndex = safeGet(improvement, "PortraitIndex")
                })
            end

            -- Get start biases
            for bias in GameInfo.Civilization_Start_Along_Ocean("CivilizationType = '" .. row.Type .. "'") do
                table.insert(civData.StartBiases, {Type = "StartAlongOcean"})
            end
            for bias in GameInfo.Civilization_Start_Along_River("CivilizationType = '" .. row.Type .. "'") do
                table.insert(civData.StartBiases, {Type = "StartAlongRiver"})
            end
            for bias in GameInfo.Civilization_Start_Region_Priority("CivilizationType = '" .. row.Type .. "'") do
                table.insert(civData.StartBiases, {
                    Type = "RegionPriority",
                    Region = bias.RegionType
                })
            end

            table.insert(civs, civData)
        end
    end

    print("Exported " .. #civs .. " civilizations")
    return civs
end

-- Export Unit data
function ExportUnits()
    print("Exporting units...")
    local units = {}

    for row in GameInfo.Units() do
        if row.ShowInPedia and (row.PrereqTech ~= nil or row.Special == nil) then
            local gameInfo = nil
            if GetHelpTextForUnit then
                local success, result = pcall(function()
                    return GetHelpTextForUnit(row.ID, true, nil, true, true)
                end)
                if success and result then
                    gameInfo = result
                end
            end

            -- Fallback to the Help field if dynamic generation fails
            if not gameInfo or gameInfo == "" then
                gameInfo = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil
            end

            local unitData = {
                Type = row.Type,
                ID = row.ID,
                Name = Locale.ConvertTextKey(row.Description),
                Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
                Strategy = safeGet(row, "Strategy") and Locale.ConvertTextKey(row.Strategy) or nil,
                Help = gameInfo,
                Class = safeGet(row, "Class"),
                Combat = safeGet(row, "Combat"),
                RangedCombat = safeGet(row, "RangedCombat"),
                Range = safeGet(row, "Range"),
                Moves = safeGet(row, "Moves"),
                IconAtlas = safeGet(row, "IconAtlas"),
                PortraitIndex = safeGet(row, "PortraitIndex"),
                Special = safeGet(row, "Special"),
                Domain = safeGet(row, "Domain"),
                PrereqTech = nil,
                EraID = nil,
                EraName = nil,
                ObsoleteTech = nil,
                FreePromotions = {}
            }

            -- Get prerequisite technology
            if safeGet(row, "PrereqTech") then
                local tech = GameInfo.Technologies[row.PrereqTech]
                if tech then
                    unitData.PrereqTech = {
                        Type = tech.Type,
                        Name = Locale.ConvertTextKey(tech.Description)
                    }
                    unitData.EraID = GameInfo.Eras[tech.Era].ID
                    unitData.EraName = Locale.ConvertTextKey(GameInfo.Eras[tech.Era].Description)
                end
            end

            if not unitData.EraID then
                unitData.EraID = 0
                unitData.EraName = Locale.ConvertTextKey(GameInfo.Eras[0].Description)
            end

            -- Get obsolete technology
            if safeGet(row, "ObsoleteTech") then
                local tech = GameInfo.Technologies[row.ObsoleteTech]
                if tech then
                    unitData.ObsoleteTech = {
                        Type = tech.Type,
                        Name = Locale.ConvertTextKey(tech.Description)
                    }
                end
            end

            -- Check if this is a civilization-specific unit
            local civNames = {}
            local civilizations = {}
            for override in GameInfo.Civilization_UnitClassOverrides("UnitType = '" .. row.Type .. "'") do
                local civ = GameInfo.Civilizations[override.CivilizationType]
                if civ and civ.Type ~= "CIVILIZATION_MINOR" then
                    table.insert(civNames, Locale.ConvertTextKey(civ.Adjective))
                    table.insert(civilizations, {
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    })
                end
            end

            -- If it's civilization-specific, prepend the civilization name(s)
            if #civNames > 0 then
                unitData.Civilizations = civilizations
                unitData.Name = "[COLOR_POSITIVE_TEXT](" .. table.concat(civNames, ", ") .. ")[ENDCOLOR] " .. unitData.Name
            end

            -- Get free promotions
            for promoRow in GameInfo.Unit_FreePromotions("UnitType = '" .. row.Type .. "'") do
                local promotion = GameInfo.UnitPromotions[promoRow.PromotionType]
                if promotion then
                    table.insert(unitData.FreePromotions, {
                        Type = promotion.Type,
                        Name = Locale.ConvertTextKey(promotion.Description)
                    })
                end
            end

            table.insert(units, unitData)
        end
    end

    print("Exported " .. #units .. " units")
    return units
end

-- Export Building data
function ExportBuildings()
    print("Exporting buildings...")
    local buildings = {}

    for row in GameInfo.Buildings() do
        -- Generate dynamic game info using the tooltip function
        local gameInfo = nil
        if GetHelpTextForBuilding then
            local success, result = pcall(function()
                return GetHelpTextForBuilding(row.ID, true, nil, false, nil, true)
            end)
            if success and result then
                gameInfo = result
            end
        end

        -- Fallback to the Help field if dynamic generation fails
        if not gameInfo or gameInfo == "" then
            gameInfo = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil
        end

        local buildingData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            Strategy = safeGet(row, "Strategy") and Locale.ConvertTextKey(row.Strategy) or nil,
            Help = gameInfo,
            Cost = safeGet(row, "Cost"),
            FaithCost = safeGet(row, "FaithCost"),
            Maintenance = safeGet(row, "GoldMaintenance"),
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            BuildingClass = safeGet(row, "BuildingClass"),

            -- Prerequisite Technology
            PrereqTech = nil,
            EraID = nil,
            EraName = nil,

            -- Required Buildings
            RequiredBuildings = {},

            -- Yields
            YieldChanges = {},
            YieldModifiers = {},

            -- Specialists
            SpecialistType = safeGet(row, "SpecialistType"),
            SpecialistCount = safeGet(row, "SpecialistCount"),
            GreatPeopleRateChange = safeGet(row, "GreatPeopleRateChange"),

            -- Special properties
            IsWonder = false,
            IsNationalWonder = false,
            IsCorporation = safeGet(row, "IsCorporation") or false,
            UnlockedByBelief = safeGet(row, "UnlockedByBelief") or false
        }

        -- Get prerequisite tech
        if safeGet(row, "PrereqTech") then
            local tech = GameInfo.Technologies[row.PrereqTech]
            if tech then
                buildingData.PrereqTech = {
                    Type = tech.Type,
                    Name = Locale.ConvertTextKey(tech.Description)
                }
                buildingData.EraID = GameInfo.Eras[tech.Era].ID
                buildingData.EraName = Locale.ConvertTextKey(GameInfo.Eras[tech.Era].Description)
            end
        end

        -- Ancient Era for everything without prereq tech
        if not buildingData.EraID then
            buildingData.EraID = 0
            buildingData.EraName = Locale.ConvertTextKey(GameInfo.Eras[0].Description)
        end

        -- Special sorting for Corporations and Religious Buildings
        if buildingData.IsCorporation then
            buildingData.EraID = 99
            buildingData.EraName = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_17_LABEL")
        end
        
        if buildingData.Cost == -1 and buildingData.FaithCost > 0 then
            buildingData.EraID = -1
            buildingData.EraName = Locale.ConvertTextKey("TXT_KEY_PEDIA_RELIGIOUS")
        end

        -- Get required buildings
        for reqRow in GameInfo.Building_ClassesNeededInCity("BuildingType = '" .. row.Type .. "'") do
            local buildingClass = GameInfo.BuildingClasses[reqRow.BuildingClassType]
            if buildingClass and buildingClass.DefaultBuilding then
                local reqBuilding = GameInfo.Buildings[buildingClass.DefaultBuilding]
                if reqBuilding then
                    table.insert(buildingData.RequiredBuildings, {
                        Type = reqBuilding.Type,
                        Name = Locale.ConvertTextKey(reqBuilding.Description)
                    })
                end
            end
        end

        -- Get buildings that require this building (leads to)
        buildingData.LeadsToBuildings = GetLeadsToBuildings(row)

        -- Get yield changes
        for yieldRow in GameInfo.Building_YieldChanges("BuildingType = '" .. row.Type .. "'") do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(buildingData.YieldChanges, {
                    YieldType = yieldRow.YieldType,
                    YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end

        -- Get yield modifiers
        for yieldRow in GameInfo.Building_YieldModifiers("BuildingType = '" .. row.Type .. "'") do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(buildingData.YieldModifiers, {
                    YieldType = yieldRow.YieldType,
                    YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end

        -- Check if it's a wonder
        local buildingClass = GameInfo.BuildingClasses[row.BuildingClass]
        if buildingClass then
            if buildingClass.MaxGlobalInstances > 0 then
                buildingData.IsWonder = true
            elseif buildingClass.MaxPlayerInstances == 1 and safeGet(row, "SpecialistCount", 0) == 0 then
                buildingData.IsNationalWonder = true
            end
        end

        -- Check if this is a civilization-specific building
        local civNames = {}
        local civilizations = {}
        for override in GameInfo.Civilization_BuildingClassOverrides("BuildingType = '" .. row.Type .. "'") do
            local civ = GameInfo.Civilizations[override.CivilizationType]
            if civ and civ.Type ~= "CIVILIZATION_MINOR" then
                table.insert(civNames, Locale.ConvertTextKey(civ.Adjective))
                table.insert(civilizations, {
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    })
            end
        end

        -- If it's civilization-specific, prepend the civilization name(s)
        if #civNames > 0 then
            buildingData.Civilizations = civilizations
            buildingData.Name = "[COLOR_POSITIVE_TEXT](" .. table.concat(civNames, ", ") .. ")[ENDCOLOR] " .. buildingData.Name
        end

        table.insert(buildings, buildingData)
    end

    print("Exported " .. #buildings .. " buildings")
    return buildings
end

-- Export Technology data
function ExportTechnologies()
    print("Exporting technologies...")
    local techs = {}

    for row in GameInfo.Technologies() do
        -- Generate dynamic game info using the tooltip function
        local gameInfo = nil
        if GetHelpTextForTech then
            local success, result = pcall(function()
                return GetHelpTextForTech(row.ID, false)
            end)
            if success and result then
                gameInfo = result
            end
        end

        -- Fallback to the Help field if dynamic generation fails
        if not gameInfo or gameInfo == "" then
            gameInfo = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil
        end

        local techData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            Help = gameInfo,
            Quote = safeGet(row, "Quote") and Locale.ConvertTextKey(row.Quote) or nil,
            Cost = safeGet(row, "Cost"),
            Era = safeGet(row, "Era"),
            EraName = nil,
            EraID = nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            GridX = safeGet(row, "GridX"),
            GridY = safeGet(row, "GridY"),
            LeadsToTechs = {},
            UnlockedUnits = {},
            UnlockedBuildings = {},
            UnlockedWonders = {},
            UnlockedProjects = {},
            RevealedResources = {},
            WorkerActions = {}
        }

        -- Get Era name
        if techData.Era then
            local era = GameInfo.Eras[techData.Era]
            if era then
                techData.EraName = Locale.ConvertTextKey(era.Description)
                techData.EraID = era.ID
            end
        end

        -- Get technologies that require this tech (leads to)
        for leadsToRow in GameInfo.Technology_PrereqTechs("PrereqTech = '" .. row.Type .. "'") do
            local leadsToTech = GameInfo.Technologies[leadsToRow.TechType]
            if leadsToTech then
                table.insert(techData.LeadsToTechs, {
                    Type = leadsToTech.Type,
                    Name = Locale.ConvertTextKey(leadsToTech.Description)
                })
            end
        end

        -- Get units unlocked by this tech
        for unit in GameInfo.Units("PrereqTech = '" .. row.Type .. "'") do
            if unit.ShowInPedia then
                table.insert(techData.UnlockedUnits, {
                    Type = unit.Type,
                    Name = Locale.ConvertTextKey(unit.Description)
                })
            end
        end

        -- Get buildings unlocked by this tech
        -- Separate into regular buildings and wonders
        for building in GameInfo.Buildings("PrereqTech = '" .. row.Type .. "'") do
            if building.ShowInPedia then
                local buildingClass = GameInfo.BuildingClasses[building.BuildingClass]
                local isWonder = false

                -- Check if it's a wonder (world or national)
                if buildingClass then
                    if buildingClass.MaxGlobalInstances > 0 then
                        isWonder = true
                    elseif buildingClass.MaxPlayerInstances == 1 and safeGet(building, "SpecialistCount", 0) == 0 then
                        isWonder = true
                    end
                end

                local buildingData = {
                    Type = building.Type,
                    Name = Locale.ConvertTextKey(building.Description)
                }

                if isWonder then
                    table.insert(techData.UnlockedWonders, buildingData)
                else
                    table.insert(techData.UnlockedBuildings, buildingData)
                end
            end
        end

        -- Get projects unlocked by this tech
        for project in GameInfo.Projects("TechPrereq = '" .. row.Type .. "'") do
            -- Ignore Spaceship parts
            if not project.Spaceship then
                table.insert(techData.UnlockedProjects, {
                    Type = project.Type,
                    Name = Locale.ConvertTextKey(project.Description)
                })
            end
        end

        -- Get resources revealed by this tech
        for resource in GameInfo.Resources("TechReveal = '" .. row.Type .. "'") do
            table.insert(techData.RevealedResources, {
                Type = resource.Type,
                Name = Locale.ConvertTextKey(resource.Description)
            })
        end

        -- Get worker actions (builds) unlocked by this tech
        for build in GameInfo.Builds("PrereqTech = '" .. row.Type .. "'") do
            if build.ShowInPedia then
                local actionName = Locale.ConvertTextKey(build.Description)
                actionName = cleanWorkerActionText(actionName)

                local actionData = {
                    Type = build.Type,
                    Name = actionName,
                    IconAtlas = safeGet(build, "IconAtlas"),
                    IconIndex = safeGet(build, "IconIndex")
                }

                -- Determine what this build creates
                if build.RouteType then
                    local route = GameInfo.Routes[build.RouteType]
                    if route then
                        actionData.CreatesRoute = {
                            Type = route.Type,
                            Name = Locale.ConvertTextKey(route.Description)
                        }
                    end
                elseif build.ImprovementType then
                    local improvement = GameInfo.Improvements[build.ImprovementType]
                    if improvement then
                        actionData.CreatesImprovement = {
                            Type = improvement.Type,
                            Name = Locale.ConvertTextKey(improvement.Description)
                        }
                    end
                end

                table.insert(techData.WorkerActions, actionData)
            end
        end

        table.insert(techs, techData)
    end

    print("Exported " .. #techs .. " technologies")
    return techs
end

-- Export Policy data
function ExportPolicies()
    print("Exporting policies...")
    local policies = {}

    -- Build maps of openers and finishers from PolicyBranchTypes
    -- This matches the logic in CivilopediaScreen.lua (lines 1023-1082)
    -- where openers are added separately via branch.FreePolicy
    local openerPolicies = {}
    local finisherPolicies = {}
    for branch in GameInfo.PolicyBranchTypes() do
        if branch.FreePolicy then
            openerPolicies[branch.FreePolicy] = branch.Type
        end
        if branch.FreeFinishingPolicy then
            finisherPolicies[branch.FreeFinishingPolicy] = branch.Type
        end
    end

    for row in GameInfo.Policies() do
        local isOpener = openerPolicies[row.Type] ~= nil
        local isFinisher = finisherPolicies[row.Type] ~= nil
        if not isFinisher then

            local policyData = {
                Type = row.Type,
                ID = row.ID,
                Name = Locale.ConvertTextKey(row.Description),
                Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
                Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
                IconAtlas = safeGet(row, "IconAtlas"),
                PortraitIndex = safeGet(row, "PortraitIndex"),
                PolicyBranchType = safeGet(row, "PolicyBranchType"),
                PolicyBranchName = nil,
                GridX = safeGet(row, "GridX"),
                GridY = safeGet(row, "GridY"),
                IsOpener = isOpener
            }

            if row.Level and row.Level > 0 then
                policyData.Name = "[COLOR_POSITIVE_TEXT](".. row.Level .. ")[ENDCOLOR] " .. policyData.Name;
            end

            -- Get Policy Branch name
            -- Openers typically don't have PolicyBranchType set, so we get it from the map
            -- Regular policies have PolicyBranchType set correctly
            if isOpener then
                local branchType = openerPolicies[row.Type]
                local branch = GameInfo.PolicyBranchTypes[branchType]
                if branch then
                    policyData.PolicyBranchName = Locale.ConvertTextKey(branch.Description)
                    policyData.PolicyBranchID = branch.ID
                end
            elseif policyData.PolicyBranchType then
                -- Regular policy: use the PolicyBranchType field
                local branch = GameInfo.PolicyBranchTypes[policyData.PolicyBranchType]
                if branch then
                    policyData.PolicyBranchName = Locale.ConvertTextKey(branch.Description)
                    policyData.PolicyBranchID = branch.ID
                end
            end

            table.insert(policies, policyData)
        end
    end

    print("Exported " .. #policies .. " policies")
    return policies
end

-- Export Religions data
function ExportReligions()
    print("Exporting religions...")
    local religions = {}

    -- Export all religions except RELIGION_PANTHEON (as per CivilopediaScreen.lua)
    for row in GameInfo.Religions("Type <> 'RELIGION_PANTHEON'") do
        local religionData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Summary = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            IconString = safeGet(row, "IconString")
        }

        table.insert(religions, religionData)
    end

    print("Exported " .. #religions .. " religions")
    return religions
end

-- Export Belief data
function ExportBeliefs()
    print("Exporting beliefs...")
    local beliefs = {}

    for row in GameInfo.Beliefs() do
        -- Determine belief category based on flags
        local category = nil
        local categoryName = nil

        if safeGet(row, "Pantheon", false) then
            category = "Pantheon"
            categoryName = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_2")
        elseif safeGet(row, "Founder", false) then
            category = "Founder"
            categoryName = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_3")
        elseif safeGet(row, "Follower", false) then
            category = "Follower"
            categoryName = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_4")
        elseif safeGet(row, "Enhancer", false) then
            category = "Enhancer"
            categoryName = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_5")
        elseif safeGet(row, "Reformation", false) then
            category = "Reformation"
            categoryName = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_6")
        end

        local beliefData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.ShortDescription),
            Summary = Locale.ConvertTextKey(row.Description), -- intentional
            Category = category,
            CategoryName = categoryName
        }

        -- Check if it's civilization-specific
        if safeGet(row, "CivilizationType") and GameInfo.Civilizations[row.CivilizationType] then
            beliefData.CivilizationType = row.CivilizationType
            beliefData.CivilizationName = Locale.ConvertTextKey(GameInfo.Civilizations[row.CivilizationType].Adjective)
            beliefData.Name = "[COLOR_POSITIVE_TEXT](" .. beliefData.CivilizationName .. ")[ENDCOLOR] " .. beliefData.Name
        end

        table.insert(beliefs, beliefData)
    end

    print("Exported " .. #beliefs .. " beliefs")
    return beliefs
end

-- Export Promotion data
function ExportPromotions()
    print("Exporting promotions...")
    local promotions = {}

    for row in GameInfo.UnitPromotions() do
        local promotionData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            Sound = safeGet(row, "Sound"),
            CannotBeChosen = safeGet(row, "CannotBeChosen"),
            PediaType = safeGet(row, "PediaType"),
            PediaTypeLabel = safeGet(row, "PediaType") and Locale.ConvertTextKey("TXT_KEY_PROMOTIONS_"..safeGet(row, "PediaType")) or nil
        }

        -- Add prerequisite promotions (PromotionPrereqOr1-6)
        local requiredPromotions = {}
        for i = 1, 6 do
            local prereqField = "PromotionPrereqOr" .. i
            local prereqType = safeGet(row, prereqField)
            if prereqType then
                local prereqPromotion = GameInfo.UnitPromotions[prereqType]
                if prereqPromotion then
                    table.insert(requiredPromotions, {
                        Type = prereqType,
                        Name = Locale.ConvertTextKey(prereqPromotion.Description)
                    })
                end
            end
        end
        if #requiredPromotions > 0 then
            promotionData.RequiredPromotions = requiredPromotions
        end

        -- Add "leads to" promotions (promotions that have this as a prerequisite)
        local leadsToPromotions = {}
        for loopPromotion in GameInfo.UnitPromotions() do
            for i = 1, 6 do
                local prereqField = "PromotionPrereqOr" .. i
                local prereqType = safeGet(loopPromotion, prereqField)
                if prereqType == row.Type then
                    table.insert(leadsToPromotions, {
                        Type = loopPromotion.Type,
                        Name = Locale.ConvertTextKey(loopPromotion.Description)
                    })
                    break  -- Don't add the same promotion multiple times
                end
            end
        end
        if #leadsToPromotions > 0 then
            promotionData.LeadsToPromotions = leadsToPromotions
        end

        table.insert(promotions, promotionData)
    end

    print("Exported " .. #promotions .. " promotions")
    return promotions
end

-- Export Terrain data
function ExportTerrain()
    print("Exporting terrain...")
    local terrains = {}

    for row in GameInfo.Terrains() do
        local terrainData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            IsFeature = false  -- Mark as terrain, not feature
        }

        -- Add movement cost
        local moveCost = row.Movement or 1
        if row.Type == "TERRAIN_HILL" then
            moveCost = moveCost + (GameDefines.HILLS_EXTRA_MOVEMENT or 1)
        end
        terrainData.MovementCost = moveCost
        if row.Type == "TERRAIN_MOUNTAIN" then
            terrainData.IsImpassable = true
        end

        -- Add combat modifier
        local combatModifier = 0
        if row.Type == "TERRAIN_HILL" or row.Type == "TERRAIN_MOUNTAIN" then
            combatModifier = GameDefines.HILLS_EXTRA_DEFENSE or 25
        elseif not row.Water then
            combatModifier = GameDefines.FLAT_LAND_EXTRA_DEFENSE or 0
        end
        terrainData.DefenseModifier = combatModifier

        -- Add base yields (from Terrain_Yields table)
        local baseYields = {}
        local condition = "TerrainType = '" .. row.Type .. "'"
        for yieldRow in GameInfo.Terrain_Yields(condition) do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(baseYields, {
                    YieldType = yieldRow.YieldType,
                    YieldName =  GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end
        if #baseYields > 0 then
            terrainData.BaseYields = baseYields
        end

        -- Add features that can exist on this terrain
        local features = {}
        for featureRow in GameInfo.Feature_TerrainBooleans(condition) do
            local feature = GameInfo.Features[featureRow.FeatureType]
            if feature then
                table.insert(features, {
                    Type = featureRow.FeatureType,
                    Name = Locale.ConvertTextKey(feature.Description)
                })
            end
        end
        if #features > 0 then
            terrainData.Features = features
        end

        -- Add resources that can be found on this terrain
        local resources = {}
        for resRow in GameInfo.Resource_TerrainBooleans(condition) do
            local resource = GameInfo.Resources[resRow.ResourceType]
            if resource then
                table.insert(resources, {
                    Type = resRow.ResourceType,
                    Name = Locale.ConvertTextKey(resource.Description)
                })
            end
        end
        if #resources > 0 then
            terrainData.Resources = resources
        end

        table.insert(terrains, terrainData)
    end

    -- Also export features in the same array
    for row in GameInfo.Features() do
        local featureData = {
            Type = row.Type,
            ID = row.ID + 1000,  -- Offset feature IDs to avoid collision
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            IsFeature = true  -- Mark as feature
        }

        -- Add movement cost
        featureData.MovementCost = row.Movement or 1

        -- Add combat modifier
        featureData.DefenseModifier = row.Defense or 0

        -- Add base yields (from Feature_YieldChanges table)
        local baseYields = {}
        local condition = "FeatureType = '" .. row.Type .. "'"
        for yieldRow in GameInfo.Feature_YieldChanges(condition) do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(baseYields, {
                    YieldType = yieldRow.YieldType,
                    YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end
        if #baseYields > 0 then
            featureData.BaseYields = baseYields
        end

        -- Add era yield changes
        local eraYields = {}
        for eraYieldRow in GameInfo.Feature_EraYieldChanges(condition) do
            local yieldInfo = GameInfo.Yields[eraYieldRow.YieldType]
            if yieldInfo then
                table.insert(eraYields, {
                    YieldType = eraYieldRow.YieldType,
                    YieldName = GameInfo.Yields[eraYieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = eraYieldRow.Yield
                })
            end
        end
        if #eraYields > 0 then
            featureData.EraYields = eraYields
        end

        -- Add terrains this feature can appear on
        local terrainList = {}
        for terrainRow in GameInfo.Feature_TerrainBooleans(condition) do
            local terrain = GameInfo.Terrains[terrainRow.TerrainType]
            if terrain then
                table.insert(terrainList, {
                    Type = terrainRow.TerrainType,
                    Name = Locale.ConvertTextKey(terrain.Description)
                })
            end
        end
        if #terrainList > 0 then
            featureData.Terrains = terrainList
        end

        -- Add resources that can be found on this feature
        local resources = {}
        for resRow in GameInfo.Resource_FeatureBooleans(condition) do
            local resource = GameInfo.Resources[resRow.ResourceType]
            if resource then
                table.insert(resources, {
                    Type = resRow.ResourceType,
                    Name = Locale.ConvertTextKey(resource.Description)
                })
            end
        end
        if #resources > 0 then
            featureData.Resources = resources
        end

        table.insert(terrains, featureData)
    end

    print("Exported " .. #terrains .. " terrains and features")
    return terrains
end

-- Export Resource data
function ExportResources()
    print("Exporting resources...")
    local resources = {}

    for row in GameInfo.Resources() do
        local resourceData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            ResourceClassType = safeGet(row, "ResourceClassType"),
            ResourceUsage = safeGet(row, "ResourceUsage")
        }

        -- Add reveal technology
        if row.TechReveal then
            local tech = GameInfo.Technologies[row.TechReveal]
            if tech then
                resourceData.TechReveal = row.TechReveal
                resourceData.TechRevealName = Locale.ConvertTextKey(tech.Description)
            end
        end

        -- Add base yields (from Resource_YieldChanges table)
        local baseYields = {}
        local condition = "ResourceType = '" .. row.Type .. "'"
        for yieldRow in GameInfo.Resource_YieldChanges(condition) do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(baseYields, {
                    YieldType = yieldRow.YieldType,
                    YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end
        if #baseYields > 0 then
            resourceData.BaseYields = baseYields
        end

        -- Add terrains where this resource can appear
        local terrains = {}
        for terrainRow in GameInfo.Resource_TerrainBooleans(condition) do
            local terrain = GameInfo.Terrains[terrainRow.TerrainType]
            if terrain then
                table.insert(terrains, {
                    Type = terrainRow.TerrainType,
                    Name = Locale.ConvertTextKey(terrain.Description)
                })
            end
        end
        if row.Hills then
            table.insert(terrains, {
                Type = "TERRAIN_HILL",
                Name = Locale.ConvertTextKey("TXT_KEY_TERRAIN_HILL")
            })
        end
        if #terrains > 0 then
            resourceData.Terrains = terrains
        end

        -- Add features where this resource can appear
        local features = {}
        for featureRow in GameInfo.Resource_FeatureBooleans(condition) do
            local feature = GameInfo.Features[featureRow.FeatureType]
            if feature then
                table.insert(features, {
                    Type = featureRow.FeatureType,
                    Name = Locale.ConvertTextKey(feature.Description)
                })
            end
        end
        if #features > 0 then
            resourceData.Features = features
        end

        -- Add improvements that connect this resource and their yields
        local improvements = {}
        for impRow in GameInfo.Improvement_ResourceTypes(condition) do
            if (impRow.ResourceTrade or 0) ~= 0 then
                local improvement = GameInfo.Improvements[impRow.ImprovementType]
                if improvement then
                    local impData = {
                        Type = impRow.ImprovementType,
                        Name = Locale.ConvertTextKey(improvement.Description),
                        Yields = {}
                    }

                    -- Get yields for this improvement + resource combination
                    local yieldCondition = "ResourceType = '" .. row.Type .. "' AND ImprovementType = '" .. impRow.ImprovementType .. "'"
                    for yieldRow in GameInfo.Improvement_ResourceType_Yields(yieldCondition) do
                        local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
                        if yieldInfo then
                            table.insert(impData.Yields, {
                                YieldType = yieldRow.YieldType,
                                YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                                Yield = yieldRow.Yield
                            })
                        end
                    end

                    table.insert(improvements, impData)
                end
            end
        end
        if #improvements > 0 then
            resourceData.Improvements = improvements
        end

        table.insert(resources, resourceData)
    end

    print("Exported " .. #resources .. " resources")
    return resources
end

-- Export Improvement data
function ExportImprovements()
    print("Exporting improvements...")
    local improvements = {}

    for row in GameInfo.Improvements() do
        local improvementData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            CivilizationType = safeGet(row, "CivilizationType")
        }

        -- Check if this is a civilization-specific improvement
        if row.CivilizationType then
            local civ = GameInfo.Civilizations[row.CivilizationType]
            if civ and civ.Type ~= "CIVILIZATION_MINOR" then
                improvementData.Civilizations = {{
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    }}
                improvementData.Name = "[COLOR_POSITIVE_TEXT](" .. Locale.ConvertTextKey(civ.Adjective) .. ")[ENDCOLOR] " .. improvementData.Name
            end
        end

        local condition = "ImprovementType = '" .. row.Type .. "'"

        -- Add prerequisite technology (from Builds table)
        for buildRow in GameInfo.Builds(condition) do
            if buildRow.PrereqTech then
                local tech = GameInfo.Technologies[buildRow.PrereqTech]
                if tech then
                    improvementData.PrereqTech = {
                            Type = buildRow.PrereqTech,
                            Name = Locale.ConvertTextKey(tech.Description)
                        }
                    break  -- Only need the first one
                end
            end
        end

        -- Add base yields (from Improvement_Yields table)
        local baseYields = {}
        for yieldRow in GameInfo.Improvement_Yields(condition) do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(baseYields, {
                    YieldType = yieldRow.YieldType,
                    YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end
        if #baseYields > 0 then
            improvementData.BaseYields = baseYields
        end

        -- Add valid terrains (from Improvement_ValidTerrains table)
        local terrains = {}
        for terrainRow in GameInfo.Improvement_ValidTerrains(condition) do
            local terrain = GameInfo.Terrains[terrainRow.TerrainType]
            if terrain then
                table.insert(terrains, {
                    Type = terrainRow.TerrainType,
                    Name = Locale.ConvertTextKey(terrain.Description)
                })
            end
        end
        if #terrains > 0 then
            improvementData.Terrains = terrains
        end

        -- Add valid features (from Improvement_ValidFeatures table)
        local features = {}
        for featureRow in GameInfo.Improvement_ValidFeatures(condition) do
            local feature = GameInfo.Features[featureRow.FeatureType]
            if feature then
                table.insert(features, {
                    Type = featureRow.FeatureType,
                    Name = Locale.ConvertTextKey(feature.Description)
                })
            end
        end
        if #features > 0 then
            improvementData.Features = features
        end

        -- Add resources that can be improved by this (we already export this from resources, but useful here too)
        local resources = {}
        for resRow in GameInfo.Improvement_ResourceTypes(condition) do
            if (resRow.ResourceTrade or 0) ~= 0 then
                local resource = GameInfo.Resources[resRow.ResourceType]
                if resource then
                    table.insert(resources, {
                        Type = resRow.ResourceType,
                        Name = Locale.ConvertTextKey(resource.Description)
                    })
                end
            end
        end
        if #resources > 0 then
            improvementData.Resources = resources
        end

        -- Add adjacency bonuses
        local adjacencyBonuses = {}

        -- Mountain adjacency
        for adjRow in GameInfo.Improvement_AdjacentMountainYieldChanges(condition) do
            local yieldInfo = GameInfo.Yields[adjRow.YieldType]
            if yieldInfo then
                table.insert(adjacencyBonuses, {
                    Type = "Mountain",
                    Description = Locale.ConvertTextKey("TXT_KEY_TERRAIN_MOUNTAIN"),
                    YieldType = adjRow.YieldType,
                    YieldName = GameInfo.Yields[adjRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = adjRow.Yield
                })
            end
        end

        -- Adjacent terrain yields
        for adjRow in GameInfo.Improvement_AdjacentTerrainYieldChanges(condition) do
            local terrain = GameInfo.Terrains[adjRow.TerrainType]
            local yieldInfo = GameInfo.Yields[adjRow.YieldType]
            if terrain and yieldInfo then
                table.insert(adjacencyBonuses, {
                    Type = "Terrain",
                    Description = Locale.ConvertTextKey(terrain.Description),
                    YieldType = adjRow.YieldType,
                    YieldName = GameInfo.Yields[adjRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = adjRow.Yield
                })
            end
        end

        -- Adjacent improvement yields (per X adjacent)
        for adjRow in GameInfo.Improvement_YieldPerXAdjacentImprovement(condition) do
            local improvement = GameInfo.Improvements[adjRow.OtherImprovementType]
            local yieldInfo = GameInfo.Yields[adjRow.YieldType]
            if improvement and yieldInfo then
                local numRequired = adjRow.NumRequired or 1
                local yieldValue = adjRow.Yield

                -- Divide yield by NumRequired if > 1 (matching CivilopediaScreen.lua logic)
                if numRequired > 1 then
                    -- Round up to hundredth decimal place
                    local decimalYield = math.ceil(100 * yieldValue / numRequired) / 100
                    yieldValue = decimalYield
                    -- Check if fractional
                    if decimalYield ~= math.floor(decimalYield) then
                        hasFractionalYield = true
                    end
                end

                table.insert(adjacencyBonuses, {
                    Type = "Improvement",
                    Description = Locale.ConvertTextKey(improvement.Description),
                    YieldType = adjRow.YieldType,
                    YieldName = GameInfo.Yields[adjRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldValue
                })
            end
        end

        -- Adjacent terrain yields (per X adjacent)
        for adjRow in GameInfo.Improvement_YieldPerXAdjacentTerrain(condition) do
            local terrain = GameInfo.Terrains[adjRow.TerrainType]
            local yieldInfo = GameInfo.Yields[adjRow.YieldType]
            if terrain and yieldInfo then
                local numRequired = adjRow.NumRequired or 1
                local yieldValue = adjRow.Yield

                -- Divide yield by NumRequired if > 1 (matching CivilopediaScreen.lua logic)
                if numRequired > 1 then
                    -- Round up to hundredth decimal place
                    local decimalYield = math.ceil(100 * yieldValue / numRequired) / 100
                    yieldValue = decimalYield
                    -- Check if fractional
                    if decimalYield ~= math.floor(decimalYield) then
                        hasFractionalYield = true
                    end
                end

                table.insert(adjacencyBonuses, {
                    Type = "Terrain",
                    Description = Locale.ConvertTextKey(terrain.Description),
                    YieldType = adjRow.YieldType,
                    YieldName = GameInfo.Yields[adjRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldValue
                })
            end
        end

        if #adjacencyBonuses > 0 then
            improvementData.AdjacencyBonuses = adjacencyBonuses
        end
        
        -- Add routes bonuses
        local routeBonuses = {}

        for routeRow in GameInfo.Improvement_RouteYieldChanges(condition) do
            local route = GameInfo.Routes[routeRow.RouteType]
            local yieldInfo = GameInfo.Yields[routeRow.YieldType]
            if route and yieldInfo then
                table.insert(routeBonuses, {
                    Type = "Route",
                    Description = Locale.ConvertTextKey(route.Description),
                    YieldType = routeRow.YieldType,
                    YieldName = GameInfo.Yields[routeRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = routeRow.Yield
                })
            end
        end

        if #routeBonuses > 0 then
            improvementData.RouteBonuses = routeBonuses
        end

        table.insert(improvements, improvementData)
    end

    print("Exported " .. #improvements .. " improvements")
    return improvements
end

-- Write to log file (workaround for io library not being available)
function WriteToLog(content, filename)
    filename = filename or "CivilopediaExport.log"

    -- Print in chunks to avoid buffer overflow
    -- Use smaller chunks and replace newlines with a placeholder
    local chunkSize = 3000
    local len = string.len(content)
    local chunks = math.ceil(len / chunkSize)

    print("==================================")
    print("BEGIN EXPORT DATA - " .. filename)
    print("Total chunks: " .. chunks)
    print("==================================")

    for i = 1, chunks do
        local startPos = (i - 1) * chunkSize + 1
        local endPos = math.min(i * chunkSize, len)
        local chunk = string.sub(content, startPos, endPos)

        -- Replace actual newlines with a unique placeholder that won't conflict with JSON
        -- Note: JSON already has \\n for escaped newlines, so we only replace actual \n chars
        chunk = string.gsub(chunk, "\n", "<!NEWLINE!>")
        chunk = string.gsub(chunk, "\r", "<!CR!>")

        print("[CHUNK " .. i .. "/" .. chunks .. "]" .. chunk)
    end

    print("==================================")
    print("END EXPORT DATA")
    print("==================================")
end

-- Export Game Concepts
function ExportConcepts()
    print("Exporting game concepts...")
    local concepts = {}

    -- Build a mapping of header types to section numbers
    -- This matches the exact mapping used in CivilopediaScreen.lua (lines 300-331)
    -- Each header type corresponds to TXT_KEY_GAME_CONCEPT_SECTION_X
    local headerToSection = {
        HEADER_CITIES = 1,
        HEADER_COMBAT = 2,
        HEADER_TERRAIN = 3,
        HEADER_RESOURCES = 4,
        HEADER_IMPROVEMENTS = 5,
        HEADER_CITYGROWTH = 6,
        HEADER_TECHNOLOGY = 7,
        HEADER_CULTURE = 8,
        HEADER_DIPLOMACY = 9,
        HEADER_HAPPINESS = 10,
        HEADER_FOW = 11,
        HEADER_POLICIES = 12,
        HEADER_GOLD = 13,
        HEADER_ADVISORS = 14,
        HEADER_PEOPLE = 15,
        HEADER_CITYSTATE = 16,
        HEADER_MOVEMENT = 17,
        HEADER_AIRCOMBAT = 18,
        HEADER_RUBARB = 19,
        HEADER_UNITS = 20,
        HEADER_VICTORY = 21,
        HEADER_ESPIONAGE = 22,
        HEADER_RELIGION = 23,
        HEADER_TRADE = 24,
        HEADER_WORLDCONGRESS = 25,
        HEADER_VASSALAGE = 26,
        HEADER_CORPORATIONS = 27,
        HEADER_SHORTCUTS = 28
    }

    for row in GameInfo.Concepts() do
        local headerType = safeGet(row, "CivilopediaHeaderType")
        local sectionNum = headerToSection[headerType]
        local headerName = nil

        -- Get localized header name from TXT_KEY_GAME_CONCEPT_SECTION_X
        if sectionNum then
            local sectionKey = "TXT_KEY_GAME_CONCEPT_SECTION_" .. tostring(sectionNum)
            if TagExists(sectionKey) then
                headerName = Locale.ConvertTextKey(sectionKey)
            end
        end

        local conceptData = {
            Type = row.Type,
            ID = row.ID,
            Description = safeGet(row, "Description") and Locale.ConvertTextKey(row.Description) or nil,
            Summary = safeGet(row, "Summary") and Locale.ConvertTextKey(row.Summary) or nil,
            Topic = safeGet(row, "Topic") and Locale.ConvertTextKey(row.Topic) or nil,
            CivilopediaHeaderType = headerType,
            HeaderName = headerName,
            Advisor = safeGet(row, "Advisor"),
            AdvisorQuestion = safeGet(row, "AdvisorQuestion") and Locale.ConvertTextKey(row.AdvisorQuestion) or nil,
            InsertBefore = safeGet(row, "InsertBefore"),
            InsertAfter = safeGet(row, "InsertAfter")
        }

        table.insert(concepts, conceptData)
    end

    print("Exported " .. #concepts .. " game concepts")
    return concepts
end

-- Export Wonders (separate from buildings)
function ExportWonders()
    print("Exporting wonders...")
    local wonders = {}

    for row in GameInfo.Buildings() do
        local buildingClass = GameInfo.BuildingClasses[row.BuildingClass]
        local isWonder = false
        local isNationalWonder = false

        if buildingClass then
            if buildingClass.MaxGlobalInstances > 0 then
                isWonder = true
            elseif buildingClass.MaxPlayerInstances == 1 and safeGet(row, "SpecialistCount", 0) == 0 then
                isNationalWonder = true
            end
        end

        -- Only export if it's a wonder
        if isWonder or isNationalWonder then
            -- Generate dynamic game info
            local gameInfo = nil
            if GetHelpTextForBuilding then
                local success, result = pcall(function()
                    return GetHelpTextForBuilding(row.ID, true, nil, false, nil, true)
                end)
                if success and result then
                    gameInfo = result
                end
            end

            if not gameInfo or gameInfo == "" then
                gameInfo = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil
            end

            local wonderData = {
                Type = row.Type,
                ID = row.ID,
                Name = Locale.ConvertTextKey(row.Description),
                Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
                Strategy = safeGet(row, "Strategy") and Locale.ConvertTextKey(row.Strategy) or nil,
                Help = gameInfo,
                Cost = safeGet(row, "Cost"),
                FaithCost = safeGet(row, "FaithCost"),
                IconAtlas = safeGet(row, "IconAtlas"),
                PortraitIndex = safeGet(row, "PortraitIndex"),
                PrereqTech = nil,
                CivilizationType = nil,
                IsWorldWonder = isWonder,
                IsNationalWonder = isNationalWonder,
                IsProject = false,
                IsCorporation = safeGet(row, "IsCorporation") or false,
                GreatWorkSlotType = safeGet(row, "GreatWorkSlotType"),
                GreatWorkCount = safeGet(row, "GreatWorkCount"),
                RequiredBuildings = {},
                LeadsToBuildings = {}
            }

            -- Get prerequisite tech
            if safeGet(row, "PrereqTech") then
                local tech = GameInfo.Technologies[row.PrereqTech]
                if tech then
                    wonderData.PrereqTech = {
                        Type = tech.Type,
                        Name = Locale.ConvertTextKey(tech.Description)
                    }
                end
            end

            -- Get required buildings
            for reqRow in GameInfo.Building_ClassesNeededInCity("BuildingType = '" .. row.Type .. "'") do
                local buildingClass = GameInfo.BuildingClasses[reqRow.BuildingClassType]
                if buildingClass and buildingClass.DefaultBuilding then
                    local reqBuilding = GameInfo.Buildings[buildingClass.DefaultBuilding]
                    if reqBuilding then
                        table.insert(wonderData.RequiredBuildings, {
                            Type = reqBuilding.Type,
                            Name = Locale.ConvertTextKey(reqBuilding.Description)
                        })
                    end
                end
            end

            -- Get buildings that require this wonder (leads to)
            wonderData.LeadsToBuildings = GetLeadsToBuildings(row)

            -- Check if this is a civilization-specific wonder
            local civNames = {}
            local civilizations = {}
            for override in GameInfo.Civilization_BuildingClassOverrides("BuildingType = '" .. row.Type .. "'") do
                local civ = GameInfo.Civilizations[override.CivilizationType]
                if civ and civ.Type ~= "CIVILIZATION_MINOR" then
                    table.insert(civNames, Locale.ConvertTextKey(civ.Adjective))
                    table.insert(civilizations, {
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    })
                end
            end

            -- If it's civilization-specific, prepend the civilization name(s)
            if #civNames > 0 then
                wonderData.Civilizations = civilizations
                wonderData.Name = "[COLOR_POSITIVE_TEXT](" .. table.concat(civNames, ", ") .. ")[ENDCOLOR] " .. wonderData.Name
            end

            table.insert(wonders, wonderData)
        end
    end

    for row in GameInfo.Projects() do

        -- Ignore Spaceship parts
        if not row.Spaceship then

            local wonderData = {
                Type = row.Type,
                ID = row.ID,
                Name = Locale.ConvertTextKey(row.Description),
                Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
                Strategy = safeGet(row, "Strategy") and Locale.ConvertTextKey(row.Strategy) or nil,
                Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
                Cost = safeGet(row, "Cost"),
                FaithCost = -1,
                IconAtlas = safeGet(row, "IconAtlas"),
                PortraitIndex = safeGet(row, "PortraitIndex"),
                PrereqTech = nil,
                CivilizationType = safeGet(row, "CivilizationType"),
                IsWorldWonder = false,
                IsNationalWonder = false,
                IsProject = true,
                IsCorporation = false,
                GreatWorkSlotType = "",
                GreatWorkCount = 0
            }

            -- Check if this is a civilization-specific project
            local civNames = {}
            local civilizations = {}
            if safeGet(row, "CivilizationType") then
                local civ = GameInfo.Civilizations[row.CivilizationType]
                if civ and civ.Type ~= "CIVILIZATION_MINOR" then
                    table.insert(civNames, Locale.ConvertTextKey(civ.Adjective))
                    table.insert(civilizations, {
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    })
                end
            end

            -- If it's civilization-specific, prepend the civilization name(s)
            if #civNames > 0 then
                wonderData.Civilizations = civilizations
                wonderData.Name = "[COLOR_POSITIVE_TEXT](" .. table.concat(civNames, ", ") .. ")[ENDCOLOR] " .. wonderData.Name
            end

            table.insert(wonders, wonderData)
        end
    end

    print("Exported " .. #wonders .. " wonders")
    return wonders
end

-- Export Great People (Specialists)
function ExportGreatPeople()
    print("Exporting great people...")
    local result = {
        specialists = {},
        greatPersons = {}
    }

    -- Export Specialists (Section 1)
    for row in GameInfo.Specialists() do
        local specialistData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            Strategy = safeGet(row, "Strategy") and Locale.ConvertTextKey(row.Strategy) or nil,
            Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
            GreatPeopleTitle = safeGet(row, "GreatPeopleTitle") and Locale.ConvertTextKey(row.GreatPeopleTitle) or nil,
            GreatPeopleRateChange = safeGet(row, "GreatPeopleRateChange"),
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            YieldChanges = {}
        }

        -- Get yield changes for this specialist
        for yieldRow in GameInfo.SpecialistYields("SpecialistType = '" .. row.Type .. "'") do
            local yieldInfo = GameInfo.Yields[yieldRow.YieldType]
            if yieldInfo then
                table.insert(specialistData.YieldChanges, {
                    YieldType = yieldRow.YieldType,
                    YieldName = GameInfo.Yields[yieldRow.YieldType].IconString .. " " .. Locale.ConvertTextKey(yieldInfo.Description),
                    Yield = yieldRow.Yield
                })
            end
        end

        -- Get the great person unit this specialist produces
        if row.GreatPeopleUnitClass then
            local unitClass = GameInfo.UnitClasses[row.GreatPeopleUnitClass]
            if unitClass then
                -- Find the default unit for this class
                local defaultUnit = GameInfo.Units[unitClass.DefaultUnit]
                if defaultUnit then
                    specialistData.GreatPeopleUnit = {
                        Type = defaultUnit.Type,
                        Name = Locale.ConvertTextKey(defaultUnit.Description),
                        Class = row.GreatPeopleUnitClass
                    }
                end
            end
        end

        table.insert(result.specialists, specialistData)
    end

    -- Export Great Person units (Section 2)
    -- These are units with no PrereqTech and a Special attribute (matching CivilopediaScreen.lua logic)
    for row in GameInfo.Units() do
        if row.PrereqTech == nil and row.Special ~= nil then
            local gameInfo = nil
            if GetHelpTextForUnit then
                -- Call GetHelpTextForBuilding(buildingID, bExcludeName, pCity, bExcludeHeader, pExcludePlayer, bNoMaintenance)
                -- We pass true for bExcludeName, nil for pCity, false for bExcludeHeader, nil for pExcludePlayer, true for bNoMaintenance
                local success, result = pcall(function()
                    return GetHelpTextForUnit(row.ID, true, nil, true, true)
                end)
                if success and result then
                    gameInfo = result
                end
            end

            -- Fallback to the Help field if dynamic generation fails
            if not gameInfo or gameInfo == "" then
                gameInfo = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil
            end

            local greatPersonData = {
                Type = row.Type,
                ID = row.ID + 1000,  -- Add offset to match CivilopediaScreen.lua
                Name = Locale.ConvertTextKey(row.Description),
                Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
                Strategy = safeGet(row, "Strategy") and Locale.ConvertTextKey(row.Strategy) or nil,
                Help = gameInfo,
                Combat = safeGet(row, "Combat"),
                RangedCombat = safeGet(row, "RangedCombat"),
                Cost = safeGet(row, "Cost"),
                Moves = safeGet(row, "Moves"),
                Class = row.Class,
                Special = row.Special,
                IconAtlas = safeGet(row, "IconAtlas"),
                PortraitIndex = safeGet(row, "PortraitIndex"),
            }

            -- Check if this is a civilization-specific unit
            local civNames = {}
            local civilizations = {}
            for override in GameInfo.Civilization_UnitClassOverrides("UnitType = '" .. row.Type .. "'") do
                local civ = GameInfo.Civilizations[override.CivilizationType]
                if civ and civ.Type ~= "CIVILIZATION_MINOR" then
                    table.insert(civNames, Locale.ConvertTextKey(civ.Adjective))
                    table.insert(civilizations, {
                        Type = civ.Type,
                        Name = Locale.ConvertTextKey(civ.ShortDescription),
                        Adjective = Locale.ConvertTextKey(civ.Adjective)
                    })
                end
            end

            -- If it's civilization-specific, prepend the civilization name(s)
            if #civNames > 0 then
                greatPersonData.Civilizations = civilizations
                greatPersonData.Name = "[COLOR_POSITIVE_TEXT](" .. table.concat(civNames, ", ") .. ")[ENDCOLOR] " .. greatPersonData.Name
            end

            table.insert(result.greatPersons, greatPersonData)
        end
    end

    print("Exported " .. #result.specialists .. " specialists and " .. #result.greatPersons .. " great person units")
    return result
end

-- Export City States (Minor Civilizations)
function ExportCityStates()
    print("Exporting city states...")
    local cityStates = {}

    for row in GameInfo.MinorCivilizations() do
        local cityStateData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Adjective = safeGet(row, "Adjective") and Locale.ConvertTextKey(row.Adjective) or nil,
            Civilopedia = safeGet(row, "Civilopedia") and Locale.ConvertTextKey(row.Civilopedia) or nil,
            MinorCivTrait = safeGet(row, "MinorCivTrait"),
            IconAtlas = safeGet(row, "ArtDefineTag")
        }

        -- Get trait description and ID for grouping
        if cityStateData.MinorCivTrait then
            local trait = GameInfo.MinorCivTraits[cityStateData.MinorCivTrait]
            if trait then
                cityStateData.TraitName = Locale.ConvertTextKey(trait.Description)
                cityStateData.TraitID = trait.ID
            end
        end

        table.insert(cityStates, cityStateData)
    end

    print("Exported " .. #cityStates .. " city states")
    return cityStates
end

-- Export World Congress Resolutions
function ExportResolutions()
    print("Exporting resolutions...")
    local resolutions = {}

    for row in GameInfo.Resolutions() do
        local resolutionData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil
        }

        table.insert(resolutions, resolutionData)
    end

    print("Exported " .. #resolutions .. " resolutions")
    return resolutions
end

-- Export League Projects (International Projects)
function ExportLeagueProjects()
    print("Exporting league projects...")
    local TrophyIcons = {
		"[ICON_TROPHY_BRONZE]",
		"[ICON_TROPHY_SILVER]",
		"[ICON_TROPHY_GOLD]",
	};

    local leagueProjects = {}

    for row in GameInfo.LeagueProjects() do
        local s = ""
        local costPerPlayer = 0
        for iTier = 3, 1, -1 do
            if (row["RewardTier" .. iTier] ~= nil) then
                local tReward = GameInfo.LeagueProjectRewards[row["RewardTier" .. iTier]];
                if (tReward ~= nil) then
                    -- Check if this reward has a building that matches the project ID for cost calculation
                    if (tReward.Building ~= nil) then
                        if (GameInfo.Buildings[tReward.Building] ~= nil and GameInfo.Buildings[tReward.Building].ID == row.ID) then
                            costPerPlayer = row.CostPerPlayer;
                        end
                    end
                    -- Display all rewards regardless of building match
                    if (tReward.Description ~= nil and tReward.Help ~= nil) then
                        s = s .. Locale.Lookup("TXT_KEY_PEDIA_LEAGUE_PROJECT_REWARD", TrophyIcons[iTier], tReward.Description, tReward.Help);
                    end
                end
            end

            if (iTier > 1) then
                s = s .. "[NEWLINE][NEWLINE]";
            end
        end

        local projectData = {
            Type = row.Type,
            ID = row.ID,
            CostPerPlayer = costPerPlayer,
            Name = Locale.ConvertTextKey(row.Description),
            Help = s,
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex")
        }

        table.insert(leagueProjects, projectData)
    end

    print("Exported " .. #leagueProjects .. " league projects")
    return leagueProjects
end

-- Export Corporations
function ExportCorporations()
    print("Exporting corporations...")
    local corporations = {}

    for row in GameInfo.Corporations() do
        local corpData = {
            Type = row.Type,
            ID = row.ID,
            Name = Locale.ConvertTextKey(row.Description),
            Help = safeGet(row, "Help") and Locale.ConvertTextKey(row.Help) or nil,
            HeadquartersBuildingClass = safeGet(row, "HeadquartersBuildingClass"),
            OfficeBuildingClass = safeGet(row, "OfficeBuildingClass"),
            FranchiseBuildingClass = safeGet(row, "FranchiseBuildingClass"),
            IconAtlas = safeGet(row, "IconAtlas"),
            PortraitIndex = safeGet(row, "PortraitIndex"),
            ResourceBonusHelp = safeGet(row, "ResourceBonusHelp") and Locale.ConvertTextKey(row.ResourceBonusHelp) or nil,
            OfficeBonusHelp = safeGet(row, "OfficeBonusHelp") and Locale.ConvertTextKey(row.OfficeBonusHelp) or nil,
            TradeRouteBonusHelp = safeGet(row, "TradeRouteBonusHelp") and Locale.ConvertTextKey(row.TradeRouteBonusHelp) or nil
        }

        -- Get monopoly resources
        local monopolyResources = {}
        local condition = "CorporationType = '" .. row.Type .. "'";
        for resRow in GameInfo.Corporation_ResourceMonopolyOrs(condition) do
            local resource = GameInfo.Resources[resRow.ResourceType]
            if resource then
                table.insert(monopolyResources, {
                    Type = resRow.ResourceType,
                    Name = Locale.ConvertTextKey(resource.Description)
                })
            end
        end
        if #monopolyResources > 0 then
            corpData.MonopolyResources = monopolyResources
        end

        -- Get headquarters building info
        if row.HeadquartersBuildingClass then
            local buildingClass = GameInfo.BuildingClasses[row.HeadquartersBuildingClass]
            if buildingClass then
                local building = GameInfo.Buildings[buildingClass.DefaultBuilding]
                if building then
                    corpData.HeadquartersBuilding = {
                        Type = building.Type,
                        Name = Locale.ConvertTextKey(building.Description)
                    }
                end
            end
        end

        -- Get office building info
        if row.OfficeBuildingClass then
            local buildingClass = GameInfo.BuildingClasses[row.OfficeBuildingClass]
            if buildingClass then
                local building = GameInfo.Buildings[buildingClass.DefaultBuilding]
                if building then
                    corpData.OfficeBuilding = {
                        Type = building.Type,
                        Name = Locale.ConvertTextKey(building.Description)
                    }
                end
            end
        end

        -- Get franchise building info
        if row.FranchiseBuildingClass then
            local buildingClass = GameInfo.BuildingClasses[row.FranchiseBuildingClass]
            if buildingClass then
                local building = GameInfo.Buildings[buildingClass.DefaultBuilding]
                if building then
                    corpData.FranchiseBuilding = {
                        Type = building.Type,
                        Name = Locale.ConvertTextKey(building.Description)
                    }
                end
            end
        end

        table.insert(corporations, corpData)
    end

    print("Exported " .. #corporations .. " corporations")
    return corporations
end

-- Export icon atlas metadata
function ExportIconAtlases()
    local atlases = {}
    local atlasMap = {}  -- Map to track highest resolution for each atlas

    -- Collect all atlas data and group by atlas name
    for row in GameInfo.IconTextureAtlases() do
        local atlasName = row.Atlas
        local iconSize = row.IconSize

        if not atlasMap[atlasName] then
            atlasMap[atlasName] = {}
        end

        -- Store all available sizes for this atlas
        table.insert(atlasMap[atlasName], {
            IconSize = iconSize,
            Filename = row.Filename,
            IconsPerRow = row.IconsPerRow,
            IconsPerColumn = row.IconsPerColumn
        })
    end

    -- For each atlas, select the highest resolution version
    for atlasName, sizes in pairs(atlasMap) do
        -- Sort by IconSize descending to get highest resolution first
        table.sort(sizes, function(a, b) return a.IconSize > b.IconSize end)

        local highestRes = sizes[1]
        table.insert(atlases, {
            Atlas = atlasName,
            IconSize = highestRes.IconSize,
            Filename = highestRes.Filename,
            IconsPerRow = highestRes.IconsPerRow,
            IconsPerColumn = highestRes.IconsPerColumn
        })
    end

    return atlases
end

-- Main export function
function ExportAllData()

    print("==================================")
    print("Starting Civilopedia Export")
    print("==================================")

    local data = {
        metadata = {
            iconAtlases = ExportIconAtlases(),
            localizedLabels = {
                gameConceptPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_GAME_CONCEPT_PAGE_LABEL"),
                homePageHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_HOME_PAGE_HELP_TEXT"),
                homePageBlurbText = Locale.ConvertTextKey("TXT_KEY_PEDIA_HOME_PAGE_BLURB_TEXT"),
                -- Page labels for sidebar headers
                buildingsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_BUILDINGS_PAGE_LABEL"),
                wondersPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_WONDERS_PAGE_LABEL"),
                unitsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_UNITS_PAGE_LABEL"),
                technologiesPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_TECH_PAGE_LABEL"),
                policiesPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_POLICIES_PAGE_LABEL"),
                beliefsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_PAGE_LABEL"),
                promotionsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_PROMOTIONS_PAGE_LABEL"),
                resourcesPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_RESOURCES_PAGE_LABEL"),
                improvementsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_IMPROVEMENTS_PAGE_LABEL"),
                terrainPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_TERRAIN_PAGE_LABEL"),
                greatPeoplePageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_PEOPLE_PAGE_LABEL"),
                cityStatesPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_CITY_STATES_PAGE_LABEL"),
                resolutionsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORLD_CONGRESS_PAGE_LABEL"),
                corporationsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORPORATIONS_PAGE_LABEL"),
                civilizationsPageLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_CIVILIZATIONS_PAGE_LABEL"),
                -- Civilopedia Category labels
                civilopediaCategoryHome = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_1_LABEL"),
                civilopediaCategoryGameConcepts = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_2_LABEL"),
                civilopediaCategoryTechnologies = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_3_LABEL"),
                civilopediaCategoryUnits = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_4_LABEL"),
                civilopediaCategoryPromotions = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_5_LABEL"),
                civilopediaCategoryBuildings = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_6_LABEL"),
                civilopediaCategoryWonders = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_7_LABEL"),
                civilopediaCategoryPolicies = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_8_LABEL"),
                civilopediaCategorySpecialists = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_9_LABEL"),
                civilopediaCategoryCivilizations = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_10_LABEL"),
                civilopediaCategoryCityStates = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_11_LABEL"),
                civilopediaCategoryTerrains = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_12_LABEL"),
                civilopediaCategoryResources = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_13_LABEL"),
                civilopediaCategoryImprovements = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_14_LABEL"),
                civilopediaCategoryReligion = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_15_LABEL"),
                civilopediaCategoryWorldCongress = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_16_LABEL"),
                civilopediaCategoryCorporations = Locale.ConvertTextKey("TXT_KEY_PEDIA_CATEGORY_17_LABEL"),
                -- Civilizations section labels
                civilizationsSection1 = Locale.ConvertTextKey("TXT_KEY_CIVILIZATIONS_SECTION_1"),
                civilizationsSection2 = Locale.ConvertTextKey("TXT_KEY_CIVILIZATIONS_SECTION_2"),
                -- Great People (Specialists) section labels
                specialistsLabel = Locale.ConvertTextKey("TXT_KEY_PEOPLE_SECTION_1"),
                greatPersonsLabel = Locale.ConvertTextKey("TXT_KEY_PEOPLE_SECTION_2"),
                -- Wonder section labels
                wonderSection1 = Locale.ConvertTextKey("TXT_KEY_WONDER_SECTION_1"),
                wonderSection2 = Locale.ConvertTextKey("TXT_KEY_WONDER_SECTION_2"),
                wonderSection3 = Locale.ConvertTextKey("TXT_KEY_WONDER_SECTION_3"),
                wonderSection4 = Locale.ConvertTextKey("TXT_KEY_WONDER_SECTION_4"),
                -- Help text for category home pages
                gameInfoLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_GAME_INFO_LABEL"),
                gameConceptsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_GAME_CONCEPT_HELP_TEXT"),
                gameConceptsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_GCONCEPTS"),
                technologiesHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_TECHNOLOGIES_HELP_TEXT"),
                technologiesQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_TECHS"),
                unitsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_UNITS_HELP_TEXT"),
                unitsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_UNITS"),
                promotionsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_PROMOTIONS_HELP_TEXT"),
                promotionsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_PROMOTIONS"),
                buildingsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_BUILDINGS_HELP_TEXT"),
                buildingsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_BUILDINGS"),
                wondersHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_WONDERS_HELP_TEXT"),
                wondersQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_WONDERS"),
                policiesHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_SOCIAL_POL_HELP_TEXT"),
                policiesQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_POLICIES"),
                greatPeopleHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_SPEC_HELP_TEXT"),
                greatPeopleQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_PEOPLE"),
                civilizationsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_CIVS_HELP_TEXT"),
                civilizationsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_CIVS"),
                cityStatesHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_CSTATES_HELP_TEXT"),
                cityStatesQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_CITYSTATES"),
                terrainHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_TERRAIN_HELP_TEXT"),
                terrainQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_TERRAIN"),
                resourcesHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_RESOURCES_HELP_TEXT"),
                resourcesQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_RESOURCES"),
                improvementsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT"),
                improvementsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_QUOTE_BLOCK_IMPROVEMENTS"),
                beliefsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_HOMEPAGE_TEXT1"),
                beliefsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_HOMEPAGE_BLURB"),
                worldCongressHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORLD_CONGRESS_HOMEPAGE_TEXT1"),
                worldCongressQuote = Locale.ConvertTextKey("TXT_KEY_WONDER_UNITEDNATIONS_QUOTE"),
                corporationsHelpText = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORPORATIONS_HOMEPAGE_TEXT1"),
                corporationsQuote = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORPORATIONS_HOMEPAGE_BLURB"),
                -- Belief category labels
                religionsCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_1"),
                pantheonCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_2"),
                founderCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_3"),
                followerCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_4"),
                enhancerCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_5"),
                reformationCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_BELIEFS_CATEGORY_6"),
                -- Other labels
                leadsToBldgLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_LEADS_TO_BLDG_LABEL"),
                leadsToTechLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_LEADS_TO_TECH_LABEL"),
                buildingsUnlockedLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_BLDG_UNLOCK_LABEL"),
                wondersUnlockedLabel = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_WONDERS_UNLOCKED"),
                resourcesRevealedLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_RESRC_RVL_LABEL"),
                workerActionLabel = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORKER_ACTION_LABEL"),
                -- Resource type categories
                bonusResourcesCategory = Locale.ConvertTextKey("TXT_KEY_RESOURCES_SECTION_0"),
                luxuryResourcesCategory = Locale.ConvertTextKey("TXT_KEY_RESOURCES_SECTION_1"),
                strategicResourcesCategory = Locale.ConvertTextKey("TXT_KEY_RESOURCES_SECTION_2"),
                -- Terrain categories
                terrainsCategory = Locale.ConvertTextKey("TXT_KEY_TERRAIN_SECTION_1"),
                featuresCategory = Locale.ConvertTextKey("TXT_KEY_TERRAIN_SECTION_2"),
                -- World Congress categories
                resolutionsCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORLD_CONGRESS_CATEGORY_1"),
                leagueProjectsCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORLD_CONGRESS_CATEGORY_2"),
                specialProjectsCategory = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORLD_CONGRESS_CATEGORY_3"),
                -- Section labels
                summaryHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_SUMMARY_LABEL"),
                baseYieldsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_YIELD_LABEL"),
                revealedByHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_REVEAL_TECH_LABEL"),
                improvedByHeader = Locale.ConvertTextKey("TXT_KEY_RESOURCE_TOOLTIP_IMPROVED"),
                foundOnHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_RESOURCESFOUND_LABEL"),
                adjacencyBonusesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_ADJIMPROVYIELD_LABEL"),
                adjacentYieldsExplanation = Locale.ConvertTextKey("TXT_KEY_PEDIA_ADJACENT_YIELDS_EXPLANATION"),
                cityConnectionBonusesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_TRADEROUTE_CBP_LABEL"),
                canBeBuiltOnHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_FOUNDON_LABEL"),
                improvesResourcesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_IMPROVES_RESRC_LABEL"),
                yieldIncreasePerEraHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_ERA_YIELD"),
                movementCostHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_MOVECOST_LABEL"),
                combatModifierHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_COMBATMOD_LABEL"),
                canAppearOnHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_FEATURES_LABEL"),
                resourcesFoundHereHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_RESOURCESFOUND_LABEL"),
                generatesGreatPersonHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_GENERATES_GREAT_PERSON_HEADER"),
                resourceBonusesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORPORATION_RESOURCE_BONUS_LABEL"),
                officeBonusesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORPORATION_OFFICE_BONUS_LABEL"),
                tradeRouteBonusesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORPORATION_TRADE_ROUTE_BONUS_LABEL"),
                requiredMonopolyResourcesHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_MONOPOLY_RESRC_LABEL"),
                headquartersHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORP_HEADQUARTERS_LABEL"),
                officeHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORP_OFFICE_LABEL"),
                franchiseHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CORP_FRANCHISE_LABEL"),
                requiredBuildingsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_REQ_BLDG_LABEL"),
                leadsToHeader = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_LEADS_TO"),
                requiredTechnologyHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_PREREQ_TECH_LABEL"),
                obsoleteTechnologyHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_OBSOLETE_TECH_LABEL"),
                unitsUnlockedHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_UNIT_UNLOCK_LABEL"),
                buildingsUnlockedHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_BLDG_UNLOCK_LABEL"),
                wondersUnlockedHeader = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_WONDERS_UNLOCKED"),
                projectsUnlockedHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_PROJ_UNLOCK_LABEL"),
                resourcesRevealedHeader = Locale.ConvertTextKey("TXT_KEY_TECH_HELP_RESOURCES_UNLOCKED"),
                workerActionsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_WORKER_ACTION_LABEL"),
                freePromotionsHeader = Locale.ConvertTextKey("TXT_KEY_PRODUCTION_UNIT_FREE_PROMOTIONS"),
                requiredPromotionsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_REQ_PROMOTIONS_LABEL"),
                leadsToPromotionsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_LEADS_TO_PROMOTIONS_LABEL"),
                strategyHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_STRATEGY_LABEL"),
                civilopediaHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_HISTORICAL_LABEL"),
                gameInfoHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_GAME_INFO_LABEL"),
                -- Civ page headers
                leaderHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_LEADERS_LABEL"),
                uniqueUnitsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_UNIQUEUNIT_LABEL"),
                uniqueBuildingsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_UNIQUEBLDG_LABEL"),
                uniqueImprovementsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_UNIQUEIMPRV_LABEL"),
                civilizationHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_CIVILIZATIONS_LABEL"),
                leaderTraitsHeader = Locale.ConvertTextKey("TXT_KEY_PEDIA_TRAITS_LABEL"),
                -- Inline text
                connectsResourceText = Locale.ConvertTextKey("TXT_KEY_PEDIA_CONNECTS_RESOURCE_TEXT"),
                impassableText = Locale.ConvertTextKey("TXT_KEY_PEDIA_IMPASSABLE"),
                noYieldText = Locale.ConvertTextKey("TXT_KEY_PEDIA_NO_YIELD")
            }
        },
        concepts = ExportConcepts(),
        civilizations = ExportCivilizations(),
        leaders = ExportLeaders(),
        units = ExportUnits(),
        buildings = ExportBuildings(),
        wonders = ExportWonders(),
        technologies = ExportTechnologies(),
        policies = ExportPolicies(),
        religions = ExportReligions(),
        beliefs = ExportBeliefs(),
        promotions = ExportPromotions(),
        terrain = ExportTerrain(),
        resources = ExportResources(),
        improvements = ExportImprovements(),
        greatPeople = ExportGreatPeople(),
        cityStates = ExportCityStates(),
        resolutions = ExportResolutions(),
        leagueProjects = ExportLeagueProjects(),
        corporations = ExportCorporations()
    }

    print("==================================")
    print("Encoding to JSON...")
    print("==================================")

    local json = JSONEncoder.encode(data)

    print("==================================")
    print("Export complete!")
    print("JSON size: " .. string.len(json) .. " bytes")
    print("==================================")

    -- Write to log
    WriteToLog(json, "civilopedia_export.json")

    return true
end

function OnLoadScreenClose()
    print("==================================")
    print("Auto-exporting Civilopedia data...")
    print("==================================")

    ExportAllData()
end

-- Export when load screen is closed
if Events then
    Events.LoadScreenClose.Add(OnLoadScreenClose)
end
