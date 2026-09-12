








include("HBMapGeneratorRectangular");
include("HBFractalWorld");
include("HBFeatureGeneratorRectangular");
include("HBTerrainGenerator");
include("IslandMaker");
include("MultilayeredFractal");


include("HBMapOptions");

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "Lekmap: Pangaea - Rectangular (v4.1)",
		Description = "A map script made for Lekmod based of HB's Mapscript v8.1. Pangaea - Rectangular",
		IsAdvancedMap = false,
		IconIndex = 0,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create({
			[4] = {
				Values = {
					"Very Low",
					"TXT_KEY_MAP_OPTION_LOW",
					"TXT_KEY_MAP_OPTION_MEDIUM",
					"TXT_KEY_MAP_OPTION_HIGH",
					"Very High",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 3,
			},
			[11] = {
				DefaultValue = 14,
			},
			[12] = {
				DefaultValue = 14,
			},
		}, {
			{
				Name = "Islands",
				Values = {
					"No Islands",
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
					"11",
					"12 - Default",
					"13",
					"14",
					"15",
					"16",
					"17",
					"18",
					"19",
					"20",
					"21",
					"22",
					"23",
					"24",
				},
				DefaultValue = 13,
				SortPriority = -86,
			},
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
			{
				Name = "Lakes",
				Values = {
					"Sparse",
					"Average",
					"Plentiful",
				},
				DefaultValue = 1,
				SortPriority = -82,
			},
		}),
	};
end

function GetMapInitData(worldSize)

	local LandSizeX = 28 + (Map.GetCustomOption(11) * 2);
	local LandSizeY = 18 + (Map.GetCustomOption(12) * 2);

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





RectangularFractalWorld = {};

function RectangularFractalWorld.Create(fracXExp, fracYExp)
	local gridWidth, gridHeight = Map.GetGridSize();
	local landHeight = (gridHeight - math.floor(gridHeight * 0.20));

	local data = {
		InitFractal = FractalWorld.InitFractal,
		ShiftPlotTypes = FractalWorld.ShiftPlotTypes,
		ShiftPlotTypesBy = FractalWorld.ShiftPlotTypesBy,
		DetermineXShift = FractalWorld.DetermineXShift,
		DetermineYShift = FractalWorld.DetermineYShift,
		GenerateCenterRift = FractalWorld.GenerateCenterRift,
		GeneratePlotTypes = RectangularFractalWorld.GeneratePlotTypes,

		iFlags = Map.GetFractalFlags(),

		fracXExp = fracXExp,
		fracYExp = fracYExp,

		iNumPlotsX = gridWidth,
		iNumPlotsY = gridHeight,

		plotTypes = table.fill(PlotTypes.PLOT_OCEAN, gridWidth * gridHeight)
	};

	return data;
end


