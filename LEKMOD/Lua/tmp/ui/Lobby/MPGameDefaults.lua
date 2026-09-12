

function ResetMultiplayerOptions()


	PreGame.SetLeaderName( 0, "");
	PreGame.SetCivilizationDescription( 0, "");
	PreGame.SetCivilizationShortDescription( 0, "");
	PreGame.SetCivilizationAdjective( 0, "");
	

	local worldSize = GameInfo.Worlds["WORLDSIZE_SMALL"];
	if(worldSize == nil) then
		worldSize = GameInfo.Worlds()();
	end
	PreGame.SetRandomWorldSize( false );
	PreGame.SetWorldSize( worldSize.ID );
	PreGame.SetNumMinorCivs( worldSize.DefaultMinorCivs );
	

	PreGame.SetLoadWBScenario(false);
	PreGame.SetRandomMapScript(false);
	local mapScript = GameInfo.MapScripts{FileName = "Assets\\Maps\\Continents.lua"}();
	if(mapScript ~= nil) then
		PreGame.SetMapScript(mapScript.FileName);
	else

		for row in GameInfo.MapScripts{SupportsMultiplayer = 1} do
			PreGame.SetMapScript(row.FileName);
			break;
		end
	end


	PreGame.SetGameSpeed(3);


	PreGame.SetEra(0);


	PreGame.SetMaxTurns(0);

	PreGame.ResetGameOptions();
	PreGame.ResetMapOptions();
	

	if (PreGame.IsHotSeatGame()) then
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", false);
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", false);
	else
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", false);
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", true);
	end
end
