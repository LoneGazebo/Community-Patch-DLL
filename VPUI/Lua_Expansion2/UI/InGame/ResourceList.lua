if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
--------------------------------------------------------------
--------------------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "SupportFunctions" );

local m_sortTable;

local eName      = 0;
local eTradeInfo = 1;

local m_SortMode = eName;
local m_bSortReverse = false;

local m_StrategicIM = InstanceManager:new( "ResourceInstance", "Root", Controls.StrategicStack );
local m_LuxuryIM = InstanceManager:new( "ResourceInstance", "Root", Controls.LuxuryStack );
local m_BonusIM = InstanceManager:new( "ResourceInstance", "Root", Controls.BonusStack );

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
	if( not bIsHide ) then
		UpdateDisplay();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-------------------------------------------------
function OnClose( )
	ContextPtr:SetHide( true );
	Events.OpenInfoCorner( InfoCornerID.None );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			OnClose();
			return true;
		end
	end
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------
-------------------------------------------------
function OnChangeEvent()
	if( ContextPtr:IsHidden() == false ) then
		UpdateDisplay();
	end
end
Events.SerialEventGameDataDirty.Add( OnChangeEvent );
Events.SerialEventCityInfoDirty.Add( OnChangeEvent );
Events.GameplaySetActivePlayer.Add( OnChangeEvent );

-------------------------------------------------
-------------------------------------------------
function UpdateDisplay()
	m_SortTable = {};

	local pPlayer = Players[Game:GetActivePlayer()];

	local bStrategicFound = false;
	local bLuxuryFound = false;
	local bBonusFound = false;

	local iTotal = 0;
	local iAvailable = 0;
	local iUsed = 0;
	local iImports = 0;
	local iExports = 0;

	m_StrategicIM:ResetInstances();
	m_LuxuryIM:ResetInstances();
	m_BonusIM:ResetInstances();

	for resource in GameInfo.Resources() do
		local instance;
		local iResource = resource.ID;
		local bIsStrategic = false;
		local strTradeInfo = "";

		iTotal = pPlayer:GetNumResourceTotal( iResource, true );
		iExports = pPlayer:GetResourceExport( iResource );

		if( iTotal > 0 or iExports > 0 ) then
			iAvailable = pPlayer:GetNumResourceAvailable( iResource, true );
			iUsed = pPlayer:GetNumResourceUsed( iResource );
			iImports = pPlayer:GetResourceImport( iResource );

			if( Game.GetResourceUsageType( iResource ) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC ) then
				instance = m_StrategicIM:GetInstance();
				bStrategicFound = true;
				bIsStrategic = true;
			end

			if( Game.GetResourceUsageType( iResource ) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY ) then
				instance = m_LuxuryIM:GetInstance();
				bLuxuryFound = true;
			end

			if( Game.GetResourceUsageType( iResource ) == ResourceUsageTypes.RESOURCEUSAGE_BONUS ) then
				instance = m_BonusIM:GetInstance();
				bBonusFound = true;
			end

			local sortEntry = {};
			m_SortTable[ tostring( instance.Root ) ] = sortEntry;
			sortEntry.name = Locale.ConvertTextKey( resource.Description );
			instance.ResourceQty:SetText( iAvailable );
			TruncateString( instance.ResourceName, 140, resource.IconString .. " " .. sortEntry.name );

			if( bIsStrategic and iUsed > 0 ) then
				strTradeInfo = Locale.ConvertTextKey( "TXT_KEY_RESOURCE_USED", iUsed );

				if( iImports ~= iExports ) then
					strTradeInfo = strTradeInfo .. ", ";
				end
			end

			if( iImports > iExports ) then
				strTradeInfo = strTradeInfo .. Locale.ConvertTextKey( "TXT_KEY_RESOURCE_IMPORTS", iImports );
			end
		
			if( iImports < iExports ) then
				strTradeInfo = strTradeInfo .. Locale.ConvertTextKey( "TXT_KEY_RESOURCE_EXPORTS", iExports );
			end

			sortEntry.tradeInfo = iTotal;

			instance.TradeInfo:SetText( strTradeInfo );

			instance.ResourceStack:CalculateSize();
			instance.ResourceStack:ReprocessAnchoring();
		
			sortEntry.resourceID = resource.ID;
		end
	end

	Controls.LuxuryStack:SortChildren( SortFunction );
	Controls.StrategicStack:SortChildren( SortFunction );
	Controls.BonusStack:SortChildren( SortFunction );

	Controls.LuxuryHeader:SetHide( not bLuxuryFound );
	Controls.StrategicHeader:SetHide( not bStrategicFound );
	Controls.BonusHeader:SetHide( not bBonusFound );

	Controls.LuxuryStack:CalculateSize();
	Controls.LuxuryStack:ReprocessAnchoring();
	Controls.StrategicStack:CalculateSize();
	Controls.StrategicStack:ReprocessAnchoring();
	Controls.BonusStack:CalculateSize();
	Controls.BonusStack:ReprocessAnchoring();

	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end

