if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

local maxIdx = 0;
if( InfoCornerID["Resources"] == nil ) then
    for index, value in pairs(InfoCornerID) do
        maxIdx = maxIdx + 1;
    end
    InfoCornerID["Resources"] = maxIdx;
	maxIdx = maxIdx + 1;
	InfoCornerID["GP"] = maxIdx;
else
    maxIdx = InfoCornerID.GP;
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
m_InfoData = {};
m_InfoData[ InfoCornerID.None ] = {
    ToolTip="TXT_KEY_CLOSE",
    Image="assets/UI/Art/Icons/MainOpen.dds",
    HL="assets/UI/Art/Icons/MainOpenHL.dds" };
    
m_InfoData[ InfoCornerID.Tech ] = {
    ToolTip="TXT_KEY_IC_RESEARCH",
    Image="assets/UI/Art/Icons/MainTechButton.dds",
    HL="assets/UI/Art/Icons/MainTechButtonHL.dds" };
    
m_InfoData[ InfoCornerID.Units ] = {
    ToolTip="TXT_KEY_IC_UNIT",
    Image="assets/UI/Art/Icons/MainUnitButton.dds",
    HL="assets/UI/Art/Icons/MainUnitButtonHL.dds" };
    
m_InfoData[ InfoCornerID.Cities ] = {
    ToolTip="TXT_KEY_IC_CITY",
    Image="assets/UI/Art/Icons/MainCityButton.dds",
    HL="assets/UI/Art/Icons/MainCityButtonHL.dds" };

m_InfoData[ InfoCornerID.Resources ] = {
    ToolTip="TXT_KEY_IC_RESOURCE",
	Image="assets/DLC/Shared/UI/Art/Icons/MainResourceButton.dds",
	HL="assets/DLC/Shared/UI/Art/Icons/MainResourceButtonHL.dds" };
	
m_InfoData[ InfoCornerID.GP ] = {
    ToolTip="TXT_KEY_IC_GP",
    Image="assets/DLC/Shared/UI/Art/Icons/MainGreatPersonButton.dds",
    HL="assets/DLC/Shared/UI/Art/Icons/MainGreatPersonButtonHL.dds" };

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnInfoButton( iInfoType )
    Events.OpenInfoCorner( iInfoType );
end
Controls.LeftPull:RegisterSelectionCallback( OnInfoButton );

local g_CurrentInfoCornerID = -1;
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnOpenInfoCorner( iInfoType )
    if( iInfoType ~= InfoCornerID.Diplo ) then 
        Controls.PDButton:SetTexture( m_InfoData[iInfoType].Image );
        Controls.PDImage:SetTexture( m_InfoData[iInfoType].Image );
        Controls.PDAlpha:SetTexture( m_InfoData[iInfoType].HL );
    end
    
    g_CurrentInfoCornerID = iInfoType;
end
Events.OpenInfoCorner.Add( OnOpenInfoCorner );

local g_PerPlayerState = {};
----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnInfoCornerActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	
	-- Restore the state per player
	local bIsHidden = ContextPtr:IsHidden() == true;
	-- Save the state per player
	if (iPrevActivePlayer ~= -1) then
		g_PerPlayerState[ iPrevActivePlayer + 1 ] = g_CurrentInfoCornerID;
	end
	
	if (iActivePlayer ~= -1) then
		if (g_PerPlayerState[ iActivePlayer + 1 ] == nil or g_PerPlayerState[ iActivePlayer + 1 ] == -1) then
			Events.OpenInfoCorner( InfoCornerID.None );
		else
			local iWantInfoCorner = g_PerPlayerState[ iActivePlayer + 1 ];
			if ( iWantInfoCorner ~= g_CurrentInfoCornerID) then
				Events.OpenInfoCorner( iWantInfoCorner );
			end
		end
	end
end

Events.GameplaySetActivePlayer.Add(OnInfoCornerActivePlayerChanged);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
local controlTable;

for i = 0, maxIdx do
    local value = m_InfoData[i];

	if( value ~= nil ) then
        controlTable = {};
        Controls.LeftPull:BuildEntry( "InstanceOne", controlTable );

        controlTable.Button:LocalizeAndSetToolTip( value.ToolTip );
    
        if( i == InfoCornerID.None ) then
            controlTable.Button:SetTexture(  "assets/UI/Art/Icons/MainClose.dds" );
            controlTable.MOImage:SetTexture( "assets/UI/Art/Icons/MainClose.dds" );
            controlTable.MOAlpha:SetTexture( "assets/UI/Art/Icons/MainCloseHL.dds" );
        else
            controlTable.Button:SetTexture( value.Image );
            controlTable.MOImage:SetTexture( value.Image );
            controlTable.MOAlpha:SetTexture( value.HL );
        end
        controlTable.Button:SetVoid1( i );
	end
end
Controls.LeftPull:CalculateInternals();

