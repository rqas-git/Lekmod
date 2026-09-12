








































function GetPlayerAndTeamInfo()

	local iNumCivs, iNumCityStates, player_ID_list = 0, 0, {};
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCivs = iNumCivs + 1;
			table.insert(player_ID_list, i);
		end
	end
	for i = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			iNumCityStates = iNumCityStates + 1;
		end
	end

	local bTeamGame = false;
	local iNumTeams = Game.CountCivTeamsEverAlive()
	local iNumTeamsOfCivs = iNumTeams - iNumCityStates;
	if iNumTeamsOfCivs < iNumCivs then
		bTeamGame = true;
	end

	local teams_with_major_civs = {};
	local number_civs_per_team = table.fill(0, GameDefines.MAX_CIV_PLAYERS)
	number_civs_per_team[0] = 0;
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[i];
		if player:IsEverAlive() then
			local teamID = player:GetTeam()
			number_civs_per_team[teamID] = number_civs_per_team[teamID] + 1;
			local bCheckTeamList = TestMembership(teams_with_major_civs, teamID)
			if bCheckTeamList == false then
				table.insert(teams_with_major_civs, teamID)
			end
		end
	end
	
	return iNumCivs, iNumCityStates, player_ID_list, bTeamGame, teams_with_major_civs, number_civs_per_team
end



function ObtainLandmassBoundaries(iAreaID)
	local iW, iH = Map.GetGridSize();

	local wrapsX = false;
	local wrapsY = false;
	local iWestX, iEastX, iSouthY, iNorthY, iWidth, iHeight;
	
	if Map:IsWrapX() then
		local foundFirstColumn = false;
		local foundLastColumn = false;
		for y = 0, iH - 1 do
			local plotFirst = Map.GetPlot(0, y);
			local plotLast = Map.GetPlot(iW - 1, y);
			local area = plotFirst:GetArea();
			if area == iAreaID then
				foundFirstColumn = true;
			end
			area = plotLast:GetArea();
			if area == iAreaID then
				foundLastColumn = true;
			end
		end
		if foundFirstColumn and foundLastColumn then
			wrapsX = true;
		end
	end
	
	if Map:IsWrapY() then
		local foundFirstRow = false;
		local foundLastRow = false;
		for y = 0, iH - 1 do
			local plotFirst = Map.GetPlot(x, 0);
			local plotLast = Map.GetPlot(x, iH - 1);
			local area = plotFirst:GetArea();
			if area == iAreaID then
				foundFirstRow = true;
			end
			area = plotLast:GetArea();
			if area == iAreaID then
				foundLastRow = true;
			end
		end
		if foundFirstRow and foundLastRow then
			wrapsY = true;
		end
	end


	if not wrapsX then
		for x = 0, iW - 1 do
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInColumn = true;
					iWestX = x;
					break
				end
			end
			if foundAreaInColumn then
				break
			end
		end
		for x = iW - 1, 0, -1 do
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInColumn = true;
					iEastX = x;
					break
				end
			end
			if foundAreaInColumn then
				break
			end
		end
	else
		local landmassSpansEntireWorldX = true;
		for x = iW - 2, 1, -1 do
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInColumn = true;
				end
			end
			if not foundAreaInColumn then
				iWestX = x + 1;
				landmassSpansEntireWorldX = false;
				break
			end
		end
		for x = 1, iW - 2 do
			local foundAreaInColumn = false;
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInColumn = true;
				end
			end
			if not foundAreaInColumn then
				iEastX = x - 1;
				landmassSpansEntireWorldX = false;
				break
			end
		end

		if landmassSpansEntireWorldX then
			wrapsX = false;
			iWestX = 0;
			iEastX = iW - 1;
		end
	end
				

	if not wrapsY then
		for y = 0, iH - 1 do
			local foundAreaInRow = false;
			for x = 0, iW - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInRow = true;
					iSouthY = y;
					break
				end
			end
			if foundAreaInRow then
				break
			end
		end
		for y = iH - 1, 0, -1 do
			local foundAreaInRow = false;
			for x = 0, iW - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInRow = true;
					iNorthY = y;
					break
				end
			end
			if foundAreaInRow then
				break
			end
		end
	else
		local landmassSpansEntireWorldY = true;
		for y = iH - 2, 1, -1 do
			local foundAreaInRow = false;
			for x = 0, iW - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInRow = true;
				end
			end
			if not foundAreaInRow then
				iSouthY = y + 1;
				landmassSpansEntireWorldY = false;
				break
			end
		end
		for y = 1, iH - 2 do
			local foundAreaInRow = false;
			for x = 0, iW - 1 do
				local plot = Map.GetPlot(x, y);
				local area = plot:GetArea();
				if area == iAreaID then
					foundAreaInRow = true;
				end
			end
			if not foundAreaInRow then
				iNorthY = y - 1;
				landmassSpansEntireWorldY = false;
				break
			end
		end

		if landmassSpansEntireWorldY then
			wrapsY = false;
			iSouthY = 0;
			iNorthY = iH - 1;
		end
	end
	

	if wrapsX then
		iWidth = (iEastX + iW) - iWestX + 1;
	else
		iWidth = iEastX - iWestX + 1;
	end
	if wrapsY then
		iHeight = (iNorthY + iH) - iSouthY + 1;
	else
		iHeight = iNorthY - iSouthY + 1;
	end












	local data = {iWestX, iSouthY, iEastX, iNorthY, iWidth, iHeight, wrapsX, wrapsY};
	return data
