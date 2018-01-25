print("This is the modded CivilopediaScreen.lua from 'CBP'")
-------------------------------------------------
-- Civilopedia screen
-------------------------------------------------
include( "InstanceManager" );
include( "IconSupport" );
--CBP
include( "TechHelpInclude" );
--END

-- table.sort method for sorting alphabetically.
function Alphabetically(a, b)
	return Locale.Compare(a.entryName, b.entryName) == -1;
end

local portraitSize = 256;
local buttonSize = 64;

-- various sizes of elements
local wideOuterFrameWidth = 436;
local wideInnerFrameWidth = 440;
local superWideOuterFrameWidth = 636;
local superWideInnerFrameWidth = 640;
local narrowOuterFrameWidth = 204;
local narrowInnerFrameWidth = 208;
local textPaddingFromInnerFrame = 34;
local offsetsBetweenFrames = 4;
local quoteButtonOffset = 60;
local numberOfButtonsPerRow = 3;
local buttonPadding = 8;
local buttonPaddingTimesTwo = 16;

-- textures that will be used a lot
local defaultErrorTextureSheet = "TechAtlasSmall.dds";

local addToList = 1;
local dontAddToList = 0;

-- defines for the various categories of topics
local CategoryHomePage = 1;
local CategoryGameConcepts = 2;
local CategoryTech = 3;
local CategoryUnits = 4;
local CategoryPromotions = 5;
local CategoryBuildings = 6;
local CategoryWonders = 7;
local CategoryPolicies = 8;
local CategoryPeople = 9;
local CategoryCivilizations = 10;
local CategoryCityStates = 11;
local CategoryTerrain = 12;
local CategoryResources = 13;
local CategoryImprovements = 14;
local CategoryBeliefs = 15;
local CategoryWorldCongress = 16;
local CategoryCorporations = 17;
local numCategories = 17;

local selectedCategory = CategoryHomePage;
local CivilopediaCategory = {};
local currentTopic = 0;
local endTopic = 0;
local listOfTopicsViewed = {};

local sortedList = {};
local otherSortedList = {};
local searchableTextKeyList = {};
local searchableList = {};
local categorizedList = {};
local absurdlyLargeNumTopicsInCategory = 10000;
local homePageOfCategoryID = 9999;

-- Putmalk: These boolean variables check whether or not certain game options are enabled.
	local g_bResearchAgreementTrading = false;
	local g_bTechTrading = true;
	local g_bNoVassalage= false;
	if(Game ~= nil)then
		g_bResearchAgreementTrading = Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS");
		g_bTechTrading = Game.IsOption("GAMEOPTION_TECH_TRADING");
		g_bNoVassalage= Game.IsOption("GAMEOPTION_NO_VASSALAGE");
	end
-- END

-- These projects were more of an implementation detail and not explicit projects
-- that the user can build.  So to avoid confusion, we shall ignore them from the pedia.
local projectsToIgnore = {
	PROJECT_SS_COCKPIT = true,
	PROJECT_SS_STASIS_CHAMBER = true,
	PROJECT_SS_ENGINE = true,
	PROJECT_SS_BOOSTER = true
};

-- Infixo more categories for promotions
local UnitPromotionsPediaTypes = {
	"PEDIA_MELEE",
	"PEDIA_MOUNTED",
	"PEDIA_RANGED",
	"PEDIA_SIEGE",
	"PEDIA_NAVAL",
	"PEDIA_CARSUB",
	"PEDIA_AIR",
	"PEDIA_SCOUTING",
	"PEDIA_DIPLO",
	"PEDIA_CIVILIAN",
	"PEDIA_HEAL",
	"PEDIA_SHARED",
	"PEDIA_ATTRIBUTES",
	"PEDIA_BARBARIAN"
}
-- Infixo end

-- the instance managers
local g_ListItemManager = InstanceManager:new( "ListItemInstance", "ListItemButton", Controls.ListOfArticles );
local g_ListHeadingManager = InstanceManager:new( "ListHeadingInstance", "ListHeadingButton", Controls.ListOfArticles );
local g_PrereqTechManager = InstanceManager:new( "PrereqTechInstance", "PrereqTechButton", Controls.PrereqTechInnerFrame );
local g_GreatWorksManager = InstanceManager:new( "GreatWorksInstance", "GreatWorksButton", Controls.GreatWorksInnerFrame );
local g_ObsoleteTechManager = InstanceManager:new( "ObsoleteTechInstance", "ObsoleteTechButton", Controls.ObsoleteTechInnerFrame );
local g_UpgradeManager = InstanceManager:new( "UpgradeInstance", "UpgradeButton", Controls.UpgradeInnerFrame );
local g_LeadsToTechManager = InstanceManager:new( "LeadsToTechInstance", "LeadsToTechButton", Controls.LeadsToTechInnerFrame );
local g_UnlockedUnitsManager = InstanceManager:new( "UnlockedUnitInstance", "UnlockedUnitButton", Controls.UnlockedUnitsInnerFrame );
local g_UnlockedBuildingsManager = InstanceManager:new( "UnlockedBuildingInstance", "UnlockedBuildingButton", Controls.UnlockedBuildingsInnerFrame );
local g_RevealedResourcesManager = InstanceManager:new( "RevealedResourceInstance", "RevealedResourceButton", Controls.RevealedResourcesInnerFrame );
local g_RequiredResourcesManager = InstanceManager:new( "RequiredResourceInstance", "RequiredResourceButton", Controls.RequiredResourcesInnerFrame );
local g_WorkerActionsManager = InstanceManager:new( "WorkerActionInstance", "WorkerActionButton", Controls.WorkerActionsInnerFrame );
local g_UnlockedProjectsManager = InstanceManager:new( "UnlockedProjectInstance", "UnlockedProjectButton", Controls.UnlockedProjectsInnerFrame );
local g_PromotionsManager = InstanceManager:new( "PromotionInstance", "PromotionButton", Controls.FreePromotionsInnerFrame );
local g_SpecialistsManager = InstanceManager:new( "SpecialistInstance", "SpecialistButton", Controls.SpecialistsInnerFrame );
local g_RequiredBuildingsManager = InstanceManager:new( "RequiredBuildingInstance", "RequiredBuildingButton", Controls.RequiredBuildingsInnerFrame );
--CBP
local g_LeadsToBuildingsManager = InstanceManager:new( "LeadsToBuildingInstance", "LeadsToBuildingButton", Controls.LeadsToBuildingsInnerFrame );
local g_MonopolyResourcesManager = InstanceManager:new( "MonopolyResourceInstance", "MonopolyResourceButton", Controls.MonopolyResourcesInnerFrame );
local g_CorporationsManager = InstanceManager:new( "CorporationInstance", "CorporationButton", Controls.CorporationInnerFrame );
local g_CorpHeadquartersManager = InstanceManager:new( "CorporationBuildingInstance", "CorporationBuildingButton", Controls.CorpHeadquartersInnerFrame );
local g_CorpOfficeManager = InstanceManager:new( "CorporationBuildingInstance", "CorporationBuildingButton", Controls.CorpOfficeInnerFrame );
local g_CorpFranchiseManager = InstanceManager:new( "CorporationBuildingInstance", "CorporationBuildingButton", Controls.CorpFranchiseInnerFrame );
--END
local g_LocalResourcesManager = InstanceManager:new( "LocalResourceInstance", "LocalResourceButton", Controls.LocalResourcesInnerFrame );
local g_RequiredPromotionsManager = InstanceManager:new( "RequiredPromotionInstance", "RequiredPromotionButton", Controls.RequiredPromotionsInnerFrame );
local g_RequiredPoliciesManager = InstanceManager:new( "RequiredPolicyInstance", "RequiredPolicyButton", Controls.RequiredPoliciesInnerFrame );
local g_FreeFormTextManager = InstanceManager:new( "FreeFormTextInstance", "FFTextFrame", Controls.FFTextStack );
local g_BBTextManager = InstanceManager:new( "BBTextInstance", "BBTextFrame", Controls.BBTextStack );
local g_LeadersManager = InstanceManager:new( "LeaderInstance", "LeaderButton", Controls.LeadersInnerFrame );
local g_UniqueUnitsManager = InstanceManager:new( "UniqueUnitInstance", "UniqueUnitButton", Controls.UniqueUnitsInnerFrame );
local g_UniqueBuildingsManager = InstanceManager:new( "UniqueBuildingInstance", "UniqueBuildingButton", Controls.UniqueBuildingsInnerFrame );
local g_UniqueImprovementsManager = InstanceManager:new( "UniqueImprovementInstance", "UniqueImprovementButton", Controls.UniqueImprovementsInnerFrame );
local g_CivilizationsManager = InstanceManager:new( "CivilizationInstance", "CivilizationButton", Controls.CivilizationsInnerFrame );
local g_TraitsManager = InstanceManager:new( "TraitInstance", "TraitButton", Controls.TraitsInnerFrame );
local g_FeaturesManager = InstanceManager:new( "FeatureInstance", "FeatureButton", Controls.FeaturesInnerFrame );
local g_ResourcesFoundManager = InstanceManager:new( "ResourceFoundInstance", "ResourceFoundButton", Controls.ResourcesFoundInnerFrame );
local g_TerrainsManager = InstanceManager:new( "TerrainInstance", "TerrainButton", Controls.TerrainsInnerFrame );
local g_ReplacesManager = InstanceManager:new( "ReplaceInstance", "ReplaceButton", Controls.ReplacesInnerFrame );
local g_RevealTechsManager = InstanceManager:new( "RevealTechInstance", "RevealTechButton", Controls.RevealTechsInnerFrame );
local g_ImprovementsManager = InstanceManager:new( "ImprovementInstance", "ImprovementButton", Controls.ImprovementsInnerFrame );

-- adjust the various parts to fit the screen size
local _, screenSizeY = UIManager:GetScreenSizeVal(); -- Controls.BackDrop:GetSize();
local scrollBarWidth = Controls.ScrollBar:GetSize().x;
local scrollPanelSize = Controls.ScrollPanel:GetSize();
local scrollBarSize = { x = scrollBarWidth; y = screenSizeY - 172; };
Controls.ScrollBar:SetSize( scrollBarSize );
Controls.LeftScrollBar:SetSize( scrollBarSize );
Controls.ScrollBar:SetOffsetVal( 0, 18 );
Controls.LeftScrollBar:SetOffsetVal( 0, 18 );
Controls.DownButton:SetOffsetVal( 0, screenSizeY - 126 - 18 );
Controls.LeftDownButton:SetOffsetVal( 0, screenSizeY - 126 - 18 );
Controls.LeftUpButton:SetOffsetVal( 0, 0 );
scrollPanelSize.y = screenSizeY - 126;
Controls.ScrollPanel:SetSize( scrollPanelSize );
local leftScrollPanelSize = Controls.LeftScrollPanel:GetSize();
leftScrollPanelSize.y = screenSizeY - 126;
Controls.LeftScrollPanel:SetSize( leftScrollPanelSize );
Controls.ScrollPanel:CalculateInternalSize();
Controls.LeftScrollPanel:CalculateInternalSize();

function SetSelectedCategory( thisCategory )
	print("SetSelectedCategory("..tostring(thisCategory)..")");
	if selectedCategory ~= thisCategory then
		selectedCategory = thisCategory;
		-- set up tab
		Controls.SelectedCategoryTab:SetOffsetVal(47 * (selectedCategory - 1), -10);
		Controls.SelectedCategoryTab:SetTexture( CivilopediaCategory[selectedCategory].buttonTexture );
		-- set up label for category
		Controls.CategoryLabel:SetText( CivilopediaCategory[selectedCategory].labelString );
		-- populate the list of entries
		if CivilopediaCategory[selectedCategory].DisplayList then
			CivilopediaCategory[selectedCategory].DisplayList();
		else
			g_ListHeadingManager:ResetInstances();
			g_ListItemManager:ResetInstances();
		end
		Controls.ListOfArticles:CalculateSize();
		Controls.ListOfArticles:ReprocessAnchoring();
	end
	-- get first entry from list (this will be a special page)
	if CivilopediaCategory[selectedCategory].DisplayHomePage then
		CivilopediaCategory[selectedCategory].DisplayHomePage();
	end	
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.LeftScrollPanel:CalculateInternalSize();
end

-------------------------------------------------------------------------------
-- setup generic stuff for each category
-------------------------------------------------------------------------------
for i = 1, numCategories, 1 do
	CivilopediaCategory[i] = {};
	CivilopediaCategory[i].tag = i;
	CivilopediaCategory[i].buttonClicked = function()
		SetSelectedCategory(CivilopediaCategory[i].tag);
	end
	local buttonName = "CategoryButton"..tostring(i);
	Controls[buttonName]:RegisterCallback( Mouse.eLClick, CivilopediaCategory[i].buttonClicked );
end

-------------------------------------------------------------------------------
-- setup the special case stuff for each category
-------------------------------------------------------------------------------
CivilopediaCategory[CategoryHomePage].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsHome.dds";
CivilopediaCategory[CategoryGameConcepts].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsGameplay.dds";
CivilopediaCategory[CategoryTech].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsTechnology.dds";
CivilopediaCategory[CategoryUnits].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsUnit.dds";
CivilopediaCategory[CategoryPromotions].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsPromotions.dds";
CivilopediaCategory[CategoryBuildings].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsBuildings.dds";
CivilopediaCategory[CategoryWonders].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsWonders.dds";
CivilopediaCategory[CategoryPolicies].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsSocialPolicy.dds";
CivilopediaCategory[CategoryPeople].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsGreatPersons.dds";
CivilopediaCategory[CategoryCivilizations].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsCivsCityStates.dds";
CivilopediaCategory[CategoryCityStates].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsCities.dds";
CivilopediaCategory[CategoryTerrain].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsTerrian.dds";
CivilopediaCategory[CategoryResources].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsResourcesImprovements.dds";
CivilopediaCategory[CategoryImprovements].buttonTexture = "Assets/UI/Art/Civilopedia/CivilopediaTopButtonsImprovements.dds";
CivilopediaCategory[CategoryBeliefs].buttonTexture = "CivilopediaTopButtonsReligion.dds";
CivilopediaCategory[CategoryWorldCongress].buttonTexture = "CivilopediaTopButtonsWorldCongress.dds";
CivilopediaCategory[CategoryCorporations].buttonTexture = "civilopediatopbuttonscorporations.dds";

CivilopediaCategory[CategoryHomePage].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_1_LABEL" );
CivilopediaCategory[CategoryGameConcepts].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_2_LABEL" );
CivilopediaCategory[CategoryTech].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_3_LABEL" );
CivilopediaCategory[CategoryUnits].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_4_LABEL" );
CivilopediaCategory[CategoryPromotions].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_5_LABEL" );
CivilopediaCategory[CategoryBuildings].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_6_LABEL" );
CivilopediaCategory[CategoryWonders].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_7_LABEL" );
CivilopediaCategory[CategoryPolicies].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_8_LABEL" );
CivilopediaCategory[CategoryPeople].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_9_LABEL" );
CivilopediaCategory[CategoryCivilizations].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_10_LABEL" );
CivilopediaCategory[CategoryCityStates].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_11_LABEL" );
CivilopediaCategory[CategoryTerrain].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_12_LABEL" );
CivilopediaCategory[CategoryResources].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_13_LABEL" );
CivilopediaCategory[CategoryImprovements].labelString = Locale.ConvertTextKey( "TXT_KEY_PEDIA_CATEGORY_14_LABEL" );
CivilopediaCategory[CategoryBeliefs].labelString = Locale.Lookup("TXT_KEY_PEDIA_CATEGORY_15_LABEL");
CivilopediaCategory[CategoryWorldCongress].labelString = Locale.Lookup("TXT_KEY_PEDIA_CATEGORY_16_LABEL");
CivilopediaCategory[CategoryCorporations].labelString = Locale.Lookup("TXT_KEY_PEDIA_CATEGORY_17_LABEL");

CivilopediaCategory[CategoryHomePage].PopulateList = function()
	sortedList[CategoryHomePage] = {};
	
	sortedList[CategoryHomePage][1] = {}; -- there is only one section 
	local tableid = 1;		
	
		-- for each major category
 		for i=1, numCategories,1 do  
 		
			-- add an entry to a list (localized name, tag, etc.)
 			local article = {};
 			local compoundName = "TXT_KEY_PEDIA_CATEGORY_" .. tostring(i) .. "_LABEL" ;
 			local name = Locale.ConvertTextKey( compoundName );
 			article.entryName = name;
 			article.entryID = i;
			article.entryCategory = CategoryHomePage;

			sortedList[CategoryHomePage][1][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[compoundName] = article;
			categorizedList[(CategoryHomePage * absurdlyLargeNumTopicsInCategory) + i] = article;
		end
end

CivilopediaCategory[CategoryGameConcepts].PopulateList = function()
	sortedList[CategoryGameConcepts] = {};
	local GameConceptsList = sortedList[CategoryGameConcepts];
	
	local conceptSections = {
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
-- CBP
		HEADER_VASSALAGE = 26,
		HEADER_CORPORATIONS = 27
-- END
	}
	
	-- Create table.
	for i,v in pairs(conceptSections) do
		GameConceptsList[v] = {
			headingOpen = false,
		}; 
	end	
	
	-- for each concept
	for thisConcept in GameInfo.Concepts() do
		
		local sectionID = conceptSections[thisConcept.CivilopediaHeaderType];
		if(sectionID ~= nil) then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisConcept.Description )
			article.entryName = name;
			article.entryID = thisConcept.ID;
			article.entryCategory = CategoryGameConcepts;
			article.InsertBefore = thisConcept.InsertBefore;
			article.InsertAfter = thisConcept.InsertAfter;
			article.Type = thisConcept.Type;

			table.insert(GameConceptsList[sectionID], article);
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisConcept.Description] = article;
			categorizedList[(CategoryGameConcepts * absurdlyLargeNumTopicsInCategory) + thisConcept.ID] = article;
		end
	end
	
	-- In order to maintain the original order as best as possible,
	-- we assign "InsertBefore" values to all items that lack any insert.
	for _, conceptList in ipairs(GameConceptsList) do
		for i = #conceptList, 1, -1 do
			local concept = conceptList[i];
			
			if(concept.InsertBefore == nil and concept.InsertAfter == nil) then
				for ii = i - 1, 1, -1 do
					local previousConcept = conceptList[ii];
					if(previousConcept.InsertBefore == nil and previousConcept.InsertAfter == nil) then
						concept.InsertAfter = previousConcept.Type;
						break;
					end
				end
			end
		end
	end
	
	
	-- sort the articles by their dependencies.
	function DependencySort(articles)
		
		-- index articles by Topic
		local articlesByType= {};
		local dependencies = {};
		
		for i,v in ipairs(articles) do
			articlesByType[v.Type] = v;
			dependencies[v] = {};
		end
		
		for i,v in ipairs(articles) do
			
			local insertBefore = v.InsertBefore;
			if(insertBefore ~= nil) then
				local article = articlesByType[insertBefore];
				dependencies[article][v] = true;
			end
			
			local insertAfter = v.InsertAfter;
			if(insertAfter ~= nil) then
				local article = articlesByType[insertAfter];
				dependencies[v][article] = true;
			end
		end
		
		local sortedList = {};
		
		local articleCount = #articles;
		while(#sortedList < articleCount) do
			
			-- Attempt to find a node with 0 dependencies
			local article;
			for i,a in ipairs(articles) do
				if(dependencies[a] ~= nil and table.count(dependencies[a]) == 0) then
					article = a;
					break;
				end
			end
			
			if(article == nil) then
				print("Failed to sort articles topologically!! There are dependency cycles.");
				return nil;
			else
			
				-- Insert Node
				table.insert(sortedList, article);
				
				-- Remove node
				dependencies[article] = nil;
				for a,d in pairs(dependencies) do
					d[article] = nil;
				end
			end
		end
		
		return sortedList;
	end
		
	for i,v in ipairs(GameConceptsList) do
		local oldList = v;
		local newList = DependencySort(v);
	
		if(newList == nil) then
			newList = oldList;
		else
			newList.headingOpen = false;
		end
		
		GameConceptsList[i] = newList;
	end
end

CivilopediaCategory[CategoryTech].PopulateList = function()
	-- add the instances of the tech entries
	
	sortedList[CategoryTech] = {};
	
	-- for each era
	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Type FROM Eras ORDER BY ID") do
	
		local eraID = era.ID;
	
		sortedList[CategoryTech][eraID] = {};
		local tableid = 1;
	
		-- for each tech in this era
 		for tech in GameInfo.Technologies("Era = '" .. era.Type .. "'") do
 		
			-- add a tech entry to a list (localized name, unit tag, etc.)
 			local article = {};
 			local name = Locale.ConvertTextKey( tech.Description )
 			article.entryName = name;
 			article.entryID = tech.ID;
			article.entryCategory = CategoryTech;			
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( tech.PortraitIndex, buttonSize, tech.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryTech][eraID][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[tech.Description] = article;
			categorizedList[(CategoryTech * absurdlyLargeNumTopicsInCategory) + tech.ID] = article;
		end

		-- sort this list alphabetically by localized name
		table.sort(sortedList[CategoryTech][eraID], Alphabetically);
	
	end
		
end

CivilopediaCategory[CategoryUnits].PopulateList = function()
	-- add the instances of the unit entries
	sortedList[CategoryUnits] = {};

	function AddArticle(categoryID, entryID, unit)	
		local article = {};
		local name = Locale.ConvertTextKey( unit.Description )
		article.entryName = name;
		article.entryID = unit.ID;
		article.entryCategory = CategoryUnits;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( unit.PortraitIndex, buttonSize, unit.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end				
		
		sortedList[CategoryUnits][categoryID][entryID] = article;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[unit.Description] = article;
		categorizedList[(CategoryUnits * absurdlyLargeNumTopicsInCategory) + unit.ID] = article;
	end
	
	local sectionID = 0;
	
	-- Add units which cost faith to a "Faith" category first.
	if(Game == nil or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		local faithUnits = {};
		sortedList[CategoryUnits][0] = {};
		local tableid = 1;
		for unit in DB.Query("SELECT Units.ID, Units.Description, Units.PortraitIndex, Units.IconAtlas From Units where Units.FaithCost > 0 and Units.Cost == -1 and not RequiresFaithPurchaseEnabled and ShowInPedia == 1") do
			AddArticle(0, tableid, unit);
			tableid = tableid + 1;
		end
		sectionID = sectionID + 1;
	end
	
	-- for each era
	local sql = [[
		SELECT Units.ID, Units.Description, Units.PortraitIndex, Units.IconAtlas 
		FROM Units INNER JOIN Technologies on PreReqTech = Technologies.Type 
		WHERE (Units.FaithCost = 0 or RequiresFaithPurchaseEnabled or Units.Cost >= 0) and Units.ShowInPedia = 1 and Technologies.Era = ?;]];
						
	local UnitsByEra = DB.CreateQuery(sql);
	
	local bFirstEra = true;
	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Type FROM Eras ORDER BY ID") do
	
		local eraID = era.ID + sectionID;
	
		sortedList[CategoryUnits][eraID] = {};
		local tableid = 1;
	
		for unit in UnitsByEra(era.Type) do
			AddArticle(eraID, tableid, unit);
			tableid = tableid + 1;
		end
			
		-- put in all of the units that do not have tech requirements in the Ancient Era for lack of a better place
		if(bFirstEra == true) then
			for unit in DB.Query("SELECT Units.ID, Units.Description, Units.PortraitIndex, Units.IconAtlas From Units where PreReqTech is NULL and Special is NULL and (Units.FaithCost = 0 or RequiresFaithPurchaseEnabled or Units.Cost >= 0) and Units.ShowInPedia = 1") do
				AddArticle(eraID, tableid, unit);
				tableid = tableid + 1;
			end
		end	
		
		bFirstEra = false;
	end
	
	-- sort this list alphabetically by localized name
	for k,v in pairs(sortedList[CategoryUnits]) do
		table.sort(v, Alphabetically);
	end	
end


CivilopediaCategory[CategoryPromotions].PopulateList = function()
	-- add the instances of the promotion entries
	sortedList[CategoryPromotions] = {};
	
	-- Infixo more categories
	for iSection, sPediaType in ipairs(UnitPromotionsPediaTypes) do
		sortedList[CategoryPromotions][iSection] = {};
		local tableid = 1;
		for thisPromotion in GameInfo.UnitPromotions() do
			if thisPromotion.PediaType == sPediaType then
				-- add an article to the list (localized name, unit tag, etc.)
				local article = {};
				local name = Locale.ConvertTextKey( thisPromotion.PediaEntry );
				article.entryName = name;
				article.entryID = thisPromotion.ID;
				article.entryCategory = CategoryPromotions;
				sortedList[CategoryPromotions][iSection][tableid] = article;
				tableid = tableid + 1;
				-- index by various keys
				searchableList[Locale.ToLower(name)] = article;
				searchableTextKeyList[thisPromotion.Description] = article;
				categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
			end
		end -- unit promos
	end -- pedia types
	
	--[[ Infixo more categories
	sortedList[CategoryPromotions][1] = {}; 
	local tableid = 1;
	
	-- for each promotion
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_MELEE" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][1][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
	
	sortedList[CategoryPromotions][2] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_RANGED" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][2][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
	
	sortedList[CategoryPromotions][3] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_NAVAL" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][3][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
		
	sortedList[CategoryPromotions][4] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_HEAL" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][4][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
	
	sortedList[CategoryPromotions][5] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_SCOUTING" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][5][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
	
	sortedList[CategoryPromotions][6] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_AIR" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][6][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end

	sortedList[CategoryPromotions][7] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_SHARED" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][7][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
	
	sortedList[CategoryPromotions][8] = {}; 
	local tableid = 1;
	for thisPromotion in GameInfo.UnitPromotions() do
		if thisPromotion.PediaType == "PEDIA_ATTRIBUTES" then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( thisPromotion.PediaEntry )
			article.entryName = name;
			article.entryID = thisPromotion.ID;
			article.entryCategory = CategoryPromotions;

			sortedList[CategoryPromotions][8][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[thisPromotion.Description] = article;
			categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + thisPromotion.ID] = article;
		end
	end
	--]]
	-- sort this list alphabetically by localized name
	-- Infixo more categories
	--for section = 1,8,1 do
	for section,_ in ipairs(UnitPromotionsPediaTypes) do
		table.sort(sortedList[CategoryPromotions][section], Alphabetically);
	end		
end


CivilopediaCategory[CategoryBuildings].PopulateList = function()
	-- add the instances of the building entries
	
	sortedList[CategoryBuildings] = {};
	
	function AddArticle(categoryID, entryID, building)
		local article = {};
		local name = Locale.ConvertTextKey( building.Description )
		article.entryName = name;
		article.entryID = building.ID;
		article.entryCategory = CategoryBuildings;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( building.PortraitIndex, buttonSize, building.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end
		
		sortedList[CategoryBuildings][categoryID][entryID] = article;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[building.Description] = article;
		categorizedList[(CategoryBuildings * absurdlyLargeNumTopicsInCategory) + building.ID] = article;
	end
	
	local sectionID = 0;
	if(Game == nil or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		--Add Faith Buildings first
		local tableid = 1;
		sortedList[CategoryBuildings][sectionID] = {};	
		for building in DB.Query("SELECT Buildings.ID, Buildings.Description, Buildings.PortraitIndex, Buildings.IconAtlas from Buildings inner join  BuildingClasses on Buildings.BuildingClass = BuildingClasses.Type where Buildings.FaithCost > 0 and Buildings.Cost == -1 and BuildingClasses.MaxGlobalInstances < 0 and (BuildingClasses.MaxPlayerInstances <> 1 or Buildings.SpecialistCount > 0) and BuildingClasses.MaxTeamInstances < 0;") do
			AddArticle(sectionID, tableid, building); -- FIXED Infixo changed 0 to sectionID
			tableid = tableid + 1;
		end
		sectionID = sectionID + 1;
	end
	
	--CORPS
	--Add Corporation Buildings Second
	local tableid = 1;
	sortedList[CategoryBuildings][sectionID] = {};
	for building in DB.Query("SELECT Buildings.ID, Buildings.Description, Buildings.PortraitIndex, Buildings.IconAtlas from Buildings inner join  BuildingClasses on Buildings.BuildingClass = BuildingClasses.Type where Buildings.IsCorporation > 0 and BuildingClasses.MaxGlobalInstances < 0 and (BuildingClasses.MaxPlayerInstances <> 1 or Buildings.SpecialistCount > 0) and BuildingClasses.MaxTeamInstances < 0;") do
		AddArticle(sectionID, tableid, building); -- FIXED Infixo changed 1 to sectionID
		tableid = tableid + 1;
	end
	sectionID = sectionID + 1;
	
	local sql = [[
		SELECT Buildings.ID, Buildings.Description, Buildings.PortraitIndex, Buildings.IconAtlas 
		FROM Buildings 
		INNER JOIN BuildingClasses ON Buildings.BuildingClass = BuildingClasses.Type 
		INNER JOIN Technologies ON Buildings.PrereqTech = Technologies.Type 
		WHERE (FaithCost == 0 or Buildings.Cost >= 0) AND Buildings.IsCorporation == 0 AND BuildingClasses.MaxGlobalInstances < 0 AND BuildingClasses.MaxPlayerInstances <> 1 AND BuildingClasses.MaxTeamInstances < 0 AND Technologies.Era = ?;]];
	
	local BuildingsByEra = DB.CreateQuery(sql);
	
	
	-- for each era
	local bFirstEra = true;
	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Type FROM Eras ORDER BY ID") do
	
		local eraID = era.ID + sectionID;
	
		sortedList[CategoryBuildings][eraID] = {};
		local tableid = 1;
		
		for building in BuildingsByEra(era.Type) do
			AddArticle(eraID, tableid, building);
			tableid = tableid + 1;
		end
	
		-- put in all of the buildings that do not have tech requirements in the first Era for lack of a better place
		if(bFirstEra) then
			local sql = [[
				SELECT Buildings.ID, Buildings.Description, Buildings.PortraitIndex, Buildings.IconAtlas 
				FROM Buildings 
				INNER JOIN BuildingClasses ON Buildings.BuildingClass = BuildingClasses.Type 
				WHERE Buildings.PrereqTech IS NULL AND (Buildings.FaithCost == 0 or Buildings.Cost >= 0) AND Buildings.IsCorporation == 0 AND BuildingClasses.MaxGlobalInstances < 0 AND (BuildingClasses.MaxPlayerInstances <> 1 or Buildings.SpecialistCount > 0) AND BuildingClasses.MaxTeamInstances < 0;]];
		
			for building in DB.Query(sql) do
				AddArticle(eraID, tableid, building);
				tableid = tableid + 1;
			end
		end
		
		bFirstEra = false;
	end
		
	for k,v in pairs(sortedList[CategoryBuildings]) do
		table.sort(v, Alphabetically);
	end
end


CivilopediaCategory[CategoryWonders].PopulateList = function()
	-- add the instances of the Wonder, National Wonder, Team Wonder, and Project entries
	
	sortedList[CategoryWonders] = {};
	
	-- first Wonders
	sortedList[CategoryWonders][1] = {};
	local tableid = 1;

	for building in GameInfo.Buildings() do	
		-- exclude wonders etc.				
		local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
		if thisBuildingClass.MaxGlobalInstances > 0 and building.IsCorporation == 0 then
			local article = {};
			local name = Locale.ConvertTextKey( building.Description )
			article.entryName = name;
			article.entryID = building.ID;
			article.entryCategory = CategoryWonders;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( building.PortraitIndex, buttonSize, building.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryWonders][1][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[building.Description] = article;
			categorizedList[(CategoryWonders * absurdlyLargeNumTopicsInCategory) + building.ID] = article;
		end
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryWonders][1], Alphabetically);
			
	-- next National Wonders
	sortedList[CategoryWonders][2] = {};
	tableid = 1;

	for building in GameInfo.Buildings() do	
		local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
		if thisBuildingClass.MaxPlayerInstances == 1 then
			local article = {};
			local name = Locale.ConvertTextKey( building.Description )
			article.entryName = name;
			article.entryID = building.ID;
			article.entryCategory = CategoryWonders;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( building.PortraitIndex, buttonSize, building.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryWonders][2][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[building.Description] = article;
			categorizedList[(CategoryWonders * absurdlyLargeNumTopicsInCategory) + building.ID] = article;
		end
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryWonders][2], Alphabetically);
	
	-- finally Projects
	sortedList[CategoryWonders][3] = {};
	tableid = 1;

	for building in GameInfo.Projects() do
		local bIgnore = projectsToIgnore[building.Type];	
		if(bIgnore ~= true) then
			local article = {};
			local name = Locale.ConvertTextKey( building.Description )
			article.entryName = name;
			article.entryID = building.ID + 1000;
			article.entryCategory = CategoryWonders;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( building.PortraitIndex, buttonSize, building.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryWonders][3][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[building.Description] = article;
			categorizedList[(CategoryWonders * absurdlyLargeNumTopicsInCategory) + building.ID + 1000] = article;
		end
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryWonders][3], Alphabetically);
	
		-- fourth Corporations
	sortedList[CategoryWonders][4] = {};
	local tableid = 1;

	for building in GameInfo.Buildings() do	
		-- exclude wonders etc.				
		local thisBuildingClass = GameInfo.BuildingClasses[building.BuildingClass];
		if thisBuildingClass.MaxGlobalInstances > 0 and building.IsCorporation > 0 then
			local article = {};
			local name = Locale.ConvertTextKey( building.Description )
			article.entryName = name;
			article.entryID = building.ID;
			article.entryCategory = CategoryWonders;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( building.PortraitIndex, buttonSize, building.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryWonders][4][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[building.Description] = article;
			categorizedList[(CategoryWonders * absurdlyLargeNumTopicsInCategory) + building.ID] = article;
		end
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryWonders][4], Alphabetically);
					
end

CivilopediaCategory[CategoryPolicies].PopulateList = function()
	-- add the instances of the policy entries
	
	sortedList[CategoryPolicies] = {};
	
	-- for each policy branch
	for branch in GameInfo.PolicyBranchTypes() do
	
		local branchID = branch.ID;
	
		sortedList[CategoryPolicies][branchID] = {};
		local tableid = 1;
	
		-- for each policy in this branch
 		for policy in GameInfo.Policies("PolicyBranchType = '" .. branch.Type .. "'") do
			local article = {};
			local name = Locale.ConvertTextKey( policy.Description )
			article.entryName = name;
			article.entryID = policy.ID;
			article.entryCategory = CategoryPolicies;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( policy.PortraitIndex, buttonSize, policy.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryPolicies][branchID][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[policy.Description] = article;
			categorizedList[(CategoryPolicies * absurdlyLargeNumTopicsInCategory) + policy.ID] = article;
		end
		
		-- put in free policies that belong to this branch in here
		if (branch.FreePolicy ~= nil) then
			local freePolicy = GameInfo.Policies[branch.FreePolicy];
			local article = {};
			local name = Locale.ConvertTextKey( freePolicy.Description )
			article.entryName = name;
			article.entryID = freePolicy.ID;
			article.entryCategory = CategoryPolicies;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( freePolicy.PortraitIndex, buttonSize, freePolicy.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
		
			sortedList[CategoryPolicies][branchID][tableid] = article;
			tableid = tableid + 1;
		
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[freePolicy.Description] = article;
			categorizedList[(CategoryPolicies * absurdlyLargeNumTopicsInCategory) + freePolicy.ID] = article;
		end

		-- sort this list alphabetically by localized name
		table.sort(sortedList[CategoryPolicies][branchID], Alphabetically);
	
	end
		
end

CivilopediaCategory[CategoryPeople].PopulateList = function()
	-- add the instances of the Specialists and Great People entries
	
	sortedList[CategoryPeople] = {};
	
	-- first Specialists
	sortedList[CategoryPeople][1] = {};
	local tableid = 1;

	for person in GameInfo.Specialists() do	
		local article = {};
		local name = Locale.ConvertTextKey( person.Description )
		article.entryName = name;
		article.entryID = person.ID;
		article.entryCategory = CategoryPeople;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( person.PortraitIndex, buttonSize, person.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end				

		sortedList[CategoryPeople][1][tableid] = article;
		tableid = tableid + 1;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[person.Description] = article;
		categorizedList[(CategoryPeople * absurdlyLargeNumTopicsInCategory) + person.ID] = article; -- add a fudge factor
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryPeople][1], Alphabetically);
			
	-- next Great People
	sortedList[CategoryPeople][2] = {};
	tableid = 1;

	for unit in GameInfo.Units() do
		if unit.PrereqTech == nil and unit.Special ~= nil then
			local article = {};
			local name = Locale.ConvertTextKey( unit.Description )
			article.entryName = name;
			article.entryID = unit.ID + 1000;
			article.entryCategory = CategoryPeople;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( unit.PortraitIndex, buttonSize, unit.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryPeople][2][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[unit.Description] = article;
			categorizedList[(CategoryPeople * absurdlyLargeNumTopicsInCategory) + unit.ID + 1000] = article; -- todo: add fudge factor
		end
	end

	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryPeople][2], Alphabetically);
					
end

CivilopediaCategory[CategoryCivilizations].PopulateList = function()
	-- add the instances of the Civilization and Leader entries
	
	sortedList[CategoryCivilizations] = {};
	
	-- first Civilizations
	sortedList[CategoryCivilizations][1] = {};
	local tableid = 1;

	for row in GameInfo.Civilizations() do
		if row.Type ~= "CIVILIZATION_MINOR" and row.Type ~= "CIVILIZATION_BARBARIAN" then
			local article = {};
			local name = Locale.ConvertTextKey( row.ShortDescription )
			article.entryName = name;
			article.entryID = row.ID;
			article.entryCategory = CategoryCivilizations;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				

			sortedList[CategoryCivilizations][1][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[row.ShortDescription] = article;
			categorizedList[(CategoryCivilizations * absurdlyLargeNumTopicsInCategory) + row.ID] = article;
		end
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryCivilizations][1], Alphabetically);
			
	-- next Leaders
	sortedList[CategoryCivilizations][2] = {};
	local tableid = 1;

	for row in GameInfo.Civilizations() do	
		if row.Type ~= "CIVILIZATION_MINOR" and row.Type ~= "CIVILIZATION_BARBARIAN" then
			local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. row.Type .. "'" )().LeaderheadType];
			local article = {};
			local name = Locale.ConvertTextKey( leader.Description )
			article.entryName = name;
			article.entryID = row.ID + 1000;
			article.entryCategory = CategoryCivilizations;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( leader.PortraitIndex, buttonSize, leader.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				

			sortedList[CategoryCivilizations][2][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[leader.Description] = article;
			categorizedList[(CategoryCivilizations * absurdlyLargeNumTopicsInCategory) + row.ID + 1000] = article;
		end
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryCivilizations][2], Alphabetically);
					
