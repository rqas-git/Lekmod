








include("HBMapGeneratorMirrored");
include("HBFractalWorld");
include("HBFeatureGeneratorMirrored");
include("HBTerrainGenerator");
include("IslandMaker");
include("MultilayeredFractal");



include("HBMapOptions");

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "Lekmap: Teamer Map (Bonus Map)",
		Description = "HellBlazers Teamer Map combined with Skirmish. Uses Leszek Deska's mirroring algorithm. Frankensteined together by Meota.",
		IsAdvancedMap = false,
		IconIndex = 17,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create({
			[11] = {
				Values = {
					"20 - Duel",
					"22",
					"24",
					"26",
					"28",
					"30 - 4 players",
					"32",
					"34 - Duel with back coast",
					"36 - 6 players",
					"38",
					"40 - 4 players with back coast",
					"42",
					"44 - 8 players; 6 players with back coast",
					"46",
					"48",
					"50 - 8 players with back coast",
					"52",
					"54",
				},
				DefaultValue = 9,
			},
			[12] = {
				Values = {
					"14 - Duel",
					"16",
					"18",
					"20 - 4 players; Duel with north/south coast",
					"22",
					"24",
					"26 - 6 players; 4 players with north/south coast",
					"28",
					"30 - 8 players",
					"32 - 6 players with north/south coast",
					"34",
					"36 - 8 players with north/south coast",
					"38",
					"40",
				},
				DefaultValue = 7,
			},
		}, {
			{
				Name = "TXT_KEY_MAP_OPTION_TEAM_SETTING",
				Values = {
					"TXT_KEY_MAP_OPTION_START_TOGETHER",
					"TXT_KEY_MAP_OPTION_START_ANYWHERE",
				},
				DefaultValue = 1,
				SortPriority = 1,
			},
			{
				Name = "Frontline Distance",
				Values = {
					"0",
					"1",
					"2 - Default",
					"3",
					"4",
					"5",
					"6",
				},
				DefaultValue = 3,
				SortPriority = -96,
			},
			{
				Name = "Back Distance",
				Values = {
					"0 - Default",
					"1",
					"2",
					"3",
					"4",
					"5",
					"6",
					"7",
					"8",
					"9",
					"10",
				},
				DefaultValue = 1,
				SortPriority = -97,
			},
			{
				Name = "Center Split",
				Values = {
					"Ocean Strip",
					"Landbridges",
					"Marsh",
					"Snow",
					"Normal Land",
					"Skirmish",
				},
				DefaultValue = 1,
				SortPriority = -99,
			},
			{
				Name = "Back Coasts",
				Values = {
					"Yes",
					"No",
				},
				DefaultValue = 2,
				SortPriority = -102,
			},
			{
				Name = "North and South Coasts",
				Values = {
					"Yes",
					"No",
				},
				DefaultValue = 2,
				SortPriority = -103,
			},
			{
				Name = "World Wrap",
				Values = {
					"Yes",
					"No",
				},
				DefaultValue = 2,
				SortPriority = -104,
			},
		}),
	};
end



function GetMapInitData(worldSize)




	local LandSizeX = 18 + (Map.GetCustomOption(11) * 2);
	local LandSizeY = 12 + (Map.GetCustomOption(12) * 2);
	local AnyTruers = Map.GetCustomOption(21);
	local ShouldWrap = false;

	if AnyTruers == 1 then
		ShouldWrap = true;
	elseif AnyTruers == 2 then
		ShouldWrap = false;
	end
	
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {LandSizeX, LandSizeY}
		}
	
	local grid_size = worldsizes[worldSize];

	local world = GameInfo.Worlds[worldSize];
	if(world ~= nil) then
	return {
		Width = grid_size[1],
		Height = grid_size[2],
		WrapX = ShouldWrap,
	};      
     end
end

