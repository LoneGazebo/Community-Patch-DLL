if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
function ModeClicked( void1, Control )
	if Control == 0 then
		ContextPtr:SetHide( true );
		--ContextPtr:LookUpControl( "../DebugMenu"):SetHide(false);
	end
end

Controls.Exit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );

------------------------------------------------
-- Terrain Debug Panel Buttons
-------------------------------------------------
g_bWaterReload = false;
g_iWaterSunSpec = 47;
g_iWaterSkySpec = 23;
g_iWaterAmplitude = 23;
g_iWaterGradientScale = 5;
g_iWaterScale = 8;
g_iWaterScrollSpeed = 18;
g_iWaterFlowStrength = 10;
g_iWaterNoiseStrength = 10;
g_iWaterNoiseScale = 2;

Water_SetAutoReload(g_bWaterReload);
Water_SetSpec(g_iWaterSunSpec, g_iWaterSkySpec);
Water_SetAmplitude(g_iWaterAmplitude);
Water_SetScale(g_iWaterScale);
Water_SetGradientScale(g_iWaterGradientScale);
Water_SetSpeed(g_iWaterScrollSpeed);
Water_SetFlowParameters(g_iWaterFlowStrength, g_iWaterNoiseStrength, g_iWaterNoiseScale);

Controls.WaterAutoReload_Checkbox:SetCheck(g_bWaterReload);
Controls.WaterSunSpecular_Label:SetText(g_iWaterSunSpec);
Controls.WaterSkySpecular_Label:SetText(g_iWaterSkySpec);
Controls.WaterAmplitude_Label:SetText(g_iWaterAmplitude);
Controls.WaterScale_Label:SetText(g_iWaterScale);
Controls.WaterGradientScale_Label:SetText(g_iWaterGradientScale);
Controls.WaterSpeed_Label:SetText(g_iWaterScrollSpeed);
Controls.WaterFlow_Label:SetText(g_iWaterFlowStrength);
Controls.WaterNoise_Label:SetText(g_iWaterNoiseStrength);
Controls.WaterNoiseScale_Label:SetText(g_iWaterNoiseScale);

---- auto reload on texture changes
function WaterToggleAutoReload()
	g_iWaterReload = not g_iWaterReload;
	Controls.WaterAutoReload_Checkbox:SetCheck(g_iWaterReload);
	Water_SetAutoReload(g_iWaterReload);
end
Controls.WaterAutoReload_Checkbox:RegisterCallback( Mouse.eLClick, WaterToggleAutoReload );

---- sun highlight
function IncWaterSunSpec( Inc )
	g_iWaterSunSpec = g_iWaterSunSpec + Inc;
	Controls.WaterSunSpecular_Label:SetText(g_iWaterSunSpec);
	Water_SetSpec(g_iWaterSunSpec, g_iWaterSkySpec);
end
Controls.WaterSunSpecular_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterSunSpec );
Controls.WaterSunSpecular_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterSunSpec );

---- sky highlight
function IncWaterSkySpec( Inc )
	g_iWaterSkySpec = g_iWaterSkySpec + Inc;
	Controls.WaterSkySpecular_Label:SetText(g_iWaterSkySpec);
	Water_SetSpec(g_iWaterSunSpec, g_iWaterSkySpec);
end
Controls.WaterSkySpecular_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterSkySpec );
Controls.WaterSkySpecular_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterSkySpec );

---- amplitude of each water layer
function IncWaterAmplitude( Inc )
	g_iWaterAmplitude = g_iWaterAmplitude + Inc;
	Controls.WaterAmplitude_Label:SetText(g_iWaterAmplitude);
	Water_SetAmplitude(g_iWaterAmplitude);
end
Controls.WaterAmplitude_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterAmplitude );
Controls.WaterAmplitude_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterAmplitude );

---- texture coordinate scaling for each water layer
function IncWaterScale( Inc )
	g_iWaterScale = g_iWaterScale + Inc;
	Controls.WaterScale_Label:SetText(g_iWaterScale);
	Water_SetScale(g_iWaterScale);
end
Controls.WaterScale_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterScale );
Controls.WaterScale_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterScale );

-- texture storage scale factor to prevent clipping artifacts
function IncWaterGradientScale( Inc )
	g_iWaterGradientScale = g_iWaterGradientScale + Inc;
	Controls.WaterGradientScale_Label:SetText(g_iWaterGradientScale);
	Water_SetGradientScale(g_iWaterGradientScale);
end
Controls.WaterGradientScale_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterGradientScale );
Controls.WaterGradientScale_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterGradientScale );

---- scrolling speed
function IncWaterSpeed( Inc )
	g_iWaterScrollSpeed = g_iWaterScrollSpeed + Inc;
	Controls.WaterSpeed_Label:SetText(g_iWaterScrollSpeed);
	Water_SetSpeed(g_iWaterScrollSpeed);
end
Controls.WaterSpeed_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterSpeed );
Controls.WaterSpeed_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterSpeed );

---- water terrain-following flow strength
function IncWaterFlow( Inc )
	g_iWaterFlowStrength = g_iWaterFlowStrength + Inc;
	Controls.WaterFlow_Label:SetText(g_iWaterFlowStrength);
	Water_SetFlowParameters(g_iWaterFlowStrength, g_iWaterNoiseStrength, g_iWaterNoiseScale);
end
Controls.WaterFlow_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterFlow );
Controls.WaterFlow_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterFlow );

---- water deep ocean current strength
function IncWaterNoise( Inc )
	g_iWaterNoiseStrength = g_iWaterNoiseStrength + Inc;
	Controls.WaterNoise_Label:SetText(g_iWaterNoiseStrength);
	Water_SetFlowParameters(g_iWaterFlowStrength, g_iWaterNoiseStrength, g_iWaterNoiseScale);
end
Controls.WaterNoise_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterNoise );
Controls.WaterNoise_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterNoise );

---- water deep ocean current scale (in hexes)
function IncWaterNoiseScale( Inc )
	g_iWaterNoiseScale = g_iWaterNoiseScale + Inc;
	Controls.WaterNoiseScale_Label:SetText(g_iWaterNoiseScale);
	Water_SetFlowParameters(g_iWaterFlowStrength, g_iWaterNoiseStrength, g_iWaterNoiseScale);
end
Controls.WaterNoiseScale_ButtonDown:RegisterCallback( Mouse.eLClick, IncWaterNoiseScale );
Controls.WaterNoiseScale_ButtonUp:RegisterCallback( Mouse.eLClick, IncWaterNoiseScale );


function ReloadIce( Inc )
	Ice_Reload();
end

function ReloadWaves( Inc )
	Waves_Reload();
end

Controls.RebuildIce:RegisterCallback( Mouse.eLClick, ReloadIce );
Controls.RebuildWaves:RegisterCallback( Mouse.eLClick, ReloadWaves );