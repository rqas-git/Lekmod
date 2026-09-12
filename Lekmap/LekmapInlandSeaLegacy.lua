








include("HBMapGenerator");
include("HBFractalWorld");
include("HBFeatureGenerator");
include("HBTerrainGenerator");
include("IslandMaker");
include("MultilayeredFractal");


include("HBMapOptions");

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "Lekmap: Inland Sea (Bonus Map)",
		Description = "A map script made for Lekmod based of HB's Mapscript v8.1. Inland Sea",
		IsAdvancedMap = false,
		IconIndex = 12,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create({
			[11] = {
				Values = LekmapOptions.NumberValues(24, 104, 2),
				DefaultValue = 9,
			},
			[12] = {
				Values = LekmapOptions.NumberValues(16, 70, 2),
				DefaultValue = 5,
			},
		}, {
			{
				Name = "Coastal Spawns",
				Values = {
					"Coastal Civs Only",
					"Random",
					"Random+ (~2 coastals)",
				},
				DefaultValue = 1,
				SortPriority = -85,
			},
			{
				Name = "Coastal Luxes",
				Values = {
					"Guaranteed",
					"Random",
				},
				DefaultValue = 1,
				SortPriority = -84,
			},
		}),
	};
end

function GetMapInitData(worldSize)

	local LandSizeX = 22 + (Map.GetCustomOption(11) * 2);
	local LandSizeY = 14 + (Map.GetCustomOption(12) * 2);

	local worldsizes = {};

	worldsizes = {

		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {LandSizeX, LandSizeY},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {LandSizeX, LandSizeY}
		}

	local grid_size = worldsizes[worldSize];

	local world = GameInfo.Worlds[worldSize];
	if (world ~= nil) then
		return {
			Width = grid_size[1],
			Height = grid_size[2],
			WrapX = false,
		};
	end

end



function MultilayeredFractal:GeneratePlotsByRegion()




	local iW, iH = Map.GetGridSize();


	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * iH);


	local iWestX = math.floor(iW * 0.18) - 1;
	local iEastX = math.ceil(iW * 0.82) - 1;
	local iWidth = iEastX - iWestX;
	local iSouthY = math.floor(iH * 0.28) - 1;
	local iNorthY = math.ceil(iH * 0.72) - 1;
	local iHeight = iNorthY - iSouthY;
	local fracFlags = {FRAC_POLAR = true};
	local grain = 1 + Map.Rand(2, "Inland Sea ocean grain - LUA");
	local seaFrac = Fractal.Create(iWidth, iHeight, grain, fracFlags, -1, -1)
	local seaThreshold = seaFrac:GetHeight(47);

	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local val = seaFrac:GetHeight(region_x, region_y);
			if val >= seaThreshold then
				local x = region_x + iWestX;
				local y = region_y + iSouthY;
				local i = y * iW + x + 1;
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end


	local centerX = (iW / 2) - 1;
	local centerY = (iH / 2) - 1;
	local xAxis = centerX / 2;
	local yAxis = centerY * 0.35;
	local xAxisSquared = xAxis * xAxis;
	local yAxisSquared = yAxis * yAxis;
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local i = y * iW + x + 1;
			local deltaX = x - centerX;
			local deltaY = y - centerY;
			local deltaXSquared = deltaX * deltaX;
			local deltaYSquared = deltaY * deltaY;
			local oval_value = deltaXSquared / xAxisSquared + deltaYSquared / yAxisSquared;
			if oval_value <= 1 then
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end


	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local args = {world_age = world_age};
	self:ApplyTectonics(args)


	return self.wholeworldPlotTypes
end