function MultilayeredFractal:GeneratePlotsByRegion()




	local iW, iH = Map.GetGridSize();
	local fracFlags = {};
	local SplitOps = Map.GetCustomOption(18);
	local BackOcean = Map.GetCustomOption(19);
	local NorthSouthOcean = Map.GetCustomOption(20);


	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);

	if SplitOps == 1 then
	

		for y = 0, iH - 1 do
			for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
				local plotIndex = y * iW + x + 1;





					self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;

			end
		end
	elseif SplitOps == 2 then
		

		for y = 4, iH - 5 do
			for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
				local plotIndex = y * iW + x + 1;
				if y >= math.floor(iH / 2) - 2 and y <= math.floor(iH / 2) + 1 then
					if x == math.floor(iW / 2) or x == math.floor(iW / 2) - 1 then

					end
				else
					self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
				end
			end
		end
	end

	if BackOcean == 1 then

		local west_half = {};
		for loop = 1, iH - 2 do
			table.insert(west_half, loop);
		end
		local west_shuffled = GetShuffledCopyOfTable(west_half)
		local iNumMountainsPerColumn = math.max(math.floor(iH * 0.33), math.floor((iH / 3) - 1));
		local x_west = 3;
		for loop = 1, iNumMountainsPerColumn do
			local y_west = west_shuffled[loop];
			local i_west_plot = y_west * iW + x_west + 1;
			self.wholeworldPlotTypes[i_west_plot] = PlotTypes.PLOT_OCEAN;
		end

		for y = 0, iH do
			for x = 0, 2 do
				local plotIndex = y * iW + x + 1;
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end
		for y = 0, iH do
			for x = iW - 3, iW do
				local plotIndex = y * iW + x + 1;
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end
	end
	if NorthSouthOcean == 1 then

		local south_half = {};
		for loop = 1, iW - 2 do
			table.insert(south_half, loop);
		end
		local south_shuffled = GetShuffledCopyOfTable(south_half)
		local iNumMountainsPerColumn = math.max(math.floor(iW * 0.33), math.floor((iW / 3) - 1));
		local y_south = 3;
		for loop = 1, iNumMountainsPerColumn do
			local x_south = south_shuffled[loop];
			local i_south_plot = y_south * iW + x_south + 1;
			self.wholeworldPlotTypes[i_south_plot] = PlotTypes.PLOT_OCEAN;
		end
		local north_half = {};
		for loop = 1, iW - 2 do
			table.insert(north_half, loop);
		end
		local north_shuffled = GetShuffledCopyOfTable(north_half)
		local iNumMountainsPerColumn = math.max(math.floor(iW * 0.33), math.floor((iW / 3) - 1));
		local y_north = iH - 4;
		for loop = 1, iNumMountainsPerColumn do
			local x_north = north_shuffled[loop];
			local i_north_plot = y_north * iW + x_north + 1;
			self.wholeworldPlotTypes[i_north_plot] = PlotTypes.PLOT_OCEAN;
		end
		for x = 0, iW do
			for y = 0, 2 do
				local plotIndex = y * iW + x + 1;
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end
		for x = 0, iW do
			for y = iH - 3, iH do
				local plotIndex = y * iW + x + 1;
				self.wholeworldPlotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end
	end


	local lakesFrac = Fractal.Create(iW, iH, lake_grain, fracFlags, 6, 6);
	local iLakesThreshold = lakesFrac:GetHeight(92);
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local lakeVal = lakesFrac:GetHeight(x, y);
			if lakeVal >= iLakesThreshold then

			end
		end
	end



	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local args = {world_age = world_age};
	self:ApplyTectonics(args)
	
	if SplitOps == 6 then
		for x = iW / 2 - 2, iW / 2 + 1 do
			for y = 1, iH - 2 do
				local i = y * iW + x + 1;
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
			end
		end
		for x = iW / 2 - 2, iW / 2 + 1 do
			local i = x + 1;
			self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
			local i = (iH - 1) * iW + x + 1;
			self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
		end
		local west_half, east_half = {}, {};
		for loop = 1, iH - 2 do
			table.insert(west_half, loop);
			table.insert(east_half, loop);
		end
		local west_shuffled = GetShuffledCopyOfTable(west_half)
		local east_shuffled = GetShuffledCopyOfTable(east_half)
		local iNumMountainsPerColumn = math.max(math.floor(iH * 0.225), math.floor((iH / 4) - 1));
		local x_west, x_east = iW / 2 - 1, iW / 2;
		for loop = 1, iNumMountainsPerColumn do
			local y_west, y_east = west_shuffled[loop], iH - 1- west_shuffled[loop];
			local i_west_plot = y_west * iW + x_west + 1;
			local i_east_plot = y_east * iW + x_east + 1;
			self.wholeworldPlotTypes[i_west_plot] = PlotTypes.PLOT_MOUNTAIN;
			self.wholeworldPlotTypes[i_east_plot] = PlotTypes.PLOT_MOUNTAIN;
		end
	end
	if SplitOps == 4 then
		for x = iW / 2 - 4, iW / 2 + 3 do
			for y = 0, iH - 1 do
				local i = y * iW + x + 1;
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
			end
		end
		local west_half, east_half = {}, {};
		for loop = 1, iH - 2 do
			table.insert(west_half, loop);
			table.insert(east_half, loop);
		end
		local west_shuffled = GetShuffledCopyOfTable(west_half)
		local east_shuffled = GetShuffledCopyOfTable(east_half)
		local iNumMountainsPerColumn = math.max(math.floor(iH * 0.45), math.floor((iH / 2) - 1));
		local x_west, x_east = iW / 2 - 4, iW / 2 + 3;
		for loop = 1, iNumMountainsPerColumn do
			local y_west, y_east = west_shuffled[loop], iH - 1- west_shuffled[loop];
			local i_west_plot = y_west * iW + x_west + 1;
			local i_east_plot = y_east * iW + x_east + 1;
			self.wholeworldPlotTypes[i_west_plot] = PlotTypes.PLOT_MOUNTAIN;
			self.wholeworldPlotTypes[i_east_plot] = PlotTypes.PLOT_MOUNTAIN;
		end
	end

	return self.wholeworldPlotTypes