end

CivilopediaCategory[CategoryCityStates].PopulateList = function()
	-- add the instances of the City States entries
	
	sortedList[CategoryCityStates] = {};
	
	-- for each trait
	for trait in GameInfo.MinorCivTraits() do
	
		local traitID = trait.ID;
	
		sortedList[CategoryCityStates][traitID] = {};
		local tableid = 1;
	
		-- for each city state that has this trait
 		for cityState in GameInfo.MinorCivilizations("MinorCivTrait = '" .. trait.Type .. "'") do
 		
			-- add a tech entry to a list (localized name, tag, etc.)
 			local article = {};
 			local name = Locale.ConvertTextKey( cityState.Description )
 			article.entryName = name;
 			article.entryID = cityState.ID;
			article.entryCategory = CategoryCityStates;
			
			sortedList[CategoryCityStates][traitID][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[cityState.Description] = article;
			categorizedList[(CategoryCityStates * absurdlyLargeNumTopicsInCategory) + cityState.ID] = article;
		end

		-- sort this list alphabetically by localized name
		table.sort(sortedList[CategoryCityStates][traitID], Alphabetically);
	
	end
					
end

CivilopediaCategory[CategoryTerrain].PopulateList = function()
	-- add the instances of the Terrain and Features entries
	
	sortedList[CategoryTerrain] = {};
	
	-- first Specialists
	sortedList[CategoryTerrain][1] = {};
	local tableid = 1;

	for row in GameInfo.Terrains() do	
		local article = {};
		local name = Locale.ConvertTextKey( row.Description )
		article.entryName = name;
		article.entryID = row.ID;
		article.entryCategory = CategoryTerrain;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end				

		sortedList[CategoryTerrain][1][tableid] = article;
		tableid = tableid + 1;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[row.Description] = article;
		categorizedList[(CategoryTerrain * absurdlyLargeNumTopicsInCategory) + row.ID] = article; -- add a fudge factor
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryTerrain][1], Alphabetically);
			
	-- next Features
	sortedList[CategoryTerrain][2] = {};
	tableid = 1;

	for row in GameInfo.Features() do	
		local article = {};
		local name = Locale.ConvertTextKey( row.Description )
		article.entryName = name;
		article.entryID = row.ID + 1000;
		article.entryCategory = CategoryTerrain;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end				

		sortedList[CategoryTerrain][2][tableid] = article;
		tableid = tableid + 1;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[row.Description] = article;
		categorizedList[(CategoryTerrain * absurdlyLargeNumTopicsInCategory) + row.ID + 1000] = article; -- add a fudge factor
	end

	-- now for the fake features (river and lake)
	for row in GameInfo.FakeFeatures() do	
		local article = {};
		local name = Locale.ConvertTextKey( row.Description )
		article.entryName = name;
		article.entryID = row.ID + 2000;
		article.entryCategory = CategoryTerrain;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end				

		sortedList[CategoryTerrain][2][tableid] = article;
		tableid = tableid + 1;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[row.Description] = article;
		categorizedList[(CategoryTerrain * absurdlyLargeNumTopicsInCategory) + row.ID + 2000] = article; -- add a fudge factor
	end

	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryTerrain][2], Alphabetically);
					
end

CivilopediaCategory[CategoryResources].PopulateList = function()
	-- add the instances of the resource entries
	
	sortedList[CategoryResources] = {};
	
	-- for each type of resource
	for resourceType = 0, 2, 1 do
		
		sortedList[CategoryResources][resourceType] = {};
		local tableid = 1;
	
		-- for each type of resource
 		for resource in GameInfo.Resources( "ResourceUsage = '" .. resourceType .. "'" ) do
 		
			-- add a tech entry to a list (localized name, tag, etc.)
 			local article = {};
 			local name = Locale.ConvertTextKey( resource.Description )
 			article.entryName = name;
 			article.entryID = resource.ID;
			article.entryCategory = CategoryResources;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( resource.PortraitIndex, buttonSize, resource.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryResources][resourceType][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[resource.Description] = article;
			categorizedList[(CategoryResources * absurdlyLargeNumTopicsInCategory) + resource.ID] = article;
		end

		-- sort this list alphabetically by localized name
		table.sort(sortedList[CategoryResources][resourceType], Alphabetically);
	
	end
					
end

CivilopediaCategory[CategoryImprovements].PopulateList = function()
	-- add the instances of the improvement entries
	
	sortedList[CategoryImprovements] = {};
	
	sortedList[CategoryImprovements][1] = {}; -- there is only one section (for now)
	local tableid = 1;
	
	-- for each improvement
	for row in GameInfo.Improvements() do	
		if not row.GraphicalOnly then
			-- add an article to the list (localized name, unit tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey( row.Description );
			article.entryName = name;
			article.entryID = row.ID;
			article.entryCategory = CategoryImprovements;
			article.tooltipTextureOffset, article.tooltipTexture = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
			if not article.tooltipTextureOffset then
				article.tooltipTexture = defaultErrorTextureSheet;
				article.tooltipTextureOffset = nullOffset;
			end				
			
			sortedList[CategoryImprovements][1][tableid] = article;
			tableid = tableid + 1;
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[row.Description] = article;
			categorizedList[(CategoryImprovements * absurdlyLargeNumTopicsInCategory) + row.ID] = article;
		end
	end
	
	--add railroads and roads
	for row in GameInfo.Routes() do
		local article = {};
		local name = Locale.ConvertTextKey( row.Description );
		article.entryName = name;
		article.entryID = row.ID + 1000;
		article.entryCategory = CategoryImprovements;
		article.tooltipTextureOffset, article.tooltipTexture = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
		if not article.tooltipTextureOffset then
			article.tooltipTexture = defaultErrorTextureSheet;
			article.tooltipTextureOffset = nullOffset;
		end				
		
		sortedList[CategoryImprovements][1][tableid] = article;
		tableid = tableid + 1;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[row.Description] = article;
		categorizedList[(CategoryImprovements * absurdlyLargeNumTopicsInCategory) + 1000 + row.ID] = article;
	end

	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryImprovements][1], Alphabetically);
			
end

CivilopediaCategory[CategoryBeliefs].PopulateList = function()
	
	sortedList[CategoryBeliefs] = {};
	
	do
		local section = {};
		for religion in GameInfo.Religions("Type <> 'RELIGION_PANTHEON'") do
	 		
			-- add a tech entry to a list (localized name, tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey(religion.Description)
			article.entryName = name;
			article.entryID = {"Religions", religion.ID};
			article.entryCategory = CategoryBeliefs;			
			
			table.insert(section, article);
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[religion.Description] = article;
			categorizedList[(CategoryBeliefs * absurdlyLargeNumTopicsInCategory) + religion.ID] = article;
		end
		
		-- sort this list alphabetically by localized name
		table.sort(section, Alphabetically);
		table.insert(sortedList[CategoryBeliefs], section);
	end
	
	-- for each type of resource
	local sectionConditions = {
		"Pantheon = 1",
		"Founder = 1",
		"Follower = 1",
		"Enhancer = 1",
		"Reformation = 1"
	};
	
	local numReligions = #GameInfo.Religions;
	for i,condition in ipairs(sectionConditions) do
		
		local section = {};
	
 		for belief in GameInfo.Beliefs(condition) do
 		
			-- add a tech entry to a list (localized name, tag, etc.)
 			local article = {};
 			local name = Locale.ConvertTextKey( belief.ShortDescription )
 			article.entryName = name;
 			article.entryID = {"Beliefs", belief.ID};
			article.entryCategory = CategoryBeliefs;			
			
			table.insert(section, article);
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[belief.ShortDescription] = article;
			categorizedList[(CategoryBeliefs * absurdlyLargeNumTopicsInCategory) + numReligions + belief.ID] = article;
		end

		-- sort this list alphabetically by localized name
		table.sort(section, Alphabetically);
		table.insert(sortedList[CategoryBeliefs], section);
	
	end			
end

CivilopediaCategory[CategoryWorldCongress].PopulateList = function()
	
	sortedList[CategoryWorldCongress] = {};
	
	do
		local section = {};
		for resolution in GameInfo.Resolutions() do
	 		
			-- add a resolution entry to a list (localized name, tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey(resolution.Description)
			article.entryName = name;
			article.entryID = {"Resolutions", resolution.ID};
			article.entryCategory = CategoryWorldCongress;			
			
			table.insert(section, article);
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[resolution.Description] = article;
			categorizedList[(CategoryWorldCongress * absurdlyLargeNumTopicsInCategory) + resolution.ID] = article;
		end
		
		-- sort this list alphabetically by localized name
		table.sort(section, Alphabetically);
		table.insert(sortedList[CategoryWorldCongress], section);
	end
	
	do
		local section = {};
		for leagueProject in GameInfo.LeagueProjects() do
	 		
			-- add a league project entry to a list (localized name, tag, etc.)
			local article = {};
			local name = Locale.ConvertTextKey(leagueProject.Description)
			article.entryName = name;
			article.entryID = {"LeagueProjects", leagueProject.ID};
			article.entryCategory = CategoryWorldCongress;			
			
			table.insert(section, article);
			
			-- index by various keys
			searchableList[Locale.ToLower(name)] = article;
			searchableTextKeyList[leagueProject.Description] = article;
			categorizedList[(CategoryWorldCongress * absurdlyLargeNumTopicsInCategory) + leagueProject.ID] = article;
		end
		
		-- sort this list alphabetically by localized name
		table.sort(section, Alphabetically);
		table.insert(sortedList[CategoryWorldCongress], section);
	end
end

CivilopediaCategory[CategoryCorporations].PopulateList = function()

	sortedList[CategoryCorporations] = {};
	
	sortedList[CategoryCorporations][1] = {}; -- there is only one section (for now)
	local tableid = 1;
	
	for corporation in GameInfo.Corporations() do
		-- add a corporation entry to a list (localized name, tag, etc.)
		local article = {};
		local name = Locale.ConvertTextKey(corporation.Description);
		article.entryName = name;
		article.entryID = corporation.ID;
		article.entryCategory = CategoryCorporations;
		
		sortedList[CategoryCorporations][1][tableid] = article;
		tableid = tableid + 1;
		
		-- index by various keys
		searchableList[Locale.ToLower(name)] = article;
		searchableTextKeyList[corporation.Description] = article;
		categorizedList[(CategoryCorporations * absurdlyLargeNumTopicsInCategory) + corporation.ID] = article;
	end
	
	-- sort this list alphabetically by localized name
	table.sort(sortedList[CategoryCorporations][1], Alphabetically);

end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function ResizeEtc()
	Controls.ListOfArticles:CalculateSize();
	Controls.WideStack:CalculateSize();
	Controls.FFTextStack:CalculateSize();
	Controls.NarrowStack:CalculateSize();
	Controls.BBTextStack:CalculateSize();
	Controls.SuperWideStack:CalculateSize();
	Controls.BBTextStack:ReprocessAnchoring();
	Controls.WideStack:ReprocessAnchoring();
	Controls.FFTextStack:ReprocessAnchoring();
	Controls.NarrowStack:ReprocessAnchoring();
	Controls.ListOfArticles:ReprocessAnchoring();
	Controls.SuperWideStack:ReprocessAnchoring();


	Controls.ScrollPanel:CalculateInternalSize();
	Controls.LeftScrollPanel:CalculateInternalSize();
end

--------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------

CivilopediaCategory[CategoryHomePage].DisplayHomePage = function()

	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey("TXT_KEY_PEDIA_HOME_PAGE_LABEL") );	
	
	-- Update some circle logo
	if IconHookup( 20, portraitSize, "TERRAIN_ATLAS", Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end

	g_BBTextManager:ResetInstances();
	
	--Welcome and insert 1st manual paragraph
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_HOME_PAGE_BLURB_TEXT" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );

	--How to use the Pedia	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_HOME_PAGE_HELP_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_HOME_PAGE_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	
	Controls.BBTextStack:SetHide( false );	
	ResizeEtc();
end;

CivilopediaCategory[CategoryGameConcepts].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_GAME_CONCEPT_PAGE_LABEL" ));	
	
	-- Update Some kind of circle logo
	if IconHookup( 47, portraitSize, "TECH_ATLAS_1", Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end

	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_GCONCEPTS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();

	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_GAME_CONCEPT_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_GAME_CONCEPT_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	
	Controls.BBTextStack:SetHide( false );
	
	--Did you know fact/tip of the day? Can be taken from rando advisor text.  Or link to random page or modding
	ResizeEtc();
end;

CivilopediaCategory[CategoryTech].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TECH_PAGE_LABEL" ));	
	
	local portraitIndex = 48;
	local portraitAtlas = "TECH_ATLAS_1";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Technologies ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_TECHS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
	
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TECH_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TECHNOLOGIES_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryUnits].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_UNITS_PAGE_LABEL" ));	
	
	local portraitIndex = 26;
	local portraitAtlas = "UNIT_ATLAS_1";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Units ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_UNITS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
	
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_UNITS_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_UNITS_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryPromotions].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PROMOTIONS_PAGE_LABEL" ));	
	
	local portraitIndex = 16;
	local portraitAtlas = "PROMOTION_ATLAS";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from UnitPromotions ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_PROMOTIONS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PROMOTIONS_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PROMOTIONS_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryBuildings].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BUILDINGS_PAGE_LABEL" ));	
	
	local portraitIndex = 24;
	local portraitAtlas = "BW_ATLAS_1";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Buildings where WonderSplashImage IS NULL ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_BUILDINGS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BUILDINGS_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BUILDINGS_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryWonders].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WONDERS_PAGE_LABEL" ));	
	
	local portraitIndex = 2;
	local portraitAtlas = "BW_ATLAS_2";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Buildings Where WonderSplashImage IS NOT NULL ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_WONDERS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
	
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WONDERS_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WONDERS_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryPolicies].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_POLICIES_PAGE_LABEL" ));	
	
	local portraitIndex = 25;
	local portraitAtlas = "POLICY_ATLAS";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Policies Where IconAtlas IS NOT NULL ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_POLICIES" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
	
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_POLICIES_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_SOCIAL_POL_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryPeople].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PEOPLE_PAGE_LABEL" ));	

	if IconHookup( 47, portraitSize, "UNIT_ATLAS_2", Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_PEOPLE" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
		
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PEOPLE_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_SPEC_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryCivilizations].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CIVILIZATIONS_PAGE_LABEL" ));	
	
	local portraitIndex = 7;
	local portraitAtlas = "LEADER_ATLAS";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Leaders where Type <> \"LEADER_BARBARIAN\" ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_CIVS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CIVILIZATIONS_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CIVS_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryCityStates].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CITY_STATES_PAGE_LABEL" ));	
	
	if IconHookup( 44, portraitSize, "UNIT_ATLAS_2", Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_CITYSTATES" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CITY_STATES_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CSTATES_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryTerrain].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_PAGE_LABEL" ));	
	
	local portraitIndex = 9;
	local portraitAtlas = "TERRAIN_ATLAS";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Terrains ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_TERRAIN" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );	
	end	
	Controls.BBTextStack:SetHide( false );
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_FEATURES_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_FEATURES_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end
	ResizeEtc();
end;

CivilopediaCategory[CategoryResources].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCES_PAGE_LABEL" ));	
	
	local portraitIndex = 6;
	local portraitAtlas = "RESOURCE_ATLAS";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Resources ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_RESOURCES" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCES_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCES_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryImprovements].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPROVEMENTS_PAGE_LABEL" ));	
	
	--eventually make random?
	local portraitIndex = 1;
	local portraitAtlas = "BW_ATLAS_1";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Improvements ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_QUOTE_BLOCK_IMPROVEMENTS" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPROVEMENTS_PAGE_LABEL" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryBeliefs].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BELIEFS_PAGE_LABEL" ));	
	
	Controls.Portrait:SetTexture("Religion256.dds");
	Controls.Portrait:SetTextureOffsetVal(0,0);
	Controls.Portrait:SetHide(false);
	
	Controls.PortraitFrame:SetHide(false);
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BELIEFS_HOMEPAGE_BLURB" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BELIEFS_HOMEPAGE_LABEL1" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BELIEFS_HOMEPAGE_TEXT1" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryWorldCongress].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WORLD_CONGRESS_PAGE_LABEL" ));	
	
	Controls.Portrait:SetTexture("WorldCongressPortrait256_EXP2.dds");
	Controls.Portrait:SetTextureOffsetVal(0,0);
	Controls.Portrait:SetHide(false);
	
	Controls.PortraitFrame:SetHide(false);
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WORLD_CONGRESS_HOMEPAGE_BLURB" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WORLD_CONGRESS_HOMEPAGE_LABEL1" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WORLD_CONGRESS_HOMEPAGE_TEXT1" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

CivilopediaCategory[CategoryCorporations].DisplayHomePage = function()
	ClearArticle();
	Controls.ArticleID:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_PAGE_LABEL" ));	
	
	local portraitIndex = 6;
	local portraitAtlas = "CORP_ATLAS";
		
	for row in DB.Query("SELECT PortraitIndex, IconAtlas from Corporations ORDER By Random() LIMIT 1") do
		portraitIndex = row.PortraitIndex;
		portraitAtlas = row.IconAtlas;
	end	
	
	if IconHookup( portraitIndex, portraitSize, portraitAtlas, Controls.Portrait ) then
		Controls.PortraitFrame:SetHide( false );
	else
		Controls.PortraitFrame:SetHide( true );
	end
	
	UpdateTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_HOMEPAGE_BLURB" ), Controls.HomePageBlurbLabel, Controls.HomePageBlurbInnerFrame, Controls.HomePageBlurbFrame );
	
	g_BBTextManager:ResetInstances();
			
	--Basic Sectional Infos	
	local thisBBTextInstance = g_BBTextManager:GetInstance();
	if thisBBTextInstance then
		thisBBTextInstance.BBTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_HOMEPAGE_LABEL1" ));
		UpdateSuperWideTextBlock( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_HOMEPAGE_TEXT1" ), thisBBTextInstance.BBTextLabel, thisBBTextInstance.BBTextInnerFrame, thisBBTextInstance.BBTextFrame );
	end	
	Controls.BBTextStack:SetHide( false );
	ResizeEtc();
end;

--------------------------------------------------------------------------------------------------------
-- a few handy-dandy helper functions
--------------------------------------------------------------------------------------------------------

-- put text into a text block and resize the block
function UpdateTextBlock( localizedString, label, innerFrame, outerFrame )
	local contentSize;
	local frameSize = {};
	label:SetText( localizedString );
	contentSize = label:GetSize();
	frameSize.x = wideInnerFrameWidth;
	frameSize.y = contentSize.y + textPaddingFromInnerFrame;
	innerFrame:SetSize( frameSize );
	frameSize.x = wideOuterFrameWidth;
	frameSize.y = contentSize.y + textPaddingFromInnerFrame - offsetsBetweenFrames;
	outerFrame:SetSize( frameSize );
	outerFrame:SetHide( false );
end

function UpdateNarrowTextBlock( localizedString, label, innerFrame, outerFrame )
	local contentSize;
	local frameSize = {};
	label:SetText( localizedString );
	contentSize = label:GetSize();
	frameSize.x = narrowInnerFrameWidth;
	frameSize.y = contentSize.y + textPaddingFromInnerFrame;
	innerFrame:SetSize( frameSize );
	frameSize.x = narrowOuterFrameWidth;
	frameSize.y = contentSize.y + textPaddingFromInnerFrame - offsetsBetweenFrames;
	outerFrame:SetSize( frameSize );
	outerFrame:SetHide( false );
end

function UpdateSuperWideTextBlock( localizedString, label, innerFrame, outerFrame )
	local contentSize;
	local frameSize = {};
	label:SetText( localizedString );
	contentSize = label:GetSize();
	frameSize.x = superWideInnerFrameWidth;
	frameSize.y = contentSize.y + textPaddingFromInnerFrame;
	innerFrame:SetSize( frameSize );
	frameSize.x = superWideOuterFrameWidth;
	frameSize.y = contentSize.y + textPaddingFromInnerFrame - offsetsBetweenFrames;
	outerFrame:SetSize( frameSize );
	outerFrame:SetHide( false );
end

function UpdateButtonFrame( numButtonsAdded, innerFrame, outerFrame )
	if numButtonsAdded > 0 then
		local frameSize = {};
		local h = (math.floor((numButtonsAdded - 1) / numberOfButtonsPerRow) + 1) * buttonSize + buttonPaddingTimesTwo;
		frameSize.x = narrowInnerFrameWidth;
		frameSize.y = h;
		innerFrame:SetSize( frameSize );
		frameSize.x = narrowOuterFrameWidth;
		frameSize.y = h - offsetsBetweenFrames;
		outerFrame:SetSize( frameSize );
		outerFrame:SetHide( false );
	end
end	

function UpdateSmallButton( buttonAdded, image, button, textureSheet, textureOffset, category, localizedText, buttonId )
	
	if(textureSheet ~= nil) then
		image:SetTexture( textureSheet );
	end
	
	if(textureOffset ~= nil) then
		image:SetTextureOffset( textureOffset );	
	end
	
	button:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );				
	button:SetToolTipString( localizedText );
	
	if(category ~= nil) then
		button:SetVoids( buttonId, addToList );
		button:RegisterCallback( Mouse.eLClick, CivilopediaCategory[category].SelectArticle );
	end
end

-- this will need to be enhanced to look at the current language
function TagExists( tag )
	return Locale.HasTextKey(tag);
end

-------------------------------------------------------------------------------
-- Generic info from individual table fields (for Extended Information)
-------------------------------------------------------------------------------
function AnalyzeObjectField(tObject, sField, sSuffix, sAddText)
	--dprint("   ...field", sField, type(tObject[sField]), tObject[sField]);
	if tObject[sField] == nil then return ""; end
	if type(tObject[sField]) == "boolean" then 
		if tObject[sField] then return "[NEWLINE][ICON_BULLET]"..sField;
		else                    return ""; 	end
	end
	if type(tObject[sField]) == "string" then 
		if tObject[sField] == nil then return ""; end
		if sSuffix == nil then return "[NEWLINE][ICON_BULLET]"..sField..string.format(" %s", tObject[sField]); end
		-- sSuffix is reference table
		return "[NEWLINE][ICON_BULLET]"..sAddText.." [COLOR_CYAN]"..Locale.Lookup(GameInfo[sSuffix][tObject[sField]].Description).."[ENDCOLOR]";
	end
	if type(tObject[sField]) == "number" then 
		if tObject[sField] == 0 or tObject[sField] == -1 then return ""; end
		local suffix = "%";
		if sSuffix ~= nil then suffix = " "..sSuffix; end
		if suffix == " " then suffix = ""; end
		return "[NEWLINE][ICON_BULLET]"..string.format("%+d", tObject[sField])..suffix.." "..sField;
	end
	return "[NEWLINE]Error: unknown field type for "..sField;
end

--------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------


CivilopediaCategory[CategoryHomePage].SelectArticle = function( pageID, shouldAddToList )
	
	ClearArticle();
	
	if pageID == 1 then
		if selectedCategory ~= CategoryHomePage then
		SetSelectedCategory(CategoryHomePage);
		end
		CivilopediaCategory[CategoryHomePage].DisplayHomePage();
	elseif pageID == 2 then
		if selectedCategory ~= CategoryGameConcepts then
		SetSelectedCategory(CategoryGameConcepts);
		end
		CivilopediaCategory[CategoryGameConcepts].DisplayHomePage();
	elseif pageID == 3 then
		if selectedCategory ~= CategoryTech then
		SetSelectedCategory(CategoryTech);
		end
		CivilopediaCategory[CategoryTech].DisplayHomePage();
	elseif pageID == 4 then
		if selectedCategory ~= CategoryUnits then
		SetSelectedCategory(CategoryUnits);
		end
		CivilopediaCategory[CategoryUnits].DisplayHomePage();
	elseif pageID == 5 then
		if selectedCategory ~= CategoryPromotions then
		SetSelectedCategory(CategoryPromotions);
		end
		CivilopediaCategory[CategoryPromotions].DisplayHomePage() ;
	elseif pageID == 6 then
		if selectedCategory ~= CategoryBuildings then
		SetSelectedCategory(CategoryBuildings);
		end
		CivilopediaCategory[CategoryBuildings].DisplayHomePage(); 
	elseif pageID == 7 then
		if selectedCategory ~= CategoryWonders then
		SetSelectedCategory(CategoryWonders);
		end
		CivilopediaCategory[CategoryWonders].DisplayHomePage(); 
	elseif pageID == 8 then
		if selectedCategory ~= CategoryPolicies then
		SetSelectedCategory(CategoryPolicies);
		end
		CivilopediaCategory[CategoryPolicies].DisplayHomePage() ;
	elseif pageID == 9 then
		if selectedCategory ~= CategoryPeople then
		SetSelectedCategory(CategoryPeople);
		end
		CivilopediaCategory[CategoryPeople].DisplayHomePage() ;
	elseif pageID == 10 then
		if selectedCategory ~= CategoryCivilizations then
		SetSelectedCategory(CategoryCivilizations);
		end
		CivilopediaCategory[CategoryCivilizations].DisplayHomePage() ;
	elseif pageID == 11 then
		if selectedCategory ~= CategoryCityStates then
		SetSelectedCategory(CategoryCityStates);
		end
		CivilopediaCategory[CategoryCityStates].DisplayHomePage() ;
	elseif pageID == 12 then
		if selectedCategory ~= CategoryTerrain then
		SetSelectedCategory(CategoryTerrain);
		end
		CivilopediaCategory[CategoryTerrain].DisplayHomePage();	
	elseif pageID == 13 then
		if selectedCategory ~= CategoryResources then
		SetSelectedCategory(CategoryResources);
		end
		CivilopediaCategory[CategoryResources].DisplayHomePage()	;
	elseif pageID == 14 then
		if selectedCategory ~= CategoryImprovements then
		SetSelectedCategory(CategoryImprovements);
		end
		CivilopediaCategory[CategoryImprovements].DisplayHomePage() ;
	elseif pageID == CategoryBeliefs then
		if selectedCategory ~= pageID then
			SetSelectedCategory(pageID);
		end
		CivilopediaCategory[pageID].DisplayHomePage();
	elseif pageID == CategoryWorldCongress then
		if selectedCategory ~= pageID then
			SetSelectedCategory(pageID);
		end
		CivilopediaCategory[pageID].DisplayHomePage();
	elseif pageID == CategoryCorporations then
		if selectedCategory ~= pageID then
			SetSelectedCategory(pageID);
		end
		CivilopediaCategory[pageID].DisplayHomePage();
	else
		print("couldn't find category home page");
	end
	
	ResizeEtc();
	
end

CivilopediaCategory[CategoryGameConcepts].SelectArticle = function( conceptID, shouldAddToList )
	print("CivilopediaCategory[CategoryGameConcepts].SelectArticle");
	if selectedCategory ~= CategoryGameConcepts then
		SetSelectedCategory(CategoryGameConcepts);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryGameConcepts * absurdlyLargeNumTopicsInCategory) + conceptID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if conceptID ~= -1 then
		local thisConcept = GameInfo.Concepts[conceptID];
		
		if thisConcept then
		
			-- update the name
			local name = Locale.ConvertTextKey( thisConcept.Description ); 	
			Controls.ArticleID:SetText( name );
			
			-- portrait
			
			-- update the summary
			if thisConcept.Summary then
				UpdateTextBlock( Locale.ConvertTextKey( thisConcept.Summary ), Controls.SummaryLabel, Controls.SummaryInnerFrame, Controls.SummaryFrame );
			end
			
			-- update the extended article
			if thisConcept.Extended then
				UpdateTextBlock( Locale.ConvertTextKey( thisConcept.Extended ), Controls.ExtendedLabel, Controls.ExtendedInnerFrame, Controls.ExtendedFrame );
			end
			
			-- update the Designer's Notes
			if thisConcept.DesignNotes then
				UpdateTextBlock( Locale.ConvertTextKey( thisConcept.DesignNotes ), Controls.DNotesLabel, Controls.DNotesInnerFrame, Controls.DNotesFrame );
			end
			
			-- related images
			
			-- related concepts
		
		end

	end	

	ResizeEtc();

end