-------------------------------------------------
-------------------------------------------------
function OnLuxuryToggle()
	local bWasHidden = Controls.LuxuryStack:IsHidden();
	Controls.LuxuryStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.LuxuryToggle:LocalizeAndSetText("TXT_KEY_RESOURCE_LUXURY_DETAILS_COLLAPSE");
	else
		Controls.LuxuryToggle:LocalizeAndSetText("TXT_KEY_RESOURCE_LUXURY_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.LuxuryToggle:RegisterCallback( Mouse.eLClick, OnLuxuryToggle );

-------------------------------------------------
-------------------------------------------------
function OnStrategicToggle()
	local bWasHidden = Controls.StrategicStack:IsHidden();
	Controls.StrategicStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.StrategicToggle:LocalizeAndSetText("TXT_KEY_RESOURCE_STRATEGIC_DETAILS_COLLAPSE");
	else
		Controls.StrategicToggle:LocalizeAndSetText("TXT_KEY_RESOURCE_STRATEGIC_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
end
Controls.StrategicToggle:RegisterCallback( Mouse.eLClick, OnStrategicToggle );

-------------------------------------------------
-------------------------------------------------
function OnBonusToggle()
	local bWasHidden = Controls.BonusStack:IsHidden();
	Controls.BonusStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.BonusToggle:LocalizeAndSetText("TXT_KEY_RESOURCE_BONUS_DETAILS_COLLAPSE");
	else
		Controls.BonusToggle:LocalizeAndSetText("TXT_KEY_RESOURCE_BONUS_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
end
Controls.BonusToggle:RegisterCallback( Mouse.eLClick, OnBonusToggle );

-------------------------------------------------
-------------------------------------------------
function SortFunction( a, b )
	local valueA, valueB;
	local entryA = m_SortTable[ tostring( a ) ];
	local entryB = m_SortTable[ tostring( b ) ];
	
	if (entryA == nil) or (entryB == nil) then 
		if entryA and (entryB == nil) then
			return false;
		elseif (entryA == nil) and entryB then
			return true;
		else
			if( m_bSortReverse ) then
				return tostring(a) > tostring(b); -- gotta do something deterministic
			else
				return tostring(a) < tostring(b); -- gotta do something deterministic
			end
		end
	else
		if( m_SortMode == eName ) then
			valueA = entryA.name;
			valueB = entryB.name;
		elseif( m_SortMode == eTradeInfo ) then
			-- Sort from most to least
			valueA = entryB.tradeInfo;
			valueB = entryA.tradeInfo;
		end
		
		if( valueA == valueB ) then
			valueA = entryA.resourceID;
			valueB = entryB.resourceID;
		end
		
		if( m_bSortReverse ) then
			return valueA > valueB;
		else
			return valueA < valueB;
		end
	end
end

-------------------------------------------------
-------------------------------------------------
function OnSort( type )
	if( m_SortMode == type ) then
		m_bSortReverse = not m_bSortReverse;
	else
		m_bSortReverse = false;
	end

	m_SortMode = type;
	Controls.LuxuryStack:SortChildren( SortFunction );
	Controls.StrategicStack:SortChildren( SortFunction );
	Controls.BonusStack:SortChildren( SortFunction );
end
Controls.SortName:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortTradeInfo:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortName:SetVoid1( eName );
Controls.SortTradeInfo:SetVoid1( eTradeInfo );

-------------------------------------------------
-------------------------------------------------
function OnOpenInfoCorner( iInfoType )
	if( iInfoType == InfoCornerID.Resources ) then
		ContextPtr:SetHide( false );
	else
		ContextPtr:SetHide( true );
	end
end
Events.OpenInfoCorner.Add( OnOpenInfoCorner );
