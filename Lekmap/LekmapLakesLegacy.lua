








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
		Name = "Lekmap: Lakes (Bonus Map)",
		Description = "A map script made for Lekmod based on HB's Mapscript v8.1. Lakes",
		IsAdvancedMap = false,
		IconIndex = 13,
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
				Name = "TXT_KEY_MAP_OPTION_BODIES_OF_WATER",
				Values = {
					{"TXT_KEY_MAP_OPTION_SMALL_LAKES", "TXT_KEY_MAP_OPTION_SMALL_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_LARGE_LAKES", "TXT_KEY_MAP_OPTION_LARGE_LAKES_HELP"},
					{"TXT_KEY_MAP_OPTION_SEAS", "TXT_KEY_MAP_OPTION_SEAS_HELP"},
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 4,
				SortPriority = 1,
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
			WrapX = true,
		};
	end

end



function MultilayeredFractal:GeneratePlotsByRegion()




	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_WRAP_X = true, FRAC_POLAR = true};


	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_LAND, iW * (iH - 2));

	for x = 0, iW - 1 do
		self.wholeworldPlotTypes[x + 1] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[iW + x + 1] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[iW * iH - x] = PlotTypes.PLOT_OCEAN;
		self.wholeworldPlotTypes[iW * (iH - 1) - x] = PlotTypes.PLOT_OCEAN;
	end


	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local userInputLakes = Map.GetCustomOption(15)
	if userInputLakes == 4 then
		userInputLakes = 1 + Map.Rand(3, "Highlands Random Lake Size - Lua");
	end


	local lake_list = {93, 90, 85};
	local lake_grains = {5, 4, 3};
	local lakes = lake_list[userInputLakes];
	local lake_grain = lake_grains[userInputLakes];

	local lakesFrac = Fractal.Create(iW, iH, lake_grain, fracFlags, -1, -1);
	local iLakesThreshold = lakesFrac:GetHeight(lakes);

	for y = 1, iH - 2 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local lakeVal = lakesFrac:GetHeight(x, y);
			if lakeVal >= iLakesThreshold then
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end
	end


	local args = {
		adjust_plates = 1.2,
		world_age = world_age,
	};
	self:ApplyTectonics(args)


	return self.wholeworldPlotTypes
end



function GeneratePlotTypes()
	print("Setting Plot Types (Lua Lakes) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsLakes = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plotsLakes);

	GenerateCoasts();
end

function GenerateTerrain()
	print("Generating Terrain (Lua Lakes) ...");


	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local args = {temperature = temp};
	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
end

function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)

	if(plot:CanHaveFeature(self.featureIce)) then
		if iY < 2 or iY >= self.iGridH - 2 then
			plot:SetFeatureType(self.featureIce, -1)

		else
			local rand = Map.Rand(100, "Add Ice Lua")/100.0;

			if(rand < 8 * (lat - 0.875)) then
				plot:SetFeatureType(self.featureIce, -1);
			elseif(rand < 4 * (lat - 0.75)) then
				plot:SetFeatureType(self.featureIce, -1);
			end
		end
	end
end

function AddFeatures()
	print("Adding Features (Lua Lakes) ...");


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


	if y < 4 or y >= iH - 4 then
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

function StartPlotSystem()

	local RegionalMethod = 1;


	local res = Map.GetCustomOption(13)
	local starts = Map.GetCustomOption(5)





	MixedBias = false;
	BalancedCoastal = false;
	OnlyCoastal = false;
	CoastLux = false;

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