CivilopediaCategory[CategoryTech].SelectArticle = function( techID, shouldAddToList )
	print("CivilopediaCategory[CategoryTech].SelectArticle");

	if selectedCategory ~= CategoryTech then
		SetSelectedCategory(CategoryTech);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryTech * absurdlyLargeNumTopicsInCategory) + techID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if techID ~= -1 then
		local thisTech = GameInfo.Technologies[techID];
					
		local name = Locale.ConvertTextKey( thisTech.Description ); 	
		Controls.ArticleID:SetText( name );

		-- if we have one, update the tech picture
		if IconHookup( thisTech.PortraitIndex, portraitSize, thisTech.IconAtlas, Controls.Portrait ) then
			Controls.PortraitFrame:SetHide( false );
		else
			Controls.PortraitFrame:SetHide( true );
		end
		
		-- update the cost
		Controls.CostFrame:SetHide( false );

		local cost = thisTech.Cost;
		if(Game ~= nil) then
			local pPlayer = Players[Game.GetActivePlayer()];
			local pTeam = Teams[pPlayer:GetTeam()];
			local pTeamTechs = pTeam:GetTeamTechs();
			cost = pTeamTechs:GetResearchCost(techID);		
		end

		if (cost > 0) then
			Controls.CostLabel:SetText( tostring(cost).." [ICON_RESEARCH]" );
		else
			Controls.CostLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_FREE" ) );
		end
		
 		local contentSize;
 		local frameSize = {};
		local buttonAdded = 0;

		local techType = thisTech.Type;
		local condition = "TechType = '" .. techType .. "'";
		local prereqCondition = "PrereqTech = '" .. techType .. "'";
		local otherPrereqCondition = "TechPrereq = '" .. techType .. "'";
		local revealCondition = "TechReveal = '" .. techType .. "'";

		-- update the prereq techs
		g_PrereqTechManager:ResetInstances();
		buttonAdded = 0;
		for row in GameInfo.Technology_PrereqTechs( condition ) do
			local prereq = GameInfo.Technologies[row.PrereqTech];
			local thisPrereqInstance = g_PrereqTechManager:GetInstance();
			if thisPrereqInstance then
				local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end				
				UpdateSmallButton( buttonAdded, thisPrereqInstance.PrereqTechImage, thisPrereqInstance.PrereqTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
				buttonAdded = buttonAdded + 1;
			end			
		end
		UpdateButtonFrame( buttonAdded, Controls.PrereqTechInnerFrame, Controls.PrereqTechFrame );

		-- update the leads to techs
		g_LeadsToTechManager:ResetInstances();
		buttonAdded = 0;
		for row in GameInfo.Technology_PrereqTechs( prereqCondition ) do
			local leadsTo = GameInfo.Technologies[row.TechType];
			local thisLeadsToInstance = g_LeadsToTechManager:GetInstance();
			if thisLeadsToInstance then
				local textureOffset, textureSheet = IconLookup( leadsTo.PortraitIndex, buttonSize, leadsTo.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end				
				UpdateSmallButton( buttonAdded, thisLeadsToInstance.LeadsToTechImage, thisLeadsToInstance.LeadsToTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( leadsTo.Description ), leadsTo.ID );
				buttonAdded = buttonAdded + 1;
			end			
		end
		UpdateButtonFrame( buttonAdded, Controls.LeadsToTechInnerFrame, Controls.LeadsToTechFrame );

		-- update the units unlocked
		g_UnlockedUnitsManager:ResetInstances();
		buttonAdded = 0;
		for thisUnitInfo in GameInfo.Units( prereqCondition ) do
			if thisUnitInfo.ShowInPedia then
				local thisUnitInstance = g_UnlockedUnitsManager:GetInstance();
				if thisUnitInstance then
					local textureOffset, textureSheet = IconLookup( thisUnitInfo.PortraitIndex, buttonSize, thisUnitInfo.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisUnitInstance.UnlockedUnitImage, thisUnitInstance.UnlockedUnitButton, textureSheet, textureOffset, CategoryUnits, Locale.ConvertTextKey( thisUnitInfo.Description ), thisUnitInfo.ID );
					buttonAdded = buttonAdded + 1;
				end
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.UnlockedUnitsInnerFrame, Controls.UnlockedUnitsFrame );
		
		-- update the buildings unlocked
		g_UnlockedBuildingsManager:ResetInstances();
		buttonAdded = 0;
		for thisBuildingInfo in GameInfo.Buildings( prereqCondition ) do
			local thisBuildingInstance = g_UnlockedBuildingsManager:GetInstance();
			if thisBuildingInstance then

				if not IconHookup( thisBuildingInfo.PortraitIndex, buttonSize, thisBuildingInfo.IconAtlas, thisBuildingInstance.UnlockedBuildingImage ) then
					thisBuildingInstance.UnlockedBuildingImage:SetTexture( defaultErrorTextureSheet );
					thisBuildingInstance.UnlockedBuildingImage:SetTextureOffset( nullOffset );
				end

				--move this button
				thisBuildingInstance.UnlockedBuildingButton:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );
				
				thisBuildingInstance.UnlockedBuildingButton:SetToolTipString( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
				thisBuildingInstance.UnlockedBuildingButton:SetVoids( thisBuildingInfo.ID, addToList );
				local thisBuildingClass = GameInfo.BuildingClasses[thisBuildingInfo.BuildingClass];
				if thisBuildingClass.MaxGlobalInstances > 0 or (thisBuildingClass.MaxPlayerInstances == 1 and thisBuildingInfo.SpecialistCount == 0) or thisBuildingClass.MaxTeamInstances > 0 then
					thisBuildingInstance.UnlockedBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
				else
					thisBuildingInstance.UnlockedBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
				end
				buttonAdded = buttonAdded + 1;
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.UnlockedBuildingsInnerFrame, Controls.UnlockedBuildingsFrame );
		
		-- update the projects unlocked
		g_UnlockedProjectsManager:ResetInstances();
		buttonAdded = 0;
		for thisProjectInfo in GameInfo.Projects( otherPrereqCondition ) do
		
			local bIgnore = projectsToIgnore[thisProjectInfo.Type];
			if(bIgnore ~= true) then
				local thisProjectInstance = g_UnlockedProjectsManager:GetInstance();
				if thisProjectInstance then
					local textureOffset, textureSheet = IconLookup( thisProjectInfo.PortraitIndex, buttonSize, thisProjectInfo.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisProjectInstance.UnlockedProjectImage, thisProjectInstance.UnlockedProjectButton, textureSheet, textureOffset, CategoryWonders, Locale.ConvertTextKey( thisProjectInfo.Description ), thisProjectInfo.ID + 1000);
					buttonAdded = buttonAdded + 1;
				end
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.UnlockedProjectsInnerFrame, Controls.UnlockedProjectsFrame );
		
		-- update the resources revealed
		g_RevealedResourcesManager:ResetInstances();
		buttonAdded = 0;
		for revealedResource in GameInfo.Resources( revealCondition ) do
			local thisRevealedResourceInstance = g_RevealedResourcesManager:GetInstance();
			if thisRevealedResourceInstance then
				local textureOffset, textureSheet = IconLookup( revealedResource.PortraitIndex, buttonSize, revealedResource.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end				
				UpdateSmallButton( buttonAdded, thisRevealedResourceInstance.RevealedResourceImage, thisRevealedResourceInstance.RevealedResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( revealedResource.Description ), revealedResource.ID );
				buttonAdded = buttonAdded + 1;
			end			
		end
		UpdateButtonFrame( buttonAdded, Controls.RevealedResourcesInnerFrame, Controls.RevealedResourcesFrame );

		-- update the build actions unlocked
		g_WorkerActionsManager:ResetInstances();
		buttonAdded = 0;
		for thisBuildInfo in GameInfo.Builds{PrereqTech = techType, ShowInPedia = 1} do
			local thisWorkerActionInstance = g_WorkerActionsManager:GetInstance();
			if thisWorkerActionInstance then
				local textureOffset, textureSheet = IconLookup( thisBuildInfo.IconIndex, buttonSize, thisBuildInfo.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end
				if thisBuildInfo.RouteType then
					UpdateSmallButton( buttonAdded, thisWorkerActionInstance.WorkerActionImage, thisWorkerActionInstance.WorkerActionButton, textureSheet, textureOffset, CategoryImprovements, Locale.ConvertTextKey( thisBuildInfo.Description ), GameInfo.Routes[thisBuildInfo.RouteType].ID + 1000 );
				elseif thisBuildInfo.ImprovementType then
					UpdateSmallButton( buttonAdded, thisWorkerActionInstance.WorkerActionImage, thisWorkerActionInstance.WorkerActionButton, textureSheet, textureOffset, CategoryImprovements, Locale.ConvertTextKey( thisBuildInfo.Description ), GameInfo.Improvements[thisBuildInfo.ImprovementType].ID );-- add fudge factor
				else -- we are a choppy thing
					UpdateSmallButton( buttonAdded, thisWorkerActionInstance.WorkerActionImage, thisWorkerActionInstance.WorkerActionButton, textureSheet, textureOffset, CategoryGameConcepts, Locale.ConvertTextKey( thisBuildInfo.Description ), GameInfo.Concepts["CONCEPT_WORKERS_CLEARINGLAND"].ID );-- add fudge factor
				end
				buttonAdded = buttonAdded + 1;
			end			
		end
		UpdateButtonFrame( buttonAdded, Controls.WorkerActionsInnerFrame, Controls.WorkerActionsFrame );
		
		-- update the related articles
		Controls.RelatedArticlesFrame:SetHide( true ); -- todo: figure out how this should be implemented

		-- update the game info
		if (thisTech.Help) then
			--CBP
			UpdateTextBlock( GetHelpTextForTech(thisTech.ID, false), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame )		
			--END
			--UpdateTextBlock( Locale.ConvertTextKey( thisTech.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
		end

		-- update the quote
		if thisTech.Quote then
			UpdateTextBlock( Locale.ConvertTextKey( thisTech.Quote ), Controls.SilentQuoteLabel, Controls.SilentQuoteInnerFrame, Controls.SilentQuoteFrame );
		end

		--Controls.QuoteLabel:SetText( Locale.ConvertTextKey( thisTech.Quote ) );
		--contentSize = Controls.QuoteLabel:GetSize();
		--frameSize.x = wideInnerFrameWidth;
		--frameSize.y = contentSize.y + textPaddingFromInnerFrame + quoteButtonOffset;
		--Controls.QuoteInnerFrame:SetSize( frameSize );
		--frameSize.x = wideOuterFrameWidth;
		--frameSize.y = contentSize.y + textPaddingFromInnerFrame - offsetsBetweenFrames + quoteButtonOffset;
		--Controls.QuoteFrame:SetSize( frameSize );
		--Controls.QuoteFrame:SetHide( false );

		-- update the special abilites
		local abilitiesString = "";
		local numAbilities = 0;
		for row in GameInfo.Route_TechMovementChanges( condition ) do
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_MOVEMENT", GameInfo.Routes[row.RouteType].Description);
			numAbilities = numAbilities + 1;
		end	
	
		for row in GameInfo.Improvement_TechYieldChanges( condition ) do
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_YIELDCHANGES", GameInfo.Improvements[row.ImprovementType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, Locale.ConvertTextKey(GameInfo.Yields[row.YieldType].IconString));
			numAbilities = numAbilities + 1;
		end	

		for row in GameInfo.Improvement_TechNoFreshWaterYieldChanges( condition ) do
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			
			abilitiesString = abilitiesString .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_NOFRESHWATERYIELDCHANGES", GameInfo.Improvements[row.ImprovementType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, Locale.ConvertTextKey(GameInfo.Yields[row.YieldType].IconString));
			numAbilities = numAbilities + 1;
		end	

		for row in GameInfo.Improvement_TechFreshWaterYieldChanges( condition ) do
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey("TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_FRESHWATERYIELDCHANGES", GameInfo.Improvements[row.ImprovementType].Description, GameInfo.Yields[row.YieldType].Description, row.Yield, Locale.ConvertTextKey(GameInfo.Yields[row.YieldType].IconString));
			numAbilities = numAbilities + 1;
		end	

		if thisTech.EmbarkedMoveChange > 0 then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_FAST_EMBARK_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.AllowsEmbarking then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ALLOWS_EMBARKING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.AllowsDefensiveEmbarking then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_DEFENSIVE_EMBARK_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.EmbarkedAllWaterPassage then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_OCEAN_EMBARK_STRING" );
			numAbilities = numAbilities + 1;
		end
--CBP
		if thisTech.Happiness > 0 then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_HAPPINESS_BUMP", thisTech.Happiness );
			numAbilities = numAbilities + 1
		end
		if thisTech.BombardRange > 0 then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_RANGE_INCREASE" );
			numAbilities = numAbilities + 1
		end
		if thisTech.BombardIndirect > 0 then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_INDIRECT_INCREASE" );
			numAbilities = numAbilities + 1;
		end
		if thisTech.CityLessEmbarkCost then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_LESS_EMBARK_COST_STRING" );
			numAbilities = numAbilities + 1;
		end
		if thisTech.CityNoEmbarkCost then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_NO_EMBARK_COST_STRING" );
			numAbilities = numAbilities + 1;
		end
		if thisTech.CorporationsEnabled then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString ..  Locale.ConvertTextKey( "TXT_KEY_ABLTY_ENABLES_CORPORATIONS" );
			numAbilities = numAbilities + 1;
		end
		for row in GameInfo.Tech_SpecialistYieldChanges( condition ) do
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey("TXT_KEY_SPECIALIST_YIELD_CHANGE", GameInfo.Specialists[row.SpecialistType].Description , GameInfo.Yields[row.YieldType].Description, row.Yield, Locale.ConvertTextKey(GameInfo.Yields[row.YieldType].IconString));
			numAbilities = numAbilities + 1;
		end	
-- END	
-- Putmalk: If this tech grants Research Agreements, and Research Agreements are enabled, then display it on the tech info page
		if (thisTech.ResearchAgreementTradingAllowed and g_bResearchAgreementTrading) then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_R_PACT_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		-- Putmalk: If this tech grants Map Trading then display it on the tech info page
		if thisTech.MapTrading then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_MAP_TRADING_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		-- Putmalk: If this tech grants Tech Trading, and Tech trading is active, then display it on the tech info page
		if (thisTech.TechTrading and g_bTechTrading) then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_TECH_TRADING_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		-- Putmalk: If this tech grants vassalage, and vassalage isn't disabled, then display it on the tech info page
		if (thisTech.VassalageTradingAllowed and not g_bNoVassalage) then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_VASSALAGE_STRING" );
			numAbilities = numAbilities + 1;
		end
-- END		
		if thisTech.AllowEmbassyTradingAllowed then
			if numAbilities > 0 then
				abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_ALLOW_EMBASSY_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.OpenBordersTradingAllowed then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_OPEN_BORDER_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.DefensivePactTradingAllowed then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_D_PACT_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.ResearchAgreementTradingAllowed then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_R_PACT_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.TradeAgreementTradingAllowed then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_T_PACT_STRING" );
			numAbilities = numAbilities + 1;
		end
	
		if thisTech.BridgeBuilding then
			if numAbilities > 0 then
				 abilitiesString = abilitiesString .. "[NEWLINE]";
			end
			abilitiesString = abilitiesString .. Locale.ConvertTextKey( "TXT_KEY_ABLTY_BRIDGE_STRING" );
			numAbilities = numAbilities + 1;
		end

		if numAbilities > 0 then
			UpdateTextBlock( Locale.ConvertTextKey( abilitiesString ), Controls.AbilitiesLabel, Controls.AbilitiesInnerFrame, Controls.AbilitiesFrame );
		else
			Controls.AbilitiesFrame:SetHide( true );			
		end
		
		-- update the historical info
		if (thisTech.Civilopedia) then
			UpdateTextBlock( Locale.ConvertTextKey( thisTech.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
		end
		
		-- update the related images
		Controls.RelatedImagesFrame:SetHide( true );
		
	end
	
	ResizeEtc();

end

CivilopediaCategory[CategoryUnits].SelectArticle = function( unitID, shouldAddToList )
	print("CivilopediaCategory[CategoryUnits].SelectArticle");
	if selectedCategory ~= CategoryUnits then
		SetSelectedCategory(CategoryUnits);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryUnits * absurdlyLargeNumTopicsInCategory) + unitID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if unitID ~= -1 then
		local thisUnit = GameInfo.Units[unitID];
					
		-- update the name
		local name = Locale.ConvertTextKey( thisUnit.Description ); 	
		Controls.ArticleID:SetText( name );

		-- update the portrait
		if IconHookup( thisUnit.PortraitIndex, portraitSize, thisUnit.IconAtlas, Controls.Portrait ) then
			Controls.PortraitFrame:SetHide( false );
		else
			Controls.PortraitFrame:SetHide( true );
		end

		-- update the cost
		Controls.CostFrame:SetHide( false );
		
		local costString = "";
		
		local cost = thisUnit.Cost;
		
		local faithCost = thisUnit.FaithCost;
		
		if(Game and not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
			faithCost = Game.GetFaithCost(unitID);
		end
		
		if(Game ~= nil) then
			cost = Players[Game.GetActivePlayer()]:GetUnitProductionNeeded( unitID );
		end
		
		if(cost == 1 and faithCost > 0) then
			costString = tostring(faithCost) .. " [ICON_PEACE]";
			
		elseif(cost > 0 and faithCost > 0) then
			costString = Locale.Lookup("TXT_KEY_PEDIA_A_OR_B", tostring(cost) .. " [ICON_PRODUCTION]", tostring(faithCost) .. " [ICON_PEACE");
		else
			if(cost > 0) then
				costString = tostring(cost) .. " [ICON_PRODUCTION]";
			elseif(faithCost > 0) then
				costString = tostring(faithCost) .. " [ICON_PEACE]";
			else
				costString = Locale.Lookup("TXT_KEY_FREE");
				
				if(thisUnit.Type == "UNIT_SETTLER") then
					Controls.CostFrame:SetHide(true);
				end
			end
		end
				
		Controls.CostLabel:SetText(costString);
		
		-- update the Combat value
		local combat = thisUnit.Combat;
		if combat > 0 then
			Controls.CombatLabel:SetText( tostring(combat).." [ICON_STRENGTH]" );
			Controls.CombatFrame:SetHide( false );
		end
		
		-- update the Ranged Combat value
		local rangedCombat = thisUnit.RangedCombat;
		if rangedCombat > 0 then
			Controls.RangedCombatLabel:SetText( tostring(rangedCombat).." [ICON_RANGE_STRENGTH]" );
			Controls.RangedCombatFrame:SetHide( false );
		end
		
		-- update the Ranged Combat value
		local rangedCombatRange = thisUnit.Range;
		if rangedCombatRange > 0 then
			Controls.RangedCombatRangeLabel:SetText( tostring(rangedCombatRange) );
			Controls.RangedCombatRangeFrame:SetHide( false );
		end
		
		-- update the Combat Type value
		if thisUnit.CombatClass then
			Controls.CombatTypeLabel:SetText( Locale.ConvertTextKey( GameInfo.UnitCombatInfos[thisUnit.CombatClass].Description ) );
			Controls.CombatTypeFrame:SetHide( false );
		end
		
		-- update the Movement value
		local movementRange = thisUnit.Moves;
		if movementRange > 0 then
			if thisUnit.Domain ~= "DOMAIN_AIR" then
				Controls.MovementLabel:SetText( tostring(movementRange).." [ICON_MOVES]" );
				Controls.MovementFrame:SetHide( false );
			end
		end

		-- CBO update the air defense value
		local airdefense = thisUnit.BaseLandAirDefense;
		if airdefense > 0 then
			Controls.AirDefenseLabel:SetText( tostring(airdefense).." [ICON_AIRSTRIKE_DEFENSE]" );
			Controls.AirDefenseFrame:SetHide( false );
		end

		
		
 		local contentSize;
 		local frameSize = {};
		local buttonAdded = 0;
 		
 		-- update the free promotions
		g_PromotionsManager:ResetInstances();
		buttonAdded = 0;

		local condition = "UnitType = '" .. thisUnit.Type .. "'";
		for row in GameInfo.Unit_FreePromotions( condition ) do
			local promotion = GameInfo.UnitPromotions[row.PromotionType];
			if promotion then
				local thisPromotionInstance = g_PromotionsManager:GetInstance();
				if thisPromotionInstance then
					local textureOffset, textureSheet = IconLookup( promotion.PortraitIndex, buttonSize, promotion.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisPromotionInstance.PromotionImage, thisPromotionInstance.PromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( promotion.Description ), promotion.ID );
					buttonAdded = buttonAdded + 1;
				end
			end	
		end
		UpdateButtonFrame( buttonAdded, Controls.FreePromotionsInnerFrame, Controls.FreePromotionsFrame );

		-- update the required resources
		Controls.RequiredResourcesLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_REQ_RESRC_LABEL" ) );
		g_RequiredResourcesManager:ResetInstances();
		buttonAdded = 0;

		local condition = "UnitType = '" .. thisUnit.Type .. "'";

		for row in GameInfo.Unit_ResourceQuantityRequirements( condition ) do
			local requiredResource = GameInfo.Resources[row.ResourceType];
			if requiredResource then
				local thisRequiredResourceInstance = g_RequiredResourcesManager:GetInstance();
				if thisRequiredResourceInstance then
					local textureOffset, textureSheet = IconLookup( requiredResource.PortraitIndex, buttonSize, requiredResource.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredResourceInstance.RequiredResourceImage, thisRequiredResourceInstance.RequiredResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( requiredResource.Description ), requiredResource.ID );
					buttonAdded = buttonAdded + 1;
				end
			end		
		end
		UpdateButtonFrame( buttonAdded, Controls.RequiredResourcesInnerFrame, Controls.RequiredResourcesFrame );

		-- update the prereq techs
		g_PrereqTechManager:ResetInstances();
		buttonAdded = 0;

		if thisUnit.PrereqTech then
			local prereq = GameInfo.Technologies[thisUnit.PrereqTech];
			if prereq then
				local thisPrereqInstance = g_PrereqTechManager:GetInstance();
				if thisPrereqInstance then
					local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisPrereqInstance.PrereqTechImage, thisPrereqInstance.PrereqTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		UpdateButtonFrame( buttonAdded, Controls.PrereqTechInnerFrame, Controls.PrereqTechFrame );

		-- update the obsolete techs
		g_ObsoleteTechManager:ResetInstances();
		buttonAdded = 0;

		if thisUnit.ObsoleteTech then
			local obs = GameInfo.Technologies[thisUnit.ObsoleteTech];
			if obs then
				local thisTechInstance = g_ObsoleteTechManager:GetInstance();
				if thisTechInstance then
					local textureOffset, textureSheet = IconLookup( obs.PortraitIndex, buttonSize, obs.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisTechInstance.ObsoleteTechImage, thisTechInstance.ObsoleteTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( obs.Description ), obs.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		UpdateButtonFrame( buttonAdded, Controls.ObsoleteTechInnerFrame, Controls.ObsoleteTechFrame );

		-- update the Upgrade units
		g_UpgradeManager:ResetInstances();
		buttonAdded = 0;
		
		if(Game ~= nil) then
			local iUnitUpgrade = Game.GetUnitUpgradesTo(unitID);
			if iUnitUpgrade ~= nil and iUnitUpgrade ~= -1 then
				local obs = GameInfo.Units[iUnitUpgrade];
				if obs then
					local thisUpgradeInstance = g_UpgradeManager:GetInstance();
					if thisUpgradeInstance then
						local textureOffset, textureSheet = IconLookup( obs.PortraitIndex, buttonSize, obs.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisUpgradeInstance.UpgradeImage, thisUpgradeInstance.UpgradeButton, textureSheet, textureOffset, CategoryUnits, Locale.ConvertTextKey( obs.Description ), obs.ID );
						buttonAdded = buttonAdded + 1;
					end	
				end
			end	
		else
			for row in GameInfo.Unit_ClassUpgrades{UnitType = thisUnit.Type} do	
				local unitClass = GameInfo.UnitClasses[row.UnitClassType];
				local upgradeUnit = GameInfo.Units[unitClass.DefaultUnit];
				if (upgradeUnit) then
					local thisUpgradeInstance = g_UpgradeManager:GetInstance();
					if thisUpgradeInstance then
						local textureOffset, textureSheet = IconLookup( upgradeUnit.PortraitIndex, buttonSize, upgradeUnit.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisUpgradeInstance.UpgradeImage, thisUpgradeInstance.UpgradeButton, textureSheet, textureOffset, CategoryUnits, Locale.ConvertTextKey( upgradeUnit.Description ), upgradeUnit.ID );
						buttonAdded = buttonAdded + 1;
					end	
				end		
			end
		end
		
		UpdateButtonFrame( buttonAdded, Controls.UpgradeInnerFrame, Controls.UpgradeFrame );
		
		-- Are we a unique unit?  If so, who do I replace?
		local replacesUnitClass = {};
		local specificCivs = {};
		
		local classOverrideCondition = string.format("UnitType='%s' and CivilizationType <> 'CIVILIZATION_BARBARIAN' and CivilizationType <> 'CIVILIZATION_MINOR'", thisUnit.Type);
		for row in GameInfo.Civilization_UnitClassOverrides(classOverrideCondition) do
			specificCivs[row.CivilizationType] = 1;
			replacesUnitClass[row.UnitClassType] = 1;
		end
		 	
		g_ReplacesManager:ResetInstances();
		buttonAdded = 0;
		for unitClassType, _ in pairs(replacesUnitClass) do
			for replacedUnit in DB.Query("SELECT u.ID, u.Description, u.PortraitIndex, u.IconAtlas from Units as u inner join UnitClasses as uc on u.Type = uc.DefaultUnit where uc.Type = ?", unitClassType) do
				local thisUnitInstance = g_ReplacesManager:GetInstance();
				if thisUnitInstance then
					local textureOffset, textureSheet = IconLookup( replacedUnit.PortraitIndex, buttonSize, replacedUnit.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisUnitInstance.ReplaceImage, thisUnitInstance.ReplaceButton, textureSheet, textureOffset, CategoryUnits, Locale.ConvertTextKey( replacedUnit.Description ), replacedUnit.ID );
					buttonAdded = buttonAdded + 1;
				end
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.ReplacesInnerFrame, Controls.ReplacesFrame );

		g_CivilizationsManager:ResetInstances();
		buttonAdded = 0;
		for civilizationType, _ in pairs(specificCivs) do
		
			local civ = GameInfo.Civilizations[civilizationType];
			if(civ ~= nil) then
				local thisCivInstance = g_CivilizationsManager:GetInstance();
				if thisCivInstance then
					local textureOffset, textureSheet = IconLookup( civ.PortraitIndex, buttonSize, civ.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisCivInstance.CivilizationImage, thisCivInstance.CivilizationButton, textureSheet, textureOffset, CategoryCivilizations, Locale.ConvertTextKey( civ.ShortDescription ), civ.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.CivilizationsInnerFrame, Controls.CivilizationsFrame );

		-- update the game info
		if thisUnit.Help then
			UpdateTextBlock( Locale.ConvertTextKey( thisUnit.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
		end
				
		-- update the strategy info
		if thisUnit.Strategy then
			UpdateTextBlock( Locale.ConvertTextKey( thisUnit.Strategy ), Controls.StrategyLabel, Controls.StrategyInnerFrame, Controls.StrategyFrame );
		end
		
		-- update the historical info
		if thisUnit.Civilopedia then
			UpdateTextBlock( Locale.ConvertTextKey( thisUnit.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
		end
		
		-- update the related images
		Controls.RelatedImagesFrame:SetHide( true );
		
		-- Infixo Extended info
		local sText = "[COLOR_CYAN]Abilities[ENDCOLOR] of this unit:"; -- change to TXT_KEY_ later
		local function AnalyzeUnit(...)
			sText = sText .. AnalyzeObjectField(thisUnit, ...);
		end
		AnalyzeUnit("Domain", "Domains", "Domain");
		--AnalyzeUnit("MinAreaSize");
		if thisUnit.MinAreaSize > 0 and thisUnit.MinAreaSize < 10 then sText = sText.."[NEWLINE][ICON_BULLET]Moves on [COLOR_CYAN]Sea[ENDCOLOR] and [COLOR_CYAN]Lake[ENDCOLOR]"; end
		if thisUnit.MinAreaSize >= 10 then sText = sText.."[NEWLINE][ICON_BULLET]Moves on [COLOR_CYAN]Sea[ENDCOLOR] only"; end
		AnalyzeUnit("CombatClass", "UnitCombatInfos", "Class");
		--AnalyzeUnit("IsMounted");
		if thisUnit.IsMounted == 1 then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_CYAN]Mounted[ENDCOLOR] unit"; end
		AnalyzeUnit("Special", "SpecialUnits", "Special unit");
		AnalyzeUnit("GoodyHutUpgradeUnitClass", "UnitClasses", "Upgraded by Ruins to");
		AnalyzeUnit("PolicyType", "Policies", "Requires policy");
		AnalyzeUnit("ProjectPrereq", "Projects", "Requires project");
		AnalyzeUnit("SpaceshipProject", "Projects", "Produced by spaceship project");
		AnalyzeUnit("ResourceType", "Resources", "Requires");
		AnalyzeUnit("BeliefRequired", "Beliefs", "Requires belief");
		AnalyzeUnit("Trade");
		--AnalyzeUnit("DefaultUnitAI", "UnitAIInfos", "AI"); -- AI stuff
		AnalyzeUnit("BaseSightRange", "[ICON_VIEW_CITY]");
		AnalyzeUnit("PurchaseOnly");
		AnalyzeUnit("MoveAfterPurchase");
		--AnalyzeUnit("RequiresFaithPurchaseEnabled"); -- internal
		AnalyzeUnit("Immobile");
		AnalyzeUnit("Capture", "UnitClasses", "Captured as");
		--AnalyzeUnit("CivilianAttackPriority"); -- AI stuff
		AnalyzeUnit("Food");
		AnalyzeUnit("NoBadGoodies");
		AnalyzeUnit("RivalTerritory");
		AnalyzeUnit("MilitarySupport");
		AnalyzeUnit("MilitaryProduction");
		AnalyzeUnit("Pillage");
		AnalyzeUnit("PillagePrereqTech");
		AnalyzeUnit("Found");
		AnalyzeUnit("FoundAbroad");
		AnalyzeUnit("CultureBombRadius");
		AnalyzeUnit("GoldenAgeTurns");
		AnalyzeUnit("FreePolicies");
		AnalyzeUnit("OneShotTourism");
		AnalyzeUnit("OneShotTourismPercentOthers");
		AnalyzeUnit("IgnoreBuildingDefense");
		AnalyzeUnit("PrereqResources");
		AnalyzeUnit("Mechanized");
		AnalyzeUnit("Suicide");
		AnalyzeUnit("CaptureWhileEmbarked");
		AnalyzeUnit("HurryCostModifier");
		AnalyzeUnit("AdvancedStartCost");
		AnalyzeUnit("AirInterceptRange", "[ICON_RANGE_STRENGTH]");
		AnalyzeUnit("AirUnitCap", "[ICON_AIRSTRIKE_DEFENSE]");
		AnalyzeUnit("NukeDamageLevel");
		AnalyzeUnit("WorkRate");
		AnalyzeUnit("NumFreeTechs", "");
		AnalyzeUnit("BaseBeakersTurnsToCount", "");
		AnalyzeUnit("BaseCultureTurnsToCount", "");
		AnalyzeUnit("RushBuilding");
		AnalyzeUnit("BaseHurry");
		AnalyzeUnit("HurryMultiplier");
		AnalyzeUnit("BaseGold", "");
		AnalyzeUnit("NumGoldPerEra");
		AnalyzeUnit("SpreadReligion");
		AnalyzeUnit("RemoveHeresy");
		AnalyzeUnit("ReligionSpreads", "[ICON_MISSIONARY]");
		AnalyzeUnit("ReligiousStrength", "[ICON_PEACE]");
		AnalyzeUnit("FoundReligion");
		AnalyzeUnit("RequiresEnhancedReligion");
		AnalyzeUnit("ProhibitsSpread");
		AnalyzeUnit("CanBuyCityState");
		--AnalyzeUnit("CombatLimit", ""); -- intrernal
		-- AnalyzeUnit("RangedCombatLimit"); -- internal
		AnalyzeUnit("RangeAttackOnlyInDomain");
		AnalyzeUnit("RangeAttackIgnoreLOS");
		AnalyzeUnit("NumExoticGoods", "");
		--AnalyzeUnit("XPValueAttack", ""); -- internal
		--AnalyzeUnit("XPValueDefense", ""); -- internal
		AnalyzeUnit("DomainCargo", "Domains", "Cargo domain");
		AnalyzeUnit("SpecialCargo", "SpecialUnits", "Special cargo");
		AnalyzeUnit("SpecialUnitCargoLoad", "SpecialUnits", "Special cargo load");
		AnalyzeUnit("Conscription", "");
		AnalyzeUnit("ExtraMaintenanceCost", "[ICON_GOLD]");
		--AnalyzeUnit("NoMaintenance");
		if thisUnit.NoMaintenance then sText = sText.."[NEWLINE][ICON_BULLET]Maintenance [COLOR_POSITIVE_TEXT]Free[ENDCOLOR]"; end
		AnalyzeUnit("Unhappiness");
		--AnalyzeUnit("UnitArtInfo");
		--AnalyzeUnit("UnitArtInfoCulturalVariation");
		--AnalyzeUnit("UnitArtInfoEraVariation");
		AnalyzeUnit("LeaderPromotion", "UnitPromotions", "Leader promotion");
		AnalyzeUnit("LeaderExperience");
		AnalyzeUnit("DontShowYields");
		--AnalyzeUnit("ShowInPedia");
		AnalyzeUnit("MoveRate"); -- MovementRates doesn't have Description
		AnalyzeUnit("FoundMid", "");
		AnalyzeUnit("FoundLate", "");
		AnalyzeUnit("CityAttackOnly", "");
		AnalyzeUnit("CulExpOnDisbandUpgrade");
		AnalyzeUnit("PuppetPurchaseOverride");
		AnalyzeUnit("GoodyModifier");
		AnalyzeUnit("SupplyCapBoost");
		AnalyzeUnit("NumFreeLux", "");
		AnalyzeUnit("GPExtra");
		AnalyzeUnit("IsConvertUnit");
		AnalyzeUnit("MinorCivGift", "");
		AnalyzeUnit("NoMinorCivGift", "");
		AnalyzeUnit("PurchaseCooldown", "");
		AnalyzeUnit("GlobalFaithPurchaseCooldown");
		--AnalyzeUnit("BaseLandAirDefense");
		AnalyzeUnit("FreeUpgrade");
		AnalyzeUnit("UnitEraUpgrade");
		AnalyzeUnit("ConvertOnDamage");
		AnalyzeUnit("ConvertUnit", "Units", "Convert unit");
		AnalyzeUnit("DamageThreshold");
		AnalyzeUnit("ConvertOnFullHP");
		AnalyzeUnit("WarOnly");
		AnalyzeUnit("ConvertEnemyUnitToBarbarian");
		AnalyzeUnit("WLTKDFromBirth");
		AnalyzeUnit("GoldenAgeFromBirth");
		AnalyzeUnit("CultureBoost");
		AnalyzeUnit("ExtraAttackHealthOnKill");
		AnalyzeUnit("HighSeaRaider");
		AnalyzeUnit("NumInfPerEra");
		AnalyzeUnit("SendCanMoveIntoEvent");
		AnalyzeUnit("CannotEmbark");
		AnalyzeUnit("NoMinorGifts");
		AnalyzeUnit("MoveAfterUpgrade");
		AnalyzeUnit("PromotionClass", "UnitCombatInfos", "Promotion class");
		AnalyzeUnit("CanRepairFleet");
		AnalyzeUnit("CanChangePort");
		AnalyzeUnit("NumberStackingUnits");
		AnalyzeUnit("StackCombat");
		--AnalyzeUnit("MaxHitPoints"); -- internal
		AnalyzeUnit("CargoCombat");
		--AnalyzeUnit("ObsoleteTech");
		if thisUnit.ObsoleteTech ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Obsoletes with [COLOR_NEGATIVE_TEXT]"..Locale.Lookup(GameInfo.Technologies[thisUnit.ObsoleteTech].Description).."[ENDCOLOR]"; end
		--------------------
		local bShow = false;
		local sql = [[
			SELECT BuildingClasses.Description
			FROM Unit_BuildingClassPurchaseRequireds INNER JOIN BuildingClasses ON Unit_BuildingClassPurchaseRequireds.BuildingClassType = BuildingClasses.Type
			WHERE Unit_BuildingClassPurchaseRequireds.UnitType = ?]];
		for row in DB.Query(sql, thisUnit.Type) do
			if not bShow then sText = sText.."[NEWLINE][COLOR_CYAN]Buildings[ENDCOLOR] required to purchase this unit:"; bShow = true; end -- change to TXT_KEY_ later
			sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description);
		end
		UpdateTextBlock( sText, Controls.ExtendedLabel,  Controls.ExtendedInnerFrame,  Controls.ExtendedFrame );
		-- end Infixo
	end
	
	ResizeEtc();

end

local defaultPromotionPortraitOffset = Vector2( 256, 256 );

CivilopediaCategory[CategoryPromotions].SelectArticle = function( promotionID, shouldAddToList )
	print("CivilopediaCategory[CategoryPromotions].SelectArticle");
	if selectedCategory ~= CategoryPromotions then
		SetSelectedCategory(CategoryPromotions);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryPromotions * absurdlyLargeNumTopicsInCategory) + promotionID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if promotionID ~= -1 then
		local thisPromotion = GameInfo.UnitPromotions[promotionID];
		
		-- update the name
		local name = Locale.ConvertTextKey( thisPromotion.Description ); 	
		Controls.ArticleID:SetText( name );
		
		-- portrait
		if IconHookup( thisPromotion.PortraitIndex, portraitSize, thisPromotion.IconAtlas, Controls.Portrait ) then
			Controls.PortraitFrame:SetHide( false );
		else
			Controls.PortraitFrame:SetHide( true );
		end
		
 		local contentSize;
 		local frameSize = {};
		local buttonAdded = 0;
 		
 		-- required promotions
		g_RequiredPromotionsManager:ResetInstances();
		
		local function CheckPromotionPrereqOr(sFieldName)
			if thisPromotion[sFieldName] == nil then return; end -- field is empty
			local thisReq = GameInfo.UnitPromotions[thisPromotion[sFieldName]];
			if thisReq == nil then return; end -- cannot find that promotion
			local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
			if thisRequiredPromotionInstance then
				local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end				
				UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
				buttonAdded = buttonAdded + 1;
			end	
		end
		CheckPromotionPrereqOr("PromotionPrereqOr1");
		CheckPromotionPrereqOr("PromotionPrereqOr2");
		CheckPromotionPrereqOr("PromotionPrereqOr3");
		CheckPromotionPrereqOr("PromotionPrereqOr4");
		CheckPromotionPrereqOr("PromotionPrereqOr5");
		CheckPromotionPrereqOr("PromotionPrereqOr6");
		--[[
		if thisPromotion.PromotionPrereqOr1 then
			local thisReq = GameInfo.UnitPromotions[thisPromotion.PromotionPrereqOr1];
			if thisReq then
				local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
				if thisRequiredPromotionInstance then
					local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		if thisPromotion.PromotionPrereqOr2 then
			local thisReq = GameInfo.UnitPromotions[thisPromotion.PromotionPrereqOr2];
			if thisReq then
				local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
				if thisRequiredPromotionInstance then
					local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		if thisPromotion.PromotionPrereqOr3 then
			local thisReq = GameInfo.UnitPromotions[thisPromotion.PromotionPrereqOr3];
			if thisReq then
				local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
				if thisRequiredPromotionInstance then
					local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end
		if thisPromotion.PromotionPrereqOr4 then
			local thisReq = GameInfo.UnitPromotions[thisPromotion.PromotionPrereqOr4];
			if thisReq then
				local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
				if thisRequiredPromotionInstance then
					local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end		
		if thisPromotion.PromotionPrereqOr5 then
			local thisReq = GameInfo.UnitPromotions[thisPromotion.PromotionPrereqOr5];
			if thisReq then
				local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
				if thisRequiredPromotionInstance then
					local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		if thisPromotion.PromotionPrereqOr6 then
			local thisReq = GameInfo.UnitPromotions[thisPromotion.PromotionPrereqOr6];
			if thisReq then
				local thisRequiredPromotionInstance = g_RequiredPromotionsManager:GetInstance();
				if thisRequiredPromotionInstance then
					local textureOffset, textureSheet = IconLookup( thisReq.PortraitIndex, buttonSize, thisReq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPromotionInstance.RequiredPromotionImage, thisRequiredPromotionInstance.RequiredPromotionButton, textureSheet, textureOffset, CategoryPromotions, Locale.ConvertTextKey( thisReq.Description ), thisReq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		--]]
		UpdateButtonFrame( buttonAdded, Controls.RequiredPromotionsInnerFrame, Controls.RequiredPromotionsFrame );

		-- update the game info
		if thisPromotion.Help then
			UpdateTextBlock( Locale.ConvertTextKey( thisPromotion.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
		end
		
		-- Infixo more info
		local sText = "[COLOR_CYAN]Abilities[ENDCOLOR] of this promotion:"; -- change to TXT_KEY_ later
		-- Generic info from main table
		local function AnalyzePromotion(sField, sSuffix)
			sText = sText .. AnalyzeObjectField(thisPromotion, sField, sSuffix);
		end
		--AnalyzePromotion("CannotBeChosen");
		--AnalyzePromotion("LostWithUpgrade");
		AnalyzePromotion("NotWithUpgrade");
		--AnalyzePromotion("InstaHeal");
		if thisPromotion.InstaHeal then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]Instant Heal[ENDCOLOR]"; end
		AnalyzePromotion("Leader");
		AnalyzePromotion("Blitz");
		AnalyzePromotion("Amphib");
		AnalyzePromotion("River");
		AnalyzePromotion("EnemyRoute");
		AnalyzePromotion("RivalTerritory");
		AnalyzePromotion("MustSetUpToRangedAttack");
		AnalyzePromotion("RangedSupportFire");
		AnalyzePromotion("CanMoveAfterAttacking");
		--AnalyzePromotion("AlwaysHeal");
		if thisPromotion.AlwaysHeal then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR]"; end
		--AnalyzePromotion("HealOutsideFriendly");
		if thisPromotion.HealOutsideFriendly then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_POSITIVE_TEXT]Heals outside friendly territory[ENDCOLOR]"; end
		AnalyzePromotion("HillsDoubleMove");
		AnalyzePromotion("RoughTerrainEndsTurn");
		AnalyzePromotion("IgnoreTerrainCost");
		AnalyzePromotion("HoveringUnit");
		AnalyzePromotion("FlatMovementCost");
		AnalyzePromotion("CanMoveImpassable");
		AnalyzePromotion("NoCapture");
		AnalyzePromotion("OnlyDefensive");
		AnalyzePromotion("NoDefensiveBonus");
		AnalyzePromotion("NukeImmune");
		AnalyzePromotion("HiddenNationality");
		AnalyzePromotion("AlwaysHostile");
		AnalyzePromotion("NoRevealMap");
		AnalyzePromotion("Recon");
		AnalyzePromotion("CanMoveAllTerrain");
		AnalyzePromotion("FreePillageMoves");
		AnalyzePromotion("AirSweepCapable");
		AnalyzePromotion("AllowsEmbarkation");
		AnalyzePromotion("EmbarkedAllWater");
		AnalyzePromotion("HealIfDestroyExcludesBarbarians");
		AnalyzePromotion("RangeAttackIgnoreLOS");
		AnalyzePromotion("CityAttackOnly");
		AnalyzePromotion("CaptureDefeatedEnemy");
		AnalyzePromotion("HealOnPillage");
		AnalyzePromotion("IgnoreGreatGeneralBenefit");
		AnalyzePromotion("IgnoreZOC");
		AnalyzePromotion("HasPostCombatPromotions");
		AnalyzePromotion("PostCombatPromotionsExclusive");
		AnalyzePromotion("RangedAttackModifier");
		AnalyzePromotion("InterceptionCombatModifier");
		AnalyzePromotion("InterceptionDefenseDamageModifier");
		AnalyzePromotion("AirSweepCombatModifier");
		AnalyzePromotion("ExtraAttacks", "");
		AnalyzePromotion("ExtraNavalMovement", "");
		--AnalyzePromotion("VisibilityChange");
		if thisPromotion.VisibilityChange ~= 0 then sText = sText.."[NEWLINE][ICON_BULLET]"..string.format("%+d", thisPromotion.VisibilityChange).." Visibility range"; end
		AnalyzePromotion("MovesChange", "");
		AnalyzePromotion("MoveDiscountChange");
		AnalyzePromotion("RangeChange", "");
		AnalyzePromotion("InterceptChanceChange", "");
		AnalyzePromotion("NumInterceptionChange", "");
		AnalyzePromotion("EvasionChange", "");
		AnalyzePromotion("CargoChange", "");
		AnalyzePromotion("EnemyHealChange", "HP");
		AnalyzePromotion("NeutralHealChange", "HP");
		AnalyzePromotion("FriendlyHealChange", "HP");
		AnalyzePromotion("SameTileHealChange", "HP");
		AnalyzePromotion("AdjacentTileHealChange", "HP");
		AnalyzePromotion("EnemyDamageChance");
		AnalyzePromotion("NeutralDamageChance");
		AnalyzePromotion("EnemyDamage", "");
		AnalyzePromotion("NeutralDamage", "");
		AnalyzePromotion("CombatPercent");
		AnalyzePromotion("CityAttack");
		AnalyzePromotion("CityDefense");
		AnalyzePromotion("RangedDefenseMod");
		AnalyzePromotion("HillsAttack");
		AnalyzePromotion("HillsDefense");
		AnalyzePromotion("OpenAttack");
		AnalyzePromotion("OpenRangedAttackMod");
		AnalyzePromotion("OpenDefense");
		AnalyzePromotion("RoughAttack");
		AnalyzePromotion("RoughRangedAttackMod");
		AnalyzePromotion("RoughDefense");
		AnalyzePromotion("AttackFortifiedMod");
		AnalyzePromotion("AttackWoundedMod");
		AnalyzePromotion("FlankAttackModifier");
		AnalyzePromotion("NearbyEnemyCombatMod");
		AnalyzePromotion("NearbyEnemyCombatRange", "");
		AnalyzePromotion("UpgradeDiscount");
		AnalyzePromotion("ExperiencePercent");
		AnalyzePromotion("AdjacentMod");
		AnalyzePromotion("AttackMod");
		AnalyzePromotion("DefenseMod");
		AnalyzePromotion("DropRange", "");
		AnalyzePromotion("GreatGeneral");
		AnalyzePromotion("GreatAdmiral");
		AnalyzePromotion("GreatGeneralModifier");
		AnalyzePromotion("GreatGeneralReceivesMovement");
		AnalyzePromotion("GreatGeneralCombatModifier");
		AnalyzePromotion("FriendlyLandsModifier");
		AnalyzePromotion("FriendlyLandsAttackModifier");
		AnalyzePromotion("OutsideFriendlyLandsModifier");
		AnalyzePromotion("HPHealedIfDestroyEnemy", "HP");
		AnalyzePromotion("ExtraWithdrawal", "");
		AnalyzePromotion("EmbarkExtraVisibility", "");
		AnalyzePromotion("EmbarkDefenseModifier");
		AnalyzePromotion("CapitalDefenseModifier");
		AnalyzePromotion("CapitalDefenseFalloff", "");
		AnalyzePromotion("CityAttackPlunderModifier");
		AnalyzePromotion("ReligiousStrengthLossRivalTerritory");
		AnalyzePromotion("TradeMissionInfluenceModifier", "");
		AnalyzePromotion("TradeMissionGoldModifier");
		AnalyzePromotion("GoldenAgeValueFromKills", "GAP");
		AnalyzePromotion("Sapper");
		AnalyzePromotion("HeavyCharge");
		--AnalyzePromotion("TechPrereq");
		if thisPromotion.TechPrereq ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]"..Locale.Lookup(GameInfo.Technologies[thisPromotion.TechPrereq].Description).."[ENDCOLOR]"; end
		AnalyzePromotion("Invisible");
		AnalyzePromotion("SeeInvisible");
		AnalyzePromotion("MinimumRangeRequired", "");
		AnalyzePromotion("MaximumRangeRequired", "");
		AnalyzePromotion("AttackFullyHealedMod");
		AnalyzePromotion("AttackAbove50HealthMod");
		AnalyzePromotion("AttackBelowEqual50HealthMod");
		AnalyzePromotion("SplashDamage", "");
		AnalyzePromotion("AOEDamageOnKill", "");
		AnalyzePromotion("AoEWhileFortified", "");
		AnalyzePromotion("WorkRateMod");
		AnalyzePromotion("ReconChange", "");
		AnalyzePromotion("PromotionDuration", "turns");
		AnalyzePromotion("IsLostOnMove");
		AnalyzePromotion("NegatesPromotion");
		AnalyzePromotion("ForcedDamageValue");
		AnalyzePromotion("ChangeDamageValue");
		AnalyzePromotion("CannotBeCaptured");
		AnalyzePromotion("BarbarianCombatBonus");
		AnalyzePromotion("MoraleBreakChance");
		--AnalyzePromotion("BarbarianOnly");
		if thisPromotion.BarbarianOnly == 1 then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_CYAN]Barbarian Only[ENDCOLOR]"; end
		AnalyzePromotion("CityStateOnly");
		--AnalyzePromotion("StrongerDamaged");
		if thisPromotion.StrongerDamaged == 1 then sText = sText.."[NEWLINE][ICON_BULLET]StrongerDamaged"; end
		AnalyzePromotion("MountainsDoubleMove");
		AnalyzePromotion("CombatBonusFromNearbyUnitClass");
		AnalyzePromotion("NearbyUnitClassBonusRange");
		AnalyzePromotion("NearbyUnitClassBonus");
		AnalyzePromotion("AddedFromNearbyPromotion");
		AnalyzePromotion("IsNearbyPromotion");
		AnalyzePromotion("NearbyRange");
		AnalyzePromotion("IsNearbyCityPromotion");
		AnalyzePromotion("IsNearbyFriendlyCityPromotion");
		AnalyzePromotion("IsNearbyEnemyCityPromotion");
		AnalyzePromotion("IsFriendlyLands");
		AnalyzePromotion("RequiredUnit");
		AnalyzePromotion("AirInterceptRangeChange", "");
		AnalyzePromotion("ConvertDomainUnit");
		AnalyzePromotion("ConvertDomain");
		AnalyzePromotion("WonderProductionModifier");
		AnalyzePromotion("LandAirDefenseBonus", "");
		AnalyzePromotion("PlagueChance");
		AnalyzePromotion("IsPlague");
		AnalyzePromotion("StackedGreatGeneralXP");
		AnalyzePromotion("GoodyHutYieldBonus");
		--AnalyzePromotion("GainsXPFromScouting");
		if thisPromotion.GainsXPFromScouting == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Gains XP from Scouting"; end
		--AnalyzePromotion("GainsXPFromPillaging");
		if thisPromotion.GainsXPFromPillaging == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Gains XP from Pillaging"; end
		--AnalyzePromotion("GainsXPFromSpotting");
		if thisPromotion.GainsXPFromSpotting == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Gains XP from Spotting"; end
		AnalyzePromotion("MultiAttackBonus");
		AnalyzePromotion("DamageReductionCityAssault");
		AnalyzePromotion("PillageBonusStrength");
		AnalyzePromotion("ReligiousPressureModifier");
		AnalyzePromotion("AdjacentCityDefenseMod");
		AnalyzePromotion("NearbyEnemyDamage", "");
		AnalyzePromotion("EnemyLands");
		AnalyzePromotion("AdjacentSameType");
		AnalyzePromotion("MilitaryProductionModifier");
		AnalyzePromotion("HighSeaRaider");
		AnalyzePromotion("AuraRangeChange", "");
		AnalyzePromotion("AuraEffectChange", "");
		AnalyzePromotion("IgnoreTerrainDamage");
		--if thisPromotion.CanCrossIce == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Can enter Ice tiles"; end
		--AnalyzePromotion("IgnoreFeatureDamage");
		if thisPromotion.IgnoreFeatureDamage == 1 then sText = sText.."[NEWLINE][ICON_BULLET]IgnoreFeatureDamage"; end
		AnalyzePromotion("ExtraTerrainDamage");
		--AnalyzePromotion("ExtraFeatureDamage");
		if thisPromotion.ExtraFeatureDamage == 1 then sText = sText.."[NEWLINE][ICON_BULLET]ExtraFeatureDamage"; end
		--AnalyzePromotion("CanCrossIce");
		if thisPromotion.CanCrossIce == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Can enter Ice tiles"; end
		--AnalyzePromotion("CanCrossMountains");
		if thisPromotion.CanCrossMountains == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Can enter Mountain tiles"; end
		--AnalyzePromotion("CanCrossOceans");
		if thisPromotion.CanCrossOceans == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Can enter Ocean tiles"; end
		--AnalyzePromotion("EmbarkedDeepWater");
		if thisPromotion.EmbarkedDeepWater == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Can embark onto Ocean tiles"; end
		AnalyzePromotion("NearbyImprovementCombatBonus");
		AnalyzePromotion("NearbyImprovementBonusRange");
		AnalyzePromotion("CombatBonusImprovement");
		AnalyzePromotion("MaxHitPointsChange", "");
		AnalyzePromotion("MaxHitPointsModifier");
		-- Domains
		for row in DB.Query("SELECT Domains.Description, UnitPromotions_Domains.Modifier FROM UnitPromotions_Domains INNER JOIN Domains ON UnitPromotions_Domains.DomainType = Domains.Type WHERE PromotionType = ?", thisPromotion.Type) do
			sText = sText.."[NEWLINE][ICON_BULLET]"..string.format("%+d", row.Modifier).."% strength against [COLOR_CYAN]"..Locale.Lookup(row.Description).."[ENDCOLOR]";
		end
		-- Classess
		for row in DB.Query("SELECT UnitClasses.Description, Modifier, Attack, Defense FROM UnitPromotions_UnitClasses INNER JOIN UnitClasses ON UnitPromotions_UnitClasses.UnitClassType = UnitClasses.Type WHERE PromotionType = ?", thisPromotion.Type) do
			sText = sText.."[NEWLINE][ICON_BULLET]";
			if row.Modifier ~= nil then sText = sText..string.format("%+d", row.Modifier).."% strength"; end
			if row.Attack ~= nil then sText = sText..string.format(" %+d", row.Attack).."% attack"; end
			if row.Defense ~= nil then sText = sText..string.format(" %+d", row.Defense).."% defense"; end
			sText = sText.." against [COLOR_CYAN]"..Locale.Lookup(row.Description).."[ENDCOLOR]";
		end
		-- Combat Mods
		for row in DB.Query("SELECT UnitCombatInfos.Description, Modifier FROM UnitPromotions_UnitCombatMods INNER JOIN UnitCombatInfos ON UnitPromotions_UnitCombatMods.UnitCombatType = UnitCombatInfos.Type WHERE PromotionType = ?", thisPromotion.Type) do
			sText = sText.."[NEWLINE][ICON_BULLET]"..string.format("%+d", row.Modifier).."% strength against [COLOR_CYAN]"..Locale.Lookup(row.Description).."[ENDCOLOR]";
		end
		-- Terrains
		for row in DB.Query("SELECT TerrainType, Attack, Defense, DoubleMove, Impassable, PassableTech FROM UnitPromotions_Terrains WHERE PromotionType = ?", thisPromotion.Type) do
			local sTerrain = Locale.Lookup(GameInfo.Terrains[row.TerrainType].Description);
			if row.DoubleMove then sText = sText.."[NEWLINE][ICON_BULLET]Double movement in "..sTerrain; end
			if row.Impassable then
				sText = sText.."[NEWLINE][ICON_BULLET]Cannot enter [COLOR_NEGATIVE_TEXT]"..sTerrain.."[ENDCOLOR]";
				if row.PassableTech ~= nil then sText = sText.." until [COLOR_CYAN]"..Locale.Lookup(GameInfo.Technologies[row.PassableTech].Description).."[ENDCOLOR]"; end
			end
			if row.Attack ~= 0 or row.Defense ~= 0 then
				sText = sText.."[NEWLINE][ICON_BULLET]";
				if row.Attack ~= 0 then sText = sText..string.format("%+d", row.Attack).."% attack"; end
				if row.Defense ~= 0 then sText = sText..string.format(" %+d", row.Defense).."% defense"; end
				sText = sText.." in "..sTerrain;
			end
		end
		-- Features
		for row in DB.Query("SELECT FeatureType, Attack, Defense, DoubleMove, Impassable, PassableTech FROM UnitPromotions_Features WHERE PromotionType = ?", thisPromotion.Type) do
			local sFeature = Locale.Lookup(GameInfo.Features[row.FeatureType].Description);
			if row.DoubleMove then sText = sText.."[NEWLINE][ICON_BULLET]Double movement in "..sFeature; end
			if row.Impassable then
				sText = sText.."[NEWLINE][ICON_BULLET]Cannot enter [COLOR_NEGATIVE_TEXT]"..sFeature.."[ENDCOLOR]";
				if row.PassableTech ~= nil then sText = sText.." until [COLOR_CYAN]"..Locale.Lookup(GameInfo.Technologies[row.PassableTech].Description).."[ENDCOLOR]"; end
			end
			if row.Attack ~= 0 or row.Defense ~= 0 then
				sText = sText.."[NEWLINE][ICON_BULLET]";
				if row.Attack ~= 0 then sText = sText..string.format("%+d", row.Attack).."% attack"; end
				if row.Defense ~= 0 then sText = sText..string.format(" %+d", row.Defense).."% defense"; end
				sText = sText.." in "..sFeature;
			end
		end
		-- Negatives at the end
		if thisPromotion.CannotBeChosen then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_NEGATIVE_TEXT]Cannot be chosen[ENDCOLOR]"; end
		if thisPromotion.LostWithUpgrade then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_NEGATIVE_TEXT]Lost with Upgrade[ENDCOLOR]"; end
		-- Promo lines (Unit Combats)
		sText = sText.."[NEWLINE][COLOR_CYAN]Combat Types[ENDCOLOR] eligible for this promotion:"; -- change to TXT_KEY_ later
		for row in DB.Query("SELECT UnitCombatInfos.Description FROM UnitPromotions_UnitCombats INNER JOIN UnitCombatInfos ON UnitPromotions_UnitCombats.UnitCombatType = UnitCombatInfos.Type WHERE UnitPromotions_UnitCombats.PromotionType = ? ORDER BY UnitPromotions_UnitCombats.UnitCombatType", thisPromotion.Type) do
			sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description);
		end
		-- Units list
		sText = sText.."[NEWLINE][COLOR_CYAN]Units[ENDCOLOR] that are granted this promotion:"; -- change to TXT_KEY_ later
		for row in DB.Query("SELECT Units.Description FROM Unit_FreePromotions INNER JOIN Units ON Unit_FreePromotions.UnitType = Units.Type WHERE Unit_FreePromotions.PromotionType = ? ORDER BY Unit_FreePromotions.UnitType", thisPromotion.Type) do
			sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description);
		end
		UpdateTextBlock( sText, Controls.ExtendedLabel,  Controls.ExtendedInnerFrame,  Controls.ExtendedFrame );
		-- end Infixo
	end	

	ResizeEtc();

end

function SelectBuildingOrWonderArticle( buildingID )
	if buildingID ~= -1 then
		local thisBuilding = GameInfo.Buildings[buildingID];
					
		-- update the name
		local name = Locale.ConvertTextKey( thisBuilding.Description ); 	
		Controls.ArticleID:SetText( name );

		-- update the portrait
		if IconHookup( thisBuilding.PortraitIndex, portraitSize, thisBuilding.IconAtlas, Controls.Portrait ) then
			Controls.PortraitFrame:SetHide( false );
		else
			Controls.PortraitFrame:SetHide( true );
		end
		
		-- update the cost
		Controls.CostFrame:SetHide( false );
		local costString = "";
		
		local cost = thisBuilding.Cost;
		if(Game ~= nil) then
			cost = Players[Game.GetActivePlayer()]:GetBuildingProductionNeeded( buildingID );
		end
		
		local faithCost = thisBuilding.FaithCost;
		if(Game and Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
			faithCost = 0;
		end
		
		local costPerPlayer = 0;
		for tLeagueProject in GameInfo.LeagueProjects() do
			if (tLeagueProject ~= nil) then
				for iTier = 1, 3, 1 do
					if (tLeagueProject["RewardTier" .. iTier] ~= nil) then
						local tReward = GameInfo.LeagueProjectRewards[tLeagueProject["RewardTier" .. iTier]];
						if (tReward ~= nil and tReward.Building ~= nil) then
							if (GameInfo.Buildings[tReward.Building] ~= nil and GameInfo.Buildings[tReward.Building].ID == buildingID) then
								costPerPlayer = tLeagueProject.CostPerPlayer;
								if (Game ~= nil and Game.GetNumActiveLeagues() > 0) then
									local pLeague = Game.GetActiveLeague();
									if (pLeague ~= nil) then
										costPerPlayer = pLeague:GetProjectCostPerPlayer(tLeagueProject.ID) / 100;
									end
								end
							end
						end
					end
				end
			end
		end
		
		if(costPerPlayer > 0) then
			costString = Locale.ConvertTextKey("TXT_KEY_LEAGUE_PROJECT_COST_PER_PLAYER", costPerPlayer);
		elseif(cost == 1 and faithCost > 0) then
			costString = tostring(faithCost) .. " [ICON_PEACE]";
		elseif(cost > 0 and faithCost > 0) then
			costString = Locale.Lookup("TXT_KEY_PEDIA_A_OR_B", tostring(cost) .. " [ICON_PRODUCTION]", tostring(faithCost) .. " [ICON_PEACE");
		else
			if(cost > 0) then
				costString = tostring(cost).. " [ICON_PRODUCTION]";
			elseif(faithCost > 0) then
				costString = tostring(faithCost) .. " [ICON_PEACE]";
			else
				costString = Locale.Lookup("TXT_KEY_FREE");
			end
		end
		
		Controls.CostLabel:SetText(costString);
		
		-- update the maintenance
		local perTurnCost = thisBuilding.GoldMaintenance;
		if perTurnCost > 0 then
			Controls.MaintenanceLabel:SetText( tostring(perTurnCost).." [ICON_GOLD]" );
			Controls.MaintenanceFrame:SetHide( false );
		end

		-- update the Corporation (if exists) - CBP
		-- loop through Corporations, find any that apply to this
		local iCorp = 0;
		for row in GameInfo.Corporations() do
			-- our building class
			local buildingClass = GameInfo.BuildingClasses[thisBuilding.BuildingClass];
			if buildingClass then
				-- HQ, office, or franchise of a corporation?
				if (row.HeadquartersBuildingClass == buildingClass.Type or row.OfficeBuildingClass == buildingClass.Type or row.FranchiseBuildingClass == buildingClass.Type) then
					local thisCorporationInstance = g_CorporationsManager:GetInstance();
					if thisCorporationInstance then
						local textureOffset, textureSheet = IconLookup( row.PortraitIndex, buttonSize, row.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( 0, thisCorporationInstance.CorporationImage, thisCorporationInstance.CorporationButton, textureSheet, textureOffset, CategoryCorporations, Locale.ConvertTextKey( row.Description ), row.ID );
						iCorp = iCorp + 1;
						-- find the first one
						break;
					end
				end
			end
		end
		UpdateButtonFrame( 1, Controls.CorporationInnerFrame, Controls.CorporationFrame );
		if(iCorp > 0) then
			Controls.CorporationFrame:SetHide(false);
			Controls.CorporationInnerFrame:SetHide(false);
		else
			Controls.CorporationFrame:SetHide(true);
			Controls.CorporationInnerFrame:SetHide(true);
		end
		
		-- update the Happiness
		local iHappiness = thisBuilding.Happiness;
		if iHappiness > 0 then
			Controls.HappinessLabel:SetText( tostring(iHappiness).." [ICON_HAPPINESS_1]" );
			Controls.HappinessFrame:SetHide( false );
		end

		-- Unmodded Happiness
		iHappiness = thisBuilding.UnmoddedHappiness;
		if iHappiness > 0 then
			Controls.UnmoddedHappinessLabel:SetText( tostring(iHappiness).." [ICON_HAPPINESS_1]" );
			Controls.UnmoddedHappinessFrame:SetHide( false );
		end

		local GetBuildingYieldChange = function(buildingID, yieldType)
			if(Game ~= nil) then
				return Game.GetBuildingYieldChange(buildingID, YieldTypes[yieldType]);
			else
				local yieldModifier = 0;
				local buildingType = GameInfo.Buildings[buildingID].Type;
				for row in GameInfo.Building_YieldChanges{BuildingType = buildingType, YieldType = yieldType} do
					yieldModifier = yieldModifier + row.Yield;
				end
				
				return yieldModifier;
			end
		
		end

		-- update the Culture
		local iCulture = GetBuildingYieldChange(buildingID, "YIELD_CULTURE");
		if iCulture > 0 then
			Controls.CultureLabel:SetText( tostring(iCulture).." [ICON_CULTURE]" );
			Controls.CultureFrame:SetHide( false );
		end

		-- update the Faith
		local iFaith = GetBuildingYieldChange(buildingID, "YIELD_FAITH");
		if iFaith > 0 then
			Controls.FaithLabel:SetText( tostring(iFaith).." [ICON_PEACE]" );
			Controls.FaithFrame:SetHide( false );
		end

		-- update the Defense
		local defenseEntries = {};
		local iDefense = thisBuilding.Defense;
		if iDefense > 0 then
			table.insert(defenseEntries, tostring(iDefense / 100).." [ICON_STRENGTH]");
		end
		
		local iExtraHitPoints = thisBuilding.ExtraCityHitPoints;
		if(iExtraHitPoints > 0) then
			table.insert(defenseEntries, Locale.Lookup("TXT_KEY_PEDIA_DEFENSE_HITPOINTS", iExtraHitPoints));
		end
		
		if(#defenseEntries > 0) then
			Controls.DefenseLabel:SetText(table.concat(defenseEntries, ", "));
			Controls.DefenseFrame:SetHide(false);
		else
			Controls.DefenseFrame:SetHide(true);
		end
		
		local GetBuildingYieldModifier = function(buildingID, yieldType)
			if(Game ~= nil) then
				return Game.GetBuildingYieldModifier(buildingID, YieldTypes[yieldType]);
			else
				local yieldModifier = 0;
				local buildingType = GameInfo.Buildings[buildingID].Type;
				for row in GameInfo.Building_YieldModifiers{BuildingType = buildingType, YieldType = yieldType} do
					yieldModifier = yieldModifier + row.Yield;
				end
				
				return yieldModifier;
			end
			
		end
		
		-- Use Game to calculate Yield Changes and modifiers.
		-- update the Food Change
		local iFood = GetBuildingYieldChange(buildingID, "YIELD_FOOD");
		if (iFood > 0) then
			Controls.FoodLabel:SetText( "+" .. tostring(iFood).." [ICON_FOOD]" );
			Controls.FoodFrame:SetHide( false );
		end

		-- update the Gold Change
		local iGold = GetBuildingYieldChange(buildingID, "YIELD_GOLD");
		if (iGold > 0) then
			Controls.GoldChangeLabel:SetText( "+" .. tostring(iGold).." [ICON_GOLD]" );
			Controls.GoldChangeFrame:SetHide( false );
		end

		-- update the Gold
		local iGold = GetBuildingYieldModifier(buildingID, "YIELD_GOLD");
		if (iGold > 0) then
			Controls.GoldLabel:SetText( "+" .. tostring(iGold).."% [ICON_GOLD]" );
			Controls.GoldFrame:SetHide( false );
		end

		-- update the Science
		local scienceItems = {};
		local iScience = GetBuildingYieldModifier(buildingID, "YIELD_SCIENCE");
		if(iScience > 0) then
			table.insert(scienceItems, "+" .. tostring(iScience).."% [ICON_RESEARCH]" );
		end
		
		-- update the Science Change
		local iScience = GetBuildingYieldChange(buildingID, "YIELD_SCIENCE");
		if(iScience > 0) then
			table.insert(scienceItems, "+" .. tostring(iScience).." [ICON_RESEARCH]" );
		end
		
		if(#scienceItems > 0) then
			Controls.ScienceLabel:SetText( table.concat(scienceItems, ", ") );
			Controls.ScienceFrame:SetHide( false );
		end

		-- update the Production % mods
		local productionItems = {};
		local iProduction = GetBuildingYieldModifier(buildingID, "YIELD_PRODUCTION");
		if(iProduction > 0) then
			table.insert(productionItems, "+" .. tostring(iProduction).."% [ICON_PRODUCTION]");
		end
        
		-- update the Production
		local iProduction = GetBuildingYieldChange(buildingID, "YIELD_PRODUCTION");
		if(iProduction > 0) then
			table.insert(productionItems, "+" .. tostring(iProduction).." [ICON_PRODUCTION]");
		end
        
		if(#productionItems > 0) then
			Controls.ProductionLabel:SetText( table.concat(productionItems, ", ") );
			Controls.ProductionFrame:SetHide( false );
		end

		-- update the Great People
		local iGPType = thisBuilding.SpecialistType;
		if iGPType ~= nil then
			local iNumPoints = thisBuilding.GreatPeopleRateChange;
			if (iNumPoints > 0) then
				local strLabel = Locale.ConvertTextKey(GameInfo.Specialists[iGPType].GreatPeopleTitle);
				Controls.GPTitle:SetText( strLabel );
				Controls.GreatPeopleLabel:SetText( tostring(iNumPoints).." [ICON_GREAT_PEOPLE]" );
				Controls.GreatPeopleFrame:SetHide( false );
			end
		end

 		local contentSize;
 		local frameSize = {};
		local buttonAdded = 0;

		-- update the prereq techs
		g_PrereqTechManager:ResetInstances();

		if thisBuilding.PrereqTech then
			local prereq = GameInfo.Technologies[thisBuilding.PrereqTech];
			if prereq then
				local thisPrereqInstance = g_PrereqTechManager:GetInstance();
				if thisPrereqInstance then
					local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisPrereqInstance.PrereqTechImage, thisPrereqInstance.PrereqTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
					buttonAdded = buttonAdded + 1;
				end	
			end
		end	
		UpdateButtonFrame( buttonAdded, Controls.PrereqTechInnerFrame, Controls.PrereqTechFrame );

		local condition = "BuildingType = '" .. thisBuilding.Type .. "'";

		-- SpecialistType
		g_SpecialistsManager:ResetInstances();
		buttonAdded = 0;

		if (thisBuilding.SpecialistCount > 0 and thisBuilding.SpecialistType) then
			local thisSpec = GameInfo.Specialists[thisBuilding.SpecialistType];
			if(thisSpec)  then
				for i = 1, thisBuilding.SpecialistCount, 1 do
					local thisSpecialistInstance = g_SpecialistsManager:GetInstance();
					if thisSpecialistInstance then
						local textureOffset, textureSheet = IconLookup( thisSpec.PortraitIndex, buttonSize, thisSpec.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisSpecialistInstance.SpecialistImage, thisSpecialistInstance.SpecialistButton, textureSheet, textureOffset, CategoryPeople, Locale.ConvertTextKey( thisSpec.Description ), thisSpec.ID );
						buttonAdded = buttonAdded + 1;
					end	
				end
			end
		end	
		UpdateButtonFrame( buttonAdded, Controls.SpecialistsInnerFrame, Controls.SpecialistsFrame );
		
		-- required buildings
		g_RequiredBuildingsManager:ResetInstances();
		buttonAdded = 0;
		for row in GameInfo.Building_ClassesNeededInCity( condition ) do
			local buildingClass = GameInfo.BuildingClasses[row.BuildingClassType];
			if(buildingClass) then
				local thisBuildingInfo = GameInfo.Buildings[buildingClass.DefaultBuilding];
				if (thisBuildingInfo) then
					local thisBuildingInstance = g_RequiredBuildingsManager:GetInstance();
					if thisBuildingInstance then

						if not IconHookup( thisBuildingInfo.PortraitIndex, buttonSize, thisBuildingInfo.IconAtlas, thisBuildingInstance.RequiredBuildingImage ) then
							thisBuildingInstance.RequiredBuildingImage:SetTexture( defaultErrorTextureSheet );
							thisBuildingInstance.RequiredBuildingImage:SetTextureOffset( nullOffset );
						end
						
						--move this button
						thisBuildingInstance.RequiredBuildingButton:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );
						
						thisBuildingInstance.RequiredBuildingButton:SetToolTipString( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
						thisBuildingInstance.RequiredBuildingButton:SetVoids( thisBuildingInfo.ID, addToList );
						local thisBuildingClass = GameInfo.BuildingClasses[thisBuildingInfo.BuildingClass];
						if thisBuildingClass.MaxGlobalInstances > 0 or (thisBuildingClass.MaxPlayerInstances == 1 and thisBuildingInfo.SpecialistCount == 0) or thisBuildingClass.MaxTeamInstances > 0 then
							thisBuildingInstance.RequiredBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
						else
							thisBuildingInstance.RequiredBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
						end
						buttonAdded = buttonAdded + 1;
					end
				end
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.RequiredBuildingsInnerFrame, Controls.RequiredBuildingsFrame );
		
		-- CBP
		local condition = "BuildingClassType = '" .. thisBuilding.BuildingClass .. "'";
		-- Leads to Building
		g_LeadsToBuildingsManager:ResetInstances();
		buttonAdded = 0;
		for row in GameInfo.Building_ClassesNeededInCity( condition ) do
			local thisBuildingInfo = GameInfo.Buildings[row.BuildingType];
			if(thisBuildingInfo) then
				local thisBuildingInstance = g_LeadsToBuildingsManager:GetInstance();
				if thisBuildingInstance then
					if not IconHookup( thisBuildingInfo.PortraitIndex, buttonSize, thisBuildingInfo.IconAtlas, thisBuildingInstance.LeadsToBuildingImage ) then
						thisBuildingInstance.LeadsToBuildingImage:SetTexture( defaultErrorTextureSheet );
						thisBuildingInstance.LeadsToBuildingImage:SetTextureOffset( nullOffset );
					end
					
					--move this button
					thisBuildingInstance.LeadsToBuildingButton:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );
					
					thisBuildingInstance.LeadsToBuildingButton:SetToolTipString( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
					thisBuildingInstance.LeadsToBuildingButton:SetVoids( thisBuildingInfo.ID, addToList );
					local thisBuildingClass = GameInfo.BuildingClasses[thisBuildingInfo.BuildingClass];
					if thisBuildingClass.MaxGlobalInstances > 0 or (thisBuildingClass.MaxPlayerInstances == 1 and thisBuildingInfo.SpecialistCount == 0) or thisBuildingClass.MaxTeamInstances > 0 then
						thisBuildingInstance.LeadsToBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
					else
						thisBuildingInstance.LeadsToBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
					end
					buttonAdded = buttonAdded + 1;
				end
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.LeadsToBuildingsInnerFrame, Controls.LeadsToBuildingsFrame );
		-- END
		local condition = "BuildingType = '" .. thisBuilding.Type .. "'";
		-- needed local resources
		g_LocalResourcesManager:ResetInstances();
		buttonAdded = 0;

		for row in GameInfo.Building_LocalResourceAnds( condition ) do
			local requiredResource = GameInfo.Resources[row.ResourceType];
			if requiredResource then
				local thisLocalResourceInstance = g_LocalResourcesManager:GetInstance();
				if thisLocalResourceInstance then
					local textureOffset, textureSheet = IconLookup( requiredResource.PortraitIndex, buttonSize, requiredResource.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisLocalResourceInstance.LocalResourceImage, thisLocalResourceInstance.LocalResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( requiredResource.Description ), requiredResource.ID );
					buttonAdded = buttonAdded + 1;
				end
			end		
		end
		UpdateButtonFrame( buttonAdded, Controls.LocalResourcesInnerFrame, Controls.LocalResourcesFrame );
		
		-- CBP
		g_MonopolyResourcesManager:ResetInstances();
		buttonAdded = 0;

		for row in GameInfo.Building_ResourceMonopolyOrs( condition ) do
			local requiredResource = GameInfo.Resources[row.ResourceType];
			if requiredResource then
				local thisLocalResourceInstance = g_MonopolyResourcesManager:GetInstance();
				if thisLocalResourceInstance then
					local textureOffset, textureSheet = IconLookup( requiredResource.PortraitIndex, buttonSize, requiredResource.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisLocalResourceInstance.MonopolyResourceImage, thisLocalResourceInstance.MonopolyResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( requiredResource.Description ), requiredResource.ID );
					buttonAdded = buttonAdded + 1;
				end
			end		
		end
		UpdateButtonFrame( buttonAdded, Controls.MonopolyResourcesInnerFrame, Controls.MonopolyResourcesFrame );
	
		--END
		
		-- update the required resources
		Controls.RequiredResourcesLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_REQ_RESRC_LABEL" ) );
		g_RequiredResourcesManager:ResetInstances();
		buttonAdded = 0;

		for row in GameInfo.Building_ResourceQuantityRequirements( condition ) do
			local requiredResource = GameInfo.Resources[row.ResourceType];
			if requiredResource then
				local thisRequiredResourceInstance = g_RequiredResourcesManager:GetInstance();
				if thisRequiredResourceInstance then
					local textureOffset, textureSheet = IconLookup( requiredResource.PortraitIndex, buttonSize, requiredResource.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredResourceInstance.RequiredResourceImage, thisRequiredResourceInstance.RequiredResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( requiredResource.Description ), requiredResource.ID );
					buttonAdded = buttonAdded + 1;
				end
			end		
		end
		UpdateButtonFrame( buttonAdded, Controls.RequiredResourcesInnerFrame, Controls.RequiredResourcesFrame );

		-- Are we a unique building?  If so, who do I replace?
		g_ReplacesManager:ResetInstances();
		buttonAdded = 0;
		local defaultBuilding = nil;
		local thisCiv = nil;
		for row in GameInfo.Civilization_BuildingClassOverrides( condition ) do
			if row.CivilizationType ~= "CIVILIZATION_BARBARIAN" and row.CivilizationType ~= "CIVILIZATION_MINOR" then
				local otherCondition = "Type = '" .. row.BuildingClassType .. "'";
				for classrow in GameInfo.BuildingClasses( otherCondition ) do
					defaultBuilding = GameInfo.Buildings[classrow.DefaultBuilding];
				end
				if defaultBuilding then
					thisCiv = GameInfo.Civilizations[row.CivilizationType];
					break;
				end
			end
		end
		if defaultBuilding then
			local thisBuildingInstance = g_ReplacesManager:GetInstance();
			if thisBuildingInstance then
				local textureOffset, textureSheet = IconLookup( defaultBuilding.PortraitIndex, buttonSize, defaultBuilding.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end				
				UpdateSmallButton( buttonAdded, thisBuildingInstance.ReplaceImage, thisBuildingInstance.ReplaceButton, textureSheet, textureOffset, CategoryBuildings, Locale.ConvertTextKey( defaultBuilding.Description ), defaultBuilding.ID );
				buttonAdded = buttonAdded + 1;
			end
		end
		UpdateButtonFrame( buttonAdded, Controls.ReplacesInnerFrame, Controls.ReplacesFrame );

		buttonAdded = 0;
		if thisCiv then
			g_CivilizationsManager:ResetInstances();
			local thisCivInstance = g_CivilizationsManager:GetInstance();
			if thisCivInstance then
				local textureOffset, textureSheet = IconLookup( thisCiv.PortraitIndex, buttonSize, thisCiv.IconAtlas );				
				if textureOffset == nil then
					textureSheet = defaultErrorTextureSheet;
					textureOffset = nullOffset;
				end				
				UpdateSmallButton( buttonAdded, thisCivInstance.CivilizationImage, thisCivInstance.CivilizationButton, textureSheet, textureOffset, CategoryCivilizations, Locale.ConvertTextKey( thisCiv.ShortDescription ), thisCiv.ID );
				buttonAdded = buttonAdded + 1;
			end	
		end
		UpdateButtonFrame( buttonAdded, Controls.CivilizationsInnerFrame, Controls.CivilizationsFrame );
		
		-- update the great works box
		buttonAdded = 0;
		g_GreatWorksManager:ResetInstances();
		if(thisBuilding.GreatWorkCount > 0) then
			for i = 1, thisBuilding.GreatWorkCount, 1 do
				local greatWorkSlot = GameInfo.GreatWorkSlots[thisBuilding.GreatWorkSlotType];
				local emptyTexture = greatWorkSlot.EmptyIcon;
			
				local thisGreatWorksInstance = g_GreatWorksManager:GetInstance();
				
				
				UpdateSmallButton( buttonAdded, thisGreatWorksInstance.GreatWorksImage, thisGreatWorksInstance.GreatWorksButton, emptyTexture, nil, nil, Locale.Lookup(greatWorkSlot.EmptyToolTipText), nil);
				buttonAdded = buttonAdded + 1;			
			end
		end	
		UpdateButtonFrame( buttonAdded, Controls.GreatWorksInnerFrame, Controls.GreatWorksFrame );

		-- update the game info
		if thisBuilding.Help then
			-- Don't add text if it's the same as the strategy text
			if (thisBuilding.Help ~= thisBuilding.Strategy) then
				UpdateTextBlock( Locale.ConvertTextKey( thisBuilding.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end
		end
				
		-- update the strategy info
		if thisBuilding.Strategy then
			UpdateTextBlock( Locale.ConvertTextKey( thisBuilding.Strategy ), Controls.StrategyLabel, Controls.StrategyInnerFrame, Controls.StrategyFrame );
		end
		
		-- update the historical info
		if thisBuilding.Civilopedia then
			UpdateTextBlock( Locale.ConvertTextKey( thisBuilding.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
		end
		
		if thisBuilding.Quote then
			UpdateTextBlock( Locale.ConvertTextKey( thisBuilding.Quote ), Controls.SilentQuoteLabel, Controls.SilentQuoteInnerFrame, Controls.SilentQuoteFrame );
		end

		-- Infixo more info
		local sText = "[COLOR_CYAN]Features[ENDCOLOR] of this building:"; -- change to TXT_KEY_ later
		-- Generic info from main table
		local function AnalyzeBuilding(sField, sSuffix)
			--print("AnalyzeBuilding", sField, sSuffix)
			sText = sText .. AnalyzeObjectField(thisBuilding, sField, sSuffix);
		end
		--AnalyzeBuilding("CapitalOnly", "");
		if thisBuilding.CapitalOnly == 1 then sText = sText.."[NEWLINE][ICON_BULLET]Only in [ICON_CAPITAL] [COLOR_CYAN]Capital[ENDCOLOR]"; end
		--AnalyzeBuilding("UnlockedByBelief");
		if thisBuilding.UnlockedByBelief then sText = sText.."[NEWLINE][ICON_BULLET]Unlocked by [COLOR_CYAN]Belief[ENDCOLOR]"; end
		--AnalyzeBuilding("UnlockedByLeague");
		if thisBuilding.UnlockedByLeague then sText = sText.."[NEWLINE][ICON_BULLET]Unlocked by [COLOR_CYAN]World Congress[ENDCOLOR]"; end
		--AnalyzeBuilding("PolicyBranchType");
		if thisBuilding.PolicyBranchType ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]"..Locale.Lookup(GameInfo.PolicyBranchTypes[thisBuilding.PolicyBranchType].Description).."[ENDCOLOR]"; end
		--AnalyzeBuilding("PolicyType");
		if thisBuilding.PolicyType ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]"..Locale.Lookup(GameInfo.Policies[thisBuilding.PolicyType].Description).."[ENDCOLOR]"; end
		--AnalyzeBuilding("IsCorporation");
		if thisBuilding.IsCorporation == 1 then sText = sText.."[NEWLINE][ICON_BULLET][COLOR_CYAN]Corporation[ENDCOLOR]"; end
		AnalyzeBuilding("TeamShare");
		AnalyzeBuilding("Water");
		--AnalyzeBuilding("MinAreaSize");
		if thisBuilding.MinAreaSize > 0 and thisBuilding.MinAreaSize < 10 then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]Lake[ENDCOLOR]"; end
		if thisBuilding.MinAreaSize >= 10 then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]Coast[ENDCOLOR]"; end
		AnalyzeBuilding("River");
		AnalyzeBuilding("FreshWater");
		AnalyzeBuilding("Mountain");
		AnalyzeBuilding("NearbyMountainRequired");
		AnalyzeBuilding("Hill");
		AnalyzeBuilding("Flat");
		--AnalyzeBuilding("NearbyTerrainRequired");
		if thisBuilding.NearbyTerrainRequired ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]"..Locale.Lookup(GameInfo.Terrains[thisBuilding.NearbyTerrainRequired].Description).."[ENDCOLOR]"; end
		--AnalyzeBuilding("ProhibitedCityTerrain");
		if thisBuilding.ProhibitedCityTerrain ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Prohibited [COLOR_NEGATIVE_TEXT]"..Locale.Lookup(GameInfo.Terrains[thisBuilding.ProhibitedCityTerrain].Description).."[ENDCOLOR]"; end
		AnalyzeBuilding("FoundsReligion");
		AnalyzeBuilding("IsReligious");
		AnalyzeBuilding("BorderObstacle", "");
		AnalyzeBuilding("PlayerBorderObstacle", "");
		AnalyzeBuilding("Capital");
		AnalyzeBuilding("GoldenAge");
		AnalyzeBuilding("MapCentering");
		AnalyzeBuilding("AllowsWaterRoutes");
		AnalyzeBuilding("ExtraLuxuries");
		AnalyzeBuilding("DiplomaticVoting");
		AnalyzeBuilding("AffectSpiesNow");
		AnalyzeBuilding("NullifyInfluenceModifier");
		AnalyzeBuilding("LeagueCost");
		AnalyzeBuilding("HolyCity");
		AnalyzeBuilding("NumCityCostMod");
		AnalyzeBuilding("CitiesPrereq");
		AnalyzeBuilding("LevelPrereq");
		AnalyzeBuilding("CultureRateModifier");
		AnalyzeBuilding("GlobalCultureRateModifier");
		AnalyzeBuilding("GreatPeopleRateModifier");
		AnalyzeBuilding("GlobalGreatPeopleRateModifier");
		AnalyzeBuilding("GreatGeneralRateModifier");
		AnalyzeBuilding("GreatPersonExpendGold");
		AnalyzeBuilding("GoldenAgeModifier");
		AnalyzeBuilding("UnitUpgradeCostMod");
		AnalyzeBuilding("Experience");
		AnalyzeBuilding("GlobalExperience");
		AnalyzeBuilding("FoodKept");
		AnalyzeBuilding("Airlift");
		AnalyzeBuilding("AirModifier", "[ICON_AIRSTRIKE_DEFENSE]");
		AnalyzeBuilding("NukeModifier");
		AnalyzeBuilding("NukeExplosionRand");
		AnalyzeBuilding("HealRateChange", "HP");
		AnalyzeBuilding("UnmoddedHappiness", "[ICON_HAPPINESS_1]");
		AnalyzeBuilding("Happiness", "[ICON_HAPPINESS_1]");
		AnalyzeBuilding("HappinessPerCity", "[ICON_HAPPINESS_1]");
		AnalyzeBuilding("HappinessPerXPolicies", "[ICON_HAPPINESS_1]");
		AnalyzeBuilding("UnhappinessModifier");
		AnalyzeBuilding("CityCountUnhappinessMod");
		AnalyzeBuilding("NoOccupiedUnhappiness");
		AnalyzeBuilding("WorkerSpeedModifier");
		AnalyzeBuilding("MilitaryProductionModifier");
		AnalyzeBuilding("SpaceProductionModifier");
		AnalyzeBuilding("GlobalSpaceProductionModifier");
		AnalyzeBuilding("BuildingProductionModifier");
		AnalyzeBuilding("WonderProductionModifier");
		AnalyzeBuilding("CityConnectionTradeRouteModifier");
		AnalyzeBuilding("CapturePlunderModifier");
		AnalyzeBuilding("PolicyCostModifier");
		AnalyzeBuilding("PlotCultureCostModifier");
		AnalyzeBuilding("GlobalPlotCultureCostModifier");
		AnalyzeBuilding("PlotBuyCostModifier");
		AnalyzeBuilding("GlobalPlotBuyCostModifier");
		AnalyzeBuilding("GlobalPopulationChange", "[ICON_CITIZEN]");
		AnalyzeBuilding("TechShare");
		AnalyzeBuilding("FreeTechs", "[ICON_RESEARCH]");
		AnalyzeBuilding("FreePolicies", "[ICON_CULTURE]");
		AnalyzeBuilding("FreeGreatPeople", "[ICON_GREAT_PEOPLE]");
		AnalyzeBuilding("MedianTechPercentChange");
		AnalyzeBuilding("Gold");
		AnalyzeBuilding("AllowsRangeStrike");
		AnalyzeBuilding("Espionage");
		AnalyzeBuilding("AllowsFoodTradeRoutes");
		AnalyzeBuilding("AllowsProductionTradeRoutes");
		AnalyzeBuilding("GlobalDefenseMod");
		AnalyzeBuilding("MinorFriendshipChange");
		AnalyzeBuilding("VictoryPoints");
		AnalyzeBuilding("ExtraMissionarySpreads", "[ICON_MISSIONARY]");
		AnalyzeBuilding("ReligiousPressureModifier");
		AnalyzeBuilding("EspionageModifier");
		AnalyzeBuilding("GlobalEspionageModifier");
		AnalyzeBuilding("ExtraSpies", "[ICON_SPY]");
		AnalyzeBuilding("SpyRankChange", "");
		AnalyzeBuilding("InstantSpyRankChange", "");
		AnalyzeBuilding("TradeRouteRecipientBonus", "[ICON_GOLD]");
		AnalyzeBuilding("TradeRouteTargetBonus", "[ICON_GOLD]");
		AnalyzeBuilding("NumTradeRouteBonus", "[ICON_INTERNATIONAL_TRADE]");
		AnalyzeBuilding("LandmarksTourismPercent");
		AnalyzeBuilding("InstantMilitaryIncrease");
		AnalyzeBuilding("GreatWorksTourismModifier");
		AnalyzeBuilding("XBuiltTriggersIdeologyChoice");
		AnalyzeBuilding("TradeRouteSeaDistanceModifier");
		AnalyzeBuilding("TradeRouteSeaGoldBonus", "");
		AnalyzeBuilding("TradeRouteLandDistanceModifier");
		AnalyzeBuilding("TradeRouteLandGoldBonus", "");
		AnalyzeBuilding("CityStateTradeRouteProductionModifier");
		AnalyzeBuilding("GreatScientistBeakerModifier");
		AnalyzeBuilding("VictoryPrereq");
		--AnalyzeBuilding("ObsoleteTech");
		if thisBuilding.ObsoleteTech ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Obsolete with [COLOR_NEGATIVE_TEXT]"..Locale.Lookup(GameInfo.Technologies[thisBuilding.ObsoleteTech].Description).."[ENDCOLOR]"; end
		--AnalyzeBuilding("EnhancedYieldTech");
		if thisBuilding.EnhancedYieldTech ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Additional yields with [COLOR_POSITIVE_TEXT]"..Locale.Lookup(GameInfo.Technologies[thisBuilding.EnhancedYieldTech].Description).."[ENDCOLOR]"; end
		AnalyzeBuilding("TechEnhancedTourism", "[ICON_TOURISM]");
		--AnalyzeBuilding("FreeBuilding");
		if thisBuilding.FreeBuilding ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Free [COLOR_POSITIVE_TEXT]"..Locale.Lookup(GameInfo.BuildingClasses[thisBuilding.FreeBuilding].Description).."[ENDCOLOR] in all Cities"; end
		--AnalyzeBuilding("FreeBuildingThisCity");
		if thisBuilding.FreeBuildingThisCity ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Free [COLOR_POSITIVE_TEXT]"..Locale.Lookup(GameInfo.BuildingClasses[thisBuilding.FreeBuildingThisCity].Description).."[ENDCOLOR] in the City"; end
		--AnalyzeBuilding("FreePromotion");
		if thisBuilding.FreePromotion ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]All units receive [COLOR_POSITIVE_TEXT]"..Locale.Lookup(GameInfo.UnitPromotions[thisBuilding.FreePromotion].Description).."[ENDCOLOR]"; end
		--AnalyzeBuilding("TrainedFreePromotion");
		if thisBuilding.TrainedFreePromotion ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Units trained in the city receive [COLOR_POSITIVE_TEXT]"..Locale.Lookup(GameInfo.UnitPromotions[thisBuilding.TrainedFreePromotion].Description).."[ENDCOLOR]"; end
		--AnalyzeBuilding("FreePromotionRemoved");
		if thisBuilding.FreePromotionRemoved ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Removes [COLOR_NEGATIVE_TEXT]"..Locale.Lookup(GameInfo.UnitPromotions[thisBuilding.FreePromotionRemoved].Description).."[ENDCOLOR]"; end
		AnalyzeBuilding("ReplacementBuildingClass");
		--AnalyzeBuilding("FreeGreatWork");
		if thisBuilding.FreeGreatWork ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Free [COLOR_POSITIVE_TEXT]"..Locale.Lookup(GameInfo.GreatWorks[thisBuilding.FreeGreatWork].Description).."[ENDCOLOR]"; end
		AnalyzeBuilding("SpecialistExtraCulture");
		AnalyzeBuilding("ExtraLeagueVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("CityWall");
		--AnalyzeBuilding("DisplayPosition");
		AnalyzeBuilding("ArtInfoCulturalVariation");
		AnalyzeBuilding("ArtInfoEraVariation");
		AnalyzeBuilding("ArtInfoRandomVariation");
		AnalyzeBuilding("IsNoWater", "");
		AnalyzeBuilding("IsNoRiver", "");
		AnalyzeBuilding("NumPoliciesNeeded", "");
		AnalyzeBuilding("NationalPopRequired", "");
		AnalyzeBuilding("LocalPopRequired", "");
		AnalyzeBuilding("BorderObstacleCity", "");
		AnalyzeBuilding("BorderObstacleWater", "");
		AnalyzeBuilding("AllowsFoodTradeRoutesGlobal");
		AnalyzeBuilding("AllowsProductionTradeRoutesGlobal");
		AnalyzeBuilding("CityConnectionGoldModifier");
		AnalyzeBuilding("PovertyHappinessChange");
		AnalyzeBuilding("DefenseHappinessChange");
		AnalyzeBuilding("IlliteracyHappinessChange");
		AnalyzeBuilding("UnculturedHappinessChange");
		AnalyzeBuilding("MinorityHappinessChange");
		AnalyzeBuilding("PovertyHappinessChangeGlobal");
		AnalyzeBuilding("DefenseHappinessChangeGlobal");
		AnalyzeBuilding("IlliteracyHappinessChangeGlobal");
		AnalyzeBuilding("UnculturedHappinessChangeGlobal");
		AnalyzeBuilding("MinorityHappinessChangeGlobal");
		AnalyzeBuilding("LocalUnhappinessModifier");
		AnalyzeBuilding("GlobalBuildingGoldMaintenanceMod");
		AnalyzeBuilding("NationalFollowerPopRequired");
		AnalyzeBuilding("GlobalFollowerPopRequired");
		AnalyzeBuilding("IsReformation", "");
		AnalyzeBuilding("ResourceType");
		--if thisPromotion.TechPrereq ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_CYAN]"..Locale.Lookup(GameInfo.Technologies[thisPromotion.TechPrereq].Description).."[ENDCOLOR]"; end
		AnalyzeBuilding("PuppetPurchaseOverride");
		AnalyzeBuilding("SingleLeagueVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("AllowsPuppetPurchase");
		AnalyzeBuilding("GrantsRandomResourceTerritory", "");
		AnalyzeBuilding("ResourceQuantityToPlace");
		AnalyzeBuilding("TradeReligionModifier");
		--AnalyzeBuilding("NeedBuildingThisCity");
		if thisBuilding.NeedBuildingThisCity ~= nil then sText = sText.."[NEWLINE][ICON_BULLET]Requires [COLOR_NEGATIVE_TEXT]"..Locale.Lookup(GameInfo.BuildingClasses[thisBuilding.NeedBuildingThisCity].Description).."[ENDCOLOR] in the City"; end
		AnalyzeBuilding("WLTKDTurns", "");
		AnalyzeBuilding("EventTourism", "[ICON_TOURISM]");
		AnalyzeBuilding("AlwaysHeal", "HP");
		AnalyzeBuilding("CitySupplyModifier");
		AnalyzeBuilding("CitySupplyModifierGlobal");
		AnalyzeBuilding("CitySupplyFlat", "[ICON_SILVER_FIST]");
		AnalyzeBuilding("CitySupplyFlatGlobal", "[ICON_SILVER_FIST]");
		AnalyzeBuilding("FinishLandTRTourism", "[ICON_TOURISM]");
		AnalyzeBuilding("FinishSeaTRTourism", "[ICON_TOURISM]");
		AnalyzeBuilding("VotesPerGPT", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("RequiresRail", "");
		AnalyzeBuilding("CivilizationRequired");
		AnalyzeBuilding("PurchaseCooldown");
		AnalyzeBuilding("CityAirStrikeDefense", "[ICON_STRENGTH]");
		AnalyzeBuilding("BuildAnywhere");
		AnalyzeBuilding("FreeArtifacts", "");
		AnalyzeBuilding("VassalLevyEra");
		AnalyzeBuilding("CannotFailSpies");
		AnalyzeBuilding("AdvancedActionGold");
		AnalyzeBuilding("AdvancedActionScience");
		AnalyzeBuilding("AdvancedActionUnrest");
		AnalyzeBuilding("AdvancedActionRebellion");
		AnalyzeBuilding("AdvancedActionGP");
		AnalyzeBuilding("AdvancedActionUnit");
		AnalyzeBuilding("AdvancedActionWonder");
		AnalyzeBuilding("AdvancedActionBuilding");
		AnalyzeBuilding("BlockBuildingDestructionSpies", "");
		AnalyzeBuilding("BlockWWDestructionSpies", "");
		AnalyzeBuilding("BlockUDestructionSpies", "");
		AnalyzeBuilding("BlockGPDestructionSpies", "");
		AnalyzeBuilding("BlockRebellionSpies", "");
		AnalyzeBuilding("BlockUnrestSpies", "");
		AnalyzeBuilding("BlockScienceTheft", "");
		AnalyzeBuilding("BlockGoldTheft", "");
		AnalyzeBuilding("EventChoiceRequiredActive");
		AnalyzeBuilding("CityEventChoiceRequiredActive");
		AnalyzeBuilding("GPRateModifierPerXFranchises");
		AnalyzeBuilding("TRSpeedBoost");
		AnalyzeBuilding("TRVisionBoost");
		AnalyzeBuilding("OfficeBenefitHelper");
		AnalyzeBuilding("DPToVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("SecondaryPantheon");
		AnalyzeBuilding("IsDummy");
		AnalyzeBuilding("AnyWater");
		AnalyzeBuilding("BuildingDefenseModifier");
		AnalyzeBuilding("GlobalLandmarksTourismPercent");
		AnalyzeBuilding("GlobalGreatWorksTourismModifier");
		AnalyzeBuilding("FaithToVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("CapitalsToVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("DoFToVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("RAToVotes", "[COLOR_POSITIVE_TEXT]votes[ENDCOLOR]");
		AnalyzeBuilding("GPExpendInfluence");
		AnalyzeBuilding("AddsFreshWater");
		AnalyzeBuilding("PurchaseOnly");
		AnalyzeBuilding("CityWorkingChange");
		AnalyzeBuilding("GlobalCityWorkingChange");
		AnalyzeBuilding("GlobalConversionModifier");
		AnalyzeBuilding("ConversionModifier");
		AnalyzeBuilding("PlayerBorderGainlessPillage");
		AnalyzeBuilding("CityGainlessPillage");
		AnalyzeBuilding("HurryCostModifier");
		AnalyzeBuilding("NeverCapture");
		AnalyzeBuilding("NukeImmune");
		AnalyzeBuilding("ConquestProb", "");
		--AnalyzeBuilding("FreeStartEra");
		if thisBuilding.FreeStartEra ~= nil then
			local sDesc = ""
			for era in DB.Query("SELECT Description FROM Eras WHERE Type = ?", thisBuilding.FreeStartEra) do sDesc = era.Description end
			sText = sText.."[NEWLINE][ICON_BULLET]Free in [COLOR_POSITIVE_TEXT]"..Locale.Lookup(sDesc).."[ENDCOLOR]"
		end
		--AnalyzeBuilding("MaxStartEra");
		if thisBuilding.MaxStartEra ~= nil then
			local sDesc = ""
			for era in DB.Query("SELECT Description FROM Eras WHERE Type = ?", thisBuilding.MaxStartEra) do sDesc = era.Description end
			sText = sText.."[NEWLINE][ICON_BULLET]Only until [COLOR_NEGATIVE_TEXT]"..Locale.Lookup(sDesc).."[ENDCOLOR]"
		end
		-------------------
		local function AnalyzeBuildingYields(sTable, sFieldName, sRefTable, sFlags, sInfo)
			--print("AnalyzeBuildingYields", sTable, sFieldName, sRefTable, sFlags, sInfo)
			local sLocText = "";
			--"SELECT [sTable].Yield, [sRefTable].Description
			--FROM [sTable]
			--INNER JOIN [sRefTable] ON [sTable].[sFieldName] = [sRefTable].Type
			--WHERE [sTable].BuildingType = ?
			--ORDER BY YieldType" -]]
			if sRefTable == "" then 
				local sql = string.format(
					"SELECT %s.Yield%s as Yield, Yields.IconString FROM %s INNER JOIN Yields ON %s.YieldType = Yields.Type WHERE %s.BuildingType = ? ORDER BY YieldType",
					sTable, sFlags, sTable, sTable, sTable);
				--print("Executing SQL ", sql);
				for row in DB.Query(sql, thisBuilding.Type) do
					sLocText = sLocText..string.format("%+d%s", row.Yield, row.IconString);
				end
			else
				local sql = string.format(
					"SELECT %s.Yield%s as Yield, Yields.IconString, %s.Description FROM %s INNER JOIN Yields ON %s.YieldType = Yields.Type INNER JOIN %s ON %s.%s = %s.Type WHERE %s.BuildingType = ? ORDER BY %s.Type, YieldType",
					sTable, sFlags, sRefTable, sTable, sTable, sRefTable, sTable, sFieldName, sRefTable, sTable, sRefTable);
				--print("Executing SQL ", sql);
				local sGroup = "";
				for row in DB.Query(sql, thisBuilding.Type) do
					if sGroup == "" then
						sLocText = sLocText..string.format("[NEWLINE][ICON_BULLET]%+d%s ", row.Yield, row.IconString);
						sGroup = row.Description;
					elseif row.Description == sGroup then 
						sLocText = sLocText..string.format("%+d%s ", row.Yield, row.IconString);
					else
						sLocText = sLocText.."from "..Locale.Lookup(sGroup);
						sLocText = sLocText..string.format("[NEWLINE][ICON_BULLET]%+d%s ", row.Yield, row.IconString);
						sGroup = row.Description;
					end
				end
				if sGroup ~= "" then sLocText = sLocText.."from "..Locale.Lookup(sGroup); end
			end
			if sLocText ~= "" then
				sText = sText.."[NEWLINE]"..sInfo..sLocText;
			end
		end
		-- main yields from building
		--AnalyzeBuildingYields("Building_YieldChanges", 				 "", 					"", "", "");
		--AnalyzeBuildingYields("Building_YieldChangesPerPop", 			 "", 					"", "%", "");
		--AnalyzeBuildingYields("Building_YieldModifiers", 			     "", 					"", "%", "");
		--                     secondary table                            field name           ref table         flags info text
		AnalyzeBuildingYields("Building_TerrainYieldChanges",   	     "TerrainType", 		"Terrains",			"", 		"Yields from [COLOR_CYAN]Terrain[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_FeatureYieldChanges",            "FeatureType",       	"Features",			"", 		"Yields from [COLOR_CYAN]Features[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_ResourceYieldChanges",           "ResourceType",      	"Resources", 		"", 		"Yields from [COLOR_CYAN]Resources[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_SeaResourceYieldChanges", 		 "", 					"", 				"", 		"Yields from [COLOR_CYAN]Sea Resources[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_ImprovementYieldChanges",        "ImprovementType",   	"Improvements",  	"", 		"Yields from [COLOR_CYAN]Improvements[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_PlotYieldChanges",               "PlotType",          	"Plots",         	"", 		"Yields from [COLOR_CYAN]Tiles[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_LakePlotYieldChanges",           "",                  	"",             	"", 		"Yields from [COLOR_CYAN]Lake[ENDCOLOR] tiles:");
		AnalyzeBuildingYields("Building_RiverPlotYieldChanges", 		 "", 					"", 				"", 		"Yields from [COLOR_CYAN]River[ENDCOLOR] tiles:");
		AnalyzeBuildingYields("Building_SeaPlotYieldChanges", 			 "", 					"", 				"", 		"Yields from [COLOR_CYAN]Sea[ENDCOLOR] tiles:");
		AnalyzeBuildingYields("Building_BuildingClassLocalYieldChanges", "BuildingClassType", 	"BuildingClasses", 	"Change", 	"Yields from [COLOR_CYAN]Buildings[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_SpecialistYieldChangesLocal",    "SpecialistType", 		"Specialists", 		"", 		"Yields from [COLOR_CYAN]Specialists[ENDCOLOR]:");
		-- global changes
		AnalyzeBuildingYields("Building_ImprovementYieldChangesGlobal",  "ImprovementType",   	"Improvements",    	"", 		"Yields from [COLOR_CYAN]Improvements[ENDCOLOR] in [COLOR_POSITIVE_TEXT]all Cities[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_BuildingClassYieldChanges",      "BuildingClassType", 	"BuildingClasses", 	"Change", 	"Yields from [COLOR_CYAN]Buildings[ENDCOLOR] in [COLOR_POSITIVE_TEXT]all Cities[ENDCOLOR]:");
		AnalyzeBuildingYields("Building_SpecialistYieldChanges", 		 "SpecialistType", 		"Specialists", 		"", 		"Yields from [COLOR_CYAN]Specialists[ENDCOLOR] in [COLOR_POSITIVE_TEXT]all Cities[ENDCOLOR]:");
		-------------------
		UpdateTextBlock( sText, Controls.ExtendedLabel,  Controls.ExtendedInnerFrame,  Controls.ExtendedFrame );
		-- end Infixo

		-- update the related images
		Controls.RelatedImagesFrame:SetHide( true );

	end
end

CivilopediaCategory[CategoryBuildings].SelectArticle = function( buildingID, shouldAddToList )
	print("CivilopediaCategory[CategoryBuildings].SelectArticle");
	if selectedCategory ~= CategoryBuildings then
		SetSelectedCategory(CategoryBuildings);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryBuildings * absurdlyLargeNumTopicsInCategory) + buildingID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	SelectBuildingOrWonderArticle( buildingID );

	ResizeEtc();

end


CivilopediaCategory[CategoryWonders].SelectArticle = function( wonderID, shouldAddToList )
	print("CivilopediaCategory[CategoryWonders].SelectArticle");
	if selectedCategory ~= CategoryWonders then
		SetSelectedCategory(CategoryWonders);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryWonders * absurdlyLargeNumTopicsInCategory) + wonderID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if wonderID < 1000 then
		SelectBuildingOrWonderArticle( wonderID );
	else
		local projectID = wonderID - 1000;
		if projectID ~= -1 then
		
			local thisProject = GameInfo.Projects[projectID];
						
			-- update the name
			local name = Locale.ConvertTextKey( thisProject.Description ); 	
			Controls.ArticleID:SetText( name );

			-- update the portrait
			if IconHookup( thisProject.PortraitIndex, portraitSize, thisProject.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end
			
			-- update the cost
			Controls.CostFrame:SetHide( false );
			local cost = thisProject.Cost;
			if(cost > 0 and Game ~= nil) then
				cost = Players[Game.GetActivePlayer()]:GetProjectProductionNeeded( projectID );
			end
			
			if(cost > 0) then
				Controls.CostLabel:SetText( tostring(cost).." [ICON_PRODUCTION]" );
			elseif(cost == 0) then
				Controls.CostLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_FREE" ) );
			else
				Controls.CostFrame:SetHide(true);
			end
	
 			local contentSize;
 			local frameSize = {};
			local buttonAdded = 0;

			-- update the prereq techs
			g_PrereqTechManager:ResetInstances();
			buttonAdded = 0;

			if thisProject.TechPrereq then
				local prereq = GameInfo.Technologies[thisProject.TechPrereq];
				if prereq then
					local thisPrereqInstance = g_PrereqTechManager:GetInstance();
					if thisPrereqInstance then
						local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisPrereqInstance.PrereqTechImage, thisPrereqInstance.PrereqTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
						buttonAdded = buttonAdded + 1;
					end	
				end
			end	
			UpdateButtonFrame( buttonAdded, Controls.PrereqTechInnerFrame, Controls.PrereqTechFrame );

			local condition = "BuildingType = '" .. thisProject.Type .. "'";
			
			-- required buildings
			g_RequiredBuildingsManager:ResetInstances();
			buttonAdded = 0;
			for row in GameInfo.Building_ClassesNeededInCity( condition ) do
				local buildingClass = GameInfo.BuildingClasses[row.BuildingClassType];
				if(buildingClass ~= nil) then
					local thisBuildingInfo = GameInfo.Buildings[buildingClass.DefaultBuilding];
					if(thisBuildingInfo ~= nil) then
						local thisBuildingInstance = g_RequiredBuildingsManager:GetInstance();
						if thisBuildingInstance then

							if not IconHookup( thisBuildingInfo.PortraitIndex, buttonSize, thisBuildingInfo.IconAtlas, thisBuildingInstance.RequiredBuildingImage ) then
								thisBuildingInstance.RequiredBuildingImage:SetTexture( defaultErrorTextureSheet );
								thisBuildingInstance.RequiredBuildingImage:SetTextureOffset( nullOffset );
							end
							
							--move this button
							thisBuildingInstance.RequiredBuildingButton:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );
							
							thisBuildingInstance.RequiredBuildingButton:SetToolTipString( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
							thisBuildingInstance.RequiredBuildingButton:SetVoids( thisBuildingInfo.ID, addToList );
							local thisBuildingClass = GameInfo.BuildingClasses[thisBuildingInfo.BuildingClass];
							if thisBuildingClass.MaxGlobalInstances > 0 or (thisBuildingClass.MaxPlayerInstances == 1 and thisBuildingInfo.SpecialistCount == 0) or thisBuildingClass.MaxTeamInstances > 0 then
								thisBuildingInstance.RequiredBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
							else
								thisBuildingInstance.RequiredBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
							end
							buttonAdded = buttonAdded + 1;
						end
					end
				end
			end
			UpdateButtonFrame( buttonAdded, Controls.RequiredBuildingsInnerFrame, Controls.RequiredBuildingsFrame );
			
			-- update the game info
			if thisProject.Help then
				UpdateTextBlock( Locale.ConvertTextKey( thisProject.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end
					
			-- update the strategy info
			if (thisProject.Strategy) then
				UpdateTextBlock( Locale.ConvertTextKey( thisProject.Strategy ), Controls.StrategyLabel, Controls.StrategyInnerFrame, Controls.StrategyFrame );
			end
			
			-- update the historical info
			if (thisProject.Civilopedia) then
				UpdateTextBlock( Locale.ConvertTextKey( thisProject.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
			end
					
			-- update the related images
			Controls.RelatedImagesFrame:SetHide( true );
		end
	end

	ResizeEtc();

end

CivilopediaCategory[CategoryPolicies].SelectArticle = function( policyID, shouldAddToList )
	print("CivilopediaCategory[CategoryPolicies].SelectArticle");
	if selectedCategory ~= CategoryPolicies then
		SetSelectedCategory(CategoryPolicies);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryPolicies * absurdlyLargeNumTopicsInCategory) + policyID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if policyID ~= -1 then
	
		local thisPolicy = GameInfo.Policies[policyID];
					
		-- update the name
		local name = Locale.ConvertTextKey( thisPolicy.Description ); 	
		Controls.ArticleID:SetText( name );

		-- update the portrait
		if IconHookup( thisPolicy.PortraitIndex, portraitSize, thisPolicy.IconAtlas, Controls.Portrait ) then
			Controls.PortraitFrame:SetHide( false );
		else
			Controls.PortraitFrame:SetHide( true );
		end
		
		-- update the policy branch
		if thisPolicy.PolicyBranchType then
			local branch = GameInfo.PolicyBranchTypes[thisPolicy.PolicyBranchType];
			if branch then
				local branchName = Locale.ConvertTextKey( branch.Description ); 	
				Controls.PolicyBranchLabel:SetText( branchName );
				Controls.PolicyBranchFrame:SetHide( false );
				-- update the prereq era
				if branch.EraPrereq then
					local era = GameInfo.Eras[branch.EraPrereq];
					if era then
						local eraName = Locale.ConvertTextKey( era.Description ); 	
						Controls.PrereqEraLabel:SetText( eraName );
						Controls.PrereqEraFrame:SetHide( false );
					end
				end
			end
		end
				
		local contentSize;
		local frameSize = {};
		local buttonAdded = 0;

		-- update the prereq policies
		g_RequiredPoliciesManager:ResetInstances();
		buttonAdded = 0;

		local condition = "PolicyType = '" .. thisPolicy.Type .. "'";

		for row in GameInfo.Policy_PrereqPolicies( condition ) do
			local requiredPolicy = GameInfo.Policies[row.PrereqPolicy];
			if requiredPolicy then
				local thisRequiredPolicyInstance = g_RequiredPoliciesManager:GetInstance();
				if thisRequiredPolicyInstance then
					local textureOffset, textureSheet = IconLookup( requiredPolicy.PortraitIndex, buttonSize, requiredPolicy.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisRequiredPolicyInstance.RequiredPolicyImage, thisRequiredPolicyInstance.RequiredPolicyButton, textureSheet, textureOffset, CategoryPolicies, Locale.ConvertTextKey( requiredPolicy.Description ), requiredPolicy.ID );
					buttonAdded = buttonAdded + 1;
				end
			end		
		end
		UpdateButtonFrame( buttonAdded, Controls.RequiredPoliciesInnerFrame, Controls.RequiredPoliciesFrame );
		
		local tenetLevelLabels = {
			"TXT_KEY_POLICYSCREEN_L1_TENET",
			"TXT_KEY_POLICYSCREEN_L2_TENET",
			"TXT_KEY_POLICYSCREEN_L3_TENET",
		}
		
		local tenetLevel = tonumber(thisPolicy.Level);
		if(tenetLevel ~= nil and tenetLevel > 0) then
			Controls.TenetLevelLabel:LocalizeAndSetText(tenetLevelLabels[tenetLevel]);
			Controls.TenetLevelFrame:SetHide(false);	
		else
			Controls.TenetLevelFrame:SetHide(true);
		end
		

		-- update the game info
		if thisPolicy.Help then
			UpdateTextBlock( Locale.ConvertTextKey( thisPolicy.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
		end
				
		-- update the strategy info
		--UpdateTextBlock( Locale.ConvertTextKey( thisPolicy.Strategy ), Controls.StrategyLabel, Controls.StrategyInnerFrame, Controls.StrategyFrame );
		
		-- update the historical info
		if (thisPolicy.Civilopedia) then
			UpdateTextBlock( Locale.ConvertTextKey( thisPolicy.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
		end
		
		-- update the related images
		Controls.RelatedImagesFrame:SetHide( true );
	end

	ResizeEtc();

end


CivilopediaCategory[CategoryPeople].SelectArticle =  function( rawPeopleID, shouldAddToList )
	print("CivilopediaCategory[CategoryPeople].SelectArticle");
	if selectedCategory ~= CategoryPeople then
		SetSelectedCategory(CategoryPeople);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryPeople * absurdlyLargeNumTopicsInCategory) + rawPeopleID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if rawPeopleID < 1000 then
		if rawPeopleID ~= -1 then
		
			local thisPerson = GameInfo.Specialists[rawPeopleID];
						
			-- update the name
			local name = Locale.ConvertTextKey( thisPerson.Description ); 	
			Controls.ArticleID:SetText( name );

			if IconHookup( thisPerson.PortraitIndex, portraitSize, thisPerson.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end

			local buttonAdded = 0;

			-- list the buildings that this can work in

			-- update the game info
			if thisPerson.Help then
				UpdateTextBlock( Locale.ConvertTextKey( thisPerson.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end
					
			-- update the strategy info
			if thisPerson.Strategy then
				UpdateTextBlock( Locale.ConvertTextKey( thisPerson.Strategy ), Controls.StrategyLabel, Controls.StrategyInnerFrame, Controls.StrategyFrame );
			end
			
			-- update the historical info
			if thisPerson.Civilopedia then
				UpdateTextBlock( Locale.ConvertTextKey( thisPerson.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
			end
					
			-- update the related images
			Controls.RelatedImagesFrame:SetHide( true );

			-- Infixo Extended info
			local sText = "[COLOR_CYAN]Buildings[ENDCOLOR] with this specialist:"; -- change to TXT_KEY_ later
			for row in GameInfo.Buildings() do
				if row.SpecialistType == thisPerson.Type and row.SpecialistCount > 0 then 
					sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %d[ICON_SPY]", row.SpecialistCount);
				end
			end
			sText = sText.."[NEWLINE][COLOR_CYAN]Global Yields[ENDCOLOR] granted by buildings:";
			local sql = [[
				SELECT Buildings.Description, Yields.IconString, Building_SpecialistYieldChanges.Yield
				FROM Building_SpecialistYieldChanges INNER JOIN Yields, Buildings ON Building_SpecialistYieldChanges.YieldType = Yields.Type AND Building_SpecialistYieldChanges.BuildingType = Buildings.Type
				WHERE Building_SpecialistYieldChanges.SpecialistType = ?]];
			for row in DB.Query(sql, thisPerson.Type) do
				sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %+d", row.Yield)..row.IconString;
			end
			sText = sText.."[NEWLINE][COLOR_CYAN]Local Yields[ENDCOLOR] granted by buildings:";
			local sql = [[
				SELECT Buildings.Description, Yields.IconString, Building_SpecialistYieldChangesLocal.Yield
				FROM Building_SpecialistYieldChangesLocal INNER JOIN Yields, Buildings ON Building_SpecialistYieldChangesLocal.YieldType = Yields.Type AND Building_SpecialistYieldChangesLocal.BuildingType = Buildings.Type
				WHERE Building_SpecialistYieldChangesLocal.SpecialistType = ?]];
			for row in DB.Query(sql, thisPerson.Type) do
				sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %+d", row.Yield)..row.IconString;
			end
			sText = sText.."[NEWLINE][COLOR_CYAN]Technologies[ENDCOLOR] that increase yields:";
			local sql = [[
				SELECT Technologies.Description, Yields.IconString, Tech_SpecialistYieldChanges.Yield
				FROM Tech_SpecialistYieldChanges INNER JOIN Yields, Technologies ON Tech_SpecialistYieldChanges.YieldType = Yields.Type AND Tech_SpecialistYieldChanges.TechType = Technologies.Type
				WHERE Tech_SpecialistYieldChanges.SpecialistType = ?
				ORDER BY Technologies.GridX]];
			for row in DB.Query(sql, thisPerson.Type) do
				sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %+d", row.Yield)..row.IconString;
			end
			UpdateTextBlock( sText, Controls.ExtendedLabel,  Controls.ExtendedInnerFrame,  Controls.ExtendedFrame );
			-- end Infixo

		end
	else
		local greatPersonID = rawPeopleID - 1000;
		if greatPersonID ~= -1 then
		
			local thisPerson = GameInfo.Units[greatPersonID];
						
			-- update the name
			local name = Locale.ConvertTextKey( thisPerson.Description ); 	
			Controls.ArticleID:SetText( name );

			-- update the portrait
			if IconHookup( thisPerson.PortraitIndex, portraitSize, thisPerson.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end
						
			local buttonAdded = 0;

			-- list the buildings that can spawn this unit
				
			-- update the game info
			if thisPerson.Help then
				UpdateTextBlock( Locale.ConvertTextKey( thisPerson.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end
					
			-- update the strategy info
			if (thisPerson.Strategy) then
				UpdateTextBlock( Locale.ConvertTextKey( thisPerson.Strategy ), Controls.StrategyLabel, Controls.StrategyInnerFrame, Controls.StrategyFrame );
			end
			
			-- update the historical info
			if (thisPerson.Civilopedia) then
				UpdateTextBlock( Locale.ConvertTextKey( thisPerson.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
			end
					
			-- update the related images
			Controls.RelatedImagesFrame:SetHide( true );
			
			-- Infixo Extended info
			local sSpecialistType = "";
			for row in DB.Query("SELECT Type FROM Specialists WHERE GreatPeopleUnitClass = ?", thisPerson.Class) do
				sSpecialistType = row.Type;
			end
			if sSpecialistType ~= "" then
				-- this Great Person has an associated Specialist
				local sText = "[COLOR_CYAN]Buildings[ENDCOLOR] that generate [ICON_GREAT_PEOPLE] for this person:"; -- change to TXT_KEY_ later
				local bShow = false;
				for row in GameInfo.Buildings() do
					if row.SpecialistType == sSpecialistType and row.GreatPeopleRateChange > 0 then 
						sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %+d[ICON_GREAT_PEOPLE]", row.GreatPeopleRateChange);
					end
				end
				UpdateTextBlock( sText, Controls.ExtendedLabel,  Controls.ExtendedInnerFrame,  Controls.ExtendedFrame );
			end
			-- end Infixo
		end
	
	end
	
	ResizeEtc();

end


CivilopediaCategory[CategoryCivilizations].SelectArticle = function( rawCivID, shouldAddToList )
	print("CivilopediaCategory[CategoryCivilizations].SelectArticle");
	if selectedCategory ~= CategoryCivilizations then
		SetSelectedCategory(CategoryCivilizations);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryCivilizations * absurdlyLargeNumTopicsInCategory) + rawCivID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end

	if rawCivID < 1000 then
		if rawCivID ~= -1 then
		
			local thisCiv = GameInfo.Civilizations[rawCivID];
			if thisCiv and thisCiv.Type ~= "CIVILIZATION_MINOR" and thisCiv.Type ~= "CIVILIZATION_BARBARIAN" then
							
				-- update the name
				local name = Locale.ConvertTextKey( thisCiv.ShortDescription )
				Controls.ArticleID:SetText( name );

				-- update the portrait
				if IconHookup( thisCiv.PortraitIndex, portraitSize, thisCiv.IconAtlas, Controls.Portrait ) then
					Controls.PortraitFrame:SetHide( false );
				else
					Controls.PortraitFrame:SetHide( true );
				end

				local buttonAdded = 0;
		 		local condition = "CivilizationType = '" .. thisCiv.Type .. "'";
				
				-- add a list of leaders
				g_LeadersManager:ResetInstances();
				buttonAdded = 0;
					
				local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. thisCiv.Type .. "'" )().LeaderheadType]; -- todo: add support for multiple leaders
				local thisLeaderInstance = g_LeadersManager:GetInstance();
				if thisLeaderInstance then
					local textureOffset, textureSheet = IconLookup( leader.PortraitIndex, buttonSize, leader.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisLeaderInstance.LeaderImage, thisLeaderInstance.LeaderButton, textureSheet, textureOffset, CategoryCivilizations, Locale.ConvertTextKey( leader.Description ), thisCiv.ID + 1000 );
					buttonAdded = buttonAdded + 1;
				end	
				UpdateButtonFrame( buttonAdded, Controls.LeadersInnerFrame, Controls.LeadersFrame );
				
				-- list of UUs
 				g_UniqueUnitsManager:ResetInstances();
				buttonAdded = 0;
				for thisOverride in GameInfo.Civilization_UnitClassOverrides( condition ) do
					if thisOverride.UnitType ~= nil then
						local thisUnitInfo = GameInfo.Units[thisOverride.UnitType];
						if thisUnitInfo then
							local thisUnitInstance = g_UniqueUnitsManager:GetInstance();
							if thisUnitInstance then
								local textureOffset, textureSheet = IconLookup( thisUnitInfo.PortraitIndex, buttonSize, thisUnitInfo.IconAtlas );				
								if textureOffset == nil then
									textureSheet = defaultErrorTextureSheet;
									textureOffset = nullOffset;
								end			
								
								local unitCategory = CategoryUnits;
								local unitEntryID = thisUnitInfo.ID;
								--if(thisUnitInfo.Special == "SPECIALUNIT_PEOPLE") then
									---- Either a great person or specialist.
									--unitCategory = CategoryPeople;
									--
									---- Figure out which one it is!
									--local bIsSpecialist = false;
									--for row in GameInfo.Specialists{GreatPeopleUnitClass = thisUnitInfo.Class} do
										--bIsSpecialist = true;
										--break;
									--end
									--
									--if(bIsSpecialist == false) then
										--unitEntryID = unitEntryID + 1000;	
									--end
								--end
									
								UpdateSmallButton( buttonAdded, thisUnitInstance.UniqueUnitImage, thisUnitInstance.UniqueUnitButton, textureSheet, textureOffset, unitCategory, Locale.ConvertTextKey( thisUnitInfo.Description ), unitEntryID);
								buttonAdded = buttonAdded + 1;
							end
						end
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.UniqueUnitsInnerFrame, Controls.UniqueUnitsFrame );	 	  
				
				-- list of UBs
				g_UniqueBuildingsManager:ResetInstances();
				buttonAdded = 0;
				for thisOverride in GameInfo.Civilization_BuildingClassOverrides( condition ) do
					if(thisOverride.BuildingType ~= nil) then
						local thisBuildingInfo = GameInfo.Buildings[thisOverride.BuildingType];
						if thisBuildingInfo then
							local thisBuildingInstance = g_UniqueBuildingsManager:GetInstance();
							if thisBuildingInstance then

								if not IconHookup( thisBuildingInfo.PortraitIndex, buttonSize, thisBuildingInfo.IconAtlas, thisBuildingInstance.UniqueBuildingImage ) then
									thisBuildingInstance.UniqueBuildingImage:SetTexture( defaultErrorTextureSheet );
									thisBuildingInstance.UniqueBuildingImage:SetTextureOffset( nullOffset );
								end
								
								--move this button
								thisBuildingInstance.UniqueBuildingButton:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );
								
								thisBuildingInstance.UniqueBuildingButton:SetToolTipString( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
								thisBuildingInstance.UniqueBuildingButton:SetVoids( thisBuildingInfo.ID, addToList );
								local thisBuildingClass = GameInfo.BuildingClasses[thisBuildingInfo.BuildingClass];
								if thisBuildingClass.MaxGlobalInstances > 0 or (thisBuildingClass.MaxPlayerInstances == 1 and thisBuildingInfo.SpecialistCount == 0) or thisBuildingClass.MaxTeamInstances > 0 then
									thisBuildingInstance.UniqueBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
								else
									thisBuildingInstance.UniqueBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
								end

								buttonAdded = buttonAdded + 1;
							end
						end
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.UniqueBuildingsInnerFrame, Controls.UniqueBuildingsFrame );
				
				-- list of unique improvements	 	  
				g_UniqueImprovementsManager:ResetInstances();
				buttonAdded = 0;
				for thisImprovement in GameInfo.Improvements( condition ) do
					local thisImprovementInstance = g_UniqueImprovementsManager:GetInstance();
					if thisImprovementInstance then

						if not IconHookup( thisImprovement.PortraitIndex, buttonSize, thisImprovement.IconAtlas, thisImprovementInstance.UniqueImprovementImage ) then
							thisImprovementInstance.UniqueImprovementImage:SetTexture( defaultErrorTextureSheet );
							thisImprovementInstance.UniqueImprovementImage:SetTextureOffset( nullOffset );
						end
						
						--move this button
						thisImprovementInstance.UniqueImprovementButton:SetOffsetVal( (buttonAdded % numberOfButtonsPerRow) * buttonSize + buttonPadding, math.floor(buttonAdded / numberOfButtonsPerRow) * buttonSize + buttonPadding );
						
						thisImprovementInstance.UniqueImprovementButton:SetToolTipString( Locale.ConvertTextKey( thisImprovement.Description ) );
						thisImprovementInstance.UniqueImprovementButton:SetVoids( thisImprovement.ID, addToList );
						thisImprovementInstance.UniqueImprovementButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryImprovements].SelectArticle );

						buttonAdded = buttonAdded + 1;
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.UniqueImprovementsInnerFrame, Controls.UniqueImprovementsFrame );
				
				-- list of special abilities
				buttonAdded = 0;
				
				-- add the free form text
				g_FreeFormTextManager:ResetInstances();
				
				local tagString = thisCiv.CivilopediaTag;
				if tagString then
					local headerString = tagString .. "_HEADING_";
					local bodyString = tagString .. "_TEXT_";
					local notFound = false;
					local i = 1;
					repeat
						local headerTag = headerString .. tostring( i );
						local bodyTag = bodyString .. tostring( i );
						if TagExists( headerTag ) and TagExists( bodyTag ) then
							local thisFreeFormTextInstance = g_FreeFormTextManager:GetInstance();
							if thisFreeFormTextInstance then
								thisFreeFormTextInstance.FFTextHeader:SetText( Locale.ConvertTextKey( headerTag ));
								UpdateTextBlock( Locale.ConvertTextKey( bodyTag ), thisFreeFormTextInstance.FFTextLabel, thisFreeFormTextInstance.FFTextInnerFrame, thisFreeFormTextInstance.FFTextFrame );
							end
						else
							notFound = true;		
						end
						i = i + 1;
					until notFound;

					local factoidHeaderString = tagString .. "_FACTOID_HEADING";
					local factoidBodyString = tagString .. "_FACTOID_TEXT";
					if TagExists( factoidHeaderString ) and TagExists( factoidBodyString ) then
						local thisFreeFormTextInstance = g_FreeFormTextManager:GetInstance();
						if thisFreeFormTextInstance then
							thisFreeFormTextInstance.FFTextHeader:SetText( Locale.ConvertTextKey( factoidHeaderString ));
							UpdateTextBlock( Locale.ConvertTextKey( factoidBodyString ), thisFreeFormTextInstance.FFTextLabel, thisFreeFormTextInstance.FFTextInnerFrame, thisFreeFormTextInstance.FFTextFrame );
						end
					end
					
					Controls.FFTextStack:SetHide( false );

				end

				-- update the related images
				Controls.RelatedImagesFrame:SetHide( true );
			end
		end
	else
		local civID = rawCivID - 1000;
		if civID ~= -1 then
		
			local thisCiv = GameInfo.Civilizations[civID];
			if thisCiv and thisCiv.Type ~= "CIVILIZATION_MINOR" and thisCiv.Type ~= "CIVILIZATION_BARBARIAN" then
							
				local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. thisCiv.Type .. "'" )().LeaderheadType];

				-- update the name
				local tagString = leader.CivilopediaTag;
				if tagString then
					local name = Locale.ConvertTextKey( tagString.."_NAME" );
					Controls.ArticleID:SetText( name );
					Controls.SubtitleLabel:SetText( Locale.ConvertTextKey( tagString.."_SUBTITLE" ) );
					Controls.SubtitleID:SetHide( false );
				else
					local name = Locale.ConvertTextKey( leader.Description )
					Controls.ArticleID:SetText( name );
				end

				-- update the portrait
				if IconHookup( leader.PortraitIndex, portraitSize, leader.IconAtlas, Controls.Portrait ) then
					Controls.PortraitFrame:SetHide( false );
				else
					Controls.PortraitFrame:SetHide( true );
				end

				local buttonAdded = 0;
				
				-- add titles etc.
				if tagString then
					Controls.LivedLabel:SetText( Locale.ConvertTextKey( tagString.."_LIVED" ) );
					Controls.LivedFrame:SetHide( false );

					local titlesString = tagString .. "_TITLES_";
					local notFound = false;
					local i = 1;
					local titles = "";
					local numTitles = 0;
					repeat
						local titlesTag = titlesString .. tostring( i );
						if TagExists( titlesTag ) then
							if numTitles > 0 then
								titles = titles .. "[NEWLINE][NEWLINE]";
							end
							numTitles = numTitles + 1;
							titles = titles .. Locale.ConvertTextKey( titlesTag );
						else
							notFound = true;		
						end
						i = i + 1;
					until notFound;
					if numTitles > 0 then
						UpdateNarrowTextBlock( Locale.ConvertTextKey( titles ), Controls.TitlesLabel, Controls.TitlesInnerFrame, Controls.TitlesFrame );
					end
				end

 				local condition = "LeaderType = '" .. leader.Type .. "'";
 	
				-- list of traits
 				--g_TraitsManager:ResetInstances();
				--buttonAdded = 0;
				--for row in GameInfo.Leader_Traits( condition ) do
					--local thisTrait = GameInfo.Traits[row.TraitType];
					--if thisTrait then
						--local thisTraitInstance = g_TraitsManager:GetInstance();
						--if thisTraitInstance then
							--local textureSheet;
							--local textureOffset;
							--textureSheet = defaultErrorTextureSheet;
							--textureOffset = nullOffset;
							--UpdateSmallButton( buttonAdded, thisTraitInstance.TraitImage, thisTraitInstance.TraitButton, textureSheet, textureOffset, CategoryGameConcepts, Locale.ConvertTextKey( thisTrait.ShortDescription ), thisTrait.ID ); -- todo: add a fudge factor
							--buttonAdded = buttonAdded + 1;
						--end
					--end
				--end
				--UpdateButtonFrame( buttonAdded, Controls.TraitsInnerFrame, Controls.TraitsFrame );	 	  
								--
				-- add the civ icon
				g_CivilizationsManager:ResetInstances();
				buttonAdded = 0;
				local thisCivInstance = g_CivilizationsManager:GetInstance();
				if thisCivInstance then
					local textureOffset, textureSheet = IconLookup( thisCiv.PortraitIndex, buttonSize, thisCiv.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisCivInstance.CivilizationImage, thisCivInstance.CivilizationButton, textureSheet, textureOffset, CategoryCivilizations, Locale.ConvertTextKey( thisCiv.ShortDescription ), thisCiv.ID );
					buttonAdded = buttonAdded + 1;
				end	
				UpdateButtonFrame( buttonAdded, Controls.CivilizationsInnerFrame, Controls.CivilizationsFrame );
						
				-- update the game info
				local leaderTrait = GameInfo.Leader_Traits(condition)();
				if leaderTrait then
					local trait = leaderTrait.TraitType;
					local traitString = Locale.ConvertTextKey( GameInfo.Traits[trait].ShortDescription ).."[NEWLINE][NEWLINE]"..Locale.ConvertTextKey( GameInfo.Traits[trait].Description );
					UpdateTextBlock( traitString, Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
				end
										
				-- add the free form text
				g_FreeFormTextManager:ResetInstances();
				if tagString then
					local headerString = tagString .. "_HEADING_";
					local bodyString = tagString .. "_TEXT_";
					local notFound = false;
					local i = 1;
					repeat
						local headerTag = headerString .. tostring( i );
						local bodyTag = bodyString .. tostring( i );
						if TagExists( headerTag ) and TagExists( bodyTag ) then
							local thisFreeFormTextInstance = g_FreeFormTextManager:GetInstance();
							if thisFreeFormTextInstance then
								thisFreeFormTextInstance.FFTextHeader:SetText( Locale.ConvertTextKey( headerTag ));
								UpdateTextBlock( Locale.ConvertTextKey( bodyTag ), thisFreeFormTextInstance.FFTextLabel, thisFreeFormTextInstance.FFTextInnerFrame, thisFreeFormTextInstance.FFTextFrame );
							end
						else
							notFound = true;		
						end
						i = i + 1;
					until notFound;
					
					notFound = false;
					i = 1;
					repeat
						local bodyString = tagString .. "_FACT_";
						local bodyTag = bodyString .. tostring( i );
						if TagExists( bodyTag ) then
							local thisFreeFormTextInstance = g_FreeFormTextManager:GetInstance();
							if thisFreeFormTextInstance then
								thisFreeFormTextInstance.FFTextHeader:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_FACTOID" ));
								UpdateTextBlock( Locale.ConvertTextKey( bodyTag ), thisFreeFormTextInstance.FFTextLabel, thisFreeFormTextInstance.FFTextInnerFrame, thisFreeFormTextInstance.FFTextFrame );
							end
						else
							notFound = true;		
						end
						i = i + 1;
					until notFound;
											
					Controls.FFTextStack:SetHide( false );

				end
				
				-- update the related images
				Controls.RelatedImagesFrame:SetHide( true );

			end
		end
	end

	ResizeEtc();

end

CivilopediaCategory[CategoryCityStates].SelectArticle = function( cityStateID, shouldAddToList )
	print("CivilopediaCategory[CategoryCityStates].SelectArticle");
	if selectedCategory ~= CategoryCityStates then
		SetSelectedCategory(CategoryCityStates);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryCityStates * absurdlyLargeNumTopicsInCategory) + cityStateID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if cityStateID ~= -1 then
		local thisCityState = GameInfo.MinorCivilizations[cityStateID];
		
		-- update the name
		local name = Locale.ConvertTextKey( thisCityState.Description ); 	
		Controls.ArticleID:SetText( name );
		
		-- portrait -- need to update this once they're in.
		--Controls.PortraitFrame:SetHide( false );
		
 		local contentSize;
 		local frameSize = {};
		local buttonAdded = 0;
 		
		-- update the game info
		
		-- update the historical info
		if (thisCityState.Civilopedia) then
			UpdateTextBlock( Locale.ConvertTextKey( thisCityState.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
		end
				
	end	

	ResizeEtc();

end


CivilopediaCategory[CategoryTerrain].SelectArticle = function( rawTerrainID, shouldAddToList )
	print("CivilopediaCategory[CategoryTerrain].SelectArticle");
	if selectedCategory ~= CategoryTerrain then
		SetSelectedCategory(CategoryTerrain);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryTerrain * absurdlyLargeNumTopicsInCategory) + rawTerrainID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end

	if rawTerrainID < 1000 then
		local terrainId = rawTerrainID;
		if terrainId ~= -1 then
		
			local thisTerrain = GameInfo.Terrains[terrainId];
			if thisTerrain then

				-- update the name
				local name = Locale.ConvertTextKey( thisTerrain.Description )
				Controls.ArticleID:SetText( name );

				-- update the portrait
				if IconHookup( thisTerrain.PortraitIndex, portraitSize, thisTerrain.IconAtlas, Controls.Portrait ) then
					Controls.PortraitFrame:SetHide( false );
				else
					Controls.PortraitFrame:SetHide( true );
				end

				local buttonAdded = 0;
		 		local condition = "TerrainType = '" .. thisTerrain.Type .. "'";
				
				-- City Yield
				Controls.YieldFrame:SetHide( false );
				local numYields = 0;
				local yieldString = "";
				for row in GameInfo.Terrain_Yields( condition ) do
					numYields = numYields + 1;
					yieldString = yieldString..tostring(row.Yield).." ";
					yieldString = yieldString..GameInfo.Yields[row.YieldType].IconString.." ";
				end
				-- special case hackery for hills
				if thisTerrain.Type == "TERRAIN_HILL" then
					--for row in GameInfo.Yields() do
						--if row.HillsChange ~= 0 then
							--numYields = numYields + 1;
							--if row.HillsChange > 0 then
								--yieldString = yieldString.."+";
							--end
							--yieldString = yieldString..tostring(row.HillsChange).." ";
							--yieldString = yieldString..row.IconString.." ";
						--end
					--end
					numYields = 1;
					yieldString = "2 [ICON_PRODUCTION]"
				end
				
				if numYields == 0 then
					Controls.YieldLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_NO_YIELD" ) );
				else
					Controls.YieldLabel:SetText( Locale.ConvertTextKey( yieldString ) );
				end
				
				-- Movement
				Controls.MovementCostFrame:SetHide( false );
				local moveCost = thisTerrain.Movement;
				-- special case hackery for hills
				if thisTerrain.Type == "TERRAIN_HILL" then
					moveCost = moveCost + GameDefines.HILLS_EXTRA_MOVEMENT;
				end
				if thisTerrain.Type == "TERRAIN_MOUNTAIN" then
					Controls.MovementCostLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPASSABLE" ) );
				else
					Controls.MovementCostLabel:SetText( Locale.ConvertTextKey( moveCost ).."[ICON_MOVES]" );
				end

				-- Combat Modifier
				Controls.CombatModFrame:SetHide( false );
				local combatModifier = 0;
				local combatModString = "";
				if thisTerrain.Type == "TERRAIN_HILL" or thisTerrain.Type == "TERRAIN_MOUNTAIN" then
					combatModifier = GameDefines.HILLS_EXTRA_DEFENSE;
				elseif thisTerrain.Water then
					combatModifier = 0;
				else
					combatModifier = GameDefines.FLAT_LAND_EXTRA_DEFENSE;
				end
				if combatModifier > 0 then
					combatModString = "+";
				end
				combatModString = combatModString..tostring(combatModifier).."%";
				Controls.CombatModLabel:SetText( combatModString );

				-- Features that can exist on this terrain
 				g_FeaturesManager:ResetInstances();
				buttonAdded = 0;
				for row in GameInfo.Feature_TerrainBooleans( condition ) do
					local thisFeature = GameInfo.Features[row.FeatureType];
					if thisFeature then
						local thisFeatureInstance = g_FeaturesManager:GetInstance();
						if thisFeatureInstance then
							local textureOffset, textureSheet = IconLookup( thisFeature.PortraitIndex, buttonSize, thisFeature.IconAtlas );				
							if textureOffset == nil then
								textureSheet = defaultErrorTextureSheet;
								textureOffset = nullOffset;
							end				
							UpdateSmallButton( buttonAdded, thisFeatureInstance.FeatureImage, thisFeatureInstance.FeatureButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisFeature.Description ), thisFeature.ID + 1000 ); -- todo: add a fudge factor
							buttonAdded = buttonAdded + 1;
						end
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.FeaturesInnerFrame, Controls.FeaturesFrame );	 	  

				-- Resources that can exist on this terrain
				Controls.ResourcesFoundLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCESFOUND_LABEL" ) );
 				g_ResourcesFoundManager:ResetInstances();
				buttonAdded = 0;
				for row in GameInfo.Resource_TerrainBooleans( condition ) do
					local thisResource = GameInfo.Resources[row.ResourceType];
					if thisResource then
						local thisResourceInstance = g_ResourcesFoundManager:GetInstance();
						if thisResourceInstance then
							local textureOffset, textureSheet = IconLookup( thisResource.PortraitIndex, buttonSize, thisResource.IconAtlas );				
							if textureOffset == nil then
								textureSheet = defaultErrorTextureSheet;
								textureOffset = nullOffset;
							end				
							UpdateSmallButton( buttonAdded, thisResourceInstance.ResourceFoundImage, thisResourceInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( thisResource.Description ), thisResource.ID );
							buttonAdded = buttonAdded + 1;
						end
					end
				end
				-- special case hackery for hills
				if thisTerrain.Type == "TERRAIN_HILL" then
					for thisResource in GameInfo.Resources() do
						if thisResource and thisResource.Hills then
							local thisResourceInstance = g_ResourcesFoundManager:GetInstance();
							if thisResourceInstance then
								local textureOffset, textureSheet = IconLookup( thisResource.PortraitIndex, buttonSize, thisResource.IconAtlas );				
								if textureOffset == nil then
									textureSheet = defaultErrorTextureSheet;
									textureOffset = nullOffset;
								end				
								UpdateSmallButton( buttonAdded, thisResourceInstance.ResourceFoundImage, thisResourceInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( thisResource.Description ), thisResource.ID );
								buttonAdded = buttonAdded + 1;
							end
						end
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.ResourcesFoundInnerFrame, Controls.ResourcesFoundFrame );	 	  

				-- generic text
				if (thisTerrain.Civilopedia) then
					UpdateTextBlock( Locale.ConvertTextKey( thisTerrain.Civilopedia ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
				end
				
				-- update the related images
				Controls.RelatedImagesFrame:SetHide( true );
			end
		end
	else
		local featureID = rawTerrainID - 1000;
		if featureID ~= -1 then
		
			local thisFeature;
			if featureID < 1000 then
				thisFeature = GameInfo.Features[featureID];
			else
				thisFeature = GameInfo.FakeFeatures[featureID-1000];
			end
			if thisFeature then

				-- update the name
				local name = Locale.ConvertTextKey( thisFeature.Description )
				Controls.ArticleID:SetText( name );

				-- update the portrait
				if IconHookup( thisFeature.PortraitIndex, portraitSize, thisFeature.IconAtlas, Controls.Portrait ) then
					Controls.PortraitFrame:SetHide( false );
				else
					Controls.PortraitFrame:SetHide( true );
				end

				local buttonAdded = 0;
		 		local condition = "FeatureType = '" .. thisFeature.Type .. "'";
				
				-- City Yield
				Controls.YieldFrame:SetHide( false );
				local numYields = 0;
				local yieldString = "";
				for row in GameInfo.Feature_YieldChanges( condition ) do
					numYields = numYields + 1;
					yieldString = yieldString..tostring(row.Yield).." ";
					yieldString = yieldString..GameInfo.Yields[row.YieldType].IconString.." ";
				end				
				-- add happiness since it is a quasi-yield
				if thisFeature.InBorderHappiness and thisFeature.InBorderHappiness ~= 0 then
					numYields = numYields + 1;
					yieldString = yieldString.." ";
					yieldString = yieldString..tostring(thisFeature.InBorderHappiness).."[ICON_HAPPINESS_1]".." ";
				end
				if numYields == 0 then
					Controls.YieldLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_NO_YIELD" ) );
				else
					Controls.YieldLabel:SetText( Locale.ConvertTextKey( yieldString ) );
				end
				
				-- Movement
				Controls.MovementCostFrame:SetHide( false );
				local moveCost = thisFeature.Movement;
				if thisFeature.Impassable then
					Controls.MovementCostLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPASSABLE" ) );
				else
					Controls.MovementCostLabel:SetText( Locale.ConvertTextKey( moveCost ).."[ICON_MOVES]" );
				end

				-- Combat Modifier
				Controls.CombatModFrame:SetHide( false );
				local combatModifier = thisFeature.Defense;
				local combatModString = "";
				if combatModifier > 0 then
					combatModString = "+";
				end
				combatModString = combatModString..tostring(combatModifier).."%";
				Controls.CombatModLabel:SetText( combatModString );

				-- Features that can exist on this terrain
 				g_FeaturesManager:ResetInstances();
				buttonAdded = 0;
				for row in GameInfo.Feature_TerrainBooleans( condition ) do
					local thisTerrain = GameInfo.Features[row.TerrainType];
					if thisTerrain then
						local thisTerrainInstance = g_FeaturesManager:GetInstance();
						if thisTerrainInstance then
							local textureOffset, textureSheet = IconLookup( thisTerrain.PortraitIndex, buttonSize, thisTerrain.IconAtlas );				
							if textureOffset == nil then
								textureSheet = defaultErrorTextureSheet;
								textureOffset = nullOffset;
							end				
							UpdateSmallButton( buttonAdded, thisTerrainInstance.FeatureImage, thisTerrainInstance.FeatureButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisTerrain.Description ), thisTerrain.ID );
							buttonAdded = buttonAdded + 1;
						end
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.TerrainsInnerFrame, Controls.TerrainsFrame );	 	  

				-- Resources that can exist on this feature
				Controls.ResourcesFoundLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCESFOUND_LABEL" ) );
  				g_ResourcesFoundManager:ResetInstances();
				buttonAdded = 0;
				for row in GameInfo.Resource_FeatureBooleans( condition ) do
					local thisResource = GameInfo.Resources[row.ResourceType];
					if thisResource then
						local thisResourceInstance = g_ResourcesFoundManager:GetInstance();
						if thisResourceInstance then
							local textureOffset, textureSheet = IconLookup( thisResource.PortraitIndex, buttonSize, thisResource.IconAtlas );				
							if textureOffset == nil then
								textureSheet = defaultErrorTextureSheet;
								textureOffset = nullOffset;
							end				
							UpdateSmallButton( buttonAdded, thisResourceInstance.ResourceFoundImage, thisResourceInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( thisResource.Description ), thisResource.ID );
							buttonAdded = buttonAdded + 1;
						end
					end
				end
				UpdateButtonFrame( buttonAdded, Controls.ResourcesFoundInnerFrame, Controls.ResourcesFoundFrame );	 	  

				if(featureID < 1000) then
					-- generic text
					if (thisFeature.Help) then
						UpdateTextBlock( Locale.ConvertTextKey( thisFeature.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
					end
					
					if (thisFeature.Civilopedia) then
						UpdateTextBlock( Locale.ConvertTextKey( thisFeature.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
					end			
				else
					if (thisFeature.Civilopedia) then
						UpdateTextBlock( Locale.ConvertTextKey( thisFeature.Civilopedia ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
					end				
				end
				
				-- update the related images
				Controls.RelatedImagesFrame:SetHide( true );
			end

		end
	end

	ResizeEtc();

end


CivilopediaCategory[CategoryResources].SelectArticle = function( resourceID, shouldAddToList )
	print("CivilopediaCategory[CategoryResources].SelectArticle");
	if selectedCategory ~= CategoryResources then
		SetSelectedCategory(CategoryResources);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryResources * absurdlyLargeNumTopicsInCategory) + resourceID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end

	if resourceID ~= -1 then
	
		local thisResource = GameInfo.Resources[resourceID];
		if thisResource then

			-- update the name
			local name = Locale.ConvertTextKey( thisResource.Description )
			Controls.ArticleID:SetText( name );

			-- update the portrait
			if IconHookup( thisResource.PortraitIndex, portraitSize, thisResource.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end

			local buttonAdded = 0;
	 		local condition = "ResourceType = '" .. thisResource.Type .. "'";
			
			-- tech visibility
			g_RevealTechsManager:ResetInstances();
			buttonAdded = 0;
			if thisResource.TechReveal then
				local prereq = GameInfo.Technologies[thisResource.TechReveal];
				local thisPrereqInstance = g_RevealTechsManager:GetInstance();
				if thisPrereqInstance then
					local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisPrereqInstance.RevealTechImage, thisPrereqInstance.RevealTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
					buttonAdded = buttonAdded + 1;
				end			
				UpdateButtonFrame( buttonAdded, Controls.RevealTechsInnerFrame, Controls.RevealTechsFrame );
			end

			-- City Yield
			Controls.YieldFrame:SetHide( false );
			local numYields = 0;
			local yieldString = "";
			for row in GameInfo.Resource_YieldChanges( condition ) do
				numYields = numYields + 1;
				if row.Yield > 0 then
					yieldString = yieldString.."+";
				end
				yieldString = yieldString..tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
			end
			if numYields == 0 then
				Controls.YieldLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_NO_YIELD" ) );
			else
				Controls.YieldLabel:SetText( Locale.ConvertTextKey( yieldString ) );
			end

			-- found on
				Controls.ResourcesFoundLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAINS_LABEL" ) );
			g_ResourcesFoundManager:ResetInstances(); -- okay, this is supposed to be a resource, but for now a round button is a round button
			buttonAdded = 0;
			for row in GameInfo.Resource_FeatureBooleans( condition ) do
				local thisFeature = GameInfo.Features[row.FeatureType];
				if thisFeature then
					local thisFeatureInstance = g_ResourcesFoundManager:GetInstance();
					if thisFeatureInstance then
						local textureOffset, textureSheet = IconLookup( thisFeature.PortraitIndex, buttonSize, thisFeature.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisFeatureInstance.ResourceFoundImage, thisFeatureInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisFeature.Description ), thisFeature.ID + 1000 ); -- todo: add a fudge factor
						buttonAdded = buttonAdded + 1;
					end
				end
			end
			
			local bAlreadyShowingHills = false;
			for row in GameInfo.Resource_TerrainBooleans( condition ) do
				local thisTerrain = GameInfo.Terrains[row.TerrainType];
				if thisTerrain then
					local thisTerrainInstance = g_ResourcesFoundManager:GetInstance();
					if thisTerrainInstance then
						if(row.TerrainType == "TERRAIN_HILL") then
							bAlreadyShowingHills = true;
						end
					
						local textureOffset, textureSheet = IconLookup( thisTerrain.PortraitIndex, buttonSize, thisTerrain.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisTerrainInstance.ResourceFoundImage, thisTerrainInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisTerrain.Description ), thisTerrain.ID );
						buttonAdded = buttonAdded + 1;
					end
				end
			end
			-- hackery for hills
			if thisResource and thisResource.Hills and not bAlreadyShowingHills then
				local thisTerrain = GameInfo.Terrains["TERRAIN_HILL"];
				local thisTerrainInstance = g_ResourcesFoundManager:GetInstance();
				if thisTerrainInstance then
					local textureOffset, textureSheet = IconLookup( thisTerrain.PortraitIndex, buttonSize, thisTerrain.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisTerrainInstance.ResourceFoundImage, thisTerrainInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisTerrain.Description ), thisTerrain.ID );
					buttonAdded = buttonAdded + 1;
				end
			end
			UpdateButtonFrame( buttonAdded, Controls.ResourcesFoundInnerFrame, Controls.ResourcesFoundFrame );	 	  
			
			-- improvement
			g_ImprovementsManager:ResetInstances();
			buttonAdded = 0;
			for row in GameInfo.Improvement_ResourceTypes( condition ) do
				local thisImprovement = GameInfo.Improvements[row.ImprovementType];
				if thisImprovement then
					local thisImprovementInstance = g_ImprovementsManager:GetInstance();
					if thisImprovementInstance then
						local textureOffset, textureSheet = IconLookup( thisImprovement.PortraitIndex, buttonSize, thisImprovement.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisImprovementInstance.ImprovementImage, thisImprovementInstance.ImprovementButton, textureSheet, textureOffset, CategoryImprovements, Locale.ConvertTextKey( thisImprovement.Description ), thisImprovement.ID );
						buttonAdded = buttonAdded + 1;
					end
				end
			end
			UpdateButtonFrame( buttonAdded, Controls.ImprovementsInnerFrame, Controls.ImprovementsFrame );	 	  
			
			--CBP
			numYields = 0;
			yieldString = "";
			fullstring = ""
			local numImprovement = 0;

			for row in GameInfo.Improvement_ResourceType_Yields( condition ) do
				numYields = numYields + 1;			
				local thisImprovement = GameInfo.Improvements[row.ImprovementType];
				if thisImprovement then
					if numImprovement > 0 then
						yieldString = yieldString.."[NEWLINE]";
					end
					if row.Yield > 0 then
						yieldString = yieldString.."+";
					end
					numImprovement = numImprovement + 1;
					yieldString = yieldString..tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ("..Locale.ConvertTextKey( thisImprovement.Description )..") "
				end
			end
			if numYields == 0 then
				Controls.ImprovementYieldFrame:SetHide( true );
			else
				fullstring = Locale.ConvertTextKey( yieldString );
				Controls.ImprovementYieldLabel:SetText( fullstring );
				Controls.ImprovementYieldFrame:SetHide( false );
			end
			--END

			-- game info
			if (thisResource.Help) then
				UpdateTextBlock( Locale.ConvertTextKey( thisResource.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end

			-- generic text
			if (thisResource.Civilopedia) then
				UpdateTextBlock( Locale.ConvertTextKey( thisResource.Civilopedia ), Controls.HistoryLabel, Controls.HistoryInnerFrame, Controls.HistoryFrame );
			end

			
			
			-- update the related images
			Controls.RelatedImagesFrame:SetHide( true );
			
			-- Infixo Extended info
			--local sText = "[COLOR_CYAN]Buildings[ENDCOLOR] with this specialist:"; -- change to TXT_KEY_ later
			--for row in GameInfo.Buildings() do
			--	if row.SpecialistType == thisPerson.Type and row.SpecialistCount > 0 then 
			--		sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %d[ICON_SPY]", row.SpecialistCount);
			--	end
			--end
			sText = "[COLOR_CYAN]Buildings[ENDCOLOR] that improve this resource:";
			local sql = [[
				SELECT Buildings.Description, Yields.IconString, Building_ResourceYieldChanges.Yield
				FROM Building_ResourceYieldChanges INNER JOIN Yields, Buildings ON Building_ResourceYieldChanges.YieldType = Yields.Type AND Building_ResourceYieldChanges.BuildingType = Buildings.Type
				WHERE Building_ResourceYieldChanges.ResourceType = ?]];
			for row in DB.Query(sql, thisResource.Type) do
				sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %+d", row.Yield)..row.IconString;
			end
			--sText = sText.."[NEWLINE][COLOR_CYAN]Local Yields[ENDCOLOR] granted by buildings:";
			--local sql = [[
			--	SELECT Buildings.Description, Yields.IconString, Building_SpecialistYieldChangesLocal.Yield
			--	FROM Building_SpecialistYieldChangesLocal INNER JOIN Yields, Buildings ON Building_SpecialistYieldChangesLocal.YieldType = Yields.Type AND Building_SpecialistYieldChangesLocal.BuildingType = Buildings.Type
			--	WHERE Building_SpecialistYieldChangesLocal.SpecialistType = ?]];
			--for row in DB.Query(sql, thisPerson.Type) do
			--	sText = sText.."[NEWLINE][ICON_BULLET]"..Locale.Lookup(row.Description)..string.format(" %+d", row.Yield)..row.IconString;
			--end
			UpdateTextBlock( sText, Controls.ExtendedLabel,  Controls.ExtendedInnerFrame,  Controls.ExtendedFrame );
			-- end Infixo
			
		end
	end

	ResizeEtc();

end


CivilopediaCategory[CategoryImprovements].SelectArticle = function( improvementID, shouldAddToList )
	print("CivilopediaCategory[CategoryImprovements].SelectArticle");
	if selectedCategory ~= CategoryImprovements then
		SetSelectedCategory(CategoryImprovements);
	end
	
	ClearArticle();
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryImprovements * absurdlyLargeNumTopicsInCategory) + improvementID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end

	if improvementID ~= -1 and improvementID < 1000 then
	
		local thisImprovement = GameInfo.Improvements[improvementID];
		if thisImprovement then

			-- update the name
			local name = Locale.ConvertTextKey( thisImprovement.Description )
			Controls.ArticleID:SetText( name );

			-- update the portrait
			if IconHookup( thisImprovement.PortraitIndex, portraitSize, thisImprovement.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end

			local buttonAdded = 0;
	 		local condition = "ImprovementType = '" .. thisImprovement.Type .. "'";
			
			-- tech visibility
			g_PrereqTechManager:ResetInstances();
			buttonAdded = 0;

			local prereq = nil;
			for row in GameInfo.Builds( condition ) do
				if row.PrereqTech then
					prereq = GameInfo.Technologies[row.PrereqTech];
				end
			end
			
			if prereq then
				local thisPrereqInstance = g_PrereqTechManager:GetInstance();
				if thisPrereqInstance then
					local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisPrereqInstance.PrereqTechImage, thisPrereqInstance.PrereqTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
					buttonAdded = buttonAdded + 1;
					UpdateButtonFrame( buttonAdded, Controls.PrereqTechInnerFrame, Controls.PrereqTechFrame );
				end
			end

			-- City Yield
			local numYields = 0;
			local yieldString = "";
			for row in GameInfo.Improvement_Yields( condition ) do
				numYields = numYields + 1;
				if row.Yield > 0 then
					yieldString = yieldString.."+";
				end
				yieldString = yieldString..tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
			end
			if numYields == 0 then
				Controls.YieldLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_NO_YIELD" ) );
			else
				Controls.YieldLabel:SetText( Locale.ConvertTextKey( yieldString ) );
				Controls.YieldFrame:SetHide( false );
			end
			
			-- add in mountain adjacency yield
			numYields = 0;
			yieldString = "";
			for row in GameInfo.Improvement_AdjacentMountainYieldChanges( condition ) do
				numYields = numYields + 1;
				yieldString = yieldString..tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
			end
			if numYields == 0 then
				Controls.MountainYieldFrame:SetHide( true );
			else
				Controls.MountainYieldLabel:SetText( Locale.ConvertTextKey( yieldString ) );
				Controls.MountainYieldFrame:SetHide( false );
			end

			--CBP
			numYields = 0;
			yieldString = "";
			improvementString = "";
			fullstring = "";
			local baseImprovement = "";
			for row in GameInfo.Improvement_RouteYieldChanges( condition ) do
				numYields = numYields + 1;
				local OtherImprovement = GameInfo.Routes[row.RouteType];
				if OtherImprovement then
					improvementString = Locale.ConvertTextKey(OtherImprovement.Description)..": ";
					if(OtherImprovement ~= baseImprovement) then
						baseImprovement = OtherImprovement;
						if(fullstring == "") then
							fullstring = fullstring .. improvementString;
						else
							fullstring = fullstring .. "[NEWLINE]" .. improvementString;
						end
					end
					yieldString = "+" .. tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
					fullstring = fullstring .. Locale.ConvertTextKey( yieldString );
				end
			end
			if numYields == 0 then
				Controls.TradeRouteYieldFrame:SetHide( true );
			else
				Controls.TradeRouteYieldLabel:SetText( fullstring );
				Controls.TradeRouteYieldFrame:SetHide( false );
			end

			--CBP

			numYields = 0;
			yieldString = "";
			terrainString = "";
			fullstring = "";
			baseTerrain = "";
			for row in GameInfo.Improvement_AdjacentTerrainYieldChanges( condition ) do
				numYields = numYields + 1;
				local Terrain = GameInfo.Terrains[row.TerrainType];
				if Terrain then
					terrainString = Locale.ConvertTextKey(Terrain.Description)..": ";
					if(Terrain ~= baseTerrain) then
						baseTerrain = Terrain;
						if(fullstring == "") then
							fullstring = fullstring .. terrainString;
						else
							fullstring = fullstring .. "[NEWLINE]" .. terrainString;
						end
					end
					yieldString = "+" .. tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
					fullstring = fullstring .. Locale.ConvertTextKey( yieldString );
				end
			end
			if numYields == 0 then
				Controls.AdjacentTerrainYieldFrame:SetHide( true );
			else
				Controls.AdjacentTerrainYieldLabel:SetText( fullstring );
				Controls.AdjacentTerrainYieldFrame:SetHide( false );
			end

			numYields = 0;
			yieldString = "";
			improvementString = "";
			fullstring = "";
			baseImprovement = "";
			for row in GameInfo.Improvement_AdjacentImprovementYieldChanges( condition ) do
				numYields = numYields + 1;
				local OtherImprovement = GameInfo.Improvements[row.OtherImprovementType];
				if OtherImprovement then
					improvementString = Locale.ConvertTextKey(OtherImprovement.Description)..": ";
					if(OtherImprovement ~= baseImprovement) then
						baseImprovement = OtherImprovement;
						if(fullstring == "") then
							fullstring = fullstring .. improvementString;
						else
							fullstring = fullstring .. "[NEWLINE]" .. improvementString;
						end
					end
					yieldString = "+" .. tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
					fullstring = fullstring .. Locale.ConvertTextKey( yieldString );
				end
			end
			if numYields == 0 then
				Controls.AdjacentYieldFrame:SetHide( true );
			else
				Controls.AdjacentYieldLabel:SetText( fullstring );
				Controls.AdjacentYieldFrame:SetHide( false );
			end

			numYields = 0;
			yieldString = "";
			improvementString = "";
			fullstring = "";
			baseImprovement = "";
			for row in GameInfo.Improvement_YieldAdjacentTwoSameType( condition ) do
				numYields = numYields + 1;
				local OtherImprovement = GameInfo.Improvements[row.ImprovementType];
				if OtherImprovement then
					improvementString = Locale.ConvertTextKey(OtherImprovement.Description)..": ";
					if(OtherImprovement ~= baseImprovement) then
						baseImprovement = OtherImprovement;
						if(fullstring == "") then
							fullstring = fullstring .. improvementString;
						else
							fullstring = fullstring .. "[NEWLINE]" .. improvementString;
						end
					end
					yieldString = "+" .. tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
					fullstring = fullstring .. Locale.ConvertTextKey( yieldString );
				end
			end
			if numYields == 0 then
				Controls.TwoAdjacentImprovYieldFrame:SetHide( true );
			else
				Controls.TwoAdjacentImprovYieldLabel:SetText( fullstring );
				Controls.TwoAdjacentImprovYieldFrame:SetHide( false );
			end

			numYields = 0;
			yieldString = "";
			improvementString = "";
			fullstring = "";
			baseImprovement = "";
			for row in GameInfo.Improvement_YieldAdjacentSameType( condition ) do
				numYields = numYields + 1;
				local OtherImprovement = GameInfo.Improvements[row.ImprovementType];
				if OtherImprovement then
					improvementString = Locale.ConvertTextKey(OtherImprovement.Description)..": ";
					if(OtherImprovement ~= baseImprovement) then
						baseImprovement = OtherImprovement;
						if(fullstring == "") then
							fullstring = fullstring .. improvementString;
						else
							fullstring = fullstring .. "[NEWLINE]" .. improvementString;
						end
					end
					yieldString = "+" .. tostring(row.Yield)..GameInfo.Yields[row.YieldType].IconString.." ";
					fullstring = fullstring .. Locale.ConvertTextKey( yieldString );
				end
			end
			if numYields == 0 then
				Controls.AdjacentImprovYieldFrame:SetHide( true );
			else
				Controls.AdjacentImprovYieldLabel:SetText( fullstring );
				Controls.AdjacentImprovYieldFrame:SetHide( false );
			end
			--END
			
			buttonAdded = 0;
			if thisImprovement.CivilizationType then
				local thisCiv = GameInfo.Civilizations[thisImprovement.CivilizationType];
				if thisCiv then
					g_CivilizationsManager:ResetInstances();
					local thisCivInstance = g_CivilizationsManager:GetInstance();
					if thisCivInstance then
						local textureOffset, textureSheet = IconLookup( thisCiv.PortraitIndex, buttonSize, thisCiv.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisCivInstance.CivilizationImage, thisCivInstance.CivilizationButton, textureSheet, textureOffset, CategoryCivilizations, Locale.ConvertTextKey( thisCiv.ShortDescription ), thisCiv.ID );
						buttonAdded = buttonAdded + 1;
					end	
				end
			end
			UpdateButtonFrame( buttonAdded, Controls.CivilizationsInnerFrame, Controls.CivilizationsFrame );
			
			-- found on
			Controls.ResourcesFoundLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_FOUNDON_LABEL" ) );
			g_ResourcesFoundManager:ResetInstances(); -- okay, this is supposed to be a resource, but for now a round button is a round button
			buttonAdded = 0;
			for row in GameInfo.Improvement_ValidFeatures( condition ) do
				local thisFeature = GameInfo.Features[row.FeatureType];
				if thisFeature then
					local thisFeatureInstance = g_ResourcesFoundManager:GetInstance();
					if thisFeatureInstance then
						local textureOffset, textureSheet = IconLookup( thisFeature.PortraitIndex, buttonSize, thisFeature.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisFeatureInstance.ResourceFoundImage, thisFeatureInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisFeature.Description ), thisFeature.ID + 1000 ); -- todo: add a fudge factor
						buttonAdded = buttonAdded + 1;
					end
				end
			end
			for row in GameInfo.Improvement_ValidTerrains( condition ) do
				local thisTerrain = GameInfo.Terrains[row.TerrainType];
				if thisTerrain then
					local thisTerrainInstance = g_ResourcesFoundManager:GetInstance();
					if thisTerrainInstance then
						local textureOffset, textureSheet = IconLookup( thisTerrain.PortraitIndex, buttonSize, thisTerrain.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisTerrainInstance.ResourceFoundImage, thisTerrainInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisTerrain.Description ), thisTerrain.ID );
						buttonAdded = buttonAdded + 1;
					end
				end
			end
			-- hackery for hills
			--if thisImprovement and thisImprovement.HillsMakesValid then
				--local thisTerrain = GameInfo.Terrains["TERRAIN_HILL"];
				--local thisTerrainInstance = g_ResourcesFoundManager:GetInstance();
				--if thisTerrainInstance then
					--local textureSheet;
					--local textureOffset;
					--textureSheet = defaultErrorTextureSheet;
					--textureOffset = nullOffset;
					--UpdateSmallButton( buttonAdded, thisTerrainInstance.ResourceFoundImage, thisTerrainInstance.ResourceFoundButton, textureSheet, textureOffset, CategoryTerrain, Locale.ConvertTextKey( thisTerrain.Description ), thisTerrain.ID );
					--buttonAdded = buttonAdded + 1;
				--end
			--end
			UpdateButtonFrame( buttonAdded, Controls.ResourcesFoundInnerFrame, Controls.ResourcesFoundFrame );	 	  
			
			Controls.RequiredResourcesLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPROVES_RESRC_LABEL" ) );
			g_RequiredResourcesManager:ResetInstances();
			buttonAdded = 0;
			for row in GameInfo.Improvement_ResourceTypes( condition ) do
				local requiredResource = GameInfo.Resources[row.ResourceType];
				if requiredResource then
					local thisRequiredResourceInstance = g_RequiredResourcesManager:GetInstance();
					if thisRequiredResourceInstance then
						local textureOffset, textureSheet = IconLookup( requiredResource.PortraitIndex, buttonSize, requiredResource.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisRequiredResourceInstance.RequiredResourceImage, thisRequiredResourceInstance.RequiredResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( requiredResource.Description ), requiredResource.ID );
						buttonAdded = buttonAdded + 1;
					end
				end		
			end
			UpdateButtonFrame( buttonAdded, Controls.RequiredResourcesInnerFrame, Controls.RequiredResourcesFrame );

			-- update the game info
			if (thisImprovement.Help) then
				UpdateTextBlock( Locale.ConvertTextKey( thisImprovement.Help ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end

			-- generic text
			if (thisImprovement.Civilopedia) then
				UpdateTextBlock( Locale.ConvertTextKey( thisImprovement.Civilopedia ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end
			
			-- update the related images
			Controls.RelatedImagesFrame:SetHide( true );
		end
	--Roads and Railroads
	elseif improvementID ~= -1 then
	
		improvementID = improvementID - 1000;
		local thisImprovement = GameInfo.Routes[improvementID];
		if thisImprovement then

			-- update the name
			local name = Locale.ConvertTextKey( thisImprovement.Description )
			Controls.ArticleID:SetText( name );

			if IconHookup( thisImprovement.PortraitIndex, portraitSize, thisImprovement.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end

			local buttonAdded = 0;
	 		local condition = "RouteType = '" .. thisImprovement.Type .. "'";
			
			-- tech visibility
			g_PrereqTechManager:ResetInstances();
			buttonAdded = 0;

			local prereq = nil;
			for row in GameInfo.Builds( condition ) do
				if row.PrereqTech then
					prereq = GameInfo.Technologies[row.PrereqTech];
				end
			end
			
			if prereq then
				local thisPrereqInstance = g_PrereqTechManager:GetInstance();
				if thisPrereqInstance then
					local textureOffset, textureSheet = IconLookup( prereq.PortraitIndex, buttonSize, prereq.IconAtlas );				
					if textureOffset == nil then
						textureSheet = defaultErrorTextureSheet;
						textureOffset = nullOffset;
					end				
					UpdateSmallButton( buttonAdded, thisPrereqInstance.PrereqTechImage, thisPrereqInstance.PrereqTechButton, textureSheet, textureOffset, CategoryTech, Locale.ConvertTextKey( prereq.Description ), prereq.ID );
					buttonAdded = buttonAdded + 1;
					UpdateButtonFrame( buttonAdded, Controls.PrereqTechInnerFrame, Controls.PrereqTechFrame );
				end
			end

			-- generic text
			if (thisImprovement.Civilopedia) then
				UpdateTextBlock( Locale.ConvertTextKey( thisImprovement.Civilopedia ), Controls.GameInfoLabel, Controls.GameInfoInnerFrame, Controls.GameInfoFrame );
			end
			
			-- update the related images
			Controls.RelatedImagesFrame:SetHide( true );
		end
	end

	ResizeEtc();
end

CivilopediaCategory[CategoryBeliefs].SelectArticle = function(entryID, shouldAddToList)
	if selectedCategory ~= CategoryBeliefs then
		SetSelectedCategory(CategoryBeliefs);
	end
	
	ClearArticle();
	
	local offset = 0;
	if(entryID[1] == "Beliefs") then
		offset = #GameInfo.Religions;
	end
	
	offset = offset + entryID[2];
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryBeliefs * absurdlyLargeNumTopicsInCategory) + offset];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if (entryID ~= nil) then
	
		local t = entryID[1];
		local id = entryID[2];
		
		if(t == "Religions") then
		
			local thisReligion = GameInfo[t][id];
		
			if (thisReligion ~= nil) then
			
				-- update the name	
				Controls.ArticleID:LocalizeAndSetText(thisReligion.Description);
				
				-- update the summary
				if (thisReligion.Civilopedia ~= nil) then
					UpdateTextBlock( Locale.ConvertTextKey( thisReligion.Civilopedia ), Controls.SummaryLabel, Controls.SummaryInnerFrame, Controls.SummaryFrame );
				end		
			end
		
		else
			local thisBelief = GameInfo[t][id];
		
			if (thisBelief ~= nil) then
			
				-- update the name
				Controls.ArticleID:LocalizeAndSetText(thisBelief.ShortDescription);
				
				-- update the summary
				if (thisBelief.Description ~= nil) then
					UpdateTextBlock( Locale.ConvertTextKey( thisBelief.Description ), Controls.SummaryLabel, Controls.SummaryInnerFrame, Controls.SummaryFrame );
				end		
			end
		end
	end	

	ResizeEtc();

end

CivilopediaCategory[CategoryWorldCongress].SelectArticle = function(entryID, shouldAddToList)
	if selectedCategory ~= CategoryWorldCongress then
		SetSelectedCategory(CategoryWorldCongress);
	end
	
	ClearArticle();
	
	local offset = 0;	
	offset = offset + entryID[2];
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryWorldCongress * absurdlyLargeNumTopicsInCategory) + offset];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if (entryID ~= nil) then
	
		local t = entryID[1];
		local id = entryID[2];
		
		if(t == "Resolutions") then
		
			local thisResolution = GameInfo[t][id];
		
			if (thisResolution ~= nil) then
			
				Controls.Portrait:SetTexture("WorldCongressPortrait256_EXP2.dds");
				Controls.Portrait:SetTextureOffsetVal(0,0);
				Controls.PortraitFrame:SetHide(false);
				
				-- update the name	
				Controls.ArticleID:LocalizeAndSetText(thisResolution.Description);
				
				-- update the summary
				if (thisResolution.Help ~= nil) then
					UpdateTextBlock( Locale.ConvertTextKey( thisResolution.Help ), Controls.SummaryLabel, Controls.SummaryInnerFrame, Controls.SummaryFrame );
				end		
			end
		
		elseif(t == "LeagueProjects") then
			
			local thisLeagueProject = GameInfo[t][id];
			
			if (thisLeagueProject ~= nil) then
			
				-- update the portrait
				if IconHookup( thisLeagueProject.PortraitIndex, portraitSize, thisLeagueProject.IconAtlas, Controls.Portrait ) then
					Controls.PortraitFrame:SetHide( false );
				else
					Controls.PortraitFrame:SetHide( true );
				end
				
				-- update the name	
				Controls.ArticleID:LocalizeAndSetText(thisLeagueProject.Description);
				
				-- update the summary
				local sSummary = GetLeagueProjectPediaText(id);
				if (sSummary ~= nil and sSummary ~= "") then
					UpdateTextBlock( sSummary , Controls.SummaryLabel, Controls.SummaryInnerFrame, Controls.SummaryFrame );
				end
				
				-- update the cost
				local cost = thisLeagueProject.CostPerPlayer;
				if(Game ~= nil and Game.GetNumActiveLeagues() > 0) then
					local pLeague = Game.GetActiveLeague();
					if (pLeague ~= nil) then
						cost = pLeague:GetProjectCostPerPlayer(id) / 100;
					end
				end
				if (cost > 0) then
					Controls.CostFrame:SetHide( false );
					Controls.CostLabel:SetText( Locale.ConvertTextKey("TXT_KEY_LEAGUE_PROJECT_COST_PER_PLAYER", cost));
				end
			end
		end
	end	

	ResizeEtc();
end

function GetLeagueProjectPediaText(iLeagueProjectID)
	local s = "";
	
	local TrophyIcons = {
		"[ICON_TROPHY_BRONZE]",
		"[ICON_TROPHY_SILVER]",
		"[ICON_TROPHY_GOLD]",
	};
	
	local tProject = GameInfo.LeagueProjects[iLeagueProjectID];
	if (tProject ~= nil) then
		for i = 3, 1, -1 do
			local reward = tProject["RewardTier" .. i];
			if (reward ~= nil) then
				local tReward = GameInfo.LeagueProjectRewards[reward];
				if (tReward ~= nil) then
					if (tReward.Description ~= nil and tReward.Help ~= nil) then
						s = s .. Locale.Lookup("TXT_KEY_PEDIA_LEAGUE_PROJECT_REWARD", TrophyIcons[i], tReward.Description, tReward.Help);
					end
				end
			end
				
			if (i > 1) then
				s = s .. "[NEWLINE][NEWLINE]";
			end
		end
	end
	
	return s;
end

CivilopediaCategory[CategoryCorporations].SelectArticle = function(corporationID, shouldAddToList)
	if selectedCategory ~= CategoryCorporations then
		SetSelectedCategory(CategoryCorporations);
	end
	
	ClearArticle();
	
	local buttonAdded = 0;
	
	if shouldAddToList == addToList then
		currentTopic = currentTopic + 1;
		listOfTopicsViewed[currentTopic] = categorizedList[(CategoryCorporations * absurdlyLargeNumTopicsInCategory) + corporationID];
		for i = currentTopic + 1, endTopic, 1 do
			listOfTopicsViewed[i] = nil;
		end
		endTopic = currentTopic;
	end
	
	if (corporationID ~= -1 and corporationID < 1000) then	
		local thisCorporation = GameInfo.Corporations[corporationID];
	
		if (thisCorporation ~= nil) then
		
			if IconHookup( thisCorporation.PortraitIndex, portraitSize, thisCorporation.IconAtlas, Controls.Portrait ) then
				Controls.PortraitFrame:SetHide( false );
			else
				Controls.PortraitFrame:SetHide( true );
			end
			
			-- update the name	
			Controls.ArticleID:LocalizeAndSetText(thisCorporation.Description);
			
			local helpStr = "";
			if(thisCorporation.Help ~= nil) then
				helpStr = helpStr .. Locale.ConvertTextKey(thisCorporation.Help);
			end
			
			local numFreeResources = 0;
			local condition = "CorporationType = '" .. thisCorporation.Type .. "'";
			for row in GameInfo.Corporation_NumFreeResource( condition ) do	
				local freeResource = GameInfo.Resources[row.ResourceType];
				if freeResource then	
					if(numFreeResources == 0) then
						helpStr = helpStr .. "[NEWLINE]";
					end	
					helpStr = helpStr .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_FREE_RESOURCE", row.NumResource, freeResource.IconString, freeResource.Description );
					numFreeResources = numFreeResources + 1;
				end
			end
			
			-- update the summary
			if (helpStr ~= nil) then
				UpdateTextBlock( helpStr, Controls.SummaryLabel, Controls.SummaryInnerFrame, Controls.SummaryFrame );
			end
			
			if(thisCorporation.ResourceBonusHelp ~= nil) then
				UpdateTextBlock( Locale.ConvertTextKey( thisCorporation.ResourceBonusHelp ), Controls.CorporationResourceBonusLabel, Controls.CorpResourceBonusInnerFrame, Controls.CorporationResourceBonusFrame );
			end
			
			if(thisCorporation.OfficeBonusHelp ~= nil) then
				UpdateTextBlock( Locale.ConvertTextKey( thisCorporation.OfficeBonusHelp ), Controls.CorporationOfficeBonusLabel, Controls.CorpOfficeBonusInnerFrame, Controls.CorporationOfficeBonusFrame);
			end
			
			if(thisCorporation.TradeRouteBonusHelp ~= nil) then
				UpdateTextBlock( Locale.ConvertTextKey( thisCorporation.TradeRouteBonusHelp ), Controls.CorporationTRBonusLabel, Controls.CorporationTRBonusInnerFrame, Controls.CorporationTRBonusFrame);
			end
			
			-- update free trade routes
			local freeTRs = thisCorporation.NumFreeTradeRoutes;
			if(freeTRs > 0) then
				Controls.FreeTRFrame:SetHide(false);
				Controls.FreeTRLabel:SetText(freeTRs .. " [ICON_INTERNATIONAL_TRADE]");
			end
			
			g_MonopolyResourcesManager:ResetInstances();
			buttonAdded = 0;

			local condition = "CorporationType = '" .. thisCorporation.Type .. "'";
			for row in GameInfo.Corporation_ResourceMonopolyOrs( condition ) do
				local requiredResource = GameInfo.Resources[row.ResourceType];
				if requiredResource then
					local thisLocalResourceInstance = g_MonopolyResourcesManager:GetInstance();
					if thisLocalResourceInstance then
						local textureOffset, textureSheet = IconLookup( requiredResource.PortraitIndex, buttonSize, requiredResource.IconAtlas );				
						if textureOffset == nil then
							textureSheet = defaultErrorTextureSheet;
							textureOffset = nullOffset;
						end				
						UpdateSmallButton( buttonAdded, thisLocalResourceInstance.MonopolyResourceImage, thisLocalResourceInstance.MonopolyResourceButton, textureSheet, textureOffset, CategoryResources, Locale.ConvertTextKey( requiredResource.Description ), requiredResource.ID );
						buttonAdded = buttonAdded + 1;
					end
				end		
			end
			UpdateButtonFrame( buttonAdded, Controls.MonopolyResourcesInnerFrame, Controls.MonopolyResourcesFrame );
			
			g_CorpHeadquartersManager:ResetInstances();
			g_CorpOfficeManager:ResetInstances();
			g_CorpFranchiseManager:ResetInstances();
			
			local headquarters = GameInfo.BuildingClasses[thisCorporation.HeadquartersBuildingClass];
			if(headquarters) then
				local buildingInfo = GameInfo.Buildings[headquarters.DefaultBuilding];
				if(buildingInfo) then
					local instance = g_CorpHeadquartersManager:GetInstance();
					if instance then
						if not IconHookup( buildingInfo.PortraitIndex, buttonSize, buildingInfo.IconAtlas, instance.CorporationBuildingImage ) then
							instance.CorporationBuildingImage:SetTexture( defaultErrorTextureSheet );
							instance.CorporationBuildingImage:SetTextureOffset( nullOffset );
						end
						
						--move this button
						instance.CorporationBuildingButton:SetOffsetVal( buttonPadding, buttonPadding );
						
						instance.CorporationBuildingButton:SetToolTipString( Locale.ConvertTextKey( buildingInfo.Description ) );
						instance.CorporationBuildingButton:SetVoids( buildingInfo.ID, addToList );
						
						if headquarters.MaxGlobalInstances > 0 or (headquarters.MaxPlayerInstances == 1 and headquarters.SpecialistCount == 0) or headquarters.MaxTeamInstances > 0 then
							instance.CorporationBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
						else
							instance.CorporationBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
						end
					end
				end
			end
			
			local office = GameInfo.BuildingClasses[thisCorporation.OfficeBuildingClass];
			if(office ~= nil) then
				local buildingInfo = GameInfo.Buildings[office.DefaultBuilding];
				if(buildingInfo) then
					local instance = g_CorpOfficeManager:GetInstance();
					if instance then
						if not IconHookup( buildingInfo.PortraitIndex, buttonSize, buildingInfo.IconAtlas, instance.CorporationBuildingImage ) then
							instance.CorporationBuildingImage:SetTexture( defaultErrorTextureSheet );
							instance.CorporationBuildingImage:SetTextureOffset( nullOffset );
						end
							
						--move this button
						instance.CorporationBuildingButton:SetOffsetVal( buttonPadding, buttonPadding );
						
						instance.CorporationBuildingButton:SetToolTipString( Locale.ConvertTextKey( buildingInfo.Description ) );
						instance.CorporationBuildingButton:SetVoids( buildingInfo.ID, addToList );
						
						if office.MaxGlobalInstances > 0 or (office.MaxPlayerInstances == 1 and office.SpecialistCount == 0) or office.MaxTeamInstances > 0 then
							instance.CorporationBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
						else
							instance.CorporationBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
						end
					end
				end
			end
			
			local franchise = GameInfo.BuildingClasses[thisCorporation.FranchiseBuildingClass];
			if(franchise ~= nil) then
				local buildingInfo = GameInfo.Buildings[franchise.DefaultBuilding];
				if(buildingInfo) then
					local instance = g_CorpFranchiseManager:GetInstance();
					if instance then
						if not IconHookup( buildingInfo.PortraitIndex, buttonSize, buildingInfo.IconAtlas, instance.CorporationBuildingImage ) then
							instance.CorporationBuildingImage:SetTexture( defaultErrorTextureSheet );
							instance.CorporationBuildingImage:SetTextureOffset( nullOffset );
						end
							
						--move this button
						instance.CorporationBuildingButton:SetOffsetVal( buttonPadding, buttonPadding );
						
						instance.CorporationBuildingButton:SetToolTipString( Locale.ConvertTextKey( buildingInfo.Description ) );
						instance.CorporationBuildingButton:SetVoids( buildingInfo.ID, addToList );
						
						if franchise.MaxGlobalInstances > 0 or (franchise.MaxPlayerInstances == 1 and franchise.SpecialistCount == 0) or franchise.MaxTeamInstances > 0 then
							instance.CorporationBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
						else
							instance.CorporationBuildingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
						end
					end
				end
			end

			UpdateButtonFrame( 1, Controls.CorpHeadquartersInnerFrame, Controls.CorpHeadquartersFrame );
			UpdateButtonFrame( 1, Controls.CorpOfficeInnerFrame, Controls.CorpOfficeFrame );
			UpdateButtonFrame( 1, Controls.CorpFranchiseInnerFrame, Controls.CorpFranchiseFrame );
		end
	end	

	ResizeEtc();
end

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------

function SortFunction( a, b )

    local aVal = otherSortedList[ tostring( a ) ];
    local bVal = otherSortedList[ tostring( b ) ];
    
    if (aVal == nil) or (bVal == nil) then 
		--print("nil : "..tostring( a ).." = "..tostring(aVal).." : "..tostring( b ).." = "..tostring(bVal))
		if aVal and (bVal == nil) then
			return false;
		elseif (aVal == nil) and bVal then
			return true;
		else
			return tostring(a) < tostring(b); -- gotta do something deterministic
        end;
    else
        return aVal < bVal;
    end
end

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------

CivilopediaCategory[CategoryHomePage].SelectHeading = function( selectedEraID, dummy )
	print("CivilopediaCategory[CategoryHomePage].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first era
	--local thisListInstance = g_ListItemManager:GetInstance();
	--if thisListInstance then
	--	sortOrder = sortOrder + 1;
	--	thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_HOME_PAGE_LABEL" ));
	--	thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
	--	thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryHomePage].buttonClicked );
	--	thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
	--	otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	--end
	
	Controls.ListOfArticles:CalculateSize();
	Controls.ScrollPanel:CalculateInternalSize();
end

CivilopediaCategory[CategoryGameConcepts].SelectHeading = function( selectedEraID, dummy )
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryGameConcepts][selectedEraID].headingOpen = not sortedList[CategoryGameConcepts][selectedEraID].headingOpen; -- ain't lua great
--start working here on making the display not f up when closes

	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first era
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_GAME_CONCEPT_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryGameConcepts].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	local numConcepts = #sortedList[CategoryGameConcepts];
	for section = 1, numConcepts, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryGameConcepts][section].headingOpen then
				local textString = "TXT_KEY_GAME_CONCEPT_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_GAME_CONCEPT_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryGameConcepts].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryGameConcepts][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryGameConcepts][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryGameConcepts].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end
	end
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
end

CivilopediaCategory[CategoryTech].SelectHeading = function( selectedEraID, dummy )
	print("CivilopediaCategory[CategoryTech].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryTech][selectedEraID].headingOpen = not sortedList[CategoryTech][selectedEraID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first era
	local thisTechInstance = g_ListItemManager:GetInstance();
	if thisTechInstance then
		sortOrder = sortOrder + 1;
		thisTechInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TECH_PAGE_LABEL" ));
		thisTechInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisTechInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTech].buttonClicked );
		thisTechInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisTechInstance.ListItemButton )] = sortOrder;
	end

	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Description FROM Eras ORDER BY ID") do
	
		local eraID = era.ID;
		-- add an era header
		local thisEraInstance = g_ListHeadingManager:GetInstance();
		if thisEraInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryTech][eraID].headingOpen then
				local textString = era.Description;
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = era.Description;
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisEraInstance.ListHeadingButton:SetVoids( eraID, 0 );
			thisEraInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTech].SelectHeading );
			otherSortedList[tostring( thisEraInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryTech][eraID].headingOpen then
			for i, v in ipairs(sortedList[CategoryTech][eraID]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTech].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryUnits].SelectHeading = function( selectedEraID, dummy )
	print("CivilopediaCategory[CategoryUnits].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryUnits][selectedEraID].headingOpen = not sortedList[CategoryUnits][selectedEraID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first era
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_UNITS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryUnits].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	function PopulateHeaderAndItems(categoryID, headingName)
		local thisEraInstance = g_ListHeadingManager:GetInstance();
		if thisEraInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryUnits][categoryID].headingOpen then
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( headingName );
				thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( headingName );
				thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisEraInstance.ListHeadingButton:SetVoids( categoryID, 0 );
			thisEraInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryUnits].SelectHeading );
			otherSortedList[tostring( thisEraInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryUnits][categoryID].headingOpen then
			for i, v in ipairs(sortedList[CategoryUnits][categoryID]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryUnits].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end
	end
	
	
	local sectionID = 0;
	if(Game == nil or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		PopulateHeaderAndItems(sectionID, "TXT_KEY_PEDIA_RELIGIOUS");
		sectionID = sectionID + 1;
	end

	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Description FROM Eras ORDER BY ID") do
		local eraID = era.ID;
		local headingName = era.Description;
		PopulateHeaderAndItems(sectionID, headingName);
		sectionID = sectionID + 1;
	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();	
end

CivilopediaCategory[CategoryPromotions].SelectHeading = function( selectedSection, dummy )
	print("CivilopediaCategory[CategoryPromotions].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryPromotions][selectedSection].headingOpen = not sortedList[CategoryPromotions][selectedSection].headingOpen; -- ain't lua great

	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first era
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PROMOTIONS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPromotions].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	-- Infixo more categories
	--for section = 1, 8, 1 do	
	for section,sPediaType in ipairs(UnitPromotionsPediaTypes) do
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			--[[ Infixo more categories
			if sortedList[CategoryPromotions][section].headingOpen then
				local textString = "TXT_KEY_PROMOTIONS_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_PROMOTIONS_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			--]]
			local localizedLabel = Locale.ConvertTextKey("TXT_KEY_PROMOTIONS_"..sPediaType);
			if sortedList[CategoryPromotions][section].headingOpen then
				localizedLabel = "[ICON_MINUS] "..localizedLabel;
			else
				localizedLabel = "[ICON_PLUS] "..localizedLabel;
			end
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			-- Infixo end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPromotions].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryPromotions][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryPromotions][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPromotions].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end
	end
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();

end

CivilopediaCategory[CategoryBuildings].SelectHeading = function( selectedEraID, dummy )
	print("CivilopediaCategory[CategoryBuildings].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryBuildings][selectedEraID].headingOpen = not sortedList[CategoryBuildings][selectedEraID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first era
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BUILDINGS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	function PopulateAndAdd(categoryID, headingName)
	
		-- add an era header
		local thisEraInstance = g_ListHeadingManager:GetInstance();
		if thisEraInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryBuildings][categoryID].headingOpen then
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey(headingName);
				thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey(headingName);
				thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisEraInstance.ListHeadingButton:SetVoids( categoryID, 0 );
			thisEraInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectHeading );
			otherSortedList[tostring( thisEraInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryBuildings][categoryID].headingOpen then
			for i, v in ipairs(sortedList[CategoryBuildings][categoryID]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end
	end

	local sectionID = 0;
	if(Game == nil or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		PopulateAndAdd(sectionID, "TXT_KEY_PEDIA_RELIGIOUS");
		sectionID = sectionID + 1;
	end
	
	PopulateAndAdd(sectionID, "TXT_KEY_TOPIC_CORPORATION");
	sectionID = sectionID + 1;

	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Description FROM Eras ORDER BY ID") do
		local eraID = era.ID;
		local headingName = era.Description;
		PopulateAndAdd(sectionID, headingName);
		sectionID = sectionID + 1;
	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();	
end


CivilopediaCategory[CategoryWonders].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryWonders].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryWonders][selectedSectionID].headingOpen = not sortedList[CategoryWonders][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WONDERS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 4, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryWonders][section].headingOpen then
				local textString = "TXT_KEY_WONDER_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_WONDER_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryWonders][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryWonders][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end


CivilopediaCategory[CategoryPolicies].SelectHeading = function( selectedBranchID, dummy )
	print("CivilopediaCategory[CategoryPolicies].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryPolicies][selectedBranchID].headingOpen = not sortedList[CategoryPolicies][selectedBranchID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first branch
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_POLICIES_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPolicies].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for branch in GameInfo.PolicyBranchTypes() do
	
		local branchID = branch.ID;
		-- add a branch header
		local thisHeadingInstance = g_ListHeadingManager:GetInstance();
		if thisHeadingInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryPolicies][branchID].headingOpen then
				local textString = branch.Description;
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeadingInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = branch.Description;
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeadingInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeadingInstance.ListHeadingButton:SetVoids( branchID, 0 );
			thisHeadingInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPolicies].SelectHeading );
			otherSortedList[tostring( thisHeadingInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryPolicies][branchID].headingOpen then
			for i, v in ipairs(sortedList[CategoryPolicies][branchID]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPolicies].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryPeople].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryPeople].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryPeople][selectedSectionID].headingOpen = not sortedList[CategoryPeople][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PEOPLE_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPeople].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryPeople][section].headingOpen then
				local textString = "TXT_KEY_PEOPLE_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_PEOPLE_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPeople].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryPeople][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryPeople][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPeople].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end


