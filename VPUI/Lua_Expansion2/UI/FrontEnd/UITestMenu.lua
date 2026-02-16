if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Options Menu
-------------------------------------------------

function OnUpdate( fDTime )
    pct = pct + fDTime;
    if pct > 1.0 
    then
        pct = 0;
        bFlip = not bFlip;
    end
        
	Controls.BarBar1:SetPercents( pct, pct * 2 );
	Controls.BarBar2:SetPercents( pct, pct * 2 );
	Controls.BarBar3:SetPercents( pct, pct * 2 );
	Controls.BarBar4:SetPercents( pct, pct * 2 );
	
	Controls.Bar1:SetPercent( pct );
	Controls.Bar2:SetPercent( pct );
	Controls.Bar3:SetPercent( pct );
	Controls.Bar4:SetPercent( pct );
	Controls.Meter:SetPercent( pct );
	Controls.Meter:SetShadowPercent( pct * 2 );
	
	size = {};
	if( bFlip )
	then
		size.x = pct * 64;
		size.y = pct * 64;
	else
		size.x = (1 - pct) * 64;
		size.y = (1 - pct) * 64;
	end
	Controls.Apple:SetSize( size );
	Controls.S1:SetSize( size );
	Controls.S2:SetSize( size );
	Controls.S3:SetSize( size );
	Controls.S4:SetSize( size );
	Controls.S5:SetSize( size );
	Controls.S6:SetSize( size );
	Controls.S7:SetSize( size );
	Controls.S8:SetSize( size );
end
pct = 0;
ContextPtr:SetUpdate( OnUpdate ); 


function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.WM_KEYDOWN )
    then
        if( wParam == Keys.VK_SPACE )
        then
            pct = 0;
            return true;
        end
        
        if( wParam == Keys.VK_F1 or
            wParam == Keys.A )
        then
            bHideBar = not bHideBar;
            Controls.Bar1:SetHide( bHideBar );
            return true;
        end
        
    elseif( uiMsg == KeyEvents.WM_KEYUP )
    then
        if( wParam == Keys.VK_TAB )
        then
            -- this needs to be on a keyUP, otherwise the editbox will get 
            -- a WM_CHAR event for the enter key and immediately respond
            Controls.EditBox:TakeFocus();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

function SliderUpdate( fValue, void1 )
    if( void1 == 1 )
    then
        Controls.BigSliderValue:SetText( "Big Slider: " .. fValue );
    else
        Controls.SmallSliderValue:SetText( "Small Slider: " .. fValue );
    end
end

Controls.BigSlider:RegisterSliderCallback( SliderUpdate );
Controls.BigSlider:SetVoid1( 1 );
Controls.BigSliderValue:SetText( "Big Slider: " .. Controls.BigSlider:GetValue() );

Controls.SmallSlider:RegisterSliderCallback( SliderUpdate );
Controls.SmallSlider:SetVoid1( 2 );
Controls.SmallSliderValue:SetText( "Small Slider: " .. Controls.SmallSlider:GetValue() );



function OnThing( string, control )
    Controls.EditLabel:SetText( string );
    --Controls.EditBox:ClearString();
end
Controls.EditBox:RegisterCallback( OnThing );



bHideBar = false;
bFlip = false;
   
    
local bThing = false;
local controlTable = {};
Controls.PullDown:BuildEntry( "InstanceOne", controlTable );
controlTable.Button:SetText( "holy cats" );

for i = 0, 30, 1 do
    controlTable = {};
    
    if( bThing ) then
        Controls.PullDown:BuildEntry( "InstanceOne", controlTable );
    else
        Controls.PullDown:BuildEntry( "InstanceTwo", controlTable );
    end
    bThing = not bThing;
    controlTable.Button:SetText( "holy cats " .. i );
    controlTable.Button:SetVoid1( i );
end

Controls.PullDown:CalculateInternals();

function OnSelectionChanged( one, two, control )
    Controls.PullDown:GetButton():SetText( one );
end
Controls.PullDown:RegisterSelectionCallback( OnSelectionChanged );