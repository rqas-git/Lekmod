








include("HBMapGenerator");
include("FractalWorld");
include("HBFeatureGenerator");
include("HBTerrainGenerator");
include("IslandMaker");
include("MultilayeredFractal");


include("HBMapOptions");

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "Lekmap: Tiny Islands (Bonus Map)",
		Description = "A map script made for Lekmod based on HB's Mapscript v8.1. Tiny Islands",
		IsAdvancedMap = false,
		IconIndex = 17,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create(),
	};
end

GetMapInitData = GetCustomSizeMapInitData;


function GeneratePlotTypes()
	print("Generating Plot Types (Lua Archipelago) ...");


	local sea = Map.GetCustomOption(4)
	if sea == 4 then
		sea = 1 + Map.Rand(3, "Random Sea Level - Lua");
	end
	local age = Map.GetCustomOption(1)
	if age == 4 then
		age = 1 + Map.Rand(3, "Random World Age - Lua");
	end

	local fractal_world = FractalWorld.Create();
	fractal_world:InitFractal{
		continent_grain = 4};

	local args = {
		sea_level = sea,
		world_age = age,
		sea_level_low = 73,
		sea_level_normal = 78,
		sea_level_high = 82,
		extra_mountains = 15,
		adjust_plates = 2.5,
		tectonic_islands = true
		}
	local plotTypes = fractal_world:GeneratePlotTypes(args);

	SetPlotTypes(plotTypes);

	local args = {expansion_diceroll_table = {10, 4, 4}};
	GenerateCoasts(args);
end

function GenerateTerrain()

	local DesertPercent = 28;


	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local grassMoist = Map.GetCustomOption(8);

	local args = {
			temperature = temp,
			iDesertPercent = DesertPercent,
			iGrassMoist = grassMoist,
			};

	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);


end


function AddFeatures()


	local rain = Map.GetCustomOption(3)
	if rain == 4 then
		rain = 1 + Map.Rand(3, "Random Rainfall - Lua");
	end

	local args = {rainfall = rain}
	local featuregen = FeatureGenerator.Create(args);


	featuregen:AddFeatures(false);
end



function StartPlotSystem()

	local RegionalMethod = 3;


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


	Map.ChangeAIMapHint(1+4);

end