CivilopediaCategory[CategoryCivilizations].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryCivilizations].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryCivilizations][selectedSectionID].headingOpen = not sortedList[CategoryCivilizations][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CIVILIZATIONS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCivilizations].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryCivilizations][section].headingOpen then
				local textString = "TXT_KEY_CIVILIZATION_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_CIVILIZATION_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCivilizations].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryCivilizations][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryCivilizations][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCivilizations].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end


CivilopediaCategory[CategoryCityStates].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryCityStates].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryCityStates][selectedSectionID].headingOpen = not sortedList[CategoryCityStates][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CITY_STATES_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCityStates].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	-- for each trait
	for trait in GameInfo.MinorCivTraits() do
	
		local traitID = trait.ID;
	
		local thisHeadingInstance = g_ListHeadingManager:GetInstance();
		if thisHeadingInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryCityStates][traitID].headingOpen then
				local textString = trait.Description;
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeadingInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = trait.Description;
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeadingInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeadingInstance.ListHeadingButton:SetVoids( traitID, 0 );
			thisHeadingInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCityStates].SelectHeading );
			otherSortedList[tostring( thisHeadingInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryCityStates][traitID].headingOpen then
			for i, v in ipairs(sortedList[CategoryCityStates][traitID]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCityStates].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryTerrain].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryTerrain].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryTerrain][selectedSectionID].headingOpen = not sortedList[CategoryTerrain][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTerrain].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryTerrain][section].headingOpen then
				local textString = "TXT_KEY_TERRAIN_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_TERRAIN_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTerrain].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryTerrain][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryTerrain][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTerrain].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryResources].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryResources].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryResources][selectedSectionID].headingOpen = not sortedList[CategoryResources][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCES_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryResources].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 0, 2, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryResources][section].headingOpen then
				local textString = "TXT_KEY_RESOURCES_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_RESOURCES_SECTION_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryResources].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryResources][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryResources][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryResources].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryImprovements].SelectHeading = function( selectedSection, dummy )
	print("CivilopediaCategory[CategoryImprovements].SelectHeading");
	-- todo: implement if there are ever sections in the Improvements page
	print("I should never get here");		
	ResizeEtc();