function RectangularFractalWorld:GeneratePlotTypes(args)
	if(args == nil) then args = {}; end

	local iW, iH = Map.GetGridSize();

	print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ MAP SIZE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	print("Map X: " .. iW);
	print("Map Y: " .. iH);
	print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

	local allcomplete = false;

	while allcomplete == false do
		local waterBorder = 7
		local smallBaysLow = 2;
		local smallBaysHigh = 3;
		local largeBaysLow = 5;
		local largeBaysHigh = 7;
		local smallBaysNS = 15;
		local smallBaysEW = 10;
		local largeBaysNS = 4;
		local largeBaysEW = 2;

		local world_age_old = 2;
		local world_age_normal = 3;
		local world_age_new = 15;

		local extra_mountains = 25;
		local grain_amount = 0;
		local adjust_plates = 1.3;
		local shift_plot_types = true;
		local tectonic_islands = true;
		local hills_ridge_flags = self.iFlags;
		local peaks_ridge_flags = self.iFlags;
		local has_center_rift = true;
		local adjadj = 1.4;
		local xshift = 0;
		local yshift = 0;
		local yshiftamt = 0;
		local xshiftamt = 0;
		local xstart, xend = 0,0;
		local ystart, yend = 0,0;

		local sea_level = Map.GetCustomOption(4)
		if sea_level == 6 then
			sea_level = 1 + Map.Rand(5, "Random Sea Level - Lua");
		end
		local world_age = Map.GetCustomOption(1)
		if world_age == 5 then
			world_age = 1 + Map.Rand(3, "Random World Age - Lua");
		end


		if sea_level == 1 then
			waterBorder = 5;
		elseif sea_level == 2 then
			waterBorder = 6;
		elseif sea_level == 4 then
			waterBorder = 8;
		elseif sea_level == 5 then
			waterBorder = 9;
		end

		local waterCenter = waterBorder;


		local Roughness = Map.GetCustomOption(19)

		if Roughness == 1 then
			smallBaysLow = 1;
			smallBaysHigh = 2;
			largeBaysLow = 3;
			largeBaysHigh = 5;
			smallBaysNS = 7;
			smallBaysEW = 5;
			largeBaysNS = 2;
			largeBaysEW = 1;
		elseif Roughness == 3 then
			smallBaysLow = 4;
			smallBaysHigh = 6;
			largeBaysLow = 8;
			largeBaysHigh = 10;
			smallBaysNS = 20;
			smallBaysEW = 15;
			largeBaysNS = 6;
			largeBaysEW = 4;
		end


		local adjustment = world_age_normal;
		if world_age == 4 then
			adjustment = world_age_old;
			adjust_plates = adjust_plates * 0.5;
		elseif world_age == 3 then
			adjustment = world_age_old;
			adjust_plates = adjust_plates * 0.5;
		elseif world_age == 1 then
			adjustment = world_age_new;
			adjust_plates = adjust_plates * 1;
		else
		end

		local hillsBottom1 = 20 - (adjustment * adjadj);
		local hillsTop1 = 20 + (adjustment * adjadj);
		local hillsBottom2 = 62 - (adjustment * adjadj);
		local hillsTop2 = 62 + (adjustment * adjadj);
		local hillsClumps = 1 + (adjustment * adjadj);
		local hillsNearMountains = 120 - (adjustment * 2) - extra_mountains;
		local mountains = 100 - adjustment - extra_mountains;

		if world_age == 4 then
			mountains = 300 - adjustment - extra_mountains;
		end


		local WorldSizeTypes = {};
		for row in GameInfo.Worlds() do
			WorldSizeTypes[row.Type] = row.ID;
		end
		local sizekey = Map.GetWorldSize();

		local sizevalues = {
			[WorldSizeTypes.WORLDSIZE_DUEL]     = 3,
			[WorldSizeTypes.WORLDSIZE_TINY]     = 3,
			[WorldSizeTypes.WORLDSIZE_SMALL]    = 3,
			[WorldSizeTypes.WORLDSIZE_STANDARD] = 3,
			[WorldSizeTypes.WORLDSIZE_LARGE]    = 3,
			[WorldSizeTypes.WORLDSIZE_HUGE]		= 3
		};
		local grain = sizevalues[sizekey] or 3;

		local platevalues = {
			[WorldSizeTypes.WORLDSIZE_DUEL]		= 100,
			[WorldSizeTypes.WORLDSIZE_TINY]     = 100,
			[WorldSizeTypes.WORLDSIZE_SMALL]    = 100,
			[WorldSizeTypes.WORLDSIZE_STANDARD] = 100,
			[WorldSizeTypes.WORLDSIZE_LARGE]    = 100,
			[WorldSizeTypes.WORLDSIZE_HUGE]     = 100
		};
		local numPlates = platevalues[sizekey] or 5;

		numPlates = numPlates * adjust_plates;



		local iWaterThreshold, biggest_area, iNumTotalLandTiles, iNumBiggestAreaTiles, iBiggestID;
		local grain_dice = Map.Rand(7, "Continental Grain roll - LUA Pangaea");
		if grain_dice < 4 then
			grain_dice = 1;
		else
			grain_dice = 2;
		end
		local rift_dice = Map.Rand(3, "Rift Grain roll - LUA Pangaea");
		if rift_dice < 1 then
			rift_dice = -1;
		end

		rift_dice = -1;
		grain_dice = 5;

		self.continentsFrac = nil;
		self:InitFractal{continent_grain = grain_dice, rift_grain = rift_dice};
		iWaterThreshold = self.continentsFrac:GetHeight(water_percent);

		iNumTotalLandTiles = 0;
		for x = 0, self.iNumPlotsX - 1 do
			for y = 0, self.iNumPlotsY - 1 do
				local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_LAND;
					iNumTotalLandTiles = iNumTotalLandTiles + 1;
			end
		end

		SetPlotTypes(self.plotTypes);
		Map.RecalculateAreas();


		self.hillsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
		self.mountainsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
		self.hillsFrac:BuildRidges(numPlates, hills_ridge_flags, 2, 1);
		self.mountainsFrac:BuildRidges((numPlates * 2) / 3, peaks_ridge_flags, 4, 1);

		local iHillsBottom1 = self.hillsFrac:GetHeight(hillsBottom1);
		local iHillsTop1 = self.hillsFrac:GetHeight(hillsTop1);
		local iHillsBottom2 = self.hillsFrac:GetHeight(hillsBottom2);
		local iHillsTop2 = self.hillsFrac:GetHeight(hillsTop2);
		local iHillsClumps = self.mountainsFrac:GetHeight(hillsClumps);
		local iHillsNearMountains = self.mountainsFrac:GetHeight(hillsNearMountains);
		local iMountainThreshold = self.mountainsFrac:GetHeight(mountains);
		local iPassThreshold = self.hillsFrac:GetHeight(hillsNearMountains);

		local iMountain100 = self.mountainsFrac:GetHeight(100);
		local iMountain99 = self.mountainsFrac:GetHeight(99);
		local iMountain97 = self.mountainsFrac:GetHeight(97);
		local iMountain95 = self.mountainsFrac:GetHeight(95);




		for x = 0, self.iNumPlotsX - 1 do
			for y = 0, self.iNumPlotsY - 1 do

				local i = y * self.iNumPlotsX + x;
				local val = self.continentsFrac:GetHeight(x, y);
				local mountainVal = self.mountainsFrac:GetHeight(x, y);
				local hillVal = self.hillsFrac:GetHeight(x, y);

				if (mountainVal >= iMountainThreshold) then
					if (hillVal >= iPassThreshold) then
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					else

						local iIsMount = Map.Rand(100, "Mountain Spwan Chance");

						local iIsMountAdj = 83 - adjustment;
						if iIsMount >= iIsMountAdj then
							self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
						else

							local iIsHill = Map.Rand(100, "Hill Spwan Chance");

							local iIsHillAdj = 67 - adjustment;
							if iIsHillAdj >= iIsHill then
								self.plotTypes[i] = PlotTypes.PLOT_HILLS;
							else
								self.plotTypes[i] = PlotTypes.PLOT_LAND;
							end
						end
					end
				elseif (mountainVal >= iHillsNearMountains) then
					self.plotTypes[i] = PlotTypes.PLOT_HILLS;
				else
					if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)) then
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					else
						self.plotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
			end
		end

		for x = 0, self.iNumPlotsX - 1 do
			for y = 0, waterBorder do
				local i = y * self.iNumPlotsX + x;
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end

		local mapTop = self.iNumPlotsY - 1;

		for x = 0, self.iNumPlotsX - 1 do
			for y = (mapTop - waterBorder), mapTop do
				local i = y * self.iNumPlotsX + x;
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end


		for x = 0, waterCenter do
			for y = 0, self.iNumPlotsY - 1 do
				local i = y * self.iNumPlotsX + x;
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end

		local edgeBorder = self.iNumPlotsX - 1;

		for x = (edgeBorder - waterCenter), edgeBorder do
			for y = 0, self.iNumPlotsY - 1 do
				local i = y * self.iNumPlotsX + x;
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
			end
		end


		local baysBorder = Map.Rand((smallBaysHigh + 1) - smallBaysLow, "") + smallBaysLow;

		local nsBays = smallBaysNS;
		local ewBays = smallBaysEW;

		local baysInsetNorth = (mapTop - waterBorder - baysBorder);
		local baysInsetNorth2 = mapTop - (waterBorder + 1);

		local baysInsetSouth = (waterBorder + baysBorder);
		local baysInsetSouth2 = (waterBorder + 1);

		local bayInsetEast = ((iW - 1) - waterCenter - baysBorder);
		local bayInsetEast2 = 	(iW - 1) - (waterCenter + 1);

		local bayInsetWest = (waterCenter + 1  + baysBorder);
		local bayInsetWest2 = (waterCenter + 1);


		for bayCount = 1, nsBays do

			local x1 = Map.Rand((bayInsetEast2 + 1) - bayInsetWest2, "") + bayInsetWest2;
			local y1 = Map.Rand((baysInsetNorth2 + 2) - baysInsetNorth, "") + baysInsetNorth;
			local x2 = x1;

			for y = y1, baysInsetNorth2 do
				for x = x1, x2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				x1 = x1 - 1;
				x2 = x2 + 1;
			end
		end


		for bayCount = 1, nsBays do

			local x1 = Map.Rand((bayInsetEast2 + 1) - bayInsetWest2, "") + bayInsetWest2;
			local y1 = Map.Rand((baysInsetSouth + 2) - baysInsetSouth2, "") + baysInsetSouth2;
			local x2 = x1;

			for y = y1, baysInsetSouth2, -1 do
				for x = x1, x2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				x1 = x1 - 1;
				x2 = x2 + 1;
			end
		end


		for bayCount = 1, ewBays do

			local x1 = Map.Rand((bayInsetWest + 1) - bayInsetWest2, "") + bayInsetWest2;
			local y1 = Map.Rand((baysInsetNorth2 + 1) - baysInsetSouth2, "") + baysInsetSouth2;
			local y2 = y1;

			for x = x1, bayInsetWest2, -1 do
				for y = y1, y2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				y1 = y1 - 1;
				y2 = y2 + 1;
			end
		end


		for bayCount = 1, ewBays do

			local x1 = Map.Rand((bayInsetEast2 + 1) - bayInsetEast, "") + bayInsetEast;
			local y1 = Map.Rand((baysInsetNorth2 + 1) - baysInsetSouth2, "") + baysInsetSouth2;
			local y2 = y1;

			for x = x1, bayInsetEast2 do
				for y = y1, y2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				y1 = y1 - 1;
				y2 = y2 + 1;
			end
		end




		local baysBorder = Map.Rand((largeBaysHigh + 1) - largeBaysLow, "") + largeBaysLow;

		local nsBays = largeBaysNS;
		local ewBays = largeBaysEW;

		local baysInsetNorth = (mapTop - waterBorder - baysBorder);
		local baysInsetNorth2 = mapTop - (waterBorder + 1);

		local baysInsetSouth = (waterBorder + baysBorder);
		local baysInsetSouth2 = (waterBorder + 1);

		local bayInsetEast = ((iW - 1) - waterCenter - baysBorder);
		local bayInsetEast2 = 	(iW - 1) - (waterCenter + 1);

		local bayInsetWest = (waterCenter + 1  + baysBorder);
		local bayInsetWest2 = (waterCenter + 1);


		for bayCount = 1, nsBays do

			local x1 = Map.Rand((bayInsetEast2 + 1) - bayInsetWest2, "") + bayInsetWest2;
			local y1 = Map.Rand((baysInsetNorth2 + 2) - baysInsetNorth, "") + baysInsetNorth;
			local x2 = x1;

			for y = y1, baysInsetNorth2 do
				for x = x1, x2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				x1 = x1 - 1;
				x2 = x2 + 1;
			end
		end


		for bayCount = 1, nsBays do

			local x1 = Map.Rand((bayInsetEast2 + 1) - bayInsetWest2, "") + bayInsetWest2;
			local y1 = Map.Rand((baysInsetSouth + 2) - baysInsetSouth2, "") + baysInsetSouth2;
			local x2 = x1;

			for y = y1, baysInsetSouth2, -1 do
				for x = x1, x2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				x1 = x1 - 1;
				x2 = x2 + 1;
			end
		end


		for bayCount = 1, ewBays do

			local x1 = Map.Rand((bayInsetWest + 1) - bayInsetWest2, "") + bayInsetWest2;
			local y1 = Map.Rand((baysInsetNorth2 + 1) - baysInsetSouth2, "") + baysInsetSouth2;
			local y2 = y1;

			for x = x1, bayInsetWest2, -1 do
				for y = y1, y2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				y1 = y1 - 1;
				y2 = y2 + 1;
			end
		end


		for bayCount = 1, ewBays do

			local x1 = Map.Rand((bayInsetEast2 + 1) - bayInsetEast, "") + bayInsetEast;
			local y1 = Map.Rand((baysInsetNorth2 + 1) - baysInsetSouth2, "") + baysInsetSouth2;
			local y2 = y1;

			for x = x1, bayInsetEast2 do
				for y = y1, y2 do
					local i = y * self.iNumPlotsX + x;
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				end

				y1 = y1 - 1;
				y2 = y2 + 1;
			end
		end

		self:ShiftPlotTypes();









		local islandQty = {
			[WorldSizeTypes.WORLDSIZE_DUEL]		= 5,
			[WorldSizeTypes.WORLDSIZE_TINY]     = 16,
			[WorldSizeTypes.WORLDSIZE_SMALL]    = 24,
			[WorldSizeTypes.WORLDSIZE_STANDARD] = 32,
			[WorldSizeTypes.WORLDSIZE_LARGE]    = 52,
			[WorldSizeTypes.WORLDSIZE_HUGE]		= 77
		}

		local firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

		local secondRingYIsEven = {
		{1, 2}, {1, 1}, {2, 0}, {1, -1}, {1, -2}, {0, -2},
		{-1, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-1, 2}, {0, 2}
		};

		local thirdRingYIsEven = {
		{1, 3}, {2, 2}, {2, 1}, {3, 0}, {2, -1}, {2, -2},
		{1, -3}, {0, -3}, {-1, -3}, {-2, -3}, {-2, -2}, {-3, -1},
		{-3, 0}, {-3, 1}, {-2, 2}, {-2, 3}, {-1, 3}, {0, 3}
		};

		local firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};

		local secondRingYIsOdd = {
		{1, 2}, {2, 1}, {2, 0}, {2, -1}, {1, -2}, {0, -2},
		{-1, -2}, {-1, -1}, {-2, 0}, {-1, 1}, {-1, 2}, {0, 2}
		};

		local thirdRingYIsOdd = {
		{2, 3}, {2, 2}, {3, 1}, {3, 0}, {3, -1}, {2, -2},
		{2, -3}, {1, -3}, {0, -3}, {-1, -3}, {-2, -2}, {-2, -1},
		{-3, 0}, {-2, 1}, {-2, 2}, {-1, 3}, {0, 3}, {1, 3}
		};


		local direction_types = {
			DirectionTypes.DIRECTION_NORTHEAST,
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHEAST,
			DirectionTypes.DIRECTION_SOUTHWEST,
			DirectionTypes.DIRECTION_WEST,
			DirectionTypes.DIRECTION_NORTHWEST
			};


		plotTypesTwo = self.plotTypes;

		local iW, iH = Map.GetGridSize();
		local islMax = islandQty[sizekey] or 24;
		local mapSize = iW * iH;
		local islCount = 0;
		local islLandInRing = 0;
		local goodX = 0;
		local goodY = 0;

		local wrapX = Map:IsWrapX();
		local wrapY = false;
		local nextX, nextY, plot_adjustments;
		local odd = firstRingYIsOdd;
		local even = firstRingYIsEven;
		local failedattemps = 0;
		local bIslandsFailure = false;

		local minIslandSize = 1;
		local maxIslandSize = 5;
		local escapeRedo = 500;
		local redoMap = false;

		print("######### Creating Islands #########");

		islandSetting = Map.GetCustomOption(15);

		islCount =  Map.GetCustomOption(15) - 1;

		while islCount > 0 and escapeRedo > 0 do

			local islLandInRing = 0;
			local startingPlot = 0;
			local landX = 0;
			local landY = 0;
			local landPlot = 0;


			local x = Map.Rand(iW, "");
			local y = 3 + Map.Rand((iH-6), "");
			local plotIndex = y * iW + x + 1;

			local radius = Map.Rand(4, "");










			if self.plotTypes[plotIndex] == PlotTypes.PLOT_OCEAN then

				startingPlot = plotIndex;


				local radiuschk = 5;

				for ripple_radius = 1, radiuschk do
					local ripple_value = radiuschk - ripple_radius + 1;
					local currentX = x - ripple_radius;
					local currentY = y;
					for direction_index = 1, 6 do
						for plot_to_handle = 1, ripple_radius do
				 			if currentY / 2 > math.floor(currentY / 2) then
								plot_adjustments = odd[direction_index];
							else
								plot_adjustments = even[direction_index];
							end
							nextX = currentX + plot_adjustments[1];
							nextY = currentY + plot_adjustments[2];
							if wrapX == false and (nextX < 0 or nextX >= iW) then

							elseif wrapY == false and (nextY < 0 or nextY >= iH) then

							else
								local realX = nextX;
								local realY = nextY;
								if wrapX then
									realX = realX % iW;
								end
								if wrapY then
									realY = realY % iH;
								end


								local plotIndex = realY * iW + realX + 1;






								if self.plotTypes[plotIndex] == PlotTypes.PLOT_LAND then
									islLandInRing = ripple_radius;

									landPlot = plotIndex;

									landX = realX;
									landY = realY;




									break;
								end

								currentX, currentY = nextX, nextY;
							end
						end

						if islLandInRing ~= 0 then
							break;
						end
					end

					if islLandInRing ~= 0 then
						break;
					end

				end


				if islLandInRing ~= 0 then



					local biggest_area = Map.FindBiggestArea(false);
					local biggest_ID = biggest_area:GetID();
					local plotCheck = Map.GetPlot(landX, landY);
					local plotArea = plotCheck:Area();
					local iAreaID = plotArea:GetID();
					local pullBack = 3;


					if plotTypesTwo[landPlot] == PlotTypes.PLOT_LAND then


						islLandInRing = islLandInRing - pullBack;



						if islLandInRing > minIslandSize and islLandInRing < maxIslandSize then

							local islThresh = 0;
							local landvarDefault = 10;

							local locationRnd = Map.Rand(100, "");

							if (locationRnd > 49) then
								self.plotTypes[startingPlot] = PlotTypes.PLOT_LAND;
							else
								self.plotTypes[startingPlot] = PlotTypes.PLOT_HILLS;
							end

							for ripple_radius = 1, islLandInRing do
								local ripple_value = islLandInRing - ripple_radius + 1;
								local currentX = x - ripple_radius;
								local currentY = y;
								for direction_index = 1, 6 do
									for plot_to_handle = 1, ripple_radius do
							 			if currentY / 2 > math.floor(currentY / 2) then
											plot_adjustments = odd[direction_index];
										else
											plot_adjustments = even[direction_index];
										end
										nextX = currentX + plot_adjustments[1];
										nextY = currentY + plot_adjustments[2];
										if wrapX == false and (nextX < 0 or nextX >= iW) then

										elseif wrapY == false and (nextY < 0 or nextY >= iH) then

										else
											local realX = nextX;
											local realY = nextY;
											if wrapX then
												realX = realX % iW;
											end
											if wrapY then
												realY = realY % iH;
											end


											local plotIndex = realY * iW + realX + 1;

											local thisislandvar = Map.Rand(60, "") + landvarDefault;


											if ripple_radius == 1  then
												islThresh = Map.Rand(50, "") + thisislandvar;
											elseif ripple_radius == 2 then
												islThresh = Map.Rand(45, "") + (thisislandvar / 1.25);
											elseif ripple_radius == 3 then
												islThresh = Map.Rand(37, "") + (thisislandvar / 1.5);
											else
												islThresh = Map.Rand(30, "") + (thisislandvar / 2);
											end

											local islRand = Map.Rand(100, "");
											local islHill = Map.Rand(100, "");



											if islRand > islThresh then
												self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN
												landvarDefault = landvarDefault + 5;
											else
												if islHill <= 40 then
													self.plotTypes[plotIndex] = PlotTypes.PLOT_LAND
												else
													self.plotTypes[plotIndex] = PlotTypes.PLOT_HILLS
												end
											end

											currentX, currentY = nextX, nextY;
										end
									end
								end
							end
							islCount = islCount -1;
						end
					end
				end
			end

			escapeRedo = escapeRedo - 1;

		end


		if escapeRedo == 0 then

			redoMap = true
		end

		print("######### Finished Islands #########");


		local iNumLandTilesInUse = 0;
		local iW, iH = Map.GetGridSize();
		local iPercent = (iW * iH) * 0.35;

		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				local i = iW * y + x;
				if self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
					iNumLandTilesInUse = iNumLandTilesInUse + 1;
				end
			end
		end

		redoMap = false;

		print("######### Map Failure Check #########");
		print("35% Of Map Area: ", iPercent);
		print("Map Land Tiles: ", iNumLandTilesInUse);

		if iNumLandTilesInUse >= iPercent and redoMap == false then
			allcomplete = true;
			print("######### Map Pass #########");
		else
			print("######### Map Failure #########");
		end
	end

	return self.plotTypes;
