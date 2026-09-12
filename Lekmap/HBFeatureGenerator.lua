








include("HBMapmakerUtilities");


FeatureGenerator = {};

function FeatureGenerator.Create(args, defaults)








	local grassMoist = Map.GetCustomOption(8);

	local args = args or {};
	local defaults = defaults or {iJunglePercent = 42, iForestPercent = 20, fMarshPercent = 8};
	local rainfall = args.rainfall or 2;
	local jungle_grain = args.jungle_grain or 5;
	local forest_grain = args.forest_grain or 6;
	local clump_grain = args.clump_grain or 10;
	local iJungleChange = args.iJungleChange or 20;
	local iForestChange = args.iForestChange or 7;
	local iClumpChange = args.iClumpChange or 5;
	local iJungleFactor = args.iJungleFactor or 7;
	local iAridFactor = args.iAridFactor or 6;
	local iWetFactor = args.iWetFactor or 2;
	local fMarshChange = args.fMarshChange or 1.5;
	local fOasisChange = args.fOasisChange or 1.5;
	local fracXExp = args.fracXExp or -1;
	local fracYExp = args.fracYExp or -1;
	

	local iJunglePercent = args.iJunglePercent or defaults.iJunglePercent;

	if grassMoist == 1 then
		iJunglePercent = iJunglePercent - 5;
	elseif grassMoist == 3 then
		iJunglePercent = iJunglePercent + 5;
	end

	local iForestPercent = args.iForestPercent or defaults.iForestPercent;
	local iClumpHeight = args.iClumpHeight or 75;
	local fMarshPercent = args.fMarshPercent or defaults.fMarshPercent;
	local iOasisPercent = args.iOasisPercent or 25;








	if rainfall == 1 then
		iJunglePercent = iJunglePercent - iJungleChange;
		iJungleFactor = iAridFactor;
		iForestPercent = iForestPercent - iForestChange;
		iClumpHeight = iClumpHeight - iClumpChange;
		fMarshPercent = fMarshPercent / fMarshChange;
		iOasisPercent = iOasisPercent / fOasisChange;
	elseif rainfall == 3 then
		iJunglePercent = iJunglePercent + iJungleChange;
		iJungleFactor = iWetFactor;
		iForestPercent = iForestPercent + iForestChange;
		iClumpHeight = iClumpHeight + iClumpChange;
		fMarshPercent = fMarshPercent * fMarshChange;
		iOasisPercent = iOasisPercent * fOasisChange;
	else
	end












	local gridWidth, gridHeight = Map.GetGridSize();
	local world_info = GameInfo.Worlds[Map.GetWorldSize()];
	jungle_grain = jungle_grain + world_info.FeatureGrainChange;
	forest_grain = forest_grain + world_info.FeatureGrainChange;


	local instance = {
	

		__initFractals		= FeatureGenerator.__initFractals,
		__initFeatureTypes	= FeatureGenerator.__initFeatureTypes,
		AddFeatures			= FeatureGenerator.AddFeatures,
		GetLatitudeAtPlot	= FeatureGenerator.GetLatitudeAtPlot,
		AddFeaturesAtPlot	= FeatureGenerator.AddFeaturesAtPlot,
		AddOasisAtPlot		= FeatureGenerator.AddOasisAtPlot,
		AddIceAtPlot		= FeatureGenerator.AddIceAtPlot,
		AddMarshAtPlot		= FeatureGenerator.AddMarshAtPlot,
		AddJunglesAtPlot	= FeatureGenerator.AddJunglesAtPlot,
		AddForestsAtPlot	= FeatureGenerator.AddForestsAtPlot,
		AddAtolls			= FeatureGenerator.AddAtolls,
		AdjustTerrainTypes	= FeatureGenerator.AdjustTerrainTypes,
		

		iGridW = gridWidth,
		iGridH = gridHeight,
		
		iJunglePercent = iJunglePercent,
		iJungleFactor = iJungleFactor,
		iForestPercent = iForestPercent,
		iClumpHeight = iClumpHeight,
		fMarshPercent = fMarshPercent,
		iOasisPercent = iOasisPercent,
	
		jungle_grain = jungle_grain,
		forest_grain = forest_grain,
		clump_grain = clump_grain,
		
		fractalFlags = Map.GetFractalFlags(),
		fracXExp = fracXExp,
		fracYExp = fracYExp,
	};


	instance:__initFractals()
	instance:__initFeatureTypes()
	
	return instance;
end