end

CivilopediaCategory[CategoryBeliefs].SelectHeading = function( selectedSectionID, dummy )
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryBeliefs][selectedSectionID].headingOpen = not sortedList[CategoryBeliefs][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BELIEFS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBeliefs].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 6, 1 do	

		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryBeliefs][section].headingOpen then
				local textString = "TXT_KEY_PEDIA_BELIEFS_CATEGORY_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_PEDIA_BELIEFS_CATEGORY_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBeliefs].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryBeliefs][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryBeliefs][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids(0, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, function(dummy, shouldAddToList) CivilopediaCategory[CategoryBeliefs].SelectArticle(v.entryID, shouldAddToList); end);
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryWorldCongress].SelectHeading = function( selectedSectionID, dummy )
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryWorldCongress][selectedSectionID].headingOpen = not sortedList[CategoryWorldCongress][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WORLD_CONGRESS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWorldCongress].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	for section = 1, 2, 1 do

		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryWorldCongress][section].headingOpen then
				local textString = "TXT_KEY_PEDIA_WORLD_CONGRESS_CATEGORY_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_PEDIA_WORLD_CONGRESS_CATEGORY_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWorldCongress].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
			
		-- for each element of the sorted list		
		if sortedList[CategoryWorldCongress][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryWorldCongress][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids(0, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, function(dummy, shouldAddToList) CivilopediaCategory[CategoryWorldCongress].SelectArticle(v.entryID, shouldAddToList); end);
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end
	end
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryCorporations].SelectHeading = function( selectedSectionID, dummy )
	print("CivilopediaCategory[CategoryCorporations].SelectHeading");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	sortedList[CategoryCorporations][selectedSectionID].headingOpen = not sortedList[CategoryCorporations][selectedSectionID].headingOpen; -- ain't lua great
	
	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCorporations].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 0, 2, 1 do	
		-- add a section header
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortOrder = sortOrder + 1;
			if sortedList[CategoryCorporations][section].headingOpen then
				local textString = "TXT_KEY_PEDIA_CORPORATIONS_CATEGORY_"..tostring( section );
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local textString = "TXT_KEY_PEDIA_CORPORATIONS_CATEGORY_"..tostring( section );
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCorporations].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		if sortedList[CategoryCorporations][section].headingOpen then
			for i, v in ipairs(sortedList[CategoryCorporations][section]) do
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCorporations].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
end