end



function AdjacentToMainland(x, y, iAreaID)

	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType();
	if plotType == PlotTypes.PLOT_OCEAN and not plot:IsLake() then

		local directions = { DirectionTypes.DIRECTION_NORTHEAST,
		                     DirectionTypes.DIRECTION_EAST,
		                     DirectionTypes.DIRECTION_SOUTHEAST,
		                     DirectionTypes.DIRECTION_SOUTHWEST,
		                     DirectionTypes.DIRECTION_WEST,
		                     DirectionTypes.DIRECTION_NORTHWEST };

		for loop, current_direction in ipairs(directions) do
			local testPlot = Map.PlotDirection(x, y, current_direction);
			if testPlot ~= nil then
				local type = testPlot:GetPlotType()
				if type ~= PlotTypes.PLOT_OCEAN then
					local thisPlotsArea = testPlot:GetArea()
					if thisPlotsArea == iAreaID then
						return true
					end
				end
			end
		end
	end

	return false
end



function GenerateMainlandCoastDataTable()
	local iW, iH = Map.GetGridSize();
	local biggest_area = Map.FindBiggestArea(False);
	local iAreaID = biggest_area:GetID();
	local plotDataMainlandCoast = {};
	table.fill(plotDataMainlandCoast, false, iW * iH);




	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plotIsAdjacent = AdjacentToMainland(x, y, iAreaID)
			if plotIsAdjacent then
				local i = iW * y + x + 1;
				plotDataMainlandCoast[i] = true;
			end
		end
	end
	

	return plotDataMainlandCoast
end



function GenerateMainlandExpandedCoastData()

	local plotDataMainlandCoast = GenerateMainlandCoastDataTable()


	local iW, iH = Map.GetGridSize();
	local plotDataExpandedCoast = {};
	table.fill(plotDataExpandedCoast, false, iW * iH);




	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotTerrain = plot:GetTerrainType();

			if plotDataMainlandCoast[i] == false and plot:IsWater() and plotTerrain == TerrainTypes.TERRAIN_COAST then

				local NEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHEAST);
				local EPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_EAST);
				local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST);
				local SWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHWEST);
				local WPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_WEST);
				local NWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHWEST);


				if NEPlot ~= nil then
					local adjX = NEPlot:GetX();
					local adjY = NEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataMainlandCoast[adjI] == true then

						plotDataExpandedCoast[i] = true;
					end
				end

				if EPlot ~= nil then
					local adjX = EPlot:GetX();
					local adjY = EPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataMainlandCoast[adjI] == true then
						plotDataExpandedCoast[i] = true;
					end
				end

				if SEPlot ~= nil then
					local adjX = SEPlot:GetX();
					local adjY = SEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataMainlandCoast[adjI] == true then
						plotDataExpandedCoast[i] = true;
					end
				end

				if SWPlot ~= nil then
					local adjX = SWPlot:GetX();
					local adjY = SWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataMainlandCoast[adjI] == true then
						plotDataExpandedCoast[i] = true;
					end
				end

				if WPlot ~= nil then
					local adjX = WPlot:GetX();
					local adjY = WPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataMainlandCoast[adjI] == true then
						plotDataExpandedCoast[i] = true;
					end
				end

				if NWPlot ~= nil then
					local adjX = NWPlot:GetX();
					local adjY = NWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataMainlandCoast[adjI] == true then
						plotDataExpandedCoast[i] = true;
					end
				end
			end
		end
	end


	return plotDataMainlandCoast, plotDataExpandedCoast, manilandCoastList