function FeatureGenerator:__initFractals()
	local width = self.iGridW;
	local height = self.iGridH;
	

	self.jungles		= Fractal.Create(width, height, self.jungle_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.forests		= Fractal.Create(width, height, self.forest_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.forestclumps	= Fractal.Create(width, height, self.clump_grain, self.fractalFlags, self.fracXExp, self.fracYExp);
	self.marsh			= Fractal.Create(width, height, 4, self.fractalFlags, self.fracXExp, self.fracYExp);
	

	self.iJungleBottom	= self.jungles:GetHeight((100 - self.iJunglePercent)/2)
	self.iJungleTop		= self.jungles:GetHeight((100 + self.iJunglePercent)/2)
	self.iJungleRange	= (self.iJungleTop - self.iJungleBottom) * self.iJungleFactor;
	self.iForestLevel	= self.forests:GetHeight(100 - self.iForestPercent)
	self.iClumpLevel	= self.forestclumps:GetHeight(self.iClumpHeight)
	self.iMarshLevel	= self.marsh:GetHeight(100 - self.fMarshPercent)
end

function FeatureGenerator:__initFeatureTypes()

	self.featureFloodPlains = FeatureTypes.FEATURE_FLOOD_PLAINS;
	self.featureIce = FeatureTypes.FEATURE_ICE;
	self.featureJungle = FeatureTypes.FEATURE_JUNGLE;
	self.featureForest = FeatureTypes.FEATURE_FOREST;
	self.featureOasis = FeatureTypes.FEATURE_OASIS;
	self.featureMarsh = FeatureTypes.FEATURE_MARSH;
	
	self.terrainIce = TerrainTypes.TERRAIN_SNOW;
	self.terrainTundra = TerrainTypes.TERRAIN_TUNDRA;
	self.terrainPlains = TerrainTypes.TERRAIN_PLAINS;
end

function FeatureGenerator:AddFeatures(allow_mountains_on_coast)
	local flag = allow_mountains_on_coast or false;







	if allow_mountains_on_coast == false then
		for x = 0, self.iGridW - 1 do
			for y = 0, self.iGridH - 1 do
				local plot = Map.GetPlot(x, y)
				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					if plot:IsCoastalLand() then
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
					end
				end
			end
		end


	end
	
	self:AddAtolls();
	

	for y = 0, self.iGridH - 1, 1 do
		for x = 0, self.iGridW - 1, 1 do
			self:AddFeaturesAtPlot(x, y);
		end
	end
	
	self:AdjustTerrainTypes();
end

function FeatureGenerator:GetLatitudeAtPlot(iX, iY)



	return math.abs((self.iGridH/2) - iY)/(self.iGridH/2);
end

function FeatureGenerator:AddFeaturesAtPlot(iX, iY)

	local lat = self:GetLatitudeAtPlot(iX, iY);
	local plot = Map.GetPlot(iX, iY);

	if plot:CanHaveFeature(self.featureFloodPlains) then

		plot:SetFeatureType(self.featureFloodPlains, -1)
	end
	
	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddOasisAtPlot(plot, iX, iY, lat);
	end

	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddIceAtPlot(plot, iX, iY, lat);
	end

	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddMarshAtPlot(plot, iX, iY, lat);
	end
		
	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddJunglesAtPlot(plot, iX, iY, lat);
	end
	
	if (plot:GetFeatureType() == FeatureTypes.NO_FEATURE) then
		self:AddForestsAtPlot(plot, iX, iY, lat);
	end
		
end

function FeatureGenerator:AddOasisAtPlot(plot, iX, iY, lat)
	if(plot:CanHaveFeature(self.featureOasis)) then
		if Map.Rand(100, "Add Oasis Lua") <= self.iOasisPercent then
			plot:SetFeatureType(self.featureOasis, -1);
		end
	end
end

function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	if(plot:CanHaveFeature(self.featureIce)) then
		if Map.IsWrapX() and (iY == 0 or iY == self.iGridH - 1) then
			plot:SetFeatureType(self.featureIce, -1)

		else
			local rand = Map.Rand(200, "Add Ice Lua")/100.0;

			if(rand < 8 * (lat - 0.875)) then
				plot:SetFeatureType(self.featureIce, -1);
			elseif(rand < 4 * (lat - 0.75)) then
				plot:SetFeatureType(self.featureIce, -1);
			end
		end
	end
end

function FeatureGenerator:AddMarshAtPlot(plot, iX, iY, lat)
	local marsh_height = self.marsh:GetHeight(iX, iY)
	if(marsh_height >= self.iMarshLevel) then
		if(plot:CanHaveFeature(self.featureMarsh)) then
			plot:SetFeatureType(self.featureMarsh, -1)
		end
	end
end

function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY, lat)
	local jungle_height = self.jungles:GetHeight(iX, iY);
	local climate_info = GameInfo.Climates[Map.GetClimate()];
	if jungle_height <= self.iJungleTop and jungle_height >= self.iJungleBottom + (self.iJungleRange * lat) then
		if(plot:CanHaveFeature(self.featureJungle)) then
			plot:SetFeatureType(self.featureJungle, -1);
		end
	end
end

function FeatureGenerator:AddForestsAtPlot(plot, iX, iY, lat)
	if (self.forests:GetHeight(iX, iY) >= self.iForestLevel) or (self.forestclumps:GetHeight(iX, iY) >= self.iClumpLevel) then
		if plot:CanHaveFeature(self.featureForest) then
			plot:SetFeatureType(self.featureForest, -1)
		end
	end
end

function FeatureGenerator:AdjustTerrainTypes()



	local width = self.iGridW - 1;
	local height = self.iGridH - 1;
	
	for y = 0, height do
		for x = 0, width do
			local plot = Map.GetPlot(x, y);
			
			if (plot:GetFeatureType() == self.featureJungle) then
				plot:SetTerrainType(self.terrainPlains, false, true)
			elseif (plot:IsRiver()) then
				local terrainType = plot:GetTerrainType();
				if (terrainType == self.terrainTundra) then
					plot:SetTerrainType(self.terrainPlains, false, true)
				elseif (terrainType == self.terrainIce) then
					plot:SetTerrainType(self.terrainTundra, false, true)					
				end
			end
		end
	end
end

function FeatureGenerator:AddAtolls()


	local iW, iH = Map.GetGridSize()
	local biggest_ocean = Map.FindBiggestArea(true)
	local iNumBiggestOceanPlots = 0;
	if biggest_ocean ~= nil then
		iNumBiggestOceanPlots = biggest_ocean:GetNumTiles()
	end
	if iNumBiggestOceanPlots <= (iW * iH) / 4 then
		return
	end
	

	local iNumAtollsPlaced = 0;
	local direction_types = {
		DirectionTypes.DIRECTION_NORTHEAST,
		DirectionTypes.DIRECTION_EAST,
		DirectionTypes.DIRECTION_SOUTHEAST,
		DirectionTypes.DIRECTION_SOUTHWEST,
		DirectionTypes.DIRECTION_WEST,
		DirectionTypes.DIRECTION_NORTHWEST
	};
	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 2,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 7,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 9,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 12,
	};
	local atoll_target = worldsizes[Map.GetWorldSize()];
	local atoll_number = atoll_target + Map.Rand(atoll_target, "Number of Atolls to place - LUA");
	local feature_atoll;
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == "FEATURE_ATOLL" then
			feature_atoll = thisFeature.ID;
		end
	end


	local temp_one_tile_island_list, temp_alpha_list, temp_beta_list = {}, {}, {};
	local temp_gamma_list, temp_delta_list, temp_epsilon_list = {}, {}, {};
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y)
			local plotType = plot:GetPlotType()
			if plotType == PlotTypes.PLOT_OCEAN then
				local featureType = plot:GetFeatureType()
				if featureType ~= FeatureTypes.FEATURE_ICE then
					if not plot:IsLake() then
						local terrainType = plot:GetTerrainType()
						if terrainType == TerrainTypes.TERRAIN_COAST then
							if plot:IsAdjacentToLand() then

								local iNumLandAdjacent, biggest_adj_area = 0, 0;
								local bPlotValid = true;
								for loop, direction in ipairs(direction_types) do
									local adjPlot = Map.PlotDirection(x, y, direction)
									if adjPlot ~= nil then
										local adjPlotType = adjPlot:GetPlotType()
										if adjPlotType ~= PlotTypes.PLOT_OCEAN then
											iNumLandAdjacent = iNumLandAdjacent + 1;

											local adjTerrainType = adjPlot:GetTerrainType()
											if adjTerrainType == TerrainTypes.TERRAIN_TUNDRA or adjTerrainType == TerrainTypes.TERRAIN_SNOW then
												bPlotValid = false;
											end
											local adjFeatureType = adjPlot:GetFeatureType()
											if adjFeatureType == FeatureTypes.FEATURE_ICE then
												bPlotValid = false;
											end
											if adjPlotType == PlotTypes.PLOT_LAND or adjPlotType == PlotTypes.PLOT_HILLS then
												local iArea = adjPlot:GetArea()
												local adjArea = Map.GetArea(iArea)
												local iNumAreaPlots = adjArea:GetNumTiles()
												if iNumAreaPlots > biggest_adj_area then
													biggest_adj_area = iNumAreaPlots;
												end
											end
										end
									end
								end

								if iNumLandAdjacent == 1 and bPlotValid == true then
									if biggest_adj_area >= 76 then

									elseif biggest_adj_area >= 41 then
										table.insert(temp_epsilon_list, i);
									elseif biggest_adj_area >= 17 then
										table.insert(temp_delta_list, i);
									elseif biggest_adj_area >= 8 then
										table.insert(temp_gamma_list, i);
									elseif biggest_adj_area >= 3 then
										table.insert(temp_beta_list, i);
									elseif biggest_adj_area >= 1 then
										table.insert(temp_alpha_list, i);


									end
								end
							end
						end
					end
				end
			end
		end
	end
	local alpha_list = GetShuffledCopyOfTable(temp_alpha_list)
	local beta_list = GetShuffledCopyOfTable(temp_beta_list)
	local gamma_list = GetShuffledCopyOfTable(temp_gamma_list)
	local delta_list = GetShuffledCopyOfTable(temp_delta_list)
	local epsilon_list = GetShuffledCopyOfTable(temp_epsilon_list)


	local max_alpha = math.ceil(table.maxn(alpha_list) / 4);
	local max_beta = math.ceil(table.maxn(beta_list) / 5);
	local max_gamma = math.ceil(table.maxn(gamma_list) / 4);
	local max_delta = math.ceil(table.maxn(delta_list) / 3);
	local max_epsilon = math.ceil(table.maxn(epsilon_list) / 4);
	

	local plotIndex;
	local i_alpha, i_beta, i_gamma, i_delta, i_epsilon = 1, 1, 1, 1, 1;
	for loop = 1, atoll_number do
		local able_to_proceed = true;
		local diceroll = 1 + Map.Rand(100, "Atoll Placement Type - LUA");
		if diceroll <= 40 and max_alpha > 0 then
			plotIndex = alpha_list[i_alpha];
			i_alpha = i_alpha + 1;
			max_alpha = max_alpha - 1;

		elseif diceroll <= 65 then
			if max_beta > 0 then
				plotIndex = beta_list[i_beta];
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;

			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;

			else

				able_to_proceed = false;
			end
		elseif diceroll <= 80 then
			if max_gamma > 0 then
				plotIndex = gamma_list[i_gamma];
				i_gamma = i_gamma + 1;
				max_gamma = max_gamma - 1;

			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta];
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;

			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;

			else

				able_to_proceed = false;
			end
		elseif diceroll <= 90 then
			if max_delta > 0 then
				plotIndex = delta_list[i_delta];
				i_delta = i_delta + 1;
				max_delta = max_delta - 1;

			elseif max_gamma > 0 then
				plotIndex = gamma_list[i_gamma];
				i_gamma = i_gamma + 1;
				max_gamma = max_gamma - 1;

			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta];
				i_beta = i_beta + 1;
				max_beta = max_beta - 1;

			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;

			else

				able_to_proceed = false;
			end
		else
			if max_epsilon > 0 then
				plotIndex = epsilon_list[i_epsilon];
				i_epsilon = i_epsilon + 1;
				max_epsilon = max_epsilon - 1;

			elseif max_delta > 0 then
				plotIndex = delta_list[i_delta];
				i_delta = i_delta + 1;
				max_delta = max_delta - 1;

			elseif max_gamma > 0 then
				plotIndex = gamma_list[i_gamma];
				i_gamma = i_gamma + 1;
				max_gamma = max_gamma - 1;

			elseif max_beta > 0 then
				plotIndex = beta_list[i_beta];

				i_beta = i_beta + 1;
				max_beta = max_beta - 1;
			elseif max_alpha > 0 then
				plotIndex = alpha_list[i_alpha];
				i_alpha = i_alpha + 1;
				max_alpha = max_alpha - 1;

			else

				able_to_proceed = false;
			end
		end
		if able_to_proceed and plotIndex ~= nil then
			local x = (plotIndex - 1) % iW;
			local y = (plotIndex - x - 1) / iW;
			local plot = Map.GetPlot(x, y)
			plot:SetFeatureType(feature_atoll, -1);
			iNumAtollsPlaced = iNumAtollsPlaced + 1;


		end
	end
	









end