----------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------

CivilopediaCategory[CategoryHomePage].DisplayList = function( selectedSection, dummy )
	print("CivilopediaCategory[CategoryHomePage].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the rest of the stuff
	--local thisListInstance = g_ListItemManager:GetInstance();
	--if thisListInstance then
	--	sortOrder = sortOrder + 1;
	--	thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_HOME_PAGE_LABEL" ));
	--	thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
	--	thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryHomePage].buttonClicked );
	--	thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
	--	otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	--end
		-- for each element of the sorted list		
	for i, v in ipairs(sortedList[CategoryHomePage][1]) do
		-- add an entry
		local thisListInstance = g_ListItemManager:GetInstance();
		if thisListInstance then
			sortOrder = sortOrder + 1;
			thisListInstance.ListItemLabel:SetText( v.entryName );
			thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
			thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryHomePage].SelectArticle );
			thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
			otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
		end
	end
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
end

CivilopediaCategory[CategoryGameConcepts].DisplayList = function( selectedSection, dummy )
	print("CivilopediaCategory[CategoryGameConcepts].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};

	-- put in a home page before the rest of the stuff
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_GAME_CONCEPT_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryGameConcepts].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	-- for each element of the sorted list
	local numSections = #sortedList[CategoryGameConcepts];
	
	local GameConceptsList = sortedList[CategoryGameConcepts];
	for section = 1,numSections,1 do
		
		local headingOpen = GameConceptsList[section].headingOpen;
		if(headingOpen == nil) then
			headingOpen = true;
			GameConceptsList[section].headingOpen = true;
		end
	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
		
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_GAME_CONCEPT_SECTION_"..tostring( section );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryGameConcepts].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
			
			if(headingOpen == true) then
				local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			else
				local localizedLabel = "[ICON_PLUS] "..Locale.ConvertTextKey( textString );
				thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			end			
		end	
		
		if(headingOpen == true) then
			for i, v in ipairs(sortedList[CategoryGameConcepts][section]) do
				-- add an entry
				local thisListInstance = g_ListItemManager:GetInstance();
				if thisListInstance then
					sortOrder = sortOrder + 1;
					thisListInstance.ListItemLabel:SetText( v.entryName );
					thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
					thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryGameConcepts].SelectArticle );
					thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
					otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
				end
			end
		end
	end
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
	Controls.ScrollPanel:CalculateInternalSize();