end




function GenerateThreeFromMainlandCoast(plotDataMainlandCoast, plotDataExpandedCoast)


	local iW, iH = Map.GetGridSize();
	local plotDataIsThreeFromMainland = {};
	table.fill(plotDataIsThreeFromMainland, false, iW * iH);



	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotTerrain = plot:GetTerrainType();

			if plotDataMainlandCoast[i] == false and plotDataExpandedCoast[i] == false and plot:IsWater() and not plot:IsLake() and plotTerrain == TerrainTypes.TERRAIN_COAST then
					

				local NEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHEAST);
				local EPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_EAST);
				local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST);
				local SWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHWEST);
				local WPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_WEST);
				local NWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHWEST);


				if NEPlot ~= nil then
					local adjX = NEPlot:GetX();
					local adjY = NEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataExpandedCoast[adjI] == true then

						plotDataIsThreeFromMainland[i] = true;
					end
				end

				if EPlot ~= nil then
					local adjX = EPlot:GetX();
					local adjY = EPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataExpandedCoast[adjI] == true then
						plotDataIsThreeFromMainland[i] = true;
					end
				end

				if SEPlot ~= nil then
					local adjX = SEPlot:GetX();
					local adjY = SEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataExpandedCoast[adjI] == true then
						plotDataIsThreeFromMainland[i] = true;
					end
				end

				if SWPlot ~= nil then
					local adjX = SWPlot:GetX();
					local adjY = SWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataExpandedCoast[adjI] == true then
						plotDataIsThreeFromMainland[i] = true;
					end
				end

				if WPlot ~= nil then
					local adjX = WPlot:GetX();
					local adjY = WPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataExpandedCoast[adjI] == true then
						plotDataIsThreeFromMainland[i] = true;
					end
				end

				if NWPlot ~= nil then
					local adjX = NWPlot:GetX();
					local adjY = NWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataExpandedCoast[adjI] == true then
						plotDataIsThreeFromMainland[i] = true;
					end
				end
			end
		end
	end
	

	return plotDataIsThreeFromMainland
end



function AdjacentToSaltWater(x, y)

	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType()
	if plotType ~= PlotTypes.PLOT_OCEAN then

		local directions = { DirectionTypes.DIRECTION_NORTHEAST,
		                     DirectionTypes.DIRECTION_EAST,
		                     DirectionTypes.DIRECTION_SOUTHEAST,
		                     DirectionTypes.DIRECTION_SOUTHWEST,
		                     DirectionTypes.DIRECTION_WEST,
		                     DirectionTypes.DIRECTION_NORTHWEST };

		for loop, current_direction in ipairs(directions) do
			local testPlot = Map.PlotDirection(x, y, current_direction);
			if testPlot ~= nil then
				local type = testPlot:GetPlotType()
				if type == PlotTypes.PLOT_OCEAN then
					if testPlot:IsLake() == false then
						return true
					end
				end
			end
		end
	end

	return false
end

function GenerateCoastalLandDataTable()
	local iW, iH = Map.GetGridSize();
	local plotDataIsCoastal = {};
	table.fill(plotDataIsCoastal, false, iW * iH);



	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local plotIsAdjacent = AdjacentToSaltWater(x, y)
			if plotIsAdjacent then
				local i = iW * y + x + 1;
				plotDataIsCoastal[i] = true;
			end
		end
	end
	

	return plotDataIsCoastal
end

