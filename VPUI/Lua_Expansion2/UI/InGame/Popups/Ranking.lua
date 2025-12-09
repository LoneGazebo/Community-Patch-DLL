if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
----------------------------------------------------------------        
----------------------------------------------------------------        
include( "InstanceManager" );

local g_RankIM = InstanceManager:new( "RankEntry", "MainStack", Controls.MainStack );
local g_RankList = {};
----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( bIsHide, bIsInit )
	PopulateResults();
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function PopulateResults()
	g_RankIM:ResetInstances();
	g_RankList = {};
	
	local pPlayer = Players[Game.GetActivePlayer()];
	local bWinner = false;
	if (pPlayer:GetTeam() == Game:GetWinner()) then
		bWinner = true;
	end
	
	local pPlayerScore = pPlayer:GetScore(true, bWinner);
	
	local count = 0;
	local playerAdded = false;
	for row in GameInfo.HistoricRankings() do
		count = count + 1;
		local controlTable = g_RankIM:GetInstance();
		g_RankList[count] = controlTable;
		controlTable.Number:SetText(Locale.ConvertTextKey("TXT_KEY_NUMBERING_FORMAT", count) );
		controlTable.LeaderName:SetText(Locale.ConvertTextKey(row.HistoricLeader));
		controlTable.LeaderQuote:LocalizeAndSetText(row.LeaderQuote);
		controlTable.LeaderScore:SetText(row.LeaderScore);

		if(pPlayerScore >= row.LeaderScore and not playerAdded)then
			
			controlTable.LeaderScore:SetText(pPlayerScore);
			
			controlTable.SelectHighlight:SetHide(false);
			controlTable.LeaderQuote:SetHide(false);
			controlTable.MyStack:CalculateSize();
			controlTable.MyStack:ReprocessAnchoring();
			local buttonY = controlTable.MyStack:GetSizeY();
			controlTable.MainStack:SetSizeY(buttonY);
			controlTable.SelectHighlight:SetSizeY(buttonY);
			Controls.Title:LocalizeAndSetText("TXT_KEY_RANKING_STATEMENT", Locale.ConvertTextKey(row.HistoricLeader));
			playerAdded = true;
		else
			controlTable.SelectHighlight:SetHide(true);
			controlTable.LeaderQuote:SetHide(true);
		end

	end
	
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.MainScroll:CalculateInternalSize();
end