end

CivilopediaCategory[CategoryTech].DisplayList = function()
	print("CivilopediaCategory[CategoryTech].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();
	
	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first era
	local thisTechInstance = g_ListItemManager:GetInstance();
	if thisTechInstance then
		sortOrder = sortOrder + 1;
		thisTechInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TECH_PAGE_LABEL" ));
		thisTechInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisTechInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTech].buttonClicked );
		thisTechInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisTechInstance.ListItemButton )] = sortOrder;
	end

	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Description FROM Eras ORDER BY ID") do
	
		local eraID = era.ID;
		-- add an era header
		local thisEraInstance = g_ListHeadingManager:GetInstance();
		if thisEraInstance then
			sortedList[CategoryTech][eraID].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = era.Description;
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			thisEraInstance.ListHeadingButton:SetVoids( eraID, 0 );
			thisEraInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTech].SelectHeading );
			otherSortedList[tostring( thisEraInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryTech][eraID]) do
			-- add a tech entry
			local thisTechInstance = g_ListItemManager:GetInstance();
			if thisTechInstance then
				sortOrder = sortOrder + 1;
				thisTechInstance.ListItemLabel:SetText( v.entryName );
				thisTechInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisTechInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTech].SelectArticle );
				thisTechInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisTechInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