end










function GeneratePlotTypes()

	print("Generating Plot Types (Lua Pangaea) ...");

	local fractal_world = RectangularFractalWorld.Create();
	local plotTypes = fractal_world:GeneratePlotTypes();

	SetPlotTypes(plotTypes);
	GenerateCoasts();
end

function GenerateTerrain()

	local DesertPercent = 22;


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

	FixIslands();

end


function FixIslands()

	local iW, iH = Map.GetGridSize();
	local biggest_area = Map.FindBiggestArea(False);
	local iAreaID = biggest_area:GetID();

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = iW * y + x;
			local plot = Map.GetPlotByIndex(i);
			plotAreaID = plot:GetArea();
			if plotAreaID ~= iAreaID then
				local terrainType = plot:GetTerrainType();
				local plotType = plot:GetPlotType();

				if terrainType == TerrainTypes.TERRAIN_TUNDRA then
					if plotType ~= PlotTypes.PLOT_HILLS then

						local tundratoplains = Map.Rand(100, "Plains Spwan Chance");
						if tundratoplains >= 30 then
							plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
						end
					end
				end
			end
		end
	end
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

	local RegionalMethod = 1;


	local AllowInlandSea = Map.GetCustomOption(18)
	local res = Map.GetCustomOption(13)
	local starts = Map.GetCustomOption(5)





	MixedBias = false;
	if Map.GetCustomOption(16) == 1 then
		OnlyCoastal = true;
		BalancedCoastal = false;
	end
	if Map.GetCustomOption(16) == 2 then
		BalancedCoastal = false;
		OnlyCoastal = false;
	end

	if Map.GetCustomOption(16) == 3 then
		OnlyCoastal = true;
		BalancedCoastal = true;
	end

	if Map.GetCustomOption(17) == 1 then
	CoastLux = true
	end

	if Map.GetCustomOption(17) == 2 then
	CoastLux = false
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()

	print("Dividing the map in to Regions.");

	local args = {
		method = RegionalMethod,
		start_locations = starts,
		resources = res,
		AllowInlandSea = AllowInlandSea,
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