function GeneratePlotTypes()
	print("Setting Plot Types (Lua Inland Sea) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsIS = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsIS);

	GenerateCoasts();
end

function TerrainGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs((self.iHeight / 2) - iY) / (self.iHeight / 2);
	lat = lat + (128 - self.variation:GetHeight(iX, iY))/(255.0 * 5.0);
	lat = math.clamp(lat, 0, 1);


	local adjusted_lat = 0.07 + 0.52 * lat;

	return adjusted_lat;
end

function GenerateTerrain()
	print("Generating Terrain (Lua Inland Sea) ...");


	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local args = {temperature = temp};
	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
end



function GetRiverValueAtPlot(plot)

	local iW, iH = Map.GetGridSize()
	local x = plot:GetX()
	local y = plot:GetY()
	local random_factor = Map.Rand(3, "River direction random factor - Inland Sea LUA");
	local direction_influence_value = (math.abs(x - (iW / 2)) + math.abs(y - (iH / 2))) * random_factor;

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = ((numPlots - plot:GetPlotType()) * 20) + direction_influence_value;

	local numDirections = DirectionTypes.NUM_DIRECTION_TYPES;
	for direction = 0, numDirections - 1, 1 do
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

function AddRivers()

	local iW, iH = Map.GetGridSize()
	print("Inland Sea - Adding Rivers");
	local passConditions = {
		function(plot)
			return plot:IsHills() or plot:IsMountain();
		end,

		function(plot)
			return (not plot:IsCoastalLand()) and (Map.Rand(8, "MapGenerator AddRivers") == 0);
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

			elseif(not plot:IsWater()) then
				if(passCondition(plot)) then
					if (not Map.FindWater(plot, riverSourceRange, true)) then
						if (not Map.FindWater(plot, seaWaterRange, false)) then
							local inlandCorner = plot:GetInlandCorner();
							if(inlandCorner) then
								local start_x = inlandCorner:GetX()
								local start_y = inlandCorner:GetY()
								local orig_direction;
								if start_y < iH / 2 then
									if start_x < iW / 3 then
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHEAST;
									elseif start_x > iW * 0.66 then
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTHWEST;
									else
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_NORTH;
									end
								else
									if start_x < iW / 3 then
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST;
									elseif start_x > iW * 0.66 then
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST;
									else
										orig_direction = FlowDirectionTypes.FLOWDIRECTION_SOUTH;
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



function FeatureGenerator:GetLatitudeAtPlot(iX, iY)
	local lat = math.abs((self.iGridH/2) - iY)/(self.iGridH/2);


	local adjusted_lat = 0.07 + 0.52 * lat;

	return adjusted_lat
end

function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	return
end

function AddFeatures()
	print("Adding Features (Lua Inland Sea) ...");


	local rain = Map.GetCustomOption(3)
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local args = {rainfall = rain}
	local featuregen = FeatureGenerator.Create(args);


	featuregen:AddFeatures(false);
end



function AssignStartingPlots:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)

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
		print("-"); print("City State candidate plot rejected: collided with already-placed civ or City State at", x, y);
		return false
	end
	return true
end


function StartPlotSystem()

	local RegionalMethod = 1;


	local res = Map.GetCustomOption(13)
	local starts = Map.GetCustomOption(5)





	MixedBias = false;
	if Map.GetCustomOption(15) == 1 then
		OnlyCoastal = true;
		BalancedCoastal = false;
	end
	if Map.GetCustomOption(15) == 2 then
		BalancedCoastal = false;
		OnlyCoastal = false;
	end

	if Map.GetCustomOption(15) == 3 then
		OnlyCoastal = true;
		BalancedCoastal = true;
	end

	if Map.GetCustomOption(16) == 1 then
	CoastLux = true
	end

	if Map.GetCustomOption(16) == 2 then
	CoastLux = false
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()

	print("Dividing the map in to Regions.");

	local args = {
		method = RegionalMethod,
		start_locations = starts,
		resources = res,
		CoastLux = CoastLux,
		NoCoastInland = OnlyCoastal,
		BalancedCoastal = BalancedCoastal,
		MixedBias = MixedBias;
		};
	start_plot_database:GenerateRegions(args)

	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations()

	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign(args)

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
	start_plot_database:PlaceNaturalWonders(wonderargs);
	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()
end