function GenerateNextToCoastalLandDataTables()

	local plotDataIsCoastal = GenerateCoastalLandDataTable()


	local iW, iH = Map.GetGridSize();
	local plotDataIsNextToCoast = {};
	table.fill(plotDataIsNextToCoast, false, iW * iH);



	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlot(x, y);
			if plotDataIsCoastal[i] == false and not plot:IsWater() then

				local NEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHEAST);
				local EPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_EAST);
				local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST);
				local SWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHWEST);
				local WPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_WEST);
				local NWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHWEST);


				if NEPlot ~= nil then
					local adjX = NEPlot:GetX();
					local adjY = NEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then

						plotDataIsNextToCoast[i] = true;
					end
				end

				if EPlot ~= nil then
					local adjX = EPlot:GetX();
					local adjY = EPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end

				if SEPlot ~= nil then
					local adjX = SEPlot:GetX();
					local adjY = SEPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end

				if SWPlot ~= nil then
					local adjX = SWPlot:GetX();
					local adjY = SWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end

				if WPlot ~= nil then
					local adjX = WPlot:GetX();
					local adjY = WPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end

				if NWPlot ~= nil then
					local adjX = NWPlot:GetX();
					local adjY = NWPlot:GetY();
					local adjI = iW * adjY + adjX + 1;
					if plotDataIsCoastal[adjI] == true then
						plotDataIsNextToCoast[i] = true;
					end
				end
			end
		end
	end
	

	return plotDataIsCoastal, plotDataIsNextToCoast
end


function GenerateThreeFromCoastTable(plotDataIsCoastal, plotDataIsNextToCoast)


	local iW, iH = Map.GetGridSize();
	local plotDataIsThreeFromCoast = {};
	table.fill(plotDataIsThreeFromCoast, false, iW * iH);



	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlot(x, y);
			if plotDataIsCoastal[i] == false and plotDataIsNextToCoast[i] == false then
				
				if not plot:IsWater() or (plot:IsWater() and plot:IsFreshWater()) then
					

					local NEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHEAST);
					local EPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_EAST);
					local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST);
					local SWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHWEST);
					local WPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_WEST);
					local NWPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_NORTHWEST);


					if NEPlot ~= nil then
						local adjX = NEPlot:GetX();
						local adjY = NEPlot:GetY();
						local adjI = iW * adjY + adjX + 1;
						if plotDataIsNextToCoast[adjI] == true then

							plotDataIsThreeFromCoast[i] = true;
						end
					end

					if EPlot ~= nil then
						local adjX = EPlot:GetX();
						local adjY = EPlot:GetY();
						local adjI = iW * adjY + adjX + 1;
						if plotDataIsNextToCoast[adjI] == true then
							plotDataIsThreeFromCoast[i] = true;
						end
					end

					if SEPlot ~= nil then
						local adjX = SEPlot:GetX();
						local adjY = SEPlot:GetY();
						local adjI = iW * adjY + adjX + 1;
						if plotDataIsNextToCoast[adjI] == true then
							plotDataIsThreeFromCoast[i] = true;
						end
					end

					if SWPlot ~= nil then
						local adjX = SWPlot:GetX();
						local adjY = SWPlot:GetY();
						local adjI = iW * adjY + adjX + 1;
						if plotDataIsNextToCoast[adjI] == true then
							plotDataIsThreeFromCoast[i] = true;
						end
					end

					if WPlot ~= nil then
						local adjX = WPlot:GetX();
						local adjY = WPlot:GetY();
						local adjI = iW * adjY + adjX + 1;
						if plotDataIsNextToCoast[adjI] == true then
							plotDataIsThreeFromCoast[i] = true;
						end
					end

					if NWPlot ~= nil then
						local adjX = NWPlot:GetX();
						local adjY = NWPlot:GetY();
						local adjI = iW * adjY + adjX + 1;
						if plotDataIsNextToCoast[adjI] == true then
							plotDataIsThreeFromCoast[i] = true;
						end
					end
				end
			end
		end
	end
	

	return plotDataIsThreeFromCoast
end


function CivNeedsCoastalStart(civType)

	for row in GameInfo.Civilization_Start_Along_Ocean{CivilizationType = civType} do
		if(row.StartAlongOcean == true) then
			return true;
		end
	end
	return false;
end

function CivNeedsRiverStart(civType)

	for row in GameInfo.Civilization_Start_Along_River{CivilizationType = civType} do
		if(row.StartAlongRiver == true) then
			return true;
		end
	end
	return false;
end

function CivNeedsPlaceFirstCoastalStart(civType)
	for row in GameInfo.Civilization_Start_Place_First_Along_Ocean{CivilizationType = civType} do
		if(row.PlaceFirst == true) then
			return true;
		end
	end
	return false;
end

function GetNumStartRegionPriorityForCiv(civType)

	for row in DB.Query("select count(*) as count from Civilization_Start_Region_Priority where CivilizationType = ?", civType) do
		return row.count;
	end
	return 0;
end