end

CivilopediaCategory[CategoryUnits].DisplayList = function()
	print("CivilopediaCategory[CategoryUnits].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first era
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_UNITS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryUnits].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	function PopulateAndAdd(categoryID, headingName)
		
		-- add an era header
		local thisEraInstance = g_ListHeadingManager:GetInstance();
		if thisEraInstance then
			sortedList[CategoryUnits][categoryID].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;

			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey(headingName);
			thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			thisEraInstance.ListHeadingButton:SetVoids( categoryID, 0 );
			thisEraInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryUnits].SelectHeading );
			otherSortedList[tostring( thisEraInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryUnits][categoryID]) do
			-- add a unit entry
			local thisUnitInstance = g_ListItemManager:GetInstance();
			if thisUnitInstance then
				sortOrder = sortOrder + 1;
				thisUnitInstance.ListItemLabel:SetText( v.entryName );
				thisUnitInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisUnitInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryUnits].SelectArticle );
				thisUnitInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisUnitInstance.ListItemButton )] = sortOrder;
			end
		end
	end
	
	local sectionID = 0;
	
	if(Game == nil or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		PopulateAndAdd(0, "TXT_KEY_PEDIA_RELIGIOUS");
		sectionID = sectionID + 1;
	end

	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Description FROM Eras ORDER BY ID") do
		local eraID = era.ID;
		local headingName = era.Description;
		
		PopulateAndAdd(eraID + sectionID, headingName);
	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryPromotions].DisplayList = function()
	print("start CivilopediaCategory[CategoryPromotions].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();
	
	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the rest of the stuff
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PROMOTIONS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPromotions].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	-- for each element of the sorted list		

	-- Infixo more categories
	--for section = 1,8,1 do
	for section,sPediaType in ipairs(UnitPromotionsPediaTypes) do
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryPromotions][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			-- Infixo more categories
			--local textString = "TXT_KEY_PROMOTIONS_SECTION_"..tostring( section );
			local textString = "TXT_KEY_PROMOTIONS_"..sPediaType;
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPromotions].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
			
		for i, v in ipairs(sortedList[CategoryPromotions][section]) do
			-- add an entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPromotions].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end
	end
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
	Controls.ScrollPanel:CalculateInternalSize();
end



CivilopediaCategory[CategoryBuildings].DisplayList = function()
	print("CivilopediaCategory[CategoryBuildings].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first era
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BUILDINGS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	function PopulateAndAdd(categoryID, headingName)
		local thisEraInstance = g_ListHeadingManager:GetInstance();
		if thisEraInstance then
			sortedList[CategoryBuildings][categoryID].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey(headingName);
			thisEraInstance.ListHeadingLabel:SetText( localizedLabel );
			thisEraInstance.ListHeadingButton:SetVoids( categoryID, 0 );
			thisEraInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectHeading );
			otherSortedList[tostring( thisEraInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryBuildings][categoryID]) do
			-- add an entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBuildings].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end
	end
	
	local sectionID = 0;
	
	if(Game == nil or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		PopulateAndAdd(0, "TXT_KEY_PEDIA_RELIGIOUS");
		sectionID = sectionID + 1;
	end
	
	PopulateAndAdd(sectionID, "TXT_KEY_TOPIC_CORPORATION");
	sectionID = sectionID + 1;

	-- Infixo GameInfo.Eras is not updated after mods are loaded
	--for era in GameInfo.Eras() do
	for era in DB.Query("SELECT ID, Description FROM Eras ORDER BY ID") do
		local eraID = era.ID;
		local headingName = era.Description;
		PopulateAndAdd(eraID + sectionID, headingName);
	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
end

CivilopediaCategory[CategoryWonders].DisplayList = function()
	print("CivilopediaCategory[CategoryWonders].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WONDERS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 4, 1 do	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryWonders][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_WONDER_SECTION_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryWonders][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWonders].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
	Controls.ScrollPanel:CalculateInternalSize();
		
end

CivilopediaCategory[CategoryPolicies].DisplayList = function()
	print("CivilopediaCategory[CategoryPolicies].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first branch
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_POLICIES_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPolicies].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for branch in GameInfo.PolicyBranchTypes() do
	
		local branchID = branch.ID;
		-- add a branch header
		local thisHeadingInstance = g_ListHeadingManager:GetInstance();
		if thisHeadingInstance then
			sortedList[CategoryPolicies][branchID].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = branch.Description;
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeadingInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeadingInstance.ListHeadingButton:SetVoids( branchID, 0 );
			thisHeadingInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPolicies].SelectHeading );
			otherSortedList[tostring( thisHeadingInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryPolicies][branchID]) do
			-- add an entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPolicies].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryPeople].DisplayList = function()
	print("CivilopediaCategory[CategoryPeople].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_PEOPLE_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPeople].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryPeople][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_PEOPLE_SECTION_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPeople].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryPeople][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryPeople].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryCivilizations].DisplayList = function()
	print("CivilopediaCategory[CategoryCivilizations].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CIVILIZATIONS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCivilizations].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryCivilizations][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_CIVILIZATIONS_SECTION_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCivilizations].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryCivilizations][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCivilizations].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryCityStates].DisplayList = function()
	print("CivilopediaCategory[CategoryCityStates].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first trait
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CITY_STATES_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCityStates].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for trait in GameInfo.MinorCivTraits() do
	
		local traitID = trait.ID;
		-- add a header
		local thisHeadingInstance = g_ListHeadingManager:GetInstance();
		if thisHeadingInstance then
			sortedList[CategoryCityStates][traitID].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = trait.Description;
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeadingInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeadingInstance.ListHeadingButton:SetVoids( traitID, 0 );
			thisHeadingInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCityStates].SelectHeading );
			otherSortedList[tostring( thisHeadingInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryCityStates][traitID]) do
			-- add an entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCityStates].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryTerrain].DisplayList = function()
	print("CivilopediaCategory[CategoryTerrain].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_TERRAIN_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTerrain].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryTerrain][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_TERRAIN_SECTION_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTerrain].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryTerrain][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryTerrain].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryResources].DisplayList = function()
	print("CivilopediaCategory[CategoryResources].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_RESOURCES_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryResources].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 0, 2, 1 do	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryResources][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_RESOURCES_SECTION_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryResources].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryResources][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryResources].SelectArticle );
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
		
end

CivilopediaCategory[CategoryImprovements].DisplayList = function()
	print("start CivilopediaCategory[CategoryImprovements].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();
	
	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the rest of the stuff
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_IMPROVEMENTS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryImprovements].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	-- for each element of the sorted list		
	for i, v in ipairs(sortedList[CategoryImprovements][1]) do
		-- add an entry
		local thisListInstance = g_ListItemManager:GetInstance();
		if thisListInstance then
			sortOrder = sortOrder + 1;
			thisListInstance.ListItemLabel:SetText( v.entryName );
			thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
			thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryImprovements].SelectArticle );
			thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
			otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
		end
	end

	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();

end

CivilopediaCategory[CategoryBeliefs].DisplayList = function()
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_BELIEFS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBeliefs].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 6, 1 do	
		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryBeliefs][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_PEDIA_BELIEFS_CATEGORY_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryBeliefs].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryBeliefs][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID[2], addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, function(dummy, shouldAddToList) CivilopediaCategory[CategoryBeliefs].SelectArticle(v.entryID, shouldAddToList); end);
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end

	end	
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();

end

CivilopediaCategory[CategoryWorldCongress].DisplayList = function()
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_WORLD_CONGRESS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWorldCongress].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end

	for section = 1, 2, 1 do

		local thisHeaderInstance = g_ListHeadingManager:GetInstance();
		if thisHeaderInstance then
			sortedList[CategoryWorldCongress][section].headingOpen = true; -- ain't lua great
			sortOrder = sortOrder + 1;
			local textString = "TXT_KEY_PEDIA_WORLD_CONGRESS_CATEGORY_"..tostring( section );
			local localizedLabel = "[ICON_MINUS] "..Locale.ConvertTextKey( textString );
			thisHeaderInstance.ListHeadingLabel:SetText( localizedLabel );
			thisHeaderInstance.ListHeadingButton:SetVoids( section, 0 );
			thisHeaderInstance.ListHeadingButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryWorldCongress].SelectHeading );
			otherSortedList[tostring( thisHeaderInstance.ListHeadingButton )] = sortOrder;
		end	
		
		-- for each element of the sorted list		
		for i, v in ipairs(sortedList[CategoryWorldCongress][section]) do
			-- add a unit entry
			local thisListInstance = g_ListItemManager:GetInstance();
			if thisListInstance then
				sortOrder = sortOrder + 1;
				thisListInstance.ListItemLabel:SetText( v.entryName );
				thisListInstance.ListItemButton:SetVoids( v.entryID[2], addToList );
				thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, function(dummy, shouldAddToList) CivilopediaCategory[CategoryWorldCongress].SelectArticle(v.entryID, shouldAddToList); end);
				thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
				otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
			end
		end
	end
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();

end

CivilopediaCategory[CategoryCorporations].DisplayList = function()
	print("CivilopediaCategory[CategoryCorporations].DisplayList");
	g_ListHeadingManager:ResetInstances();
	g_ListItemManager:ResetInstances();

	local sortOrder = 0;
	otherSortedList = {};
	
	-- put in a home page before the first section
	local thisListInstance = g_ListItemManager:GetInstance();
	if thisListInstance then
		sortOrder = sortOrder + 1;
		thisListInstance.ListItemLabel:SetText( Locale.ConvertTextKey( "TXT_KEY_PEDIA_CORPORATIONS_PAGE_LABEL" ));
		thisListInstance.ListItemButton:SetVoids( homePageOfCategoryID, addToList );
		thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCorporations].buttonClicked );
		thisListInstance.ListItemButton:SetToolTipCallback( TipHandler );
		otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
	end
	
	-- for each element of the sorted list		
	for i, v in ipairs(sortedList[CategoryCorporations][1]) do
		-- add a unit entry
		local thisListInstance = g_ListItemManager:GetInstance();
		if thisListInstance then
			sortOrder = sortOrder + 1;
			thisListInstance.ListItemLabel:SetText( v.entryName );
			thisListInstance.ListItemButton:SetVoids( v.entryID, addToList );
			thisListInstance.ListItemButton:RegisterCallback( Mouse.eLClick, CivilopediaCategory[CategoryCorporations].SelectArticle );
			thisListInstance.ListItemButton:SetToolTipCallback( TipHandler )
			otherSortedList[tostring( thisListInstance.ListItemButton )] = sortOrder;
		end
	end
	
	Controls.ListOfArticles:SortChildren( SortFunction );
	ResizeEtc();
	
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function ClearArticle()
	Controls.ScrollPanel:SetScrollValue( 0 );
	Controls.PortraitFrame:SetHide( true );
	Controls.CostFrame:SetHide( true );
	Controls.MaintenanceFrame:SetHide( true );
	Controls.HappinessFrame:SetHide( true );
	Controls.UnmoddedHappinessFrame:SetHide( true );
	Controls.CultureFrame:SetHide( true );
	Controls.FaithFrame:SetHide( true );
	Controls.DefenseFrame:SetHide( true );
	Controls.FoodFrame:SetHide( true );
	Controls.GoldChangeFrame:SetHide( true );
	Controls.GoldFrame:SetHide( true );
	Controls.ScienceFrame:SetHide( true );
	Controls.ProductionFrame:SetHide( true );
	Controls.GreatPeopleFrame:SetHide( true );
	Controls.CombatFrame:SetHide( true );
	Controls.RangedCombatFrame:SetHide( true );
	Controls.RangedCombatRangeFrame:SetHide( true );
	Controls.MovementFrame:SetHide( true );
	Controls.AirDefenseFrame:SetHide( true );	
	Controls.FreePromotionsFrame:SetHide( true );
	Controls.PrereqTechFrame:SetHide( true );
	Controls.GreatWorksFrame:SetHide( true );
	Controls.LeadsToTechFrame:SetHide( true );
	Controls.ObsoleteTechFrame:SetHide( true );
	Controls.UpgradeFrame:SetHide( true );
	Controls.UnlockedUnitsFrame:SetHide( true );
	Controls.UnlockedBuildingsFrame:SetHide( true );
	Controls.RequiredBuildingsFrame:SetHide( true );
	-- CBP
	Controls.LeadsToBuildingsFrame:SetHide( true );	
	Controls.MonopolyResourcesFrame:SetHide( true );
	Controls.CorporationFrame:SetHide( true );
	Controls.CorpHeadquartersFrame:SetHide( true );
	Controls.CorpOfficeFrame:SetHide( true );
	Controls.CorpFranchiseFrame:SetHide( true );
	Controls.FreeTRFrame:SetHide( true );
	-- End
	Controls.RevealedResourcesFrame:SetHide( true );
	Controls.RequiredResourcesFrame:SetHide( true );
	Controls.RequiredPromotionsFrame:SetHide( true );
	Controls.LocalResourcesFrame:SetHide( true );
	Controls.WorkerActionsFrame:SetHide( true );
	Controls.UnlockedProjectsFrame:SetHide( true );
	Controls.SpecialistsFrame:SetHide( true );
	Controls.RelatedArticlesFrame:SetHide( true );
	Controls.GameInfoFrame:SetHide( true );
	Controls.QuoteFrame:SetHide( true );
	Controls.SilentQuoteFrame:SetHide( true );
	Controls.AbilitiesFrame:SetHide( true );			
	Controls.HistoryFrame:SetHide( true );
	Controls.StrategyFrame:SetHide( true );
	Controls.RelatedImagesFrame:SetHide( true );		
	Controls.SummaryFrame:SetHide( true );		
	Controls.ExtendedFrame:SetHide( true );		
	Controls.DNotesFrame:SetHide( true );		
	Controls.RequiredPoliciesFrame:SetHide( true );		
	Controls.PrereqEraFrame:SetHide( true );		
	Controls.PolicyBranchFrame:SetHide( true );	
	Controls.TenetLevelFrame:SetHide(true);	
	Controls.LeadersFrame:SetHide( true );
	Controls.UniqueUnitsFrame:SetHide( true );
	Controls.UniqueBuildingsFrame:SetHide( true );
	Controls.UniqueImprovementsFrame:SetHide( true );
	Controls.CivilizationsFrame:SetHide ( true );
	Controls.TraitsFrame:SetHide( true );
	Controls.LivedFrame:SetHide( true );
	Controls.TitlesFrame:SetHide( true );
	Controls.SubtitleID:SetHide( true );
	Controls.YieldFrame:SetHide( true );
	Controls.MountainYieldFrame:SetHide( true );
	--CBP
	Controls.CorporationResourceBonusFrame:SetHide( true );
	Controls.CorporationOfficeBonusFrame:SetHide( true );
	Controls.CorporationTRBonusFrame:SetHide( true );
	Controls.TradeRouteYieldFrame:SetHide( true );
	Controls.AdjacentYieldFrame:SetHide( true );
	Controls.AdjacentTerrainYieldFrame:SetHide( true );	
	Controls.AdjacentImprovYieldFrame:SetHide( true );
	Controls.TwoAdjacentImprovYieldFrame:SetHide( true );	
	Controls.ImprovementYieldFrame:SetHide( true );
	--END
	Controls.MovementCostFrame:SetHide( true );
	Controls.CombatModFrame:SetHide( true );
	Controls.FeaturesFrame:SetHide( true );
	Controls.ResourcesFoundFrame:SetHide( true );
	Controls.TerrainsFrame:SetHide( true );
	Controls.CombatTypeFrame:SetHide( true );
	Controls.ReplacesFrame:SetHide( true );
	Controls.RevealTechsFrame:SetHide( true );
	Controls.ImprovementsFrame:SetHide( true );
	Controls.HomePageBlurbFrame:SetHide( true );
	Controls.WideTextBlockFrame:SetHide( true );
	Controls.FFTextStack:SetHide( true );
	Controls.BBTextStack:SetHide ( true );
	
	Controls.PartialMatchPullDown:SetHide( true );
	Controls.SearchButton:SetHide( false );	
	
	Controls.Portrait:UnloadTexture();
	Controls.Portrait:SetTexture("256x256Frame.dds");
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function OnClose()
	Controls.Portrait:UnloadTexture();
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function OnBackButtonClicked()
	if currentTopic > 1 then
		currentTopic = currentTopic - 1;
		local article = listOfTopicsViewed[currentTopic];
		if article then
			SetSelectedCategory( article.entryCategory );
			CivilopediaCategory[article.entryCategory].SelectArticle( article.entryID, dontAddToList );
		end
	end
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBackButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function OnForwardButtonClicked()
	if currentTopic < endTopic then
		currentTopic = currentTopic + 1;
		local article = listOfTopicsViewed[currentTopic];
		if article then
			SetSelectedCategory( article.entryCategory );
			CivilopediaCategory[article.entryCategory].SelectArticle( article.entryID, dontAddToList );
		end
	end
end
Controls.ForwardButton:RegisterCallback( Mouse.eLClick, OnForwardButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function SearchForPediaEntry( searchString )

	UIManager:SetUICursor( 1 );
	
    if( searchString ~= nil and searchString ~= "" ) then
    	local article = searchableTextKeyList[searchString];
    	if article == nil then
    		article = searchableList[Locale.ToLower(searchString)];
    	end
    	
    	if article then
    		SetSelectedCategory( article.entryCategory );
    		CivilopediaCategory[article.entryCategory].SelectArticle( article.entryID, addToList );
    	else
    		SetSelectedCategory( CategoryGameConcepts );
    	end
    end
	
	if( searchString == "OPEN_VIA_HOTKEY" ) then
    	if( ContextPtr:IsHidden() == false ) then
    	    OnClose();
	    else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.eUtmost );
    	end
	else
    	UIManager:QueuePopup( ContextPtr, PopupPriority.eUtmost );
	end

	UIManager:SetUICursor( 0 );

end
Events.SearchForPediaEntry.Add( SearchForPediaEntry );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function GoToPediaHomePage ( iHomePage )
	UIManager:SetUICursor( 1 );
	UIManager:QueuePopup( ContextPtr, PopupPriority.Civilopedia );
	SetSelectedCategory(iHomePage);
	UIManager:SetUICursor( 0 );
end
Events.GoToPediaHomePage.Add( GoToPediaHomePage );



----------------------------------------------------------------
----------------------------------------------------------------
function ValidateText(text)

	if #text < 3 then
		return false;
	end

	local isAllWhiteSpace = true;
	local numNonWhiteSpace = 0;
	for i = 1, #text, 1 do
		if string.byte(text, i) ~= 32 then
			isAllWhiteSpace = false;
			numNonWhiteSpace = numNonWhiteSpace + 1;
		end
	end
	
	if isAllWhiteSpace then
		return false;
	end
	
	if numNonWhiteSpace < 3 then
		return false;
	end
	
	-- don't allow % character
	for i = 1, #text, 1 do
		if string.byte(text, i) == 37 then
			return false;
		end
	end
	
	local invalidCharArray = { '\"', '<', '>', '|', '\b', '\0', '\t', '\n', '/', '\\', '*', '?', '%[', ']' };

	for i, ch in ipairs(invalidCharArray) do
		if string.find(text, ch) ~= nil then
			return false;
		end
	end
	
	-- don't allow control characters
	for i = 1, #text, 1 do
		if string.byte(text, i) < 32 then
			return false;
		end
	end
	
	return true;
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function OnSearchButtonClicked()
	UIManager:SetUICursor( 1 );
	local searchString = Controls.SearchEditBox:GetText();
	local lowerCaseSearchString = nil;
	if searchString ~= nil and searchString ~= "" and ValidateText(searchString) then

		local article = searchableTextKeyList[searchString];
		if article == nil then
			lowerCaseSearchString = Locale.ToLower(searchString);
			article = searchableList[lowerCaseSearchString];
		end
	    
		if article then
			SetSelectedCategory( article.entryCategory );
			CivilopediaCategory[article.entryCategory].SelectArticle( article.entryID, addToList );
		else
		
			-- try to see if there is a partial match
			local partialMatch = {};
			local numberMatches = 0;
			for i, v in pairs(searchableList) do
				if string.find(Locale.ToLower(v.entryName), lowerCaseSearchString) ~= nil then
					numberMatches = numberMatches + 1;
					partialMatch[numberMatches] = v.entryName;
				end
			end
			if numberMatches == 1 then
				article = searchableList[Locale.ToLower(partialMatch[1])];
				if article then
					SetSelectedCategory( article.entryCategory );
					CivilopediaCategory[article.entryCategory].SelectArticle( article.entryID, addToList );
				end
			elseif numberMatches > 1 then -- populate a dropdown with the matches
				Controls.PartialMatchPullDown:ClearEntries();
				--print "---------------------------------"
				for i, v in pairs( partialMatch ) do
					local controlTable = {};
					Controls.PartialMatchPullDown:BuildEntry( "InstanceOne", controlTable );
					controlTable.Button:SetText( v );
					
					controlTable.Button:RegisterCallback(Mouse.eLClick, 
					function()
						SearchForPediaEntry( v );
					end);

					controlTable.Button:SetVoid1( i );
					--print(v);
				end
				Controls.PartialMatchPullDown:CalculateInternals();
				--print "---------------------------------"
				Controls.SearchButton:SetHide( true );
				Controls.PartialMatchPullDown:SetHide( false );
				Controls.PartialMatchPullDown:GetButton():SetText( searchString );
			else
				Controls.SearchNotFoundText:LocalizeAndSetText("TXT_KEY_SEARCH_NOT_FOUND", searchString);
				Controls.SearchFoundNothing:SetHide(false);
			end
		end
	end
	UIManager:SetUICursor( 0 );
end
Controls.SearchButton:RegisterCallback( Mouse.eLClick, OnSearchButtonClicked );


function OnSearchTextEnter( dummyString, dummyControl )
	OnSearchButtonClicked();
end
Controls.SearchEditBox:RegisterCallback( OnSearchTextEnter );

function OnSearchNotFoundOK()
	Controls.SearchFoundNothing:SetHide(true);
end
Controls.OK:RegisterCallback(Mouse.eLClick, OnSearchNotFoundOK );
 
function InputHandler( uiMsg, wParam, lParam )
    if(uiMsg == KeyEvents.KeyDown) then
		if(not Controls.SearchFoundNothing:IsHidden()) then
			if(wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN) then
				Controls.SearchFoundNothing:SetHide(true);
				return true;
			end
		else
			if(wParam == Keys.VK_ESCAPE) then
				OnClose();
				return true;
			end
		end
    end
end
ContextPtr:SetInputHandler( InputHandler );


function ShowHideHandler( isHide )
    if( isHide ) then
		Controls.Portrait:UnloadTexture();
        Events.SystemUpdateUI.CallImmediate( SystemUpdateUIType.BulkShowUI );
	else
        Events.SystemUpdateUI.CallImmediate( SystemUpdateUIType.BulkHideUI );
        
		if currentTopic then
			local article = listOfTopicsViewed[currentTopic];
			if article then
				SetSelectedCategory( article.entryCategory );
				CivilopediaCategory[article.entryCategory].SelectArticle( article.entryID, dontAddToList );
			else
				SetSelectedCategory(CategoryTerrain); -- this is a dummy so that the trigger for the next one fires
				SetSelectedCategory(CategoryHomePage);
				CivilopediaCategory[CategoryHomePage].DisplayHomePage();
			end
		end

	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);


----------------------------------------------------------------
-- If we hear a multiplayer game invite was sent, exit
-- so we don't interfere with the transition
----------------------------------------------------------------
function OnMultiplayerGameInvite()
   	if( ContextPtr:IsHidden() == false ) then
		OnClose();
	end
end
Events.MultiplayerGameLobbyInvite.Add( OnMultiplayerGameInvite );
Events.MultiplayerGameServerInvite.Add( OnMultiplayerGameInvite );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

local tipControlTable = {};
TTManager:GetTypeControlTable( "TypeRoundImage", tipControlTable );
function TipHandler( control )
	local id = control:GetVoid1();	
	local article = categorizedList[(selectedCategory * absurdlyLargeNumTopicsInCategory) + id];
	if article and article.tooltipTexture then
		tipControlTable.ToolTipImage:SetTexture( article.tooltipTexture );
		tipControlTable.ToolTipImage:SetTextureOffset( article.tooltipTextureOffset );
		tipControlTable.ToolTipFrame:SetHide( false );
	else
		tipControlTable.ToolTipFrame:SetHide( true );
	end		
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

for i = 1, numCategories, 1 do
	if CivilopediaCategory[i].PopulateList then
		CivilopediaCategory[i].PopulateList();
	end
end

SetSelectedCategory(CategoryTerrain); -- this is a dummy so that the trigger for the next one fires
SetSelectedCategory(CategoryHomePage);
CivilopediaCategory[CategoryHomePage].DisplayHomePage();


