if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );
----------------------------------------------------------------        
----------------------------------------------------------------        

g_SpacerManager = InstanceManager:new("SpacerInstance", "Spacer", Controls.CreditsList);
g_MajorTitleManager = InstanceManager:new("MajorTitleInstance", "Text", Controls.CreditsList);
g_MinorTitleManager = InstanceManager:new("MinorTitleInstance", "Text", Controls.CreditsList);
g_HeadingManager = InstanceManager:new("HeadingInstance", "Text", Controls.CreditsList);
g_EntryManager = InstanceManager:new("EntryInstance", "Text", Controls.CreditsList);


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnBack()
	UIManager:DequeuePopup( ContextPtr );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE ) then
			OnBack();
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( bIsHide, bIsInit )
	if( not bIsHide ) then
    	Controls.SlideAnim:SetToBeginning();
    	Controls.SlideAnim:Play();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------        
---------------------------------------------------------------- 
function ReadCredits()
	local creditsFile;		
	local endHeader = 0;		
	local creditLine;		
	local creditHeader;		

	creditsFile = UI.GetCredits()
		
	if(not creditsFile) then		
		--print("Can't find file");	
		return	
	end		
	
	local creditsTable = makeTable(creditsFile);
		
	--print each line out, with header information formatting string
	for key,currentLine in ipairs(creditsTable) do	

		local creditHeader = string.sub(currentLine, 2, 2);
		local creditLine = string.sub(currentLine, 4);

		if creditHeader == "N" then
			local spacer = g_SpacerManager:GetInstance();
		elseif creditHeader == "1" then	
			local majorTitle = g_MajorTitleManager:GetInstance();
			majorTitle.Text:SetText(creditLine);
		elseif creditHeader == "2" then	
			local minorTitle = g_MinorTitleManager:GetInstance();
			minorTitle.Text:SetText(creditLine);
		elseif creditHeader == "3" then	
			local heading = g_HeadingManager:GetInstance();
			heading.Text:SetText(creditLine);
		elseif creditHeader == "4" then	
			local entry = g_EntryManager:GetInstance();
			entry.Text:SetText(creditLine);
		else	
			print("Header type not found.");
		end
	end		

	Controls.CreditsList:CalculateSize();		
	Controls.CreditsList:ReprocessAnchoring();		
	Controls.MajorScroll:CalculateInternalSize();	
	
	Controls.CreditsList:SetOffsetVal(0, 0);
	Controls.SlideAnim:SetEndVal(0, -(Controls.CreditsList:GetSizeY() - 2651));	
			
			
	Controls.SlideAnim:Play();
end

----------------------------------------------------------------        
---------------------------------------------------------------- 
function makeTable(creditsFile)
	
	local i = 0;
	local prev_i = 1;
	local t = {};
	while true do
		i = string.find(creditsFile, "\r\n", i+1, true)    -- find 'next' newline
		
		if i == nil then break end
		
		local line = string.sub(creditsFile, prev_i, i - 1);
		
		table.insert(t, line)
		prev_i = i + 2;
	end
	
	return t;
	
end
----------------------------------------------------------------        
---------------------------------------------------------------- 

ReadCredits();