end

function GeneratePlotTypes()
	print("Setting Plot Types (Lua West vs East) ...");

	local layered_world = MultilayeredFractal.Create();
	local plot_list = layered_world:GeneratePlotsByRegion();
	local SplitOps = Map.GetCustomOption(18);

	SetPlotTypes(plot_list);

	if SplitOps == 4 then
		local plot_list = layered_world:GeneratePlotsByRegion();
		local iW, iH = Map.GetGridSize();
		local firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
		local firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
		for x = iW / 2 - 5, iW / 2 + 4 do
			for y = 1, iH - 2 do
				local plot = Map.GetPlot(x, y)
				if plot:IsFlatlands() then
					local isEvenY, search_table = true, {};
					if y / 2 > math.floor(y / 2) then
					isEvenY = false;
					end
					if isEvenY then
						search_table = firstRingYIsEven;
					else
						search_table = firstRingYIsOdd;
					end

					for loop, plot_adjustments in ipairs(search_table) do
						local searchX, searchY;
						searchX = x + plot_adjustments[1];
						searchY = y + plot_adjustments[2];
						local searchPlot = Map.GetPlot(searchX, searchY)
						local plotType = searchPlot:GetPlotType()
						if plotType == PlotTypes.PLOT_MOUNTAIN then
							plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
							break
						end
					end
				end
			end
		end
	end
	if SplitOps == 6 then
		local iW, iH = Map.GetGridSize();
		local firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
		local firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
		for x = iW / 2 - 2, iW / 2 + 1 do
			for y = 1, iH - 2 do
				local plot = Map.GetPlot(x, y)
				if plot:IsFlatlands() then
					local isEvenY, search_table = true, {};
					if y / 2 > math.floor(y / 2) then
					isEvenY = false;
					end
					if isEvenY then
						search_table = firstRingYIsEven;
					else
						search_table = firstRingYIsOdd;
					end

					for loop, plot_adjustments in ipairs(search_table) do
						local searchX, searchY;
						searchX = x + plot_adjustments[1];
						searchY = y + plot_adjustments[2];
						local searchPlot = Map.GetPlot(searchX, searchY)
						local plotType = searchPlot:GetPlotType()
						if plotType == PlotTypes.PLOT_MOUNTAIN then
							plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false)
							break
						end
					end
				end
			end
		end
	end

	local args = {bExpandCoasts = false};
	GenerateCoasts(args);