function GetNumStartRegionAvoidForCiv(civType)

	for row in DB.Query("select count(*) as count from Civilization_Start_Region_Avoid where CivilizationType = ?", civType) do
		return row.count;
	end
	return 0;
end

function GetStartRegionPriorityListForCiv_GetIDs(civType)


	local priorityRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Priority {CivilizationType = civType} do
		table.insert(priorityRegionTypes, row.RegionType);
	end
    local priorityRegionIDs = {};
    for i, v in ipairs(priorityRegionTypes) do
    	local region_type = GameInfo.Regions[v].ID;
    	local bTest = TestMembership(priorityRegionIDs, region_type);
    	if bTest == false then
    		table.insert(priorityRegionIDs, region_type);
    	end
    end
    table.sort(priorityRegionIDs);
    return priorityRegionIDs;
end

function GetStartRegionAvoidListForCiv_GetIDs(civType)


	local avoidRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Avoid {CivilizationType = civType} do
		table.insert(avoidRegionTypes, row.RegionType);
	end
    local avoidRegionIDs = {};
    for i, v in ipairs(avoidRegionTypes) do
    	local region_type = GameInfo.Regions[v].ID;
    	local bTest = TestMembership(avoidRegionIDs, region_type);
    	if bTest == false then
    		table.insert(avoidRegionIDs, region_type);
    	end
    end
    table.sort(avoidRegionIDs);
    return avoidRegionIDs;
end

function GetStartRegionPriorityListForCiv_GetTypes(civType)


	local priorityRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Priority{CivilizationType = civType} do
		table.insert(priorityRegionTypes, row.RegionType);
	end
	return priorityRegionTypes;
end

function GetStartRegionAvoidListForCiv_GetTypes(civType)


	local avoidRegionTypes = {};
	for row in GameInfo.Civilization_Start_Region_Avoid{CivilizationType = civType} do
		table.insert(avoidRegionTypes, row.RegionType);
	end
	return avoidRegionTypes;
end



function TestMembership(table, value)
	local testResult = false;
	for index, data in pairs(table) do
		if data == value then
			testResult = true;
			break
		end
	end
	return testResult
end

function GetShuffledCopyOfTable(incoming_table)

	local len = table.maxn(incoming_table);
	local copy = {};
	local shuffledVersion = {};
	local dense = true;

	for loop = 1, len do
		copy[loop] = incoming_table[loop];
		if copy[loop] == nil then dense = false; end
	end



	if len >= 1024 and len % 1 == 0 and dense then
		local tree, low = {}, {};
		for i = 1, len do
			low[i] = i % 2 == 1 and 1 or 2 * low[i / 2];
			tree[i] = low[i];
		end
		local top = 1;
		while top * 2 <= len do top = top * 2; end
		for loop = 1, len do
			local rank = 1 + Map.Rand(len - loop + 1, "Shuffling table entry - Lua");
			local index, bit = 0, top;
			while bit >= 1 do
				local nextIndex = index + bit;
				if nextIndex <= len and tree[nextIndex] < rank then
					rank = rank - tree[nextIndex];
					index = nextIndex;
				end
				bit = bit / 2;
			end
			index = index + 1;
			shuffledVersion[loop] = copy[index];
			while index <= len do
				tree[index] = tree[index] - 1;
				index = index + low[index];
			end
		end
		return shuffledVersion
	end

	local left_to_do = table.maxn(copy);
	for loop = 1, len do
		local random_index = 1 + Map.Rand(left_to_do, "Shuffling table entry - Lua");
		table.insert(shuffledVersion, copy[random_index]);
		table.remove(copy, random_index);
		left_to_do = left_to_do - 1;
	end
	return shuffledVersion
end

function IdentifyTableIndex(incoming_table, value)


	local bFoundValue = false;
	local iNumTimesFoundValue = 0;
	local table_of_indices = {};
	for loop, test_value in pairs(incoming_table) do
		if test_value == value then
			bFoundValue = true;
			iNumTimesFoundValue = iNumTimesFoundValue + 1;
			table.insert(table_of_indices, loop);
		end
	end
	return bFoundValue, iNumTimesFoundValue, table_of_indices;
end

function PrintContentsOfTable(incoming_table)
	print("--------------------------------------------------");
	print("Table printout for table ID:", table);
	for index, data in pairs(incoming_table) do
		print("Table index:", index, "Table entry:", data);
	end
	print("- - - - - - - - - - - - - - - - - - - - - - - - - -");
end
