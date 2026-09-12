








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
		Name = "Lekmap: Oval (Bonus Map)",
		Description = "A map script made for Lekmod based on HB's Mapscript v8.1. Oval",
		IsAdvancedMap = false,
		IconIndex = 15,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create({
			[11] = {
				DefaultValue = 8,
			},
			[12] = {
				DefaultValue = 13,
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
			{
				Name = "Inland Sea Spawns",
				Values = {
					"Allowed",
					"Not Allowed for Coastal Civs",
				},
				DefaultValue = 2,
				SortPriority = -83,
			},
		}),
	};
end

GetMapInitData = GetCustomSizeMapInitData;


function MultilayeredFractal:GeneratePlotsByRegion()




	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_POLAR = true};

	local sea_level = Map.GetCustomOption(4)
	if sea_level == 4 then
		sea_level = 1 + Map.Rand(3, "Random Sea Level - Lua");
	end
	local world_age = Map.GetCustomOption(1)
	if world_age == 4 then
		world_age = 1 + Map.Rand(3, "Random World Age - Lua");
	end
	local axis_list = {0.87, 0.81, 0.75};
	local axis_multiplier = axis_list[sea_level];
	local cohesion_list = {0.41, 0.38, 0.35};
	local cohesion_multiplier = cohesion_list[sea_level];


	self.wholeworldPlotTypes = table.fill(PlotTypes.PLOT_OCEAN, iW * iH);


	local centerX = iW / 2;
	local centerY = iH / 2;
	local majorAxis = centerX * axis_multiplier;
	local minorAxis = centerY * axis_multiplier;
	local majorAxisSquared = majorAxis * majorAxis;
	local minorAxisSquared = minorAxis * minorAxis;
	for x = 0, iW - 1 do
		for y = 0, iH - 1 do
			local deltaX = x - centerX;
			local deltaY = y - centerY;
			local deltaXSquared = deltaX * deltaX;
			local deltaYSquared = deltaY * deltaY;
			local d = deltaXSquared/majorAxisSquared + deltaYSquared/minorAxisSquared;
			if d <= 1 then
				local i = y * iW + x + 1;
				self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
			end
		end
	end


	local baysFrac = Fractal.Create(iW, iH, 3, fracFlags, -1, -1);
	local iBaysThreshold = baysFrac:GetHeight(82);
	local centerX = iW / 2;
	local centerY = iH / 2;
	local majorAxis = centerX * cohesion_multiplier;
	local minorAxis = centerY * cohesion_multiplier;
	local majorAxisSquared = majorAxis * majorAxis;
	local minorAxisSquared = minorAxis * minorAxis;
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local deltaX = x - centerX;
			local deltaY = y - centerY;
			local deltaXSquared = deltaX * deltaX;
			local deltaYSquared = deltaY * deltaY;
			local d = deltaXSquared/majorAxisSquared + deltaYSquared/minorAxisSquared;
			if d > 1 then
				local i = y * iW + x + 1;
				local baysVal = baysFrac:GetHeight(x, y);
				if baysVal >= iBaysThreshold then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
				end
			end
		end
	end


	local args = {
		adjust_plates = 1.5,
		world_age = world_age,
	};
	self:ApplyTectonics(args)


	return self.wholeworldPlotTypes
end

function GeneratePlotTypes()
	print("Setting Plot Types (Lua Oval) ...");

	local layered_world = MultilayeredFractal.Create();
	local plot_list = layered_world:GeneratePlotsByRegion();

	SetPlotTypes(plot_list);

	local args = {bExpandCoasts = false};
	GenerateCoasts(args);
end



function GenerateTerrain()
	print("Adding Terrain (Lua Oval) ...");


	local temp = Map.GetCustomOption(2)
	if temp == 4 then
		temp = 1 + Map.Rand(3, "Random Temperature - Lua");
	end

	local args = {temperature = temp};
	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();

	SetTerrainTypes(terrainTypes);
end


include("HBRegionalStartPlotSystem");