end



function TerrainGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs((self.iHeight / 2) - iY) / (self.iHeight / 2);
	lat = lat + (128 - self.variation:GetHeight(iX, iY))/(255.0 * 5.0);
	lat = 0.8 * (math.clamp(lat, 0, 1));
	return lat;
end

function GenerateTerrain()
	print("Generating Terrain (Lua West vs East) ...");
	

	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local args = {temperature = temp};
	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();
	
	SetTerrainTypes(terrainTypes);
end





function FeatureGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs((self.iGridH/2) - iY)/(self.iGridH/2);

	local adjusted_lat = 0.8 * lat;
	
	return adjusted_lat
end

function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	return
end

function AddFeatures()
	print("Adding Features (Lua West vs East) ...");


	local rain = Map.GetCustomOption(3)
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end
	
	local args = {rainfall = rain}
	local featuregen = FeatureGenerator.Create(args);

	featuregen:AddFeatures(false);
end


function GetRiverValueAtPlot(plot)

	local iW, iH = Map.GetGridSize()
	local x = plot:GetX()
	local y = plot:GetY()
	local random_factor = Map.Rand(3, "River direction random factor - Skirmish LUA");
	local direction_influence_value = (math.abs(iW - (x - (iW / 2))) + ((math.abs(y - (iH / 2))) / 3)) * random_factor;

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = ((numPlots - plot:GetPlotType()) * 20) + direction_influence_value;

	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if (adjacentPlot ~= nil) then
			sum = sum + (numPlots - adjacentPlot:GetPlotType());
		else
			sum = sum + (numPlots * 10);
		end
	end
	sum = sum + Map.Rand(10, "River Rand");

	return sum;
end

include("HBRiverGenerator");

function AddRivers()


	local iW, iH = Map.GetGridSize()
	print("Skirmish - Adding Rivers");
	local passConditions = {
		function(plot)
			return plot:IsHills() or plot:IsMountain();
		end,
		
		function(plot)
			return (not plot:IsCoastalLand()) and (Map.Rand(8, "HBTMapGenerator AddRivers") == 0);
		end,
		
		function(plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines["PLOTS_PER_RIVER_EDGE"];
			return (plot:IsHills() or plot:IsMountain()) and (area:GetNumRiverEdges() <	((area:GetNumTiles() / plotsPerRiverEdge) + 1));
		end,
		
		function(plot)
			local area = plot:Area();
			local plotsPerRiverEdge = GameDefines["PLOTS_PER_RIVER_EDGE"];
			return (area:GetNumRiverEdges() < (area:GetNumTiles() / plotsPerRiverEdge) + 1);
		end
	}
	for iPass, passCondition in ipairs(passConditions) do
		local riverSourceRange;
		local seaWaterRange;
		if (iPass <= 2) then
			riverSourceRange = GameDefines["RIVER_SOURCE_MIN_RIVER_RANGE"];
			seaWaterRange = GameDefines["RIVER_SOURCE_MIN_SEAWATER_RANGE"];
		else
			riverSourceRange = (GameDefines["RIVER_SOURCE_MIN_RIVER_RANGE"] / 2);
			seaWaterRange = (GameDefines["RIVER_SOURCE_MIN_SEAWATER_RANGE"] / 2);
		end
		for i, plot in Plots() do
			local current_x = plot:GetX()
			local current_y = plot:GetY()
			if current_x < 1 or current_x >= iW - 2 or current_y < 2 or current_y >= iH - 1 then

			elseif current_x >= (iW / 2) - 3 and current_x <= (iW / 2) + 2 then

			elseif (not plot:IsWater()) then
				if(passCondition(plot)) then
					if (not Map.FindWater(plot, riverSourceRange, true)) then
						if (not Map.FindWater(plot, seaWaterRange, false)) then
							local inlandCorner = plot:GetInlandCorner();
							if(inlandCorner) then
								local start_x = inlandCorner:GetX()
								local start_y = inlandCorner:GetY()
								local orig_direction;
								if start_y < iH / 2 then
									if start_x < iW / 2 then
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
									else
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
									end
								else
									if start_x < iW / 2 then
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
									else
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
									end
								end
								DoRiver(inlandCorner, nil, orig_direction, nil);
							end
						end
					end
				end			
			end
		end
	end		
end

function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");


	local args = args or {};
	local iW, iH = Map.GetGridSize();
	local res = Map.GetCustomOption(13)
	if res == 9 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	local setback = Map.GetCustomOption(16)-1;

	local setforward = Map.GetCustomOption(17)-1;

	local setrange = setforward + setback;

	print("Moveback: ", setback);

	self.resource_setting = res;
	self.method = 3;


	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo()
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);


	iNumTeams = table.maxn(self.teams_with_major_civs);
	print("-"); print("Teams:", iNumTeams);


	local team_setting = Map.GetCustomOption(15)


	if iNumTeams == 2 and team_setting == 1 then
		print("-"); print("Number of Teams present is two! Using custom team start placement for West vs East."); print("-");
		


		local shuffled_team_list = GetShuffledCopyOfTable(self.teams_with_major_civs)
		teamWestID = self.teams_with_major_civs[1];
		teamEastID = self.teams_with_major_civs[2];
		iNumCivsInWest = self.number_civs_per_team[teamWestID];
		iNumCivsInEast = self.number_civs_per_team[teamEastID];


		self.inhabited_WestX = 0 + setforward;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = (math.floor(iW / 2)) - setrange;
		self.inhabited_Height = iH;


		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)

		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount};

		self:DivideIntoRegions(iNumCivsInWest, fert_table, rect_table)


		self.inhabited_WestX = (math.floor(iW / 2)) + setback;
		self.inhabited_SouthY = 0;
		self.inhabited_Width = math.floor(iW / 2) - setrange;
		self.inhabited_Height = iH;


		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)

		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount};

		self:DivideIntoRegions(iNumCivsInEast, fert_table, rect_table)



	else	
		print("-"); print("Dividing the map at random."); print("-");
		self.method = 2;	
		local best_areas = {};
		local globalFertilityOfLands = {};


		local iGlobalFertilityOfLands = 0;
		local iNumLandPlots = 0;
		local iNumLandAreas = 0;
		local land_area_IDs = {};
		local land_area_plots = {};
		local land_area_fert = {};

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				if not plot:IsWater() then
					iNumLandPlots = iNumLandPlots + 1;
					local iArea = plot:GetArea();
					local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true);
					iGlobalFertilityOfLands = iGlobalFertilityOfLands + plotFertility;

					if TestMembership(land_area_IDs, iArea) == false then
						iNumLandAreas = iNumLandAreas + 1;
						table.insert(land_area_IDs, iArea);
						land_area_plots[iArea] = 1;
						land_area_fert[iArea] = plotFertility;
					else
						land_area_plots[iArea] = land_area_plots[iArea] + 1;
						land_area_fert[iArea] = land_area_fert[iArea] + plotFertility;
					end
				end
			end
		end
		




		local interim_table = {};
		for loop_index, data_entry in pairs(land_area_fert) do
			table.insert(interim_table, data_entry);
		end

		table.sort(interim_table);

		local iNumRelevantLandAreas = math.min(iNumLandAreas, self.iNumCivs);



		local best_areas = {};

		local end_of_interim_table = table.maxn(interim_table);

		for areaTestLoop = end_of_interim_table, (end_of_interim_table - iNumRelevantLandAreas + 1), -1 do
			for loop_index, AreaID in ipairs(land_area_IDs) do
				if interim_table[areaTestLoop] == land_area_fert[land_area_IDs[loop_index]] then
					table.insert(best_areas, AreaID);
					table.remove(land_area_IDs, landLoop);
					break
				end
			end
		end


		local inhabitedAreaIDs = {};
		local numberOfCivsPerArea = table.fill(0, iNumRelevantLandAreas);
		for civToAssign = 1, self.iNumCivs do
			local bestRemainingArea;
			local bestRemainingFertility = 0;
			local bestAreaTableIndex;


			for area_loop, AreaID in ipairs(best_areas) do
				local thisLandmassCurrentFertility = land_area_fert[AreaID] / (1 + numberOfCivsPerArea[area_loop]);
				if thisLandmassCurrentFertility > bestRemainingFertility then
					bestRemainingArea = AreaID;
					bestRemainingFertility = thisLandmassCurrentFertility;
					bestAreaTableIndex = area_loop;
				end
			end

			numberOfCivsPerArea[bestAreaTableIndex] = numberOfCivsPerArea[bestAreaTableIndex] + 1;
			if TestMembership(inhabitedAreaIDs, bestRemainingArea) == false then
				table.insert(inhabitedAreaIDs, bestRemainingArea);
			end
		end
				



		local iNumInhabitedLandmasses = table.maxn(inhabitedAreaIDs);
		for loop, currentLandmassID in ipairs(inhabitedAreaIDs) do

			local landmass_data = ObtainLandmassBoundaries(currentLandmassID);
			local iWestX = landmass_data[1];
			local iSouthY = landmass_data[2];
			local iEastX = landmass_data[3];
			local iNorthY = landmass_data[4];
			local iWidth = landmass_data[5];
			local iHeight = landmass_data[6];
			local wrapsX = landmass_data[7];
			local wrapsY = landmass_data[8];



			local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(currentLandmassID, 
		  	                                         iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);

			local rect_table = {iWestX, iSouthY, iWidth, iHeight, currentLandmassID, fertCount, plotCount};

			iNumCivsOnThisLandmass = numberOfCivsPerArea[loop];
			if iNumCivsOnThisLandmass > 0 and iNumCivsOnThisLandmass <= 22 then
				self:DivideIntoRegions(iNumCivsOnThisLandmass, fert_table, rect_table)
			else
				print("Invalid number of civs assigned to a landmass: ", iNumCivsOnThisLandmass);
			end
		end
	end
end

function AssignStartingPlots:BalanceAndAssign()




	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		print("Normalize Region: ", region_number);
		self:NormalizeStartLocation(region_number)
	end


	local team_setting = Map.GetCustomOption(15)
	if iNumTeams == 2 and team_setting == 1 then

		print("-"); print("This is a team game with two teams! Place one team in West, other in East."); print("-");
		local playerList, westList, eastList = {}, {}, {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
			local player = Players[player_ID];
			local team_ID = player:GetTeam()
			if team_ID == teamWestID then
				print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the North.");
				table.insert(westList, player_ID);
			elseif team_ID == teamEastID then
				print("Player #", player_ID, "belongs to Team #", team_ID, "and will be placed in the South.");
				table.insert(eastList, player_ID);
			else
				print("* ERROR * - Player #", player_ID, "belongs to Team #", team_ID, "which is neither West nor East!");
			end
		end
		

		if table.maxn(westList) ~= iNumCivsInWest then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on West team and number of civs assigned to west locations.");
		end
		if table.maxn(eastList) ~= iNumCivsInEast then
			print("-"); print("*** ERROR! *** . . . Mismatch between number of Civs on East team and number of civs assigned to east locations.");
		end
		
		local westListShuffled = GetShuffledCopyOfTable(westList)
		local eastListShuffled = GetShuffledCopyOfTable(eastList)
		for region_number, player_ID in ipairs(westListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end
		for loop, player_ID in ipairs(eastListShuffled) do
			local x = self.startingPlots[loop + iNumCivsInWest][1];
			local y = self.startingPlots[loop + iNumCivsInWest][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end
	else
		print("-"); print("This game does not have specific start zone assignments."); print("-");
		local playerList = {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for region_number, player_ID in ipairs(playerListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end



		if self.bTeamGame == true and team_setting ~= 2 then
			print("However, this IS a team game, so we will try to group team members together."); print("-");
			self:NormalizeTeamLocations()
		end
	end
end



function AssignStartingPlots:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)

	

	if 1<2 then
		return false
	end

	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y)
	local area = plot:GetArea()
	

	if x < 1 or x >= iW - 1 or y < 1 or y >= iH - 1 then
		return false
	end

	
	if area ~= area_ID and area_ID ~= -1 then
		return false
	end
	local plotType = plot:GetPlotType()
	if plotType == PlotTypes.PLOT_OCEAN or plotType == PlotTypes.PLOT_MOUNTAIN then
		return false
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_SNOW then
		return false
	end
	local plotIndex = y * iW + x + 1;
	if self.cityStateData[plotIndex] > 0 and force_it == false then
		return false
	end
	local plotIndex = y * iW + x + 1;
	if self.playerCollisionData[plotIndex] == true and ignore_collisions == false then

		return false
	end
	return true
end

function SetDivide()

	local SplitOps = Map.GetCustomOption(18);
	local iW, iH = Map.GetGridSize();

	if SplitOps == 2 then



		for y = 0, 2 do
			for x = math.floor(iW / 2) - 4, math.floor(iW / 2) + 3 do
				local plot = Map.GetPlot(x, y)
				

				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
				elseif plot:IsLake() then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				end
			end
		end


		for y = (iH-3), (iH-1) do
			for x = math.floor(iW / 2) - 4, math.floor(iW / 2) + 3 do
				local plot = Map.GetPlot(x, y)
				

				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
				elseif plot:IsLake() then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				end
			end
		end


		for y = math.floor(iH / 2) - 1, math.floor(iH / 2) + 1 do
			for x = math.floor(iW / 2) - 4, math.floor(iW / 2) + 3 do
				local plot = Map.GetPlot(x, y)
				

				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
				elseif plot:IsLake() then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				end
			end
		end
	elseif SplitOps == 3 then

		

		for y = 0, iH - 1 do
			for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
				local plot = Map.GetPlot(x, y)
				plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
				plot:SetFeatureType(FeatureTypes.FEATURE_MARSH, -1);
			end
		end
	elseif SplitOps == 4 then



		for y = 0, iH - 1 do
			for x = math.floor(iW / 2) - 3, math.floor(iW / 2) + 2 do
				local plot = Map.GetPlot(x, y)
				plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
				plot:SetTerrainType(TerrainTypes.TERRAIN_TUNDRA, false, false);
			end
		end

		for y = 0, iH - 1 do
			for x = math.floor(iW / 2) - 2, math.floor(iW / 2) + 1 do
				local plot = Map.GetPlot(x, y)
				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, false);
				elseif plot:IsLake() then
					plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
				end
				plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
				plot:SetTerrainType(TerrainTypes.TERRAIN_SNOW, false, false);
			end
		end
	end
end


function getMirroredPlot(plot)
	local iW, iH = Map.GetGridSize();
	local x = iW - plot:GetX() - 1;
	local y = iH - plot:GetY() - 1;
	print("mirror x/y=",x,y);
	local mirrorPlot = Map.GetPlot(x, y);
	return mirrorPlot;
end

function isValidPlayer(pPlayer)
	return  pPlayer ~= nil and pPlayer:GetStartingPlot() ~= nil and pPlayer:IsAlive();
end

function StartPlotSystem()

	local MirroredMapPlease;
	MirroredMapPlease = 1;
	local res = Map.GetCustomOption(13)
	if res == 9 then
		res = 1 + Map.Rand(3, "Random Resources Option - Lua");
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()

	print("Dividing the map in to Regions.");
	print("Resource Setting: ", res);
	local args = {
		resources = res,
		};
	start_plot_database:GenerateRegions()


	print("Setting divide section of map");
	SetDivide()

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations()
	
	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign()




	print("Placing Natural Wonders.");
	local wonders = Map.GetCustomOption(7)
	if wonders == 14 then
		wonders = Map.Rand(13, "Number of Wonders To Spawn - Lua");
	else
		wonders = wonders - 1;
	end

	print("########## Wonders ##########");
	print("Natural Wonders To Place: ", wonders);

	local wonderargs = {
		wonderamt = wonders,
	};

	start_plot_database:PlaceNaturalWonders(wonderargs)

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()
	
	if Map.GetCustomOption(18) == 4 then
		local iW, iH = Map.GetGridSize()
		for x = math.floor(iW / 2) - 2, (iW / 2) + 1 do
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y)
				plot:SetWOfRiver(false,FlowDirectionTypes.NO_FLOWDIRECTION)
				plot:SetNWOfRiver(false,FlowDirectionTypes.NO_FLOWDIRECTION)
				plot:SetNEOfRiver(false,FlowDirectionTypes.NO_FLOWDIRECTION)
			end
		end
	end
	if MirroredMapPlease == 1 then





	local iW, iH = Map.GetGridSize()
	print("iW/iH=",iW, iH)
	for x = 0, iW * 0.5 do
		for y = 0, iH - 1 do
			if( iW-x-y%2 ~= x ) then
				print("x/y=",x,y);
				local plot = Map.GetPlot(x, y);
				local mirrorPlot = getMirroredPlot(plot);
				local plotType = plot:GetPlotType();
				local terrainType = plot:GetTerrainType();
				local featureType = plot:GetFeatureType();
				local improvementType = plot:GetImprovementType();
				local resourceType = plot:GetResourceType(-1)
				mirrorPlot:SetPlotType(plotType,false,false)
				mirrorPlot:SetTerrainType(terrainType,false,false)
				mirrorPlot:SetFeatureType(featureType)
				mirrorPlot:SetResourceType(resourceType,plot:GetNumResource())
				mirrorPlot:SetImprovementType(improvementType)
			end
		end
	end



	for x = math.floor(iW / 2)+1, iW - 1 do
		for y = 0, iH - 1 do
			local plot = Map.GetPlot(x, y)
			plot:SetWOfRiver(false,FlowDirectionTypes.NO_FLOWDIRECTION)
			plot:SetNWOfRiver(false,FlowDirectionTypes.NO_FLOWDIRECTION)
			plot:SetNEOfRiver(false,FlowDirectionTypes.NO_FLOWDIRECTION)
		end
	end
	for y = 0, iH - 1 do
		for x = 0, iW * 0.5 do
		    if ( x < (iH/2 + 1) ) then
            	local plot = Map.GetPlot(x, y);
				if ( plot:IsWOfRiver() ) then
					local mirrorPlot = getMirroredPlot(plot);
					mirrorPlot = Map.PlotDirection(mirrorPlot:GetX(), mirrorPlot:GetY(), DirectionTypes.DIRECTION_WEST);
					local dir = FlowDirectionTypes.FLOWDIRECTION_NORTH;
					if( plot:GetRiverEFlowDirection() == FlowDirectionTypes.FLOWDIRECTION_NORTH ) then
						dir = FlowDirectionTypes.FLOWDIRECTION_SOUTH;
					end
					mirrorPlot:SetWOfRiver(true, dir);
				end
				if ( plot:IsNWOfRiver() ) then
					local mirrorPlot = getMirroredPlot(plot);
					mirrorPlot = Map.PlotDirection(mirrorPlot:GetX(), mirrorPlot:GetY(), DirectionTypes.DIRECTION_NORTHWEST);
					local dir = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
					if( plot:GetRiverSEFlowDirection() == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST) then
						dir = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
					end;
					mirrorPlot:SetNWOfRiver(true, dir);
				end
				if ( plot:IsNEOfRiver() ) then
					local mirrorPlot = getMirroredPlot(plot);
					mirrorPlot = Map.PlotDirection(mirrorPlot:GetX(), mirrorPlot:GetY(), DirectionTypes.DIRECTION_NORTHEAST);
					local dir = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
					if( plot:GetRiverSWFlowDirection() == FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST) then
						dir = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
					end;
					mirrorPlot:SetNEOfRiver(true, dir);
				end
			end
		end
	end

	local playerStartPlot;
	local searchMode = 0;
	for i = 0, GameDefines.MAX_MAJOR_CIVS + GameDefines.MAX_MINOR_CIVS - 1 do
		local player = Players[i];
		if (isValidPlayer(player)) then
			if player:IsEverAlive() then
				if( searchMode == 0 ) then
					searchMode = 1;
					player = Players[i];
					playerStartPlot = player:GetStartingPlot();
				else
					searchMode = 0;
					player = Players[i];
					player:SetStartingPlot(getMirroredPlot(playerStartPlot));
				end
			end
		end
	end
		Map:RecalculateAreas();
		Game.SetOption(1, true);
	
	end
end
