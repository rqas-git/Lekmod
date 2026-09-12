












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
		Name = "Lekmap v6.2",
		Description = "A map script made for Lekmod based of HB's Mapscript v8.1. Pangaea - Fractal with Beta options by Jacobian",
		IsAdvancedMap = false,
		IconIndex = 0,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create({
			[6] = {
				Values = {
					"Close",
					"Normal",
					"Far - Default",
				},
				DefaultValue = 3,
			},
			[7] = {
				Values = {
					"0",
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
					"12",
					"Random",
					"Default",
					"Between 3-5",
					"Between 2-6",
				},
			},
			[11] = {
				Values = {
					"Default -10 tiles",
					"Default -8 tiles",
					"Default -6 tiles",
					"Default -4 tiles",
					"Default -2 tiles",
					"Default (58 on Small)",
					"Default +2 tiles",
					"Default +4 tiles",
					"Default +6 tiles",
					"Default +8 tiles",
					"Default +10 tiles",
				},
				DefaultValue = 6,
			},
			[12] = {
				Values = {
					"Default -10 tiles",
					"Default -8 tiles",
					"Default -6 tiles",
					"Default -4 tiles",
					"Default -2 tiles",
					"Default (52 on Small)",
					"Default +2 tiles",
					"Default +4 tiles",
					"Default +6 tiles",
					"Default +8 tiles",
					"Default +10 tiles",
				},
				DefaultValue = 6,
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
					"Not allowed",
				},
				DefaultValue = 2,
				SortPriority = -83,
			},
			{
				Name = "No Flat Desert Luxes",
				Values = {
					"Off - Old Default",
					"On - Default",
				},
				DefaultValue = 2,
				SortPriority = -99,
			},
			{
				Name = "Error Notifications",
				Values = {
					"[COLOR_NEGATIVE_TEXT]Off - This Will Make Jacobian Very Sad[ENDCOLOR]",
					"On",
					"[COLOR_NEGATIVE_TEXT]Error Testing - This Will Intentionally Cause an Error[ENDCOLOR]",
				},
				DefaultValue = 2,
				SortPriority = -99,
			},
			{
				Name = "Mountain Reduction",
				Values = {
					"0% - Old Default",
					"5%",
					"10% - Default",
					"15%",
					"20%",
					"30%",
				},
				DefaultValue = 3,
				SortPriority = -99,
			},
			{
				Name = "Independent Hill Reduction",
				Values = {
					"0% - Old Default",
					"5%",
					"10%",
					"20%",
					"30%",
					"40%",
					"50%",
				},
				DefaultValue = 1,
				SortPriority = -99,
			},
			{
				Name = "Isolation Fix",
				Values = {
					"5.0 Cutoff (Low Remake Chance)",
					"7.0 Cutoff (Medium Remake Chance) - Default",
					"9.0 Cutoff (High Remake Chance)",
				},
				DefaultValue = 2,
				SortPriority = -99,
			},
			{
				Name = "Non-Coastal-CS Deadband",
				Values = {
					"Off - Old Default",
					"2 Hex",
					"3 Hex - Default",
					"4 Hex",
				},
				DefaultValue = 3,
				SortPriority = -99,
			},
			{
				Name = "Sea Side Cliffs",
				Values = {
					"Off - Old Default",
					"On - Default",
				},
				DefaultValue = 2,
				SortPriority = -99,
			},
		}),
	};
end


function i_to_xy(i, iW, iH)
	local x = (i - 1) % iW
	return x, (i - x - 1) / iW
end
function xy_to_i(x, y, iW, iH)
	return y * iW + x + 1
end
function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

function GetAdjacent(x, y)
	local radius = 1;


	local impact_value = 99;
	local odd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
	local even = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	local nextX, nextY, plot_adjustments;

	local ripple_value = radius - 1 + 1;





	local currentX = x - 1;
	local currentY = y;



	local adj_indices = {};
	for direction_index = 1, 6 do

		if currentY / 2 > math.floor(currentY / 2) then
			plot_adjustments = odd[direction_index];
		else
			plot_adjustments = even[direction_index];
		end

		nextX = currentX + plot_adjustments[1];
		nextY = currentY + plot_adjustments[2];

		if wrapX == false and (nextX < 0 or nextX >= iW) then

			adj_indices[direction_index] = -1;
		elseif wrapY == false and (nextY < 0 or nextY >= iH) then
			adj_indices[direction_index] = -1;

		else

			local realX = nextX;
			local realY = nextY;
			if wrapX then
				realX = realX % iW;
			end
			if wrapY then
				realY = realY % iH;
			end

			adj_indices[direction_index] = realY * iW + realX + 1;
		end
		currentX, currentY = nextX, nextY;
	end
	return adj_indices;
end


function GetMapInitData(worldSize)
	
	local LandSizeXDuel = 22 + (Map.GetCustomOption(11) * 2);
	local LandSizeYDuel = 18 + (Map.GetCustomOption(12) * 2);

	local LandSizeXTiny = 36 + (Map.GetCustomOption(11) * 2);
	local LandSizeYTiny = 30 + (Map.GetCustomOption(12) * 2);

	local LandSizeXSmall = 46 + (Map.GetCustomOption(11) * 2);
	local LandSizeYSmall = 40 + (Map.GetCustomOption(12) * 2);

	local LandSizeXStandard = 54 + (Map.GetCustomOption(11) * 2);
	local LandSizeYStandard = 48 + (Map.GetCustomOption(12) * 2);

	local LandSizeXLarge = 62 + (Map.GetCustomOption(11) * 2);
	local LandSizeYLarge = 54 + (Map.GetCustomOption(12) * 2);

	local LandSizeXHuge = 70 + (Map.GetCustomOption(11) * 2);
	local LandSizeYHuge = 62 + (Map.GetCustomOption(12) * 2);

	local worldsizes = {};

	worldsizes = {

		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {LandSizeXDuel, LandSizeYDuel},
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {LandSizeXTiny, LandSizeYTiny},
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {LandSizeXSmall, LandSizeYSmall},
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {LandSizeXStandard, LandSizeYStandard},
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {LandSizeXLarge, LandSizeYLarge},
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {LandSizeXHuge, LandSizeYHuge}
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





PangaeaFractalWorld = {};


function PangaeaFractalWorld.Create(fracXExp, fracYExp)
	local gridWidth, gridHeight = Map.GetGridSize();
	
	local data = {
		InitFractal = FractalWorld.InitFractal,
		ShiftPlotTypes = FractalWorld.ShiftPlotTypes,
		ShiftPlotTypesBy = FractalWorld.ShiftPlotTypesBy,
		DetermineXShift = FractalWorld.DetermineXShift,
		DetermineYShift = FractalWorld.DetermineYShift,
		GenerateCenterRift = FractalWorld.GenerateCenterRift,
		GeneratePlotTypes = PangaeaFractalWorld.GeneratePlotTypes,
		
		iFlags = Map.GetFractalFlags(),
		
		fracXExp = fracXExp,
		fracYExp = fracYExp,
		
		iNumPlotsX = gridWidth,
		iNumPlotsY = gridHeight,
		plotTypes = table.fill(PlotTypes.PLOT_OCEAN, gridWidth * gridHeight)
	};
		
	return data;
end





function FractalWorld:ShiftPlotTypes()
	local stripRadius = self.stripRadius;
	local shift_x = 0; 
	local shift_y = 0;

	shift_x = self:DetermineXShift();	
	shift_y = self:DetermineYShift();
	
	print("X Shift: " .. shift_x)
	print("Y Shift: " .. shift_y)

	if beta_tectonic_mounts == 2 then
		ShiftPlotTypesByGeneric(shift_x, shift_y, BETA_TECTONIC_LANDS)
	end

	self:ShiftPlotTypesBy(shift_x, shift_y);
end



function FractalWorld:ShiftPlotTypesBy(xshift, yshift)
	if(xshift > 0 or yshift > 0) then

		local buf = {};




		for iDestY = 0, self.iNumPlotsY do
			for iDestX = 0, self.iNumPlotsX do
				local i = self.iNumPlotsX * iDestY + iDestX + 1;
				buf[i] = self.plotTypes[i];
			end
		end
		
		for iDestY = 0, self.iNumPlotsY do
			for iDestX = 0, self.iNumPlotsX do
				local iDestI = self.iNumPlotsX * iDestY + iDestX + 1;
				local iSourceX = (iDestX + xshift) % self.iNumPlotsX;
				local iSourceY = (iDestY + yshift) % self.iNumPlotsY;
				local iSourceI = self.iNumPlotsX * iSourceY + iSourceX + 1;
				self.plotTypes[iDestI] = buf[iSourceI]
			end
		end
	end
end

function ShiftPlotTypesByGeneric(xshift, yshift, graph)
	if(xshift > 0 or yshift > 0) then

		local buf = {};
		for iDestY = 0, iH do
			for iDestX = 0, iW do
				local i = iW * iDestY + iDestX + 1;
				buf[i] = graph[i];
			end
		end
		
		for iDestY = 0, iH do
			for iDestX = 0, iW do
				local iDestI = iW * iDestY + iDestX + 1;
				local iSourceX = (iDestX + xshift) % iW;
				local iSourceY = (iDestY + yshift) % iH;
				
				local iSourceI = iW * iSourceY + iSourceX + 1
				graph[iDestI] = buf[iSourceI]
			end
		end
	end
end

function choke_check(i, map)
	local adj_is = adj_is_cache[i]
	local toggle = -1
	local toggles = 0
	for loop2, adj_i in ipairs(adj_is) do
		if map[adj_i] ~= nil then
			local toggle_value = 1
			if map[adj_i] == -1 then
				toggle_value = 0
			end
			if toggle ~= toggle_value then
				toggles = toggles + 1
				toggle = toggle_value
			end
		end
	end
	return (toggles >= 4)
end

function impassable_check(i, map)
	local adj_is = adj_is_cache[i]
	local count = 0
	for loop2, adj_i in ipairs(adj_is) do
		if map[adj_i] ~= nil then
			local toggle_value = 1
			if map[adj_i] == -1 then
				count = count+1
			end
		end
	end
	return (count >= 4)
end

function get_blobs(map)
	local blob_to_region = {}
	local blob_count = {}
	local blob_graph = table.fill(0, iW * iH)
	local blob_number = 1



	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH);
			if map[i] == nil then
				print("MAP GOT A NIL?? "..i)
			else
				if map[i] > 0 then
					if blob_graph[i] == 0 then
						blob_to_region[blob_number] = map[i]
						local blob_fill = {}
						PlotDFS(i, blob_fill, map, map[i])
						blob_count[blob_number] = tablelength(blob_fill)
						for l = 1, table.maxn(blob_fill) do
							if blob_fill[l] ~= nil then
								blob_graph[l] = blob_number
							end
						end
						blob_number = blob_number+1
					end
				end
			end
		end
	end

	local blobs = {}
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH);
			if blob_graph[i] > 0 then
				if blobs[blob_graph[i]] == nil then
					blobs[blob_graph[i]] = {}
				end
				blobs[blob_graph[i]][i] = true
			end
		end
	end

	return blob_graph, blobs
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
				local plot_type = testPlot:GetPlotType()
				if plot_type == PlotTypes.PLOT_OCEAN then
					if testPlot:IsLake() == false then
						return true
					end
				end
			end
		end
	end

	return false
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

			if plotDataIsCoastal[i] == false and (not plot:IsWater() or (plot:IsWater() and plot:IsFreshWater())) then

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




function PangaeaFractalWorld:GeneratePlotTypes(args)




	if(args == nil) then args = {}; end


	local beta_tectonic_mounts = 2
	local allcomplete = false;

	while allcomplete == false do

		local sea_level_low = 64;
		local sea_level_normal = 67;
		local sea_level_high = 70;
		local world_age_old = 3;
		local world_age_normal = 4;
		local world_age_new = 5;

		local extra_mountains = 6;
		local grain_amount = 0;
		local adjust_plates = 1.3;
		local shift_plot_types = true;
		local tectonic_islands = true;
		local hills_ridge_flags = self.iFlags;
		local peaks_ridge_flags = self.iFlags;
		local has_center_rift = true;
		local adjadj = 1.2;
		local xshift = 0;
		local yshift = 0;
		local yshiftamt = 0;
		local xshiftamt = 0;
		local xstart, xend = 0,0;
		local ystart, yend = 0,0;

		local sea_level = Map.GetCustomOption(4)
		if sea_level == 4 then
			sea_level = 1 + Map.Rand(3, "Random Sea Level - Lua");
		end
		local world_age = Map.GetCustomOption(1)
		if world_age == 5 then
			world_age = 1 + Map.Rand(3, "Random World Age - Lua");
		end


		local water_percent = sea_level_normal;

		local fjorddistmodif = 1

		local fjordlengthmodif = 1

		local beta_mountain_mod = Map.GetCustomOption(20);
		local beta_hill_mod = Map.GetCustomOption(21);



		local hill_prob = 100
		if beta_hill_mod == 2 then
			hill_prob = 95
		elseif beta_hill_mod == 3 then
			hill_prob = 90
		elseif beta_hill_mod == 4 then
			hill_prob = 80
		elseif beta_hill_mod == 5 then
			hill_prob = 70
		elseif beta_hill_mod == 6 then
			hill_prob = 60
		elseif beta_hill_mod == 7 then
			hill_prob = 50
		end

		local fjordmodif = (fjorddistmodif - 1) * (fjordlengthmodif + 1);
		if sea_level == 1 then
			water_percent = sea_level_low
		elseif sea_level == 3 then
			water_percent = sea_level_high
		else
		
		end
		water_percent = water_percent - math.floor(fjordmodif / 10);


		if beta_tectonic_mounts == 2 then
			water_percent = water_percent+3
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

		local hillsBottom1 = 26 - (adjustment * adjadj);
		local hillsTop1 = 26 + (adjustment * adjadj);
		local hillsBottom2 = 72 - (adjustment * adjadj);
		local hillsTop2 = 72 + (adjustment * adjadj);
		local hillsClumps = 1 + (adjustment * adjadj);
		local hillsNearMountains = 91 - (adjustment * 2) - extra_mountains;
		local mountains = 95 - adjustment - extra_mountains;
	
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



		local bMapOK = false;
		while bMapOK == false do
			local done = false;
			local iAttempts = 0;
			local iWaterThreshold, biggest_area, iNumTotalLandTiles, iNumBiggestAreaTiles, iBiggestID;
			while done == false do
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
				grain_dice = 7;

				self.continentsFrac = nil;
				self:InitFractal{continent_grain = grain_dice, rift_grain = rift_dice};
				iWaterThreshold = self.continentsFrac:GetHeight(water_percent);
		
				iNumTotalLandTiles = 0;
				for x = 0, self.iNumPlotsX - 1 do
					for y = 0, self.iNumPlotsY - 1 do
						local i = y * self.iNumPlotsX + x + 1;
						local val = self.continentsFrac:GetHeight(x, y);
						if(val <= iWaterThreshold) then
							self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
						else
							self.plotTypes[i] = PlotTypes.PLOT_LAND;
							iNumTotalLandTiles = iNumTotalLandTiles + 1;
						end
					end
				end

				SetPlotTypes(self.plotTypes);
				Map.RecalculateAreas();
		
				biggest_area = Map.FindBiggestArea(false);
				iNumBiggestAreaTiles = biggest_area:GetNumTiles();

				if iNumBiggestAreaTiles >= iNumTotalLandTiles * 1 then
					done = true;
					iBiggestID = biggest_area:GetID();
				end
				iAttempts = iAttempts + 1;











		
			end


			self.hillsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
			self.mountainsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
			self.hillsFrac:BuildRidges(numPlates, hills_ridge_flags, 1, 2);
			self.mountainsFrac:BuildRidges((numPlates * 2) / 3, peaks_ridge_flags, 6, 1);







			local iHillsBottom1 = self.hillsFrac:GetHeight(hillsBottom1);
			local iHillsTop1 = self.hillsFrac:GetHeight(hillsTop1);
			local iHillsBottom2 = self.hillsFrac:GetHeight(hillsBottom2);
			local iHillsTop2 = self.hillsFrac:GetHeight(hillsTop2);
			local iHillsClumps = self.mountainsFrac:GetHeight(hillsClumps);
			local iHillsNearMountains = 0;
			local iMountainThreshold = 0;


			if beta_mountain_mod == 1 then
				iHillsNearMountains = self.mountainsFrac:GetHeight(hillsNearMountains);
				iMountainThreshold = self.mountainsFrac:GetHeight(mountains);
			elseif beta_mountain_mod == 2 then
				iHillsNearMountains = self.mountainsFrac:GetHeight(math.floor((100+hillsNearMountains*19)/20));
				iMountainThreshold = self.mountainsFrac:GetHeight(math.floor((100+mountains*19)/20));
			elseif beta_mountain_mod == 3 then
				iHillsNearMountains = self.mountainsFrac:GetHeight(math.floor((100+hillsNearMountains*9)/10));
				iMountainThreshold = self.mountainsFrac:GetHeight(math.floor((100+mountains*9)/10));
			elseif beta_mountain_mod == 4 then
				iHillsNearMountains = self.mountainsFrac:GetHeight(math.floor((300+hillsNearMountains*17)/20));
				iMountainThreshold = self.mountainsFrac:GetHeight(math.floor((300+mountains*17)/20));
			elseif beta_mountain_mod == 5 then
				iHillsNearMountains = self.mountainsFrac:GetHeight(math.floor((200+hillsNearMountains*8)/10));
				iMountainThreshold = self.mountainsFrac:GetHeight(math.floor((200+mountains*8)/10));
			elseif beta_mountain_mod == 6 then
				iHillsNearMountains = self.mountainsFrac:GetHeight(math.floor((300+hillsNearMountains*7)/10));
				iMountainThreshold = self.mountainsFrac:GetHeight(math.floor((300+mountains*7)/10));






			end
			local iPassThreshold = self.hillsFrac:GetHeight(hillsNearMountains);

			
			local iMountain100 = self.mountainsFrac:GetHeight(100);
			local iMountain99 = self.mountainsFrac:GetHeight(99);
			local iMountain97 = self.mountainsFrac:GetHeight(97);
			local iMountain95 = self.mountainsFrac:GetHeight(95);

			local iShallow = self.mountainsFrac:GetHeight(93);

			local iWaterThresholdIslands1 = self.continentsFrac:GetHeight(water_percent-4);
			local iWaterThresholdIslands2 = self.continentsFrac:GetHeight(water_percent-20);
			local iWaterShallow = self.continentsFrac:GetHeight(water_percent-28);

			if beta_tectonic_mounts == 2 then
				iMountain100 = self.mountainsFrac:GetHeight(94);
				iMountain99 = self.mountainsFrac:GetHeight(85);

				iMountain95 = self.mountainsFrac:GetHeight(68);
				iShallow = self.mountainsFrac:GetHeight(54);
			end











			if beta_tectonic_mounts == 2 then
				BETA_TECTONIC_LANDS = table.fill(-1, iW * iH)
			end

			for x = 0, self.iNumPlotsX - 1 do
				for y = 0, self.iNumPlotsY - 1 do
		
					local i = y * self.iNumPlotsX + x + 1;
					local val = self.continentsFrac:GetHeight(x, y);
					local mountainVal = self.mountainsFrac:GetHeight(x, y);
					local hillVal = self.hillsFrac:GetHeight(x, y);
	
					if(val <= iWaterThreshold) then
						self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				
						if tectonic_islands then
							if beta_tectonic_mounts == 1 then
								if (mountainVal == iMountain100) then
									self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
								elseif (mountainVal == iMountain99) then
									self.plotTypes[i] = PlotTypes.PLOT_HILLS;
								elseif (mountainVal == iMountain97) or (mountainVal == iMountain95) then
									self.plotTypes[i] = PlotTypes.PLOT_LAND;
								end
							else
								if (mountainVal >= iMountain100) and (val >= iWaterThresholdIslands2) and (val >= iWaterThresholdIslands1) == false then
									self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
									BETA_TECTONIC_LANDS[i] = 1
								elseif (mountainVal >= iMountain99) and (val >= iWaterThresholdIslands2) and (val >= iWaterThresholdIslands1) == false then
									self.plotTypes[i] = PlotTypes.PLOT_HILLS;
									BETA_TECTONIC_LANDS[i] = 1
								elseif (mountainVal >= iMountain95) and (val >= iWaterThresholdIslands2) and (val >= iWaterThresholdIslands1) == false then
									self.plotTypes[i] = PlotTypes.PLOT_LAND;
									BETA_TECTONIC_LANDS[i] = 1
								elseif (mountainVal >= iShallow) and (val >= iWaterShallow) and (val >= iWaterThreshold) == false then
									local iIsShallows = Map.Rand(100, "Shallows Chance")
									if iIsShallows > 70 then
										BETA_TECTONIC_LANDS[i] = 3
									end
								end
							end
						end
					
					else
						if beta_tectonic_mounts == 2 then
							BETA_TECTONIC_LANDS[i] = 0
						end
						if (mountainVal >= iMountainThreshold) then
							if (hillVal >= iPassThreshold) then
								self.plotTypes[i] = PlotTypes.PLOT_HILLS;
							else

								local iIsMount = Map.Rand(100, "Mountain Spawn Chance");

								local iIsMountAdj = 48 - adjustment;
								if iIsMount >= iIsMountAdj then
									self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
								else

									local iIsHill = Map.Rand(100, "Hill Spawn Chance");

									local iIsHillAdj = 30 - adjustment;
									if iIsHill >= iIsHillAdj then
										self.plotTypes[i] = PlotTypes.PLOT_HILLS;
									else
										self.plotTypes[i] = PlotTypes.PLOT_LAND;
									end
								end
							end
						elseif (mountainVal >= iHillsNearMountains) then
							self.plotTypes[i] = PlotTypes.PLOT_HILLS;
						else
							local iHillChance = Map.Rand(100, "Beta Hill Roll")
							if (((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2))) and (iHillChance < hill_prob) then
								self.plotTypes[i] = PlotTypes.PLOT_HILLS;
							else
								self.plotTypes[i] = PlotTypes.PLOT_LAND;
							end
						end
					end
				end
			end


			self:ShiftPlotTypes();




			if beta_tectonic_mounts == 2 then
			
				local blob_graph, blobs = get_blobs(BETA_TECTONIC_LANDS)

				local blob_islands = {}


				

				for loop, blob in ipairs(blobs) do

					blob_islands[loop] = true
					local blob_connectors = {}
					local blob_depths = {}
					local blob_size = tablelength(blob)
					for i, dumby in pairs(blob) do


						if self.plotTypes[i] == PlotTypes.PLOT_LAND then
							local hill_restore = Map.Rand(blob_size*3+9, "Hill Restore")
							if hill_restore < 3 then
								self.plotTypes[i] = PlotTypes.PLOT_HILLS
							end
						end

						adj_is = adj_is_cache[i]
						for loop2, adj_i in ipairs(adj_is) do
							if BETA_TECTONIC_LANDS[adj_i] == 0 then
								blob_islands[loop] = false
								blob_connectors[i] = true




							end
						end
					end

					if blob_islands[loop] == false then
						local blob_depths = {}
						PlotBFS(blob_connectors, blob_depths, BETA_TECTONIC_LANDS, 1, 1)
						local total_plots = tablelength(blob_depths)
						local max_depth = 0
						local depth_plots = {}
						for i, depth in pairs(blob_depths) do
							if depth_plots[depth] == nil then
								depth_plots[depth] = {}
								if depth > max_depth then
									max_depth = depth
								end
							end
							depth_plots[depth][i] = true
						end

						local sever_plots = {}
						local severed = false
						local remaining_plots = total_plots
						for depth = 1, max_depth do
							remaining_plots = remaining_plots - tablelength(depth_plots[depth])
							if remaining_plots >= 5 and (severed == false) then
								local sever_depth = true
								for i, dumby in pairs(depth_plots[depth]) do
									if choke_check(i, BETA_TECTONIC_LANDS) == false then
										sever_depth = false
									end
								end

								if sever_depth then
									severed = true
									sever_plots = depth_plots[depth]
								end
							end
						end

						for i, dumby in pairs(sever_plots) do
							BETA_TECTONIC_LANDS[i] = 2
						end
					end
				end
			end























	


			if false then
				bMapOK = true
			else

				


				local bfland = false;
				local startcol = 0;
				local cont = 0;
				local bprev = false;
				local biggest = 0;
				local mainstart = 0;
				local mainend = 0;
				local cencol = 0;
				local colshift = 0;
				local landincol = 0;
				local chkstart = 0;
				local chkend = 0;
				local chokepoint = 16;
				if beta_tectonic_mounts == 2 then
					chokepoint = 14;
				end
				local bXChkFail = false;
				local bYChkFail = false;
				local bLastLand = false;
				local contlandincol = 0;
				local xcen = 0;
				local ycen = 0;


				print("-----------------------------------");
				print("Checking Y Chokes");
				print("-----------------------------------");

				local valid_xs_from = 1
				local valid_xs_to = iW

				if beta_tectonic_mounts == 2 then
					valid_xs_from = math.ceil(iW * 1 / 4)
					valid_xs_to = math.ceil(iW * 3 / 4)
				end

				for x = valid_xs_from, valid_xs_to do
					bfland = false;
					landincol = 0;
			
					for y = 2, iH-2  do
						local i = iW * y + x + 1;

						local tec_check = true
						if beta_tectonic_mounts == 2 then
							if BETA_TECTONIC_LANDS[i] > 0 then
								tec_check = false
							end
						end

						if self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN and tec_check then
							landincol = landincol + 1;
							bfland = true;
						end
					end
			
					if bfland == false then

						bprev = false;
						if cont > biggest then
							biggest = cont;
							mainstart = startcol;
							mainend = x-1;
						end
						cont = 0;
						startcol = 0;
					else

						if startcol == 0 then
							startcol = x;
						end
						bprev = true;
						cont = cont + 1;	
					end
				end
			
				xstart = mainstart;
				xend = mainend;

				chkstart = mainstart + 8;
				chkend = mainend -  8;

				local landincol_prev1 = chokepoint;
				local landincol_prev2 = chokepoint;

				for x = chkstart, chkend do
					landincol = 0;
					contlandincol = 0;
					for y = 2, iH-2  do
						local i = iW * y + x + 1;

						local tec_check = true
						if beta_tectonic_mounts == 2 then
							if BETA_TECTONIC_LANDS[i] > 0 then
								tec_check = false
							end
						end

						if self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN and tec_check then
						
							if bLastLand == true then
								landincol = landincol + 1;
								bLastLand = true;
							else
								landincol = 1;
								bLastLand = true;
							end
						else
							if contlandincol < landincol then
								contlandincol = landincol;
							end
							bLastLand = false;
							landincol = 0;
						end
					end



					if landincol_prev1 + landincol_prev2 + contlandincol < 3 * chokepoint then

						bXChkFail = true;
					end
					landincol_prev2 = contlandincol;
					landincol_prev1 = landincol_prev2;
				end




				print("-----------------------------------");
				print("Checking X Chokes");
				print("-----------------------------------");
				startcol = 0;
				cont = 0;
				biggest = 0;

				local valid_ys_from = 1
				local valid_ys_to = iH-2

				if beta_tectonic_mounts == 2 then
					valid_ys_from = math.ceil((iH-2) * 1 / 3)
					valid_ys_to = math.ceil((iH-2) * 2 / 3)
				end
				
				for y = valid_ys_from, valid_ys_to do
					bfland = false;
					landincol = 0;
			
					for x = 1, iW  do
						local i = iW * y + x + 1;

						local tec_check = true
						if beta_tectonic_mounts == 2 then
							if BETA_TECTONIC_LANDS[i] > 0 then
								tec_check = false
							end
						end

						if self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN and tec_check then
							landincol = landincol + 1;
							bfland = true;
						end
					end
			
					if bfland == false then

						bprev = false;
						if cont > biggest then
							biggest = cont;
							mainstart = startcol;
							mainend = y-1;
						end
						cont = 0;
						startcol = 0;
					else

						if startcol == 0 then
							startcol = y;
						end
						bprev = true;
						cont = cont + 1;	
					end
				end
		
				ystart = mainstart;
				yend = mainend;

				chkstart = mainstart + 5;
				chkend = mainend -  5;




				for y = chkstart, chkend do
					landincol = 0;
					contlandincol = 0;
					for x = 1, iW  do
						local i = iW * y + x + 1;

						local tec_check = true
						if beta_tectonic_mounts == 2 then
							if BETA_TECTONIC_LANDS[i] > 0 then
								tec_check = false
							end
						end

						if self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN and tec_check then
							if bLastLand == true then
								landincol = landincol + 1;
								bLastLand = true;
							else
								landincol = 1;
								bLastLand = true;
							end
						else
							if contlandincol < landincol then
								contlandincol = landincol;
							end
							bLastLand = false;
							landincol = 0;
						end
					end



					if contlandincol < chokepoint then

						bYChkFail = true;
					end
				end



				if bXChkFail == true then
					print("X Check: False");
				else
					print("X Check: True");
				end

				if bYChkFail == true then
					print("Y Check: False");
				else
					print("Y Check: True");
				end

				if (bXChkFail == true or bYChkFail == true) then
					print("##############################################");
					print("Map No Good");
					print("##############################################");
					bMapOK = false;
				else
					print("##############################################");
					print("Map Passes");
					print("##############################################");
					bMapOK = true;
				
					cencol = xstart + ((xend - xstart) / 2);
					colshift = (iW/2)-cencol;
					print("Pangaea X Starts At Col: ", xstart, " And Edns At Col: ", xend);
					print("Center X of Lanmass is at Col: ", cencol, "Shift Need: ", colshift);
					xshiftamt = math.ceil(colshift);
					print("Actual Integer Shift Applied: ", xshiftamt);
					if xshiftamt > 0 then
						xshift = 1;
					elseif xshiftamt < 0 then
						xshift = 2;
					else
						xshift = 0;
					end

					print("##############################################");
					cencol = ystart + ((yend - ystart) / 2);
					colshift = (iH/2)-cencol;
					print("Pangaea Y Starts At Col: ", ystart, " And Edns At Col: ", yend);
					print("Center Y of Lanmass is at Col: ", cencol, "Shift Need: ", colshift);
					yshiftamt = math.ceil(colshift);
					print("Actual Integer Shift Applied: ", yshiftamt);
					print("##############################################");
					if yshiftamt > 0 then
						yshift = 1;
					elseif yshiftamt < 0 then
						yshift = 2;
					else
						yshift = 0;
					end
				end

			
			end

		end




		if beta_tectonic_mounts == 1 then

			for x = 0, xstart - 1 do
				for y = 0, iH  do
					destPlotIndex = iW * y + x + 1;
					self.plotTypes[destPlotIndex] = PlotTypes.PLOT_OCEAN;
				end
			end


			for x = xend + 1, iW  do
				for y = 0, iH  do
					destPlotIndex = iW * y + x + 1;
					self.plotTypes[destPlotIndex] = PlotTypes.PLOT_OCEAN;
				end
			end

			for y = 0, ystart - 1 do
				for x = 0, iW  do
					destPlotIndex = iW * y + x + 1;
					self.plotTypes[destPlotIndex] = PlotTypes.PLOT_OCEAN;
				end
			end
		
			for y = yend + 1, iH  do
				for x = 0, iW  do
					destPlotIndex = iW * y + x + 1;
					self.plotTypes[destPlotIndex] = PlotTypes.PLOT_OCEAN;
				end
			end



			if xshift == 1 then
				print("-----------------------------------");
				print("Shifting East........");
				print("-----------------------------------");

				for x = iW, 0, -1 do
					for y = iH, 0, -1 do
						local destPlotIndex = iW * y + x + 1;
						local sourcePlotIndex = destPlotIndex - math.abs(xshiftamt);

						self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
					end	
				end
			elseif xshift == 2 then
				print("-----------------------------------");
				print("Shifting West........");
				print("-----------------------------------");

				for x = 0, iW do
					for y = 0, iH do
						local destPlotIndex = iW * y + x + 1;
						local sourcePlotIndex = destPlotIndex + math.abs(xshiftamt);

						self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
					end	
				end

			else

			end


			if yshift == 1 then
				print("-----------------------------------");
				print("Shifting North........");
				print("-----------------------------------");

				for y = iH, 0, -1 do
					for x = iW, 0, -1 do
						local destPlotIndex = iW * y + x + 1;
						local sourcePlotIndex = destPlotIndex - iW * (math.abs(yshiftamt));

						self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
					end	
				end
			
				local i = math.abs(yshiftamt);
				for y = 0, i do
					for x = 0, iW do
						destPlotIndex = iW * y + x + 1;
						self.plotTypes[destPlotIndex] = PlotTypes.PLOT_OCEAN;
					end
				end

			elseif yshift == 2 then
				print("-----------------------------------");
				print("Shifting South........");
				print("-----------------------------------");

				for y = 0, iH do
					for x = 0, iW do
						local destPlotIndex = iW * y + x + 1;
						local sourcePlotIndex = destPlotIndex + iW * (math.abs(yshiftamt));

						self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
					end	
				end
			
				local i = math.abs(yshiftamt);
				for y = iH-i, iH do
					for x = 0, iW do
						destPlotIndex = iW * y + x + 1;
						self.plotTypes[destPlotIndex] = PlotTypes.PLOT_OCEAN;
					end
				end

			else

			end
		else

			for y = 0, iH-1  do
				local i_left = iW * y + 1 + 1;
				local i_right = iW * y + (iW-1)+1;
				self.plotTypes[i_left] = PlotTypes.PLOT_OCEAN;
				self.plotTypes[i_right] = PlotTypes.PLOT_OCEAN;
			end
			for x = 0, iW-1  do
				local i_top = iW * (iH-1) + x + 1;
				local i_bottom = iW * (1) + x + 1;
				self.plotTypes[i_top] = PlotTypes.PLOT_OCEAN;
				self.plotTypes[i_bottom] = PlotTypes.PLOT_OCEAN;
			end
		end



		



		fjord_distance_setting = 1;
		if fjord_distance_setting ~= 1 then
			if fjord_distance_setting == 2 then
				fjord_d = 20;
			elseif fjord_distance_setting == 3 then
				fjord_d = 15;
			elseif fjord_distance_setting == 4 then
				fjord_d = 12;
			elseif fjord_distance_setting == 5 then
				fjord_d = 10;
			elseif fjord_distance_setting == 6 then
				fjord_d = 8;
			else
				fjord_d = 6;
			end
		

			fjord_length_setting = 1
			if fjord_length_setting == 1 then
				fjord_l = 2;
			elseif fjord_length_setting == 2 then
				fjord_l = 3;
			elseif fjord_length_setting == 3 then
				fjord_l = 4;
			elseif fjord_length_setting == 4 then
				fjord_l = 5;
			else
				fjord_l = 6;
			end

			
			y = 9;
			k = 0;
			while (k == 0)
			do
				x = 6;
				i = 0;
				while (i == 0)
				do
					local PlotIndex = iW * y + x + 1;
					if self.plotTypes[PlotIndex] ~= PlotTypes.PLOT_OCEAN then
						self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
						j = 1;
						while (j < fjord_l - 1 + Map.Rand(3, ""))
						do
							local rdm = Map.Rand(4, "")
							if (y % 2 == 0) then
								if rdm == 0 then
									y = y + 1;
								elseif rdm == 1 then
									y = y - 1;
								else
									x = x + 1;
								end
							else
								x = x + 1;
								if rdm == 0 then
									y = y + 1;
								elseif rdm == 1 then
									y = y - 1;
								end
							end
							if x > iW - 18 then
								i = 1;
							end
							local PlotIndex = iW * y + x + 1;
							self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
							j = j + 1;
						end
						i = 1;
					else
						x = x + 1;
						if x > iW - 18 then
							i = 1;
						end
					end
				end
				y = y + fjord_d - 2 + Map.Rand(5, "");
				if y > iH - 9 then
					k = 1;
				end
				i = 0;
			end
			y = 9;
			k = 0;
			while (k == 0)
			do
				x = iW - 6;
				i = 0;
				while (i == 0)
				do
					local PlotIndex = iW * y + x + 1;
					if self.plotTypes[PlotIndex] ~= PlotTypes.PLOT_OCEAN then
						self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
						j = 1;
						while (j < fjord_l - 1 + Map.Rand(3, ""))
						do
							local rdm = Map.Rand(4, "")
							if (y % 2 == 0) then
								x = x - 1;
								if rdm == 0 then
									y = y + 1;
								elseif rdm == 1 then
									y = y - 1;
								end
							else
								if rdm == 0 then
									y = y + 1;
								elseif rdm == 1 then
									y = y - 1;
								else
									x = x - 1;
								end
							end
							if x < 18 then
								i = 1;
							end
							local PlotIndex = iW * y + x + 1;
							self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
							j = j + 1;
						end
						i = 1;
					else
						x = x - 1;
						if x < 18 then
							i = 1;
						end
					end
				end
				y = y + fjord_d - 2 + Map.Rand(5, "");
				if y > iH - 9 then
					k = 1;
				end
				i = 0;
			end
			x = 10;
			k = 0;
			while (k == 0)
			do
				y = iH - 6;
				i = 0;
				while (i == 0)
				do
					local PlotIndex = iW * y + x + 1;
					if self.plotTypes[PlotIndex] ~= PlotTypes.PLOT_OCEAN then
						self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
						j = 1;
						while (j < fjord_l - 1 + Map.Rand(3, ""))
						do
							local rdm = Map.Rand(10, "")
							if (y % 2 == 0) then
								if rdm < 4 then
									y = y - 1;
								elseif rdm > 5 then
									y = y - 1;
									x = x - 1;
								elseif rdm == 4 then
									x = x - 1;
								else
									x = x + 1;
								end
							else
								if rdm < 4 then
									y = y - 1;
								elseif rdm > 5 then
									y = y - 1;
									x = x + 1;
								elseif rdm == 4 then
									x = x - 1;
								else
									x = x + 1;
								end
							end
							if y < 3 then
								i = 1;
							end
							local PlotIndex = iW * y + x + 1;
							self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
							j = j + 1;
						end
						i = 1;
					else
						y = y - 1;
						if y < 10 then
							i = 1;
						end
					end
				end
				x = x + fjord_d - 2 + Map.Rand(5, "");
				if x > iW - 10 then
					k = 1;
				end
				i = 0;
			end
			x = 10;
			k = 0;
			while (k == 0)
			do
				y = 6;
				i = 0;
				while (i == 0)
				do
					local PlotIndex = iW * y + x + 1;
					if self.plotTypes[PlotIndex] ~= PlotTypes.PLOT_OCEAN then
						self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
						j = 1;
						while (j < fjord_l - 1 + Map.Rand(3, ""))
						do
							local rdm = Map.Rand(10, "")
							if (y % 2 == 0) then
								if rdm < 4 then
									y = y + 1;
								elseif rdm > 5 then
									y = y + 1;
									x = x - 1;
								elseif rdm == 4 then
									x = x - 1;
								else
									x = x + 1;
								end
							else
								if rdm < 4 then
									y = y + 1;
								elseif rdm > 5 then
									y = y + 1;
									x = x + 1;
								elseif rdm == 4 then
									x = x - 1;
								else
									x = x + 1;
								end
							end
							if y > iH - 9 then
								i = 1;
							end
							local PlotIndex = iW * y + x + 1;
							self.plotTypes[PlotIndex] = PlotTypes.PLOT_OCEAN;
							j = j + 1;
						end
						i = 1;
					else
						y = y + 1;
						if y > iH - 9 then
							i = 1;
						end
					end
				end
				x = x + fjord_d - 2 + Map.Rand(5, "");
				if x > iW - 10 then
					k = 1;
				end
				i = 0;
			end
		end
		






























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


		islandSetting = 1
		
		if islandSetting < 26 then	
			islCount = Map.GetCustomOption(15) - 1;
		elseif islandSetting == 26 then
			islCount = Map.Rand(5, "") + 6
		elseif islandSetting == 27 then
			islCount = Map.Rand(5, "") + 8
		else
			islCount = Map.Rand(5, "") + 10
		end
		

		


		maxIslandSize = 3;
		


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
					if _lek_islands_nerfed then
						pullBack = 3;
					end




						if plotTypesTwo[landPlot] == PlotTypes.PLOT_LAND then


							islLandInRing = islLandInRing - pullBack;



							if islLandInRing > minIslandSize and islLandInRing < maxIslandSize then

								local islThresh = 0;
								local landvarDefault = 10;

								local locationRnd = Map.Rand(100, "");


								local hill_thresh = 70;
								local inner_hill_thresh = 50;

								if (locationRnd > inner_hill_thresh) then
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
											


										
												local thisislandvar = Map.Rand(30, "") + landvarDefault;
											


												if _lek_islands_nerfed then
													if ripple_radius == 1  then
														islThresh = Map.Rand(50, "") + thisislandvar;
													elseif ripple_radius == 2 then
														islThresh = Map.Rand(45, "") + (thisislandvar / 1.25);
													elseif ripple_radius == 3 then
														islThresh = Map.Rand(37, "") + (thisislandvar / 1.5);
													else
														islThresh = Map.Rand(30, "") + (thisislandvar / 2);
													end
												else
													if ripple_radius == 1  then
														islThresh = Map.Rand(50, "") + thisislandvar;
													elseif ripple_radius == 2 then
														islThresh = Map.Rand(45, "") + (thisislandvar / 1.25);
													elseif ripple_radius == 3 then
														islThresh = Map.Rand(37, "") + (thisislandvar / 1.5);
													else
														islThresh = Map.Rand(30, "") + (thisislandvar / 2);
													end
												end

												local islRand = Map.Rand(100, "");
												local islHill = Map.Rand(100, "");



												if islRand > islThresh then
													self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN
													landvarDefault = landvarDefault + 5;
												else
													if islHill <= hill_thresh then
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
		

		local centerX = iW / 2;
		local centerY = iH / 2;
		local fracFlags = {FRAC_POLAR = true};

		local baysFrac = nil;
		local iBaysThreshold = nil;
		




		if beta_bay_diffusion == 1 then
			baysFrac = Fractal.Create(iW, iH, 3, fracFlags, -1, -1);
			iBaysThreshold = baysFrac:GetHeight(96);
		else

			baysFrac = Fractal.Create(iW, iH, 5, fracFlags, 7, 7);

			iBaysThreshold = baysFrac:GetHeight(96);

		end
		





		local axis_list = {0.87, 0.81, 0.75};
		local axis_multiplier = axis_list[sea_level];
		local cohesion_list = {0.36, 0.33, 0.30};
		local cohesion_multiplier = cohesion_list[sea_level];
		majorAxis = centerX * cohesion_multiplier;
		minorAxis = centerY * cohesion_multiplier;
		majorAxisSquared = majorAxis * majorAxis;
		minorAxisSquared = minorAxis * minorAxis;
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
							self.plotTypes[i] = PlotTypes.PLOT_OCEAN;


						end










				end
			end
		end
		
		if beta_tectonic_mounts == 2 then
			allcomplete = true
		else

			local iNumLandTilesInUse = 0;

			local iPercent = (iW * iH) * 0.30;

			for y = 0, iH - 1 do
				for x = 0, iW - 1 do
					local i = iW * y + x + 1;
					if self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN then
						iNumLandTilesInUse = iNumLandTilesInUse + 1;
					end
				end
			end

			print("######### Map Failure Check #########");
			print("30% Of Map Area: ", iPercent);
			print("Map Land Tiles: ", iNumLandTilesInUse);

			if iNumLandTilesInUse >= iPercent then
				allcomplete = true;
				print("######### Map Pass #########");
			else
				print("######### Map Failure #########");
			end
		end
	end

	return self.plotTypes;
end

function PlaceImpactAndRipples2(x, y, betaDistanceData, regionNumber)

	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;


	local ripple_values = {97, 95, 92, 88, 83, 77, 70, 62, 51, 41, 30, 18};	
	if ripple_decider == 1 then
		local ripple_values = {97, 95, 92, 89, 69, 57, 24, 15};
	end
	if ripple_decider == 2 then	
		local ripple_values = {97, 95, 92, 88, 83, 77, 70, 62, 51, 41, 30, 18};
	end
	if ripple_decider == 3 then	
		local ripple_values = {99, 98, 97, 89, 88, 83, 77, 70, 62, 51, 41, 30, 18, 12};
	end
	


	local odd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
	local even = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	local nextX, nextY, plot_adjustments;
	local plot = Map.GetPlot(x, y);

	local impactPlotIndex = y * iW + x + 1;



	for ripple_radius, ripple_value in ipairs(ripple_values) do





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

					local ringPlotIndex = realY * iW + realX + 1;

					for region, distanceData in pairs(betaDistanceData) do
						if region ~= regionNumber then
							if distanceData[ringPlotIndex] > 0 then

								local stronger_value = math.max(distanceData[ringPlotIndex], ripple_value);

								local overlap_value = math.min(97, math.floor(stronger_value * 1.4));
								distanceData[ringPlotIndex] = overlap_value;
							else
								distanceData[ringPlotIndex] = ripple_value;
							end
						end
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
	end
end



function PlaceImpactAndRipplesGeneric(x, y, rippleData, radius, impact_value)

	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();

	local ripple_values = table.fill(impact_value, radius)


	local odd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}};
	local even = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	local nextX, nextY, plot_adjustments;
	local plot = Map.GetPlot(x, y);

	local impactPlotIndex = y * iW + x + 1;

	rippleData[impactPlotIndex] = impact_value


	for ripple_radius, ripple_value in ipairs(ripple_values) do





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

					local ringPlotIndex = realY * iW + realX + 1;



					if rippleData[ringPlotIndex] > 0 then

						local stronger_value = math.max(rippleData[ringPlotIndex], ripple_value);

						local overlap_value = math.min(97, math.floor(stronger_value * 1.4));
						rippleData[ringPlotIndex] = overlap_value;
					else
						rippleData[ringPlotIndex] = ripple_value;
					end


				end
				currentX, currentY = nextX, nextY;
			end
		end
	end
end



function AssignStartingPlots:PlaceImpactAndRipples(x, y)


































	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;


	local ripple_values = {97, 95, 92, 88, 83, 77, 70, 62, 51, 41, 30, 18};	
	if ripple_decider == 1 then
		local ripple_values = {97, 95, 92, 89, 69, 57, 24, 15};
	end
	if ripple_decider == 2 then	
		local ripple_values = {97, 95, 92, 88, 83, 77, 70, 62, 51, 41, 30, 18};
	end
	if ripple_decider == 3 then	
		local ripple_values = {99, 98, 97, 89, 88, 83, 77, 70, 62, 51, 41, 30, 18, 12};
	end
	


	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local plot = Map.GetPlot(x, y);


	self:PlaceResourceImpact(x, y, 1, 0)
	self:PlaceResourceImpact(x, y, 2, 3)
	self:PlaceResourceImpact(x, y, 3, 3)
	self:PlaceResourceImpact(x, y, 4, 3)
	if plot:IsCoastalLand(50) then
		if self._lek_collide_coastals then
			self:PlaceResourceImpactCoastalMod(x, y, 5, 3, 4)
		else
			self:PlaceResourceImpact(x, y, 5, 5)
		end
	else
		self:PlaceResourceImpact(x, y, 5, 5)
	end

	self:PlaceResourceImpact(x, y, 6, 4)


	local impactPlotIndex = y * iW + x + 1;
	self.distanceData[impactPlotIndex] = impact_value;
	self.playerCollisionData[impactPlotIndex] = true;
	self.cityStateData[impactPlotIndex] = 1;



	for ripple_radius, ripple_value in ipairs(ripple_values) do





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

					local ringPlotIndex = realY * iW + realX + 1;
					if self.distanceData[ringPlotIndex] > 0 then

						local stronger_value = math.max(self.distanceData[ringPlotIndex], ripple_value);

						local overlap_value = math.min(97, math.floor(stronger_value * 1.4));
						self.distanceData[ringPlotIndex] = overlap_value;
					else
						self.distanceData[ringPlotIndex] = ripple_value;
					end

					if ripple_radius <= 6 then
						self.cityStateData[ringPlotIndex] = 1;
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
	end
end



function AssignStartingPlots:ChooseLocations(args)




	local beta_start_rework = 3

	local beta_tectonic_mounts = 2

	local master_center_x = 0
	local master_center_y = 0

	local biggest_area = Map.FindBiggestArea(false);
	local iBiggestAreaID = biggest_area:GetID();
	



	if beta_tectonic_mounts == 2 then

		local real_tiles = 0

		local region_xs = {}
		local region_ys = {}
		

		for region_y = 0, iH - 1 do
			for region_x = 0, iW - 1 do
				local x = (region_x) % iW;
				local y = (region_y) % iH;
				local i = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				local plotType = plot:GetPlotType();

				if (plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS) then
					real_tiles = real_tiles+1

					table.insert(region_xs, region_x)
					table.insert(region_ys, region_y)
				end
			end
		end
		
		table.sort(region_xs)
		table.sort(region_ys)


		local Xc = region_xs[math.ceil(real_tiles/2)]
		local Yc = region_ys[math.ceil(real_tiles/2)]






		local mass_center_x = Xc
		local mass_center_y = Yc
		

		local bullseyeX = mass_center_x

		local bullseyeY = mass_center_y
		if bullseyeY / 2 ~= math.floor(bullseyeY / 2) then
			bullseyeX = bullseyeX + 0.5;
		end

		master_center_x = bullseyeX
		master_center_y = bullseyeY



		
	end


	if beta_start_rework == 1 then

		print("Map Generation - Choosing Start Locations for Civilizations");
		local args = args or {};
		local mustBeCoast = args.mustBeCoast or false;
		



		self.centerBias = args.centerBias or self.centerBias;
		self.middleBias = args.middleBias or self.middleBias;
		print(string.format("DEV/SAPHT Center bias %d, %d", self.centerBias, self.middleBias))
		self.minFoodInner = args.minFoodInner or self.minFoodInner;
		self.minProdInner = args.minProdInner or self.minProdInner;
		self.minGoodInner = args.minGoodInner or self.minGoodInner;
		self.minFoodMiddle = args.minFoodMiddle or self.minFoodMiddle;
		self.minProdMiddle = args.minProdMiddle or self.minProdMiddle;
		self.minGoodMiddle = args.minGoodMiddle or self.minGoodMiddle;
		self.minFoodOuter = args.minFoodOuter or self.minFoodOuter;
		self.minProdOuter = args.minProdOuter or self.minProdOuter;
		self.minGoodOuter = args.minGoodOuter or self.minGoodOuter;
		self.maxJunk = args.maxJunk or self.maxJunk;


		self:MeasureTerrainInRegions()
		

		self:DetermineRegionTypes()




		local regionAssignList = {};
		local averageFertilityListUnsorted = {};
		local averageFertilityListSorted = {};
		for i, region_data in ipairs(self.regionData) do
			local thisRegionAvgFert = region_data[8];
			table.insert(averageFertilityListUnsorted, {i, thisRegionAvgFert});
			table.insert(averageFertilityListSorted, thisRegionAvgFert);
		end

		table.sort(averageFertilityListSorted);

		local iNumRegions = table.maxn(averageFertilityListSorted);
		for region_order = 1, iNumRegions do
			for loop, data_pair in ipairs(averageFertilityListUnsorted) do
				local unsorted_fert = data_pair[2];
				if averageFertilityListSorted[region_order] == unsorted_fert then
					local unsorted_reg_num = data_pair[1];
					table.insert(regionAssignList, unsorted_reg_num);



					table.remove(averageFertilityListUnsorted, loop);
					break
				end
			end
		end



		
		print("<<<<<<<<<<<<<<<<<< START OF REGION MANIPLUATION >>>>>>>>>>>>>>>>>>>>>");
		
		local iNumCoastNeeded = 0;
		local iNumRiverCivs, iNumPriorityCivs = 0, 0;
		local priority_lists = {};
		local res_reg = table.fill(false, self.iNumCivs);
		local reg_still_active = {};
		
		for loop = 1, self.iNumCivs do
			table.insert(reg_still_active, loop);
		end
		
		for loop = 1, self.iNumCivs do
			local playerNum = self.player_ID_list[loop];
			local player = Players[playerNum];
			local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
			print("Player", playerNum, "of Civ Type", civType);
			local bNeedsCoastalStart = CivNeedsCoastalStart(civType);

			if self.MixedBias and Map.Rand(100, "") >= 60 and CivNeedsPlaceFirstCoastalStart(civType) then
				bNeedsCoastalStart = false;
			end
			if bNeedsCoastalStart == true then
				print("- - - - - - - needs Coastal Start!"); print("-");
				iNumCoastNeeded = iNumCoastNeeded + 1;
			else
				local bNeedsRiverStart = CivNeedsRiverStart(civType)
				if bNeedsRiverStart == true then
					print("- - - - - - - needs River Start!"); print("-");
					iNumRiverCivs = iNumRiverCivs + 1;
				else
					local iNumRegionPriority = GetNumStartRegionPriorityForCiv(civType)
					if iNumRegionPriority > 0 then
						print("- - - - - - - needs Region Priority!"); print("-");
						local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
						iNumPriorityCivs = iNumPriorityCivs + 1;
						priority_lists[playerNum] = table_of_this_civs_priority_needs;
					end
				end
			end
		end
		
		for regcount = 1, iNumRegions do
			print("Region #", regcount, " Is type: ", self.regionTypes[regcount]);
		end
		
		print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
		local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority, iNumReserved = 0, 0, 0, 0;
		local single_priority, multi_priority, fallback_priority = {}, {}, {};
		local single_sorted, multi_sorted = {}, {};

		for playerNum, priority_needs in pairs(priority_lists) do
			local len = table.maxn(priority_needs)
			if len == 1 then
				print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
				
				local found_reg = false;
				

				for regcount = 1, iNumRegions do
					if self.regionTypes[regcount] == priority_needs[1] and found_reg == false then	

						
						if res_reg[regcount] == false then
							print("Region match found for player #", playerNum, " Region #:", regcount);
							print("--");
							res_reg[regcount] = true;
							iNumReserved = iNumReserved + 1;
							found_reg = true;
							table.remove(reg_still_active, regcount);
						end
					end
				end
				

				if found_reg == false then
					local iPriorityType = priority_needs[1];
					local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, reg_still_active)
					print("Fallback region found for player #", playerNum, " Region #:", choose_this_region);
					res_reg[choose_this_region] = true;
					iNumReserved = iNumReserved + 1;
					table.remove(reg_still_active, choose_this_region);
				end
			else
				print("Player#", playerNum, "has multiple Region Priority, this many types:", len);



			end
		end

		if self.BalancedCoastal then
			iRoll = Map.Rand(100, "Roll for extra coast");
			local iNumCoastStart = iNumCoastNeeded;
			if iNumRegions == 6 then
				if iNumCoastStart == 0 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 20 and 1 or 0) + (iRoll >= 45 and 1 or 0) + (iRoll >= 95 and 1 or 0);
				end
				if iNumCoastStart == 1 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 15 and 1 or 0) + (iRoll >= 90 and 1 or 0)
				end
				if iNumCoastStart == 2 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 90 and 1 or 0)
				end
			end
			
			if iNumRegions == 8 then
				if iNumCoastStart == 0 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 15 and 1 or 0) + (iRoll >= 35 and 1 or 0) + (iRoll >= 65 and 1 or 0) + (iRoll >= 85 and 1 or 0)
				end
				if iNumCoastStart == 1 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 10 and 1 or 0) + (iRoll >= 55 and 1 or 0) + (iRoll >= 85 and 1 or 0)
				end
				if iNumCoastStart == 2 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 35 and 1 or 0) + (iRoll >= 80 and 1 or 0)  + (iRoll >= 95 and 1 or 0)
				end
				if iNumCoastStart == 3 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 60 and 1 or 0)  + (iRoll >= 90 and 1 or 0)
				end
				if iNumCoastStart == 4 then
					iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 75 and 1 or 0)  + (iRoll >= 95 and 1 or 0)
				end
			end
			

			local i = 1;
			while iNumRegions - iNumReserved > iNumCoastNeeded and i <= 100 do
			iRoll = Map.Rand(iNumRegions, "Roll region number to clear");
				if res_reg[iRoll] then
					res_reg[iRoll] = false;
					iNumReserved = iNumReserved - 1;
				end
				i = i + 1;
			end
		end

		
		for assignIndex = 1, iNumRegions do
			local currentRegionNumber = regionAssignList[assignIndex];
			local bSuccessFlag = false;
			local bForcedPlacementFlag = false;
			
			print("Region #" .. currentRegionNumber);
			print("Num coastal still needed " .. tostring(iNumCoastNeeded));


			if res_reg[currentRegionNumber] == false and iNumCoastNeeded > 0 then

				bSuccessFlag, bForcedPlacementFlag = self:FindCoastalStart(currentRegionNumber)
				iNumCoastNeeded = iNumCoastNeeded - 1;
			else
				print("Don't Allow Spawning on Coast: " .. tostring(self.NoCoastInland));

				bSuccessFlag, bForcedPlacementFlag = self:FindStart(currentRegionNumber, self.NoCoastInland)
			end
			

			print("- - -");
			print("Start Plot for Region #", currentRegionNumber, " was successful: ", bSuccessFlag);
			print("Start Plot for Region #", currentRegionNumber, " was forced: ", bForcedPlacementFlag);		
		end













		




	else


			beta_cliffs = 2

			print("Map Generation - Choosing Start Locations for Civilizations");
			local args = args or {};

			local mustBeCoast = args.mustBeCoast or false;
			



			self.centerBias = args.centerBias or self.centerBias;
			self.middleBias = args.middleBias or self.middleBias;
			print(string.format("DEV/SAPHT Center bias %d, %d", self.centerBias, self.middleBias))
			self.minFoodInner = args.minFoodInner or self.minFoodInner;
			self.minProdInner = args.minProdInner or self.minProdInner;
			self.minGoodInner = args.minGoodInner or self.minGoodInner;
			self.minFoodMiddle = args.minFoodMiddle or self.minFoodMiddle;
			self.minProdMiddle = args.minProdMiddle or self.minProdMiddle;
			self.minGoodMiddle = args.minGoodMiddle or self.minGoodMiddle;
			self.minFoodOuter = args.minFoodOuter or self.minFoodOuter;
			self.minProdOuter = args.minProdOuter or self.minProdOuter;
			self.minGoodOuter = args.minGoodOuter or self.minGoodOuter;
			self.maxJunk = args.maxJunk or self.maxJunk;


			self:MeasureTerrainInRegions()
			

			self:DetermineRegionTypes()




			local regionAssignList = {};
			local averageFertilityListUnsorted = {};
			local averageFertilityListSorted = {};
			for i, region_data in ipairs(self.regionData) do
				local thisRegionAvgFert = region_data[8];
				table.insert(averageFertilityListUnsorted, {i, thisRegionAvgFert});
				table.insert(averageFertilityListSorted, thisRegionAvgFert);
			end

			table.sort(averageFertilityListSorted);

			local iNumRegions = table.maxn(averageFertilityListSorted);
			for region_order = 1, iNumRegions do
				for loop, data_pair in ipairs(averageFertilityListUnsorted) do
					local unsorted_fert = data_pair[2];
					if averageFertilityListSorted[region_order] == unsorted_fert then
						local unsorted_reg_num = data_pair[1];
						table.insert(regionAssignList, unsorted_reg_num);



						table.remove(averageFertilityListUnsorted, loop);
						break
					end
				end
			end



			
			print("<<<<<<<<<<<<<<<<<< START OF REGION MANIPLUATION >>>>>>>>>>>>>>>>>>>>>");
			
			local iNumCoastNeeded = 0;
			local iNumRiverCivs, iNumPriorityCivs = 0, 0;
			local priority_lists = {};
			local res_reg = table.fill(false, self.iNumCivs);
			local reg_still_active = {};
			
			for loop = 1, self.iNumCivs do
				table.insert(reg_still_active, loop);
			end
			
			for loop = 1, self.iNumCivs do
				local playerNum = self.player_ID_list[loop];
				local player = Players[playerNum];
				local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
				print("Player", playerNum, "of Civ Type", civType);
				local bNeedsCoastalStart = CivNeedsCoastalStart(civType);

				if self.MixedBias and Map.Rand(100, "") >= 60 and CivNeedsPlaceFirstCoastalStart(civType) then
					bNeedsCoastalStart = false;
				end
				if bNeedsCoastalStart == true then
					print("- - - - - - - needs Coastal Start!"); print("-");
					iNumCoastNeeded = iNumCoastNeeded + 1;
				else
					local bNeedsRiverStart = CivNeedsRiverStart(civType)
					if bNeedsRiverStart == true then
						print("- - - - - - - needs River Start!"); print("-");
						iNumRiverCivs = iNumRiverCivs + 1;
					else
						local iNumRegionPriority = GetNumStartRegionPriorityForCiv(civType)
						if iNumRegionPriority > 0 then
							print("- - - - - - - needs Region Priority!"); print("-");
							local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
							iNumPriorityCivs = iNumPriorityCivs + 1;
							priority_lists[playerNum] = table_of_this_civs_priority_needs;
						end
					end
				end
			end
			
			for regcount = 1, iNumRegions do
				print("Region #", regcount, " Is type: ", self.regionTypes[regcount]);
			end
			
			print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
			local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority, iNumReserved = 0, 0, 0, 0;
			local single_priority, multi_priority, fallback_priority = {}, {}, {};
			local single_sorted, multi_sorted = {}, {};

			for playerNum, priority_needs in pairs(priority_lists) do
				local len = table.maxn(priority_needs)
				if len == 1 then
					print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
					
					local found_reg = false;
					

					for regcount = 1, iNumRegions do
						if self.regionTypes[regcount] == priority_needs[1] and found_reg == false then	

							
							if res_reg[regcount] == false then
								print("Region match found for player #", playerNum, " Region #:", regcount);
								print("--");
								res_reg[regcount] = true;
								iNumReserved = iNumReserved + 1;
								found_reg = true;
								table.remove(reg_still_active, regcount);
							end
						end
					end
					

					if found_reg == false then
						local iPriorityType = priority_needs[1];
						local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, reg_still_active)
						print("Fallback region found for player #", playerNum, " Region #:", choose_this_region);
						res_reg[choose_this_region] = true;
						iNumReserved = iNumReserved + 1;
						table.remove(reg_still_active, choose_this_region);
					end
				else
					print("Player#", playerNum, "has multiple Region Priority, this many types:", len);



				end
			end

			if self.BalancedCoastal then
				iRoll = Map.Rand(100, "Roll for extra coast");
				local iNumCoastStart = iNumCoastNeeded;
				if iNumRegions == 6 then
					if iNumCoastStart == 0 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 20 and 1 or 0) + (iRoll >= 45 and 1 or 0) + (iRoll >= 95 and 1 or 0);
					end
					if iNumCoastStart == 1 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 15 and 1 or 0) + (iRoll >= 90 and 1 or 0)
					end
					if iNumCoastStart == 2 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 90 and 1 or 0)
					end
				end
				
				if iNumRegions == 8 then
					if iNumCoastStart == 0 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 15 and 1 or 0) + (iRoll >= 35 and 1 or 0) + (iRoll >= 65 and 1 or 0) + (iRoll >= 85 and 1 or 0)
					end
					if iNumCoastStart == 1 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 10 and 1 or 0) + (iRoll >= 55 and 1 or 0) + (iRoll >= 85 and 1 or 0)
					end
					if iNumCoastStart == 2 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 35 and 1 or 0) + (iRoll >= 80 and 1 or 0)  + (iRoll >= 95 and 1 or 0)
					end
					if iNumCoastStart == 3 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 60 and 1 or 0)  + (iRoll >= 90 and 1 or 0)
					end
					if iNumCoastStart == 4 then
						iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 75 and 1 or 0)  + (iRoll >= 95 and 1 or 0)
					end
				end
				

				local i = 1;
				while iNumRegions - iNumReserved > iNumCoastNeeded and i <= 100 do
				iRoll = Map.Rand(iNumRegions, "Roll region number to clear");
					if res_reg[iRoll] then
						res_reg[iRoll] = false;
						iNumReserved = iNumReserved - 1;
					end
					i = i + 1;
				end
			end



		local impassable_terrain = table.fill(0.0, iW * iH)

		for x = 0, iW-1 do
			for y = 0, iH-1 do
				local i = xy_to_i(x, y, iW, iH)
				local plot_type = Map.GetPlot(x, y):GetPlotType()
				if plot_type == PlotTypes.PLOT_OCEAN or plot_type == PlotTypes.PLOT_MOUNTAIN then
					impassable_terrain[i] = -1.0
				end
			end
		end

		local coastal_assignments = {}
		local betaDistanceData = {}
		local startPlotIndices = {}

		local wrapX = Map:IsWrapX();
		local wrapY = Map:IsWrapY();


		for assignIndex = 1, iNumRegions do
			local currentRegionNumber = regionAssignList[assignIndex];
			betaDistanceData[currentRegionNumber] = table.fill(0, iW * iH);
			if res_reg[currentRegionNumber] == false and iNumCoastNeeded > 0 then

				coastal_assignments[currentRegionNumber] = true
				iNumCoastNeeded = iNumCoastNeeded - 1;
			else
				coastal_assignments[currentRegionNumber] = false
			end		
		end

		local bootstrap_iters = 9

		local region_centers = {}

		

		for b_iter = 1, bootstrap_iters do

			for assignIndex = 1, iNumRegions do
				local region_number = regionAssignList[assignIndex];
				local isCoastal = coastal_assignments[region_number]

				local region_data_table = self.regionData[region_number];
				local iWestX = region_data_table[1];
				local iSouthY = region_data_table[2];
				local iWidth = region_data_table[3];
				local iHeight = region_data_table[4];
				local iAreaID = region_data_table[5];
				local iMembershipEastX = iWestX + iWidth - 1;
				local iMembershipNorthY = iSouthY + iHeight - 1;
				local terrainCounts = self.regionTerrainCounts[region_number];
				local region_type = self.regionTypes[region_number];

				local plotScores = {}






				local Xsum = 0
				local Ysum = 0
				local real_tiles = 0

				local region_xs = {}
				local region_ys = {}
				

				for region_y = 0, iHeight - 1 do
					for region_x = 0, iWidth - 1 do
						local x = (region_x + iWestX) % iW;
						local y = (region_y + iSouthY) % iH;
						local i = y * iW + x + 1;
						local plot = Map.GetPlot(x, y);
						local plotType = plot:GetPlotType();

						if plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS then
							Xsum = Xsum+region_x
							Ysum = Ysum+region_y
							real_tiles = real_tiles+1

							table.insert(region_xs, region_x)
							table.insert(region_ys, region_y)
						end
					end
				end

				table.sort(region_xs)
				table.sort(region_ys)


				local Xc = region_xs[math.ceil(real_tiles/2)]
				local Yc = region_ys[math.ceil(real_tiles/2)]




				local mass_center_x = Xc+iWestX
				local mass_center_y = Yc+iSouthY
				

				local bullseyeX = mass_center_x
				if bullseyeX < iWestX then
					bullseyeX = bullseyeX + iW;
				end

				local bullseyeY = mass_center_y
				if bullseyeY < iSouthY then
					bullseyeY = bullseyeY + iH;
				end
				if bullseyeY / 2 ~= math.floor(bullseyeY / 2) then
					bullseyeX = bullseyeX + 0.5;
				end

				region_centers[assignIndex] = {math.floor(bullseyeX % iW), math.floor(bullseyeY % iH)}

				for region_y = 0, iHeight - 1 do
					for region_x = 0, iWidth - 1 do
						local x = (region_x + iWestX) % iW;
						local y = (region_y + iSouthY) % iH;
						local i = y * iW + x + 1;
						local plot = Map.GetPlot(x, y);
						local plotType = plot:GetPlotType();

						local plotMult = 1.0;


						
						local adjusted_x = x;
						local adjusted_y = y;
						if y / 2 ~= math.floor(y / 2) then
							adjusted_x = x + 0.5;
						end
						
						if x < iWestX then
							adjusted_x = adjusted_x + iW;
						end
						if y < iSouthY then
							adjusted_y = y + iH;
						end
						local fDistance = math.sqrt( (adjusted_x - bullseyeX)^2 + (adjusted_y - bullseyeY)^2 );
						local fMasterDistance = math.sqrt( (adjusted_x - master_center_x)^2 + (adjusted_y - master_center_y)^2 );
						local fMedDistance = math.sqrt( (mass_center_x - master_center_x)^2 + (mass_center_y - master_center_y)^2 );
						fMasterDistance = fMasterDistance/math.max(1e-10, fMedDistance)




						

						local reg_factor = 1
						if beta_start_rework == 2 then
							reg_factor = (0.75-(fDistance/math.max(1, math.min(iWidth, iHeight))))*4;
							reg_factor = 1.0/(1+(math.exp(-reg_factor))^2);
						elseif beta_start_rework == 3 then
							reg_factor = (0.5-(fDistance/math.max(1, math.min(iWidth, iHeight))))*4;
							reg_factor = 1.0/(1+(math.exp(-reg_factor))^2);
						else
							reg_factor = (0.25-(fDistance/math.max(1, math.min(iWidth, iHeight))))*6;
							reg_factor = 1.0/(1+(math.exp(-reg_factor))^2);
						end
						
						
						plotMult = plotMult*reg_factor;

						if (plotType ~= PlotTypes.PLOT_HILLS) and (plotType ~= PlotTypes.PLOT_LAND) then
							plotMult = -1.0;
						end
						if self.plotDataIsNextToCoast[i] == true and (isCoastal ~= true) then
							plotMult = plotMult*0.003;
						elseif self.plotDataIsThreeFromCoast[i] == true and (isCoastal ~= true) then
							plotMult = plotMult*0.01;
						end
						if (self.plotDataIsCoastal[i] == true) and (isCoastal ~= true) then
							plotMult = -1.0
						end
						if (plot:IsCoastalLand(50) ~= true) and (isCoastal == true) then
							plotMult = plotMult*0.00001
						end
						if choke_check(i, impassable_terrain) or impassable_check(i, impassable_terrain) then
							plotMult = plotMult*0.00001
						end
						if plotMult > 0.0 then
							if plot:IsRiverSide() then
								plotMult = plotMult*1.3
							elseif plot:IsFreshWater() then
								plotMult = plotMult*1.2
							end
						end
						
						if betaDistanceData[region_number][i] > 0 then
							plotMult = plotMult*(1.0-(betaDistanceData[region_number][i]/100.0))
						end

						if plot:GetArea() ~= iBiggestAreaID then
							plotMult = 0.0
						end

						if beta_inland_prox_penalty == 2 and (isCoastal ~= true) then
							local coast_prox = GetCoastDistance(x, y)
							if coast_prox < 3 then
								plotMult = 0.0
							else
								local coast_penalty = (1.0/(1+(math.exp(-(coast_prox-3)*1.5))));
								plotMult = plotMult*coast_penalty
							end
						end
						
						if plotMult <= 0.0 then
							plotScores[i] = 0.0
						else
							local landScore = 0.0
							local blockingScore = 0.0

							local isEvenY = true;
							if y / 2 > math.floor(y / 2) then
								isEvenY = false;
							end
							local adjacentMountainCount = 0;
							local foodTotal, prodTotal, goodTotal, junkTotal, riverTotal, coastScore = 0, 0, 0, 0, 0, 0;
							local search_table = {};
							local innerRingScore = 0;
							local result = self:MeasureSinglePlot(x, y, region_type, 0)
							if result[2] then
								innerRingScore = innerRingScore + 4;
								if result[5] then
									innerRingScore = innerRingScore + 4;
								end
							end
							if result[3] then
								goodTotal = goodTotal + 1;
								if result[5] then
									goodTotal = goodTotal + 1;
								end
							end
							if plot:IsRiverSide() or plot:IsFreshWater() then
								riverTotal = riverTotal + 4;
							end

							if isEvenY then
								search_table = self.firstRingYIsEven;
							else
								search_table = self.firstRingYIsOdd;
							end

							for loop, plot_adjustments in ipairs(search_table) do
								local searchX, searchY;
								if wrapX then
									searchX = (x + plot_adjustments[1]) % iW;
								else
									searchX = x + plot_adjustments[1];
								end
								if wrapY then
									searchY = (y + plot_adjustments[2]) % iH;
								else
									searchY = y + plot_adjustments[2];
								end

								if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

									junkTotal = junkTotal + 1;
								else
									local searchPlot = Map.GetPlot(searchX, searchY);
									local result = self:MeasureSinglePlot(searchX, searchY, region_type, 1)

									if searchPlot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN or searchPlot:IsLake() then
										blockingScore = blockingScore + 4.0
									end

									if result[4] then
										junkTotal = junkTotal + 1;
										if searchPlot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN and adjacentMountainCount == 0 then

											adjacentMountainCount = adjacentMountainCount + 1;
										elseif result[5] then
											junkTotal = junkTotal + 1;
										end

										if searchPlot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN and beta_cliffs and isCoastal == true then
											junkTotal = junkTotal + 20;
										end

									else
										if result[1] then
											foodTotal = foodTotal + 1;
											if result[5] then
												foodTotal = foodTotal + 1;
											end
										end
										if result[2] then
											prodTotal = prodTotal + 1;
											if result[5] then
												prodTotal = prodTotal + 1;
											end
										end
										if result[3] then
											goodTotal = goodTotal + 1;
											if result[5] then
												goodTotal = goodTotal + 1;
											end
										end
										if searchPlot:IsRiverSide() or plot:IsFreshWater() then
											riverTotal = riverTotal + 2;
										end
									end
								end
							end

							if foodTotal < self.minFoodInner then
								goodSoFar = false;
							elseif prodTotal < self.minProdInner then
								goodSoFar = false;
							elseif goodTotal < self.minGoodInner then
								goodSoFar = false;
							end

							local weightedFoodInner = {0, 8, 14, 19, 22, 24, 25};
							local foodResultInner = weightedFoodInner[foodTotal + 1];
							local weightedProdInner = {0, 10, 16, 20, 20, 12, 0};
							local prodResultInner = weightedProdInner[prodTotal + 1];
							local goodResultInner = goodTotal * 2;
							innerRingScore = innerRingScore + foodResultInner + prodResultInner + goodResultInner + riverTotal - (junkTotal * 3);
							

							if isEvenY then
								search_table = self.secondRingYIsEven;
							else
								search_table = self.secondRingYIsOdd;
							end

							for loop, plot_adjustments in ipairs(search_table) do
								local searchX, searchY;
								if wrapX then
									searchX = (x + plot_adjustments[1]) % iW;
								else
									searchX = x + plot_adjustments[1];
								end
								if wrapY then
									searchY = (y + plot_adjustments[2]) % iH;
								else
									searchY = y + plot_adjustments[2];
								end
								if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

									junkTotal = junkTotal + 1;
								else
									local result = self:MeasureSinglePlot(searchX, searchY, region_type, 2)
									local searchPlot = Map.GetPlot(searchX, searchY);

									if searchPlot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN or searchPlot:IsLake() then
										blockingScore = blockingScore + 2.0
									end

									if result[4] then
										junkTotal = junkTotal + 1;
										if result[5] then
											junkTotal = junkTotal + 1;
										end
									else
										if result[1] then
											foodTotal = foodTotal + 1;
											if result[5] then
												foodTotal = foodTotal + 1;
											end
										end
										if result[2] then
											prodTotal = prodTotal + 1;
											if result[5] then
												prodTotal = prodTotal + 1;
											end
										end
										if result[3] then
											goodTotal = goodTotal + 1;
											if result[5] then
												goodTotal = goodTotal + 1;
											end
										end
										if searchPlot:IsRiverSide() or plot:IsFreshWater() then
											riverTotal = riverTotal + 2;
										end
									end
								end
							end


							if foodTotal < self.minFoodMiddle then
								goodSoFar = false;

							elseif prodTotal < self.minProdMiddle then
								goodSoFar = false;
							elseif goodTotal < self.minGoodMiddle then
								goodSoFar = false;
							end


							local weightedFoodMiddle = {0, 2, 5, 10, 20, 25, 28, 30, 32, 34, 35};
							local foodResultMiddle = 35;
							if foodTotal < 10 then
								foodResultMiddle = weightedFoodMiddle[foodTotal + 1];
							end
							local weightedProdMiddle = {0, 10, 20, 25, 30, 35};
							local effectiveProdTotal = prodTotal;
							if foodTotal * 2 < prodTotal then
								effectiveProdTotal = math.ceil(foodTotal / 2);
							end
							local prodResultMiddle = 35;
							if effectiveProdTotal < 5 then
								prodResultMiddle = weightedProdMiddle[effectiveProdTotal + 1];
							end
							local goodResultMiddle = goodTotal * 2;
							local middleRingScore = foodResultMiddle + prodResultMiddle + goodResultMiddle + riverTotal - (junkTotal * 3);
							

							if isEvenY then
								search_table = self.thirdRingYIsEven;
							else
								search_table = self.thirdRingYIsOdd;
							end

							for loop, plot_adjustments in ipairs(search_table) do
								local searchX, searchY;
								if wrapX then
									searchX = (x + plot_adjustments[1]) % iW;
								else
									searchX = x + plot_adjustments[1];
								end
								if wrapY then
									searchY = (y + plot_adjustments[2]) % iH;
								else
									searchY = y + plot_adjustments[2];
								end
								if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

									junkTotal = junkTotal + 1;
								else
									local result = self:MeasureSinglePlot(searchX, searchY, region_type, 3)
									local searchPlot = Map.GetPlot(searchX, searchY);
									if searchPlot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN or searchPlot:IsLake() then
										blockingScore = blockingScore + 1.0
									end

									if result[4] then
										junkTotal = junkTotal + 1;
										if result[5] then
											junkTotal = junkTotal + 1;
										end
									else
										if result[1] then
											foodTotal = foodTotal + 1;
											if result[5] then
												foodTotal = foodTotal + 1;
											end
										end
										if result[2] then
											prodTotal = prodTotal + 1;
											if result[5] then
												prodTotal = prodTotal + 1;
											end
										end
										if result[3] then
											goodTotal = goodTotal + 1;
											if result[5] then
												goodTotal = goodTotal + 1;
											end
										end
										if searchPlot:IsRiverSide() or plot:IsFreshWater() then
											riverTotal = riverTotal + 2;
										end
									end
								end
							end


							if foodTotal < self.minFoodOuter then
								goodSoFar = false;
							elseif prodTotal < self.minProdOuter then
								goodSoFar = false;
							elseif goodTotal < self.minGoodOuter then
								goodSoFar = false;
							end
							if junkTotal > self.maxJunk then
								goodSoFar = false;
							end


							local outerRingScore = foodTotal + prodTotal + goodTotal + riverTotal - (junkTotal * 2);
							local finalScore = innerRingScore + middleRingScore + outerRingScore + coastScore;

							if goodSoFar == true then
								landScore = math.max(0, finalScore)
							else
								landScore = math.max(finalScore)*0.7
							end

							local block_factor = (0.75-((blockingScore*1.5)/(math.max(1, math.min(iWidth, iHeight))*8.0)))*4;
							if beta_tectonic_mounts == 2 then
								local center_mult = 2.0*(1.0/(1+(math.exp(-(fMasterDistance-1.0)*1.5))^2));
								block_factor = 10.0*block_factor*center_mult-(fMasterDistance-1.0)*1.5
							end
							local blocking_mult = 1.0/(1+(math.exp(-block_factor))^2);

							plotScores[i] = plotMult*landScore*blocking_mult
						end
					end
				end


				local iBest = 0;
				local scoreBest = -1;
				for region_y = 0, iHeight - 1 do
					for region_x = 0, iWidth - 1 do
						local x = (region_x + iWestX) % iW;
						local y = (region_y + iSouthY) % iH;
						local i = y * iW + x + 1;

						if plotScores[i] > scoreBest then
							iBest = i
							scoreBest = plotScores[i]
						end
					end
				end
				startPlotIndices[region_number] = iBest;
			end






			for assignIndex = 1, iNumRegions do
				local currentRegionNumber = regionAssignList[assignIndex];
				for y = 0, iH - 1 do
					for x = 0, iW - 1 do
						local i = xy_to_i(x, y, iW, iH)
						betaDistanceData[currentRegionNumber][i] = math.ceil(betaDistanceData[currentRegionNumber][i]/7)
					end
				end

			end

			for assignIndex = 1, iNumRegions do
				local currentRegionNumber = regionAssignList[assignIndex];
				local i = startPlotIndices[currentRegionNumber];
				local x = (i - 1) % iW;
				local y = (i - x - 1) / iW;
				PlaceImpactAndRipples2(x, y, betaDistanceData, currentRegionNumber);
				print("Center X:" .. region_centers[assignIndex][1])
				PlaceImpactAndRipples2(region_centers[assignIndex][1], region_centers[assignIndex][2], betaDistanceData, currentRegionNumber);
			end
		end


		for assignIndex = 1, iNumRegions do
			local currentRegionNumber = regionAssignList[assignIndex];
			local i = startPlotIndices[currentRegionNumber]
			local x = (i - 1) % iW;
			local y = (i - x - 1) / iW;
			self.startingPlots[currentRegionNumber] = {x, y, 1}
			self:PlaceImpactAndRipples(x, y)
		end
	end
end



function AssignStartingPlots:MeasureSinglePlot(x, y, region_type, distance_from_city)



	local data = table.fill(false, 5);












	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType()
	local terrainType = plot:GetTerrainType()
	local featureType = plot:GetFeatureType()
	
	if plotType == PlotTypes.PLOT_MOUNTAIN then
		data[4] = true;
		return data
	elseif plotType == PlotTypes.PLOT_OCEAN then
		if featureType == FeatureTypes.FEATURE_ICE then
			data[4] = true;
		elseif plot:IsLake() then
			data[1] = true;
		elseif terrainType == TerrainTypes.TERRAIN_COAST then 
			data[1] = true;
			if self.method == 3 or self.method == 4 then
				data[3] = true;
			end
		end

		return data
	end

	if featureType == FeatureTypes.FEATURE_JUNGLE and distance_from_city ~= 0 then
		if plotType == PlotTypes.PLOT_HILLS then
			data[2] = true;
		end

		data[1] = true;









		return data
	elseif featureType == FeatureTypes.FEATURE_FOREST and distance_from_city ~= 0 then
		data[2] = true;
		if plotType == PlotTypes.PLOT_HILLS then
			data[3] = true;
		else

			if terrainType ~= TerrainTypes.TERRAIN_TUNDRA then
				data[1] = true;
			end
		end

		return data
	elseif featureType == FeatureTypes.FEATURE_OASIS then
		data[1] = true;
		data[3] = true;
		return data
	elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
		data[1] = true;
		data[3] = true;
		return data
	elseif featureType == FeatureTypes.FEATURE_MARSH and distance_from_city ~= 0 then
		

		








		return data
	end

	if plotType == PlotTypes.PLOT_HILLS then
		data[2] = true;
		data[3] = true;
		return data
	end
	

	if terrainType == TerrainTypes.TERRAIN_SNOW then
		data[4] = true;
		data[5] = true;
		return data
		
	elseif terrainType == TerrainTypes.TERRAIN_DESERT then



		data[4] = true;

		return data

	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then







		return data

	elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
		data[3] = true;



		return data

	elseif terrainType == TerrainTypes.TERRAIN_GRASS then
		data[1] = true;
		data[3] = true;
		return data
	end


	print("Encountered non-standard terrain.");
	return data
end




function PinchTectonics()
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			i = xy_to_i(x, y, iW, iH)
			if BETA_TECTONIC_LANDS[i] == 2 then
				plot = Map.GetPlot(x, y)
				plot:SetPlotType(PlotTypes.PLOT_OCEAN)
				local adj_is = adj_is_cache[i]
				for loop, adj_i in ipairs(adj_is) do
					local adj_x, adj_y = i_to_xy(adj_i, iW, iH)
					local adj_plot = Map.GetPlot(adj_x, adj_y)
					if adj_plot ~= nil then
						if adj_plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
							adj_plot:SetPlotType(PlotTypes.PLOT_HILLS)
						end
					end
				end
			end 
		end
	end
	Map.RecalculateAreas()
end

function GeneratePlotTypes()

	print("Generating Plot Types (Lua Pangaea) ...");
	
	local fractal_world = PangaeaFractalWorld.Create();

	local plotTypes = fractal_world:GeneratePlotTypes();

	
	SetPlotTypes(plotTypes);
	if beta_tectonic_mounts == 2 then
		PinchTectonics();
	end

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
	


	local terrainTypes = terraingen:GenerateTerrain();
	
	SetTerrainTypes(terrainTypes);


	Map.RecalculateAreas();
	FixCoastLine();
	
	FixIslands();

end


function FixIslands()


	local biggest_area = Map.FindBiggestArea(false);
	local iAreaID = biggest_area:GetID();

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlotByIndex(i-1);
			plotAreaID = plot:GetArea();
			if plotAreaID ~= iAreaID then
				local terrainType = plot:GetTerrainType();
				local plotType = plot:GetPlotType();

				if terrainType == TerrainTypes.TERRAIN_TUNDRA then
					if plotType ~= PlotTypes.PLOT_HILLS then

						local tundratoplains = Map.Rand(100, "Plains Spawn Chance");
						if tundratoplains >= 30 then
							plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true);
						end
					end
				end
			end
		end
	end
end

function LakeCheck(x, y)

	local adj_is1 = GetAdjacent(x,y);
	for loop, adj_i1 in ipairs(adj_is1) do
		local adjx1, adjy1 = i_to_xy(adj_i1, iW, iH);
		local adj_plot1 = Map.GetPlot(adjx1, adjy1);
		if adj_plot1:IsLake() == true then
			return true
		end
	end
	return false
end



function GetCoastDistance(x, y)

	if has_cached_coast == false then
		CacheCoast()
	end
	local i = xy_to_i(x, y, iW, iH);
	if coast_cache[i] ~= nil then
		return coast_cache[i]
	else
		return -1
	end
end


function CacheCoast()
	local coast_adj = {};

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlot(x, y);
			if plot:IsCoastalLand(50) then
				coast_cache[i] = 0;
				local adj_is1 = adj_is_cache[i]
				for loop, adj_i1 in ipairs(adj_is1) do
					if(adj_i1 ~= -1) then

						local adjx1, adjy1 = i_to_xy(adj_i1, iW, iH);
						local adj_plot1 = Map.GetPlot(adjx1, adjy1);
						if adj_plot1 ~= nil then
							if (adj_plot1:IsCoastalLand(50) ~= true) and (adj_plot1:GetPlotType() ~= PlotTypes.PLOT_OCEAN) then
								coast_adj[adj_i1] = 1;
							end
						else
							coast_adj[adj_i1] = 1;
						end
					end
				end
			end
		end
	end
	local loop_depth = 1
	while tablelength(coast_adj) > 0 and loop_depth < 100 do
		local coast_adj_temp = {}
		for i, dumby in pairs(coast_adj) do
			coast_cache[i] = loop_depth

			local adj_is1 = adj_is_cache[i]
			for loop, adj_i1 in ipairs(adj_is1) do
				if (coast_cache[adj_i1] == nil) and (coast_adj[adj_i1] == nil) then
					coast_adj_temp[adj_i1] = 1;
				end
			end
		end
		coast_adj = {}
		for i, dumby in pairs(coast_adj_temp) do
			coast_adj[i] = 1
		end
		loop_depth = loop_depth+1
	end
	has_cached_coast = true
end

function AssignStartingPlots:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)





	local plot = Map.GetPlot(x, y);
	local area = plot:GetArea()
	local biggest_area = Map.FindBiggestArea(false);
	local iAreaID = biggest_area:GetID();

	if beta_coast_zone ~= 1 then
		if plot:IsCoastalLand(50) == false then
			local coast_dist = GetCoastDistance(x, y)
			if coast_dist ~= -1 and coast_dist <= beta_coast_zone then
				return false
			end
		end
	end

	if beta_tectonic_mounts == 2 then
		local i = xy_to_i(x, y, iW, iH)
		local adj_is = adj_is_cache[i]
		local adj_dead = 0
		local adj2_dead = 0
		for loop, adj_i in ipairs(adj_is) do
			local adj_x, adj_y = i_to_xy(adj_i, iW, iH)
			local adj_plot = Map.GetPlot(adj_x, adj_y)
			if adj_plot == nil then
				adj_dead = adj_dead+1
				adj2_dead = adj2_dead+4
			else
				if adj_plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN or adj_plot:GetPlotType() == PlotTypes.PLOT_OCEAN then
					adj_dead = adj_dead+1
					adj2_dead = adj2_dead+1
				end

				

			end
		end
		if adj_dead >= 3 then
			return false
		end
	end

	if self.method == 1 then
		if area_ID ~= iAreaID then
			return false
		end
	end

	if area ~= area_ID and area_ID ~= -1 then
		return false
	end
	local plotType = plot:GetPlotType()
	if plotType == PlotTypes.PLOT_OCEAN then
		return false
	end
	if beta_cliffs == 1 then
		if plotType == PlotTypes.PLOT_MOUNTAIN then
			return false
		end
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_SNOW then
		return false
	end
	local featureType = plot:GetFeatureType()
	if featureType == FeatureTypes.FEATURE_OASIS then
		return false
	end
	local plotIndex = y * iW + x + 1;
	if self.cityStateData[plotIndex] > 0 and force_it == false then
		return false
	end
	if self.playerCollisionData[plotIndex] == true and ignore_collisions == false then

		return false
	end
	if self.plotDataIsNextToCoast[plotIndex] == true then
		return false
	end
	return true
end


function FixCoastLine()



	local biggest_area = Map.FindBiggestArea(false);
	local iAreaID = biggest_area:GetID();

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = iW * y + x + 1;
			local plot = Map.GetPlotByIndex(i-1);
			plotAreaID = plot:GetArea();

			if plotAreaID == iAreaID then

				local plotType = plot:GetPlotType();
				if plot:IsCoastalLand(50) and (plot:IsFreshWater() == false) then 
					if plotType ~= PlotTypes.PLOT_HILLS then
						if not plot:IsRiverSide() then
							local flatToHill = Map.Rand(100, "Plains Spawn Chance");
							if flatToHill >= 80 then
								print("Adding Hills to Coast");
								plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
							end
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



	beta_cliffs = 2
	if beta_cliffs == 1 then
		featuregen:AddFeatures(false);
	else
		featuregen:AddFeatures(true);
	end
end




function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. ''..k..': ' .. dump(v) .. ', '
      end
      return s .. '}'
   else
      return tostring(o)
   end
end

function dump_map(graph)
	local map_output = {};

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = iW * y + x+1;












			







			map_output[i] = {x, y, graph[i]}





			





			




















		end
	end
	print("MAP READOUT:");
	print(dump(map_output));
	print("MAP READOUT OVER");
end



function AssignStartingPlots:FixResourceGraphics()

	local rain = Map.GetCustomOption(2)
	local remove_desert_lux = Map.GetCustomOption(18);
	

	


	








		 

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			
			local plot = Map.GetPlot(x, y)
			local res_ID = plot:GetResourceType(-1)
			local featureType = plot:GetFeatureType()
			local terrainType = plot:GetTerrainType()
			local plotType = plot:GetPlotType()

			









			if res_ID == self.marble_ID or 
			   res_ID == self.gold_ID or 
			   res_ID == self.silver_ID or 
			   res_ID == self.copper_ID or 
			   res_ID == self.gems_ID or 
			   res_ID == self.salt_ID or 
			   res_ID == self.lapis_ID or 
			   res_ID == self.jade_ID or 
			   res_ID == self.obsidian_ID or
			   res_ID == self.amber_ID then 
			   

				if (featureType ~= FeatureTypes.FEATURE_FOREST) and (featureType ~= FeatureTypes.FEATURE_JUNGLE) then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end


				

			elseif res_ID == self.cocoa_ID or 
			       res_ID == self.citrus_ID or 
				   res_ID == self.spices_ID or 
				   res_ID == self.sugar_ID or 
				   res_ID == self.truffles_ID or 
				   res_ID == self.silk_ID or 
				   res_ID == self.dye_ID or 
				   res_ID == self.fur_ID or
				   red_ID == self.coconut_ID or
				   red_ID == self.rubber_ID or
				   res_ID == self.hardwood_ID or
				   res_ID == self.deer_ID then
				
				if (featureType ~= FeatureTypes.FEATURE_FOREST) then
					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
				elseif (featureType ~= FeatureTypes.FEATURE_JUNGLE) then
					plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
				end
				
				if res_ID == self.fur_ID then

					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
				end
				

				local lat = 0
				if (y >= (iH/2)) then
					lat = math.abs((iH/2) - y)/(iH/2)
				else
					lat = math.abs((iH/2) - (y + 1))/(iH/2)
				end
				local AvgJungleRange = 0




				
				if rain == 1 then		

					AvgJungleRange = 0.08
				elseif rain == 3 then	

					AvgJungleRange = 0.25
				else					

					AvgJungleRange = 0.12
				end
				

				if (featureType == FeatureTypes.FEATURE_MARSH) then
					if res_ID == self.sugar_ID or res_ID == self.spices_ID or res_ID == self.dye_ID or res_ID == self.rubber_ID or res_ID == self.coconut_ID then

					else

						if lat <= AvgJungleRange then
							if res_ID ~= self.deer_ID and res_ID ~= self.fur_ID then
								plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
								plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
							else
								plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
							end
						else
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end	
					end
				else

					if lat <= AvgJungleRange then
						if res_ID ~= self.deer_ID and res_ID ~= self.fur_ID then
							plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
							plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
						else
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end
					else
						plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
					end		
				end
				

			elseif res_ID == self.incense_ID or 
			       res_ID == self.ivory_ID or 
				   res_ID == self.wine_ID or 
				   res_ID == self.olives_ID or 
				   res_ID == self.coffee_ID or
				   res_ID == self.tobacco_ID or 
				   res_ID == self.tea_ID or 
				   res_ID == self.perfume_ID or 
				   res_ID == self.cotton_ID then 
				


					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)

				

				if (featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS) then	
					plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end
				












			end

			
			featureType = plot:GetFeatureType()
			terrainType = plot:GetTerrainType()
			plotType = plot:GetPlotType()
			if remove_desert_lux == 2 then
				if 	res_ID == self.marble_ID or 
					res_ID == self.gold_ID or 
					res_ID == self.silver_ID or 
					res_ID == self.copper_ID or 
					res_ID == self.gems_ID or 
					res_ID == self.salt_ID or 
					res_ID == self.lapis_ID or 
					res_ID == self.jade_ID or 
					res_ID == self.obsidian_ID or
					res_ID == self.amber_ID or

					res_ID == self.cocoa_ID or 
					res_ID == self.citrus_ID or 
					res_ID == self.spices_ID or 
					res_ID == self.sugar_ID or 
					res_ID == self.truffles_ID or 
					res_ID == self.silk_ID or 
					res_ID == self.dye_ID or 
					res_ID == self.fur_ID or
					red_ID == self.coconut_ID or
					red_ID == self.rubber_ID or
					res_ID == self.hardwood_ID or
					res_ID == self.deer_ID or

					res_ID == self.incense_ID or 
					res_ID == self.ivory_ID or 
					res_ID == self.wine_ID or 
					res_ID == self.olives_ID or 
					res_ID == self.coffee_ID or
					res_ID == self.tobacco_ID or 
					res_ID == self.tea_ID or 
					res_ID == self.perfume_ID or 
					res_ID == self.cotton_ID then

						if terrainType == TerrainTypes.TERRAIN_DESERT and featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS and plotType ~= PlotTypes.PLOT_HILLS then
							plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)


















							
						end
				end
			end
		end
	end























end



function AssignStartingPlots:GenerateLuxuryPlotListsInRegion(region_number, smart_lux, smart_plots)


	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	local region_area_object;
	if iAreaID ~= -1 then
		region_area_object = Map.GetArea(iAreaID);
	end

	local region_coast, region_marsh, region_flood_plains, region_tundra_flat_including_forests = {}, {}, {}, {};
	local region_hills_open, region_hills_covered, region_hills_jungle, region_hills_forest = {}, {}, {}, {};
	local region_desert_flat_no_feature, region_plains_flat_no_feature, region_jungle_flat = {}, {}, {};
	local region_forest_flat, region_forest_flat_but_not_tundra = {}, {};
	local region_dry_grass_flat_no_feature, region_fresh_water_grass_flat_no_feature = {}, {};
	local region_dry_plains_flat_no_feature, region_fresh_water_plains_flat_no_feature = {}, {};
	local region_desert_or_tundra_flat_no_feature, region_grass_flat_no_feature = {}, {};
	local region_tundra_flat_no_feature, region_tundra_flat_forest = {}, {};
	local region_hills_open_no_tundra, region_hills_open_no_desert, region_hills_open_no_tundra_no_desert = {}, {}, {};
	local region_hills_open_no_grass, region_hills_open_no_grass_no_tundra, region_hills_covered_no_tundra = {}, {}, {};
	local region_hills_covered_no_grass, region_hills_covered_no_grass_no_tundra, region_flat_covered = {}, {}, {};
	local region_flat_covered_no_grass, region_flat_covered_no_tundra, region_flat_covered_no_grass_no_tundra = {}, {}, {};
	local region_flat_open, region_flat_open_no_grass_no_plains, region_flat_open_no_tundra_no_desert = {}, {}, {};
	local region_flat_open_no_desert, region_flat_desert_including_flood, region_hills_open_no_grass_no_plains = {}, {}, {};


	if smart_lux then
		print("USING SMART LUX")
		for plotLoop, plot in ipairs(smart_plots) do
			local x = plot:GetX()
			local y = plot:GetY()
			local plotIndex = y * iW + x + 1;

			local area_of_plot = plot:GetArea();

			local plotType = plot:GetPlotType()
			local terrainType = plot:GetTerrainType()
			local featureType = plot:GetFeatureType()

			if plotType == PlotTypes.PLOT_OCEAN then
				if terrainType == TerrainTypes.TERRAIN_COAST then
					if plot:IsLake() == false then
						if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then
							if iAreaID == -1 then
								if plot:IsAdjacentToLand() then
									table.insert(region_coast, plotIndex);
								end
							else
								if plot:IsAdjacentToArea(region_area_object) then
									table.insert(region_coast, plotIndex);
								end
							end
						end
					end
				end
			elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
				if featureType == FeatureTypes.NO_FEATURE then
					table.insert(region_hills_open, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_hills_open_no_desert, plotIndex);
						table.insert(region_hills_open_no_grass, plotIndex);
						table.insert(region_hills_open_no_grass_no_plains, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						table.insert(region_hills_open_no_tundra, plotIndex);
						table.insert(region_hills_open_no_grass, plotIndex);
						table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
						table.insert(region_hills_open_no_grass_no_plains, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_hills_open_no_tundra, plotIndex);
						table.insert(region_hills_open_no_desert, plotIndex);
						table.insert(region_hills_open_no_grass, plotIndex);
						table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
						table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_hills_open_no_tundra, plotIndex);
						table.insert(region_hills_open_no_desert, plotIndex);
						table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
					end
				elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
					table.insert(region_hills_jungle, plotIndex);
					table.insert(region_hills_covered, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
						table.insert(region_hills_covered_no_grass, plotIndex);
						table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
					end
				elseif featureType == FeatureTypes.FEATURE_FOREST then		
					table.insert(region_hills_forest, plotIndex);
					table.insert(region_hills_covered, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_hills_covered_no_grass, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
						table.insert(region_hills_covered_no_grass, plotIndex);
						table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
					end
				end
			elseif plotType == PlotTypes.PLOT_LAND then
				if featureType == FeatureTypes.NO_FEATURE then
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_tundra_flat_no_feature, plotIndex);
						table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
						table.insert(region_tundra_flat_including_forests, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_desert, plotIndex);
						table.insert(region_flat_open_no_grass_no_plains, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						table.insert(region_desert_flat_no_feature, plotIndex);
						table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_grass_no_plains, plotIndex);
						table.insert(region_flat_desert_including_flood, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_plains_flat_no_feature, plotIndex);
						table.insert(region_flat_open_no_desert, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
						if plot:IsFreshWater() then
							table.insert(region_fresh_water_plains_flat_no_feature, plotIndex);
						else
							table.insert(region_dry_plains_flat_no_feature, plotIndex);
						end
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_grass_flat_no_feature, plotIndex);
						table.insert(region_flat_open_no_desert, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
						if plot:IsFreshWater() then
							table.insert(region_fresh_water_grass_flat_no_feature, plotIndex);
						else
							table.insert(region_dry_grass_flat_no_feature, plotIndex);
						end
					end
				elseif featureType == FeatureTypes.FEATURE_MARSH then		
					table.insert(region_marsh, plotIndex);
				elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
					table.insert(region_flood_plains, plotIndex);
					table.insert(region_flat_desert_including_flood, plotIndex);
				elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
					table.insert(region_jungle_flat, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_flat_covered_no_tundra, plotIndex);
						table.insert(region_flat_covered_no_grass, plotIndex);
						table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_flat_covered_no_tundra, plotIndex);
					end
				elseif featureType == FeatureTypes.FEATURE_FOREST then		
					table.insert(region_forest_flat, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_tundra_flat_including_forests, plotIndex);

						table.insert(region_tundra_flat_forest, plotIndex);
						table.insert(region_flat_covered_no_grass, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_forest_flat_but_not_tundra, plotIndex);
						table.insert(region_flat_covered_no_tundra, plotIndex);
						table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
						table.insert(region_flat_covered_no_grass, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_forest_flat_but_not_tundra, plotIndex);
						table.insert(region_flat_covered_no_tundra, plotIndex);
					end
				end
			end
		end
	else
		for region_loop_y = 0, iHeight - 1 do
			for region_loop_x = 0, iWidth - 1 do
				local x = (region_loop_x + iWestX) % iW;
				local y = (region_loop_y + iSouthY) % iH;
				local plotIndex = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				local area_of_plot = plot:GetArea();

				local plotType = plot:GetPlotType()
				local terrainType = plot:GetTerrainType()
				local featureType = plot:GetFeatureType()

				if plotType == PlotTypes.PLOT_OCEAN then
					if terrainType == TerrainTypes.TERRAIN_COAST then
						if plot:IsLake() == false then
							if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then
								if iAreaID == -1 then
									if plot:IsAdjacentToLand() then
										table.insert(region_coast, plotIndex);
									end
								else
									if plot:IsAdjacentToArea(region_area_object) then
										table.insert(region_coast, plotIndex);
									end
								end
							end
						end
					end
				elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
					if featureType == FeatureTypes.NO_FEATURE then
						table.insert(region_hills_open, plotIndex);
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(region_hills_open_no_desert, plotIndex);
							table.insert(region_hills_open_no_grass, plotIndex);
							table.insert(region_hills_open_no_grass_no_plains, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_DESERT then
							table.insert(region_hills_open_no_tundra, plotIndex);
							table.insert(region_hills_open_no_grass, plotIndex);
							table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
							table.insert(region_hills_open_no_grass_no_plains, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(region_hills_open_no_tundra, plotIndex);
							table.insert(region_hills_open_no_desert, plotIndex);
							table.insert(region_hills_open_no_grass, plotIndex);
							table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
							table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(region_hills_open_no_tundra, plotIndex);
							table.insert(region_hills_open_no_desert, plotIndex);
							table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
						end
					elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
						table.insert(region_hills_jungle, plotIndex);
						table.insert(region_hills_covered, plotIndex);
						if terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(region_hills_covered_no_tundra, plotIndex);
							table.insert(region_hills_covered_no_grass, plotIndex);
							table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(region_hills_covered_no_tundra, plotIndex);
						end
					elseif featureType == FeatureTypes.FEATURE_FOREST then		
						table.insert(region_hills_forest, plotIndex);
						table.insert(region_hills_covered, plotIndex);
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(region_hills_covered_no_grass, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(region_hills_covered_no_tundra, plotIndex);
							table.insert(region_hills_covered_no_grass, plotIndex);
							table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(region_hills_covered_no_tundra, plotIndex);
						end
					end
				elseif plotType == PlotTypes.PLOT_LAND then
					if featureType == FeatureTypes.NO_FEATURE then
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(region_tundra_flat_no_feature, plotIndex);
							table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
							table.insert(region_tundra_flat_including_forests, plotIndex);
							table.insert(region_flat_open, plotIndex);
							table.insert(region_flat_open_no_desert, plotIndex);
							table.insert(region_flat_open_no_grass_no_plains, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_DESERT then
							table.insert(region_desert_flat_no_feature, plotIndex);
							table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
							table.insert(region_flat_open, plotIndex);
							table.insert(region_flat_open_no_grass_no_plains, plotIndex);
							table.insert(region_flat_desert_including_flood, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(region_plains_flat_no_feature, plotIndex);
							table.insert(region_flat_open_no_desert, plotIndex);
							table.insert(region_flat_open, plotIndex);
							table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
							if plot:IsFreshWater() then
								table.insert(region_fresh_water_plains_flat_no_feature, plotIndex);
							else
								table.insert(region_dry_plains_flat_no_feature, plotIndex);
							end
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(region_grass_flat_no_feature, plotIndex);
							table.insert(region_flat_open_no_desert, plotIndex);
							table.insert(region_flat_open, plotIndex);
							table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
							if plot:IsFreshWater() then
								table.insert(region_fresh_water_grass_flat_no_feature, plotIndex);
							else
								table.insert(region_dry_grass_flat_no_feature, plotIndex);
							end
						end
					elseif featureType == FeatureTypes.FEATURE_MARSH then		
						table.insert(region_marsh, plotIndex);
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
						table.insert(region_flood_plains, plotIndex);
						table.insert(region_flat_desert_including_flood, plotIndex);
					elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
						table.insert(region_jungle_flat, plotIndex);
						if terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(region_flat_covered_no_tundra, plotIndex);
							table.insert(region_flat_covered_no_grass, plotIndex);
							table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(region_flat_covered_no_tundra, plotIndex);
						end
					elseif featureType == FeatureTypes.FEATURE_FOREST then		
						table.insert(region_forest_flat, plotIndex);
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(region_tundra_flat_including_forests, plotIndex);

							table.insert(region_tundra_flat_forest, plotIndex);
							table.insert(region_flat_covered_no_grass, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(region_forest_flat_but_not_tundra, plotIndex);
							table.insert(region_flat_covered_no_tundra, plotIndex);
							table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
							table.insert(region_flat_covered_no_grass, plotIndex);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(region_forest_flat_but_not_tundra, plotIndex);
							table.insert(region_flat_covered_no_tundra, plotIndex);
						end
					end
				end
			end
		end
	end
	

	local results_table = {
	region_coast,
	region_marsh,
	region_flood_plains,
	region_hills_open,
	region_hills_covered,
	region_hills_jungle,
	region_hills_forest,
	region_jungle_flat,
	region_forest_flat,
	region_desert_flat_no_feature,
	region_plains_flat_no_feature,
	region_dry_grass_flat_no_feature,
	region_fresh_water_grass_flat_no_feature,
	region_tundra_flat_including_forests,
	region_forest_flat_but_not_tundra,
	region_grass_flat_no_feature,
	region_tundra_flat_no_feature,
	region_dry_plains_flat_no_feature,
	region_fresh_water_plains_flat_no_feature,
	region_desert_or_tundra_flat_no_feature,
	region_tundra_flat_forest,
	region_hills_open_no_tundra,
	region_hills_open_no_desert,
	region_hills_open_no_tundra_no_desert,
	region_hills_open_no_grass,
	region_hills_open_no_grass_no_tundra,
	region_hills_open_no_grass_no_plains,
	region_hills_covered_no_tundra,
	region_hills_covered_no_grass,
	region_hills_covered_no_grass_no_tundra,
	region_flat_covered,
	region_flat_covered_no_grass,
	region_flat_covered_no_tundra,
	region_flat_covered_no_grass_no_tundra,
	region_flat_open,
	region_flat_open_no_grass_no_plains,
	region_flat_open_no_tundra_no_desert,
	region_flat_open_no_desert,
	region_flat_desert_including_flood,
	};
	return results_table
end




function AssignStartingPlots:PlaceLuxuries()


	local beta_double_secondary = 1

	local beta_smart_lux = 2




	local used_randoms_as_secondaries =	table.fill(false, 99);

	for loop, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		local this_region_luxury = reg_data[2];
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		print("-"); print("Attempting to place Luxury#", this_region_luxury, "at start plot", x, y, "in Region#", region_number);

		local iNumToPlace = 2;
		if self.start_locations == 1 or self.start_locations == 2 then
			iNumToPlace = 3;
		end
















		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(this_region_luxury);


		luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumToPlace, 0.5, -1, 0, 0, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quinary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and senary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end

		if iNumLeftToPlace > 0 then

			luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
		end

		if iNumLeftToPlace > 0 then



			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] - iNumLeftToPlace;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] - iNumLeftToPlace;
		end

		if iNumLeftToPlace > 0 and self.iNumTypesRandom > 0 then

			local randoms_to_place = 1;
			for loop, random_res in ipairs(self.resourceIDs_assigned_to_random) do

				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(random_res);
				if randoms_to_place > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
			end
		end
	end
	


	for city_state = 1, self.iNumCityStates do

		if self.city_state_validity_table[city_state] == false then

		else
			

			local region_number = self.city_state_region_assignments[city_state];
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			local lux_possible_for_cs = {};


			local cs_only_types = {};
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[res_ID] == true then
					table.insert(cs_only_types, res_ID);
				end
			end
			local iNumCSAllowed = table.maxn(cs_only_types);
			if iNumCSAllowed > 0 then
				for loop, res_ID in ipairs(cs_only_types) do
					lux_possible_for_cs[res_ID] = 80 / iNumCSAllowed;
				end
			end


			if self.iNumTypesRandom > 0 or region_number > 0 then
				local random_types_allowed = {};
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
					if allowed_luxuries[res_ID] == true then
						table.insert(random_types_allowed, res_ID);
					end
				end
				local iNumRandAllowed = table.maxn(random_types_allowed);
				local iNumAllowed = iNumRandAllowed;










				if iNumRandAllowed > 0 then
					for loop, res_ID in ipairs(random_types_allowed) do
						lux_possible_for_cs[res_ID] = 20 / iNumAllowed;
					end
				end
			end


			local iNumAvailableTypes = table.maxn(lux_possible_for_cs);
			if iNumAvailableTypes == 0 then
				print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else

				local res_threshold = {};
				local totalWeight, accumulatedWeight = 0, 0;
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					totalWeight = totalWeight + this_weight;
				end


				local use_this_ID;
				local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To City State - Lua");

				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					local threshold = (this_weight + accumulatedWeight) * 10000 / totalWeight;
					if diceroll < threshold then
						use_this_ID = res_ID;
						print("CS Given Lux ID: " .. tostring(use_this_ID));
						break
					end
					accumulatedWeight = accumulatedWeight + this_weight;
				end

				print("-"); print("-"); print("-Assigned Luxury Type", use_this_ID, "to City State#", city_state);

				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
                primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
				end
			end
		end
	end

	local smart_region_plots = {}
	local region_number_to_loop = {}

	if beta_smart_lux == 2 then
		local player_plots = {}
		local player_is_coastal = {}

		for loop, reg_data in ipairs(self.regions_sorted_by_type) do
			local region_number = reg_data[1];
			region_number_to_loop[region_number] = loop;
			local this_region_luxury = reg_data[2];
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local player_plot = Map.GetPlot(x,y)
			table.insert(player_plots, player_plot)
			player_is_coastal[loop] = player_plot:IsCoastalLand(50)
		end

		local cs_plots = {}
		for cs_number = 1, self.iNumCityStates do
			if self.city_state_validity_table[cs_number] == true then
				local data_table = self.cityStatePlots[cs_number];
				local x = data_table[1];
				local y = data_table[2];
				table.insert(cs_plots, Map.GetPlot(x, y))
			end
		end

		local land_graph, total_flow_graph = CalculateFlows(player_plots, 40, true, cs_plots, 4, 0.5)




		local smart_region_plots_temp = {}
		for loop, reg_data in ipairs(self.regions_sorted_by_type) do
			smart_region_plots[loop] = {}
			smart_region_plots_temp[loop] = {}
		end

		for i, region_num in pairs(land_graph) do
			if region_num ~= 0 then
				if player_is_coastal[region_num] then

					local adj_is = adj_is_cache[i]
					for loop, adj_i in ipairs(adj_is) do
						local adj_x, adj_y = i_to_xy(adj_i, iW, iH)
						local adj_plot = Map.GetPlot(adj_x, adj_y)
						if adj_plot ~= nil then
							if (adj_plot:IsLake() == false) and (adj_plot:GetPlotType() == PlotTypes.PLOT_OCEAN) then
								smart_region_plots_temp[region_num][adj_i] = 1
							end
						end	
					end
				else
					smart_region_plots_temp[region_num][i] = 1
				end
			end
			
		end

		for loop, region in pairs(smart_region_plots_temp) do
			for i, dumby in pairs(region) do
				local x, y = i_to_xy(i, iW, iH)
				table.insert(smart_region_plots[loop], Map.GetPlot(x, y))
			end
		end

	end
		

	for region_number, res_ID in ipairs(self.region_luxury_assignment) do
		print("-"); print("- - -"); print("Attempting to place regional luxury #", res_ID, "in Region#", region_number);
		local iNumAlreadyPlaced = self.amounts_of_resources_placed[res_ID + 1];
		local assignment_split = self.luxury_assignment_count[res_ID];
		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list, iNumLeftToPlace;
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);
		if beta_smart_lux == 1 then
			luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number, false, {})
		else
			if tablelength(smart_region_plots[region_number_to_loop[region_number]]) > 6 then
				luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number, true, smart_region_plots[region_number_to_loop[region_number]])

			else
				luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number, false, {})
			end
		end




		local target_list = self:GetRegionLuxuryTargetNumbers()
		local targetNum = target_list[self.iNumCivs]

		targetNum = targetNum - self.region_low_fert_compensation[region_number];

		if self.resource_setting == 1 or self.resource_setting == 2 then
			targetNum = targetNum - 2;
		elseif self.resource_setting == 3 or self.resource_setting == 4 or self.resource_setting == 5 or self.resource_setting == 6 then
			targetNum = targetNum - 1;
		elseif self.resource_setting == 7 then
			targetNum = targetNum + 1;
		elseif self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
			targetNum = targetNum + 2;
		end
		local iNumThisLuxToPlace = math.max(1, targetNum);

		print("-"); print("Target number for Luxury#", res_ID, "with assignment split of", assignment_split, "is", targetNum);
		

		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, 2, 1, 3, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 0, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and quinary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and senary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		print("-"); print("-"); print("Number of LuxuryID", res_ID, "left to place in Region#", region_number, "is", iNumLeftToPlace);
		



		if iNumLeftToPlace > 0 then	

			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			print("-"); print("Number of LuxuryID", res_ID, "not placed in Region#", region_number, "is", iNumLeftToPlace);
		end	
	end


	if self.iNumTypesRandom > 0 then
		print("* *"); print("* iNumTypesRandom = ", self.iNumTypesRandom); print("* *");





		local world_size_data = self:GetWorldLuxuryTargetNumbers()
		local targetLuxForThisWorldSize = world_size_data[1];
		local loopTarget = world_size_data[2];
		local extraLux = Map.Rand(self.iNumCivs, "Luxury Resource Variance - Place Resources LUA");
		local iNumRandomLuxTarget = targetLuxForThisWorldSize + extraLux - self.totalLuxPlacedSoFar;
		print("* *"); print("* targetLuxForThisWorldSize = ", targetLuxForThisWorldSize); print("* *");
		print("* *"); print("* random to add to target = ", extraLux); print("* *");
		print("* *"); print("* totalLuxPlacedSoFar = ", self.totalLuxPlacedSoFar); print("* *");
		print("* *"); print("* iNumRandomLuxTarget = ", iNumRandomLuxTarget); print("* *");
		local iNumRandomLuxPlaced, iNumThisLuxToPlace = 0, 0;

		local random_lux_ratios_table = {
		{1},
		{1, 1},
		{1, 1, 1},
		{1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1} };

		for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
			local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, current_list, iNumLeftToPlace;
			primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);


				local LandXY = iW * iH;
				local NumRandToAdd = 4;

				if LandXY < 2500 then
					NumRandToAdd = 4;
				elseif LandXY < 6000 then
					NumRandToAdd = 5;
				elseif LandXY < 10000 then
					NumRandToAdd = 6;
				end

				iNumThisLuxToPlace = math.max(NumRandToAdd, math.ceil(iNumRandomLuxTarget / 10));






			local lux_distance = 3;


			current_list = self.global_luxury_plot_lists[primary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, 2, lux_distance, 0, current_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				current_list = self.global_luxury_plot_lists[secondary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.3, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				current_list = self.global_luxury_plot_lists[tertiary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.35, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				current_list = self.global_luxury_plot_lists[quaternary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.4, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then
				current_list = self.global_luxury_plot_lists[quinary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.5, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then
				current_list = self.global_luxury_plot_lists[senary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.75, 2, lux_distance, 0, current_list);
			end
			iNumRandomLuxPlaced = iNumRandomLuxPlaced + iNumThisLuxToPlace - iNumLeftToPlace;
			print("-"); 
			print("Random Luxury ID#:", res_ID);
			print("-"); print("Random Luxury Target Number:", iNumThisLuxToPlace);
			print("Random Luxury Target Placed:", iNumThisLuxToPlace - iNumLeftToPlace); print("-");
		end
		print("-"); print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Target Number:", iNumRandomLuxTarget);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Number Placed:", iNumRandomLuxPlaced);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); print("-");
	end




	local placed2ndLux = true;

	if self.resource_setting ~= 1 then

		print("|||||||||||||||||||||||||||||||||||| Secondary Lux Check ||||||||||||||||||||||||||||||||||||");

		local coastal_rotation = 1;
		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];

			local cplot = Map.GetPlot(x, y)

			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");

			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
					print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end

			if (self.start_locations ~= 1) and (self.start_locations ~= 2) and (self.start_locations ~= 3) then
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end


			if self._lek_coastal_refish and Map.GetPlot(x, y):IsCoastalLand() then
				iNumTypesAllowed = 0;
				candidate_types = {};
				table.insert(candidate_types, self.whale_ID);
				table.insert(candidate_types, self.pearls_ID);
				table.insert(candidate_types, self.crab_ID);
				table.insert(candidate_types, self.coral_ID);

				local force_id = coastal_rotation % 4
				coastal_rotation = coastal_rotation + 1
				use_this_ID = candidate_types[force_id];

				print("sapht: forcing a coastal lux rot/id = ", force_id, use_this_ID)
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");

				iNumTypesAllowed = iNumTypesAllowed + 1;


			elseif iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				print("sapht: rolling lux dice")
				use_this_ID = candidate_types[diceroll];
			else

				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
				if iNumTypesAllowed > 0 then
					local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
					use_this_ID = candidate_types[diceroll];
				else

					local region_lux_ID = self.region_luxury_assignment[region_number];
					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
						if res_ID ~= region_lux_ID then
							if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
								print("- Found eligible luxury type:", res_ID);
								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else
						print("-"); print("Failed to place second Luxury type in 2nd ring at start in Region#", region_number, "-- no eligible types!"); print("-");
						placed2ndLux = false;
					end
				end
			end
			print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");
			print("Random Res 2 Rings: " .. tostring(use_this_ID));

			if use_this_ID ~= nil then
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
					used_randoms_as_secondaries[use_this_ID] = true;
					print("Random Res State: " .. tostring(used_randoms_as_secondaries[use_this_ID]));
				end
			end
		end
	end


	if placed2ndLux == false then
		
		local placed2ndLux = true;

		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 3)
			print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");

			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
					print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end

			if (self.start_locations ~= 1) and (self.start_locations ~= 2) and (self.start_locations ~= 3) then
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end


			if self._lek_coastal_refish and Map.GetPlot(x, y):IsCoastalLand() then
				iNumTypesAllowed = 0;
				candidate_types = {};
				table.insert(candidate_types, self.whale_ID);
				table.insert(candidate_types, self.pearls_ID);
				table.insert(candidate_types, self.crab_ID);
				table.insert(candidate_types, self.coral_ID);
				iNumTypesAllowed = iNumTypesAllowed + 4;
				print("sapht: forcing a coastal lux (2)")


			end
		
			if iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				use_this_ID = candidate_types[diceroll];
			else

				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
				if iNumTypesAllowed > 0 then
					local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
					use_this_ID = candidate_types[diceroll];
				else

					local region_lux_ID = self.region_luxury_assignment[region_number];
					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
						if res_ID ~= region_lux_ID then
							if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
								print("- Found eligible luxury type:", res_ID);
								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else
						print("-"); print("Failed to place second Luxury type in 3rd ring at start in Region#", region_number, "-- no eligible types!"); print("-");
						placed2ndLux = false;
					end
				end
			end
			print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");

			print("Random Res 3 Rings: " .. tostring(use_this_ID));

			if use_this_ID ~= nil then
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, beta_double_secondary, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
					used_randoms_as_secondaries[use_this_ID] = true;
					print("Random Res State: " .. tostring(used_randoms_as_secondaries[random_res]));
				end
			end
		end
	end


	self.realtotalLuxPlacedSoFar = self.totalLuxPlacedSoFar
end




function GraphBFS(current_list, node_list, graph, depth)
	local next_list = {}

	for node, dumby in pairs(current_list) do
		if node_list[node] == nil then
			node_list[node] = depth
			for new_node, dumby in pairs(graph[node]) do
				next_list[new_node] = true
			end
		end
	end

	if tablelength(next_list) > 0 then
		GraphBFS(next_list, node_list, graph, depth+1)
	end
end

function GraphSuspendedBFS(current_list, node_list, graph, depth, suspend_val)
	local next_list = {}

	for node, dumby in pairs(current_list) do
		if node ~= suspend_val then
			if node_list[node] == nil then
				node_list[node] = depth
				for new_node, dumby in pairs(graph[node]) do
					if new_node ~= suspend_val then
						next_list[new_node] = true
					end
				end
			end
		end
	end

	if tablelength(next_list) > 0 then
		GraphSuspendedBFS(next_list, node_list, graph, depth+1, suspend_val)
	end
end


function PlotBFS(i_list, plot_list, comp_list, comp_val, depth)
	local next_is = {}
	for i, dumby in pairs(i_list) do
		if plot_list[i] == nil then
			plot_list[i] = depth
			local adj_is = adj_is_cache[i]
			for loop2, adj_i in ipairs(adj_is) do
				if comp_list[adj_i] ~= nil then
					if comp_list[adj_i] == comp_val then
						next_is[adj_i] = true
					end
				end
			end
		end
	end
	if tablelength(next_is) > 0 then
		 PlotBFS(next_is, plot_list, comp_list, comp_val, depth+1)
	end
end

function PlotDFS(i, plot_list, comp_list, comp_val)

	if plot_list[i] ~= nil then
		return
	end
	plot_list[i] = 1
	local adj_is = adj_is_cache[i]
	for loop2, adj_i in ipairs(adj_is) do
		if comp_list[adj_i] ~= nil then
			if comp_list[adj_i] == comp_val then
				PlotDFS(adj_i, plot_list, comp_list, comp_val)
			end
		end
	end
end


function CalculateFlows(test_plots, pulse_depth, use_CS, CS_plots, CS_mult, impass_mult)



	local cs_resistance = table.fill(0.0, iW * iH);
	if use_CS then
		for cs_number = 1, tablelength(CS_plots) do
			local x = CS_plots[cs_number]:GetX()
			local y = CS_plots[cs_number]:GetY()
			PlaceImpactAndRipplesGeneric(x, y, cs_resistance, 1, 4*CS_mult)
			PlaceImpactAndRipplesGeneric(x, y, cs_resistance, 2, 2*CS_mult)
			PlaceImpactAndRipplesGeneric(x, y, cs_resistance, 3, 1*CS_mult)
		end
	end






	local is_coastal_cache = table.fill(false, iW * iH)
	local plot_type_cache = table.fill(false, iW * iH)
	
	local is_riverW_cache = table.fill(false, iW * iH)
	local is_riverNW_cache = table.fill(false, iW * iH)
	local is_riverNE_cache = table.fill(false, iW * iH)

	local mountain_base_cache = table.fill(0, iW * iH)
	local mountain_base_cache_coast = table.fill(0, iW * iH)


	

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH);
			local plot = Map.GetPlot(x,y);
			plot_type_cache[i] = plot:GetPlotType();


			if plot:IsLake() == true then
				plot_type_cache[i] = PlotTypes.PLOT_MOUNTAIN
			end
			is_coastal_cache[i] = plot:IsCoastalLand(50);
			is_riverW_cache[i] = plot:IsWOfRiver()
			is_riverNW_cache[i] = plot:IsNWOfRiver()
			is_riverNE_cache[i] = plot:IsNEOfRiver()

			mountain_base_cache[i] = mountain_base_cache[i]+cs_resistance[i]
			mountain_base_cache_coast[i] = mountain_base_cache[i]


			local adj_is = adj_is_cache[i]
			for loop2, adj_i in ipairs(adj_is) do
				local adj_x, adj_y = i_to_xy(adj_i, iW, iH);

				if plot_type_cache[adj_i] ~= nil then
					if plot_type_cache[adj_i] == PlotTypes.PLOT_OCEAN then
						if is_coastal_cache[i] == false then
							mountain_base_cache[i] = mountain_base_cache[i]+20.0*impass_mult
							mountain_base_cache_coast[i] = mountain_base_cache_coast[i]+2.0
						else
							mountain_base_cache[i] = mountain_base_cache[i]+2.0
						end
					end
					if plot_type_cache[adj_i] == PlotTypes.PLOT_MOUNTAIN then
						mountain_base_cache[i] = mountain_base_cache[i]+20.0*impass_mult
						mountain_base_cache_coast[i] = mountain_base_cache_coast[i]+20.0*impass_mult
					end
				end
			end
		end
	end
	

	local land_graph = table.fill(-1, iW * iH)
	local flow_graph = table.fill(-1.0, iW * iH)
	local l_to_coastal = {}

	for l = 1, tablelength(test_plots) do
		
		local test_player_plot = test_plots[l]
		l_to_coastal[l] = test_player_plot:IsCoastalLand(50);
		local test_play_i = xy_to_i(test_player_plot:GetX(), test_player_plot:GetY(), iW, iH)
		flow_graph[test_play_i] = 1.0
		land_graph[test_play_i] = l
	end

	for loop = 1, pulse_depth do
		local temp_flow_graph = table.fill(-1.0, iW * iH)
		local temp_land_graph = table.fill(-1.0, iW * iH)

		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				local i = xy_to_i(x, y, iW, iH);
				if flow_graph[i] > 0.0 then
					temp_flow_graph[i] = flow_graph[i]
					temp_land_graph[i] = land_graph[i]
				elseif plot_type_cache[i] == PlotTypes.PLOT_OCEAN or plot_type_cache[i] == PlotTypes.PLOT_MOUNTAIN then
					temp_flow_graph[i] = 0.0
					temp_land_graph[i] = 0
				else
					local a_hit = false
					local a_tot = 0.0
					local a_num = 0

					local a_flow_tot = 0
					local a_flows = {}
					local a_lands = {}
					local a_mountain_base = mountain_base_cache[i]
					local a_mountain_base_coast = mountain_base_cache_coast[i]

					local adj_is = adj_is_cache[i];

					for loop2, adj_i in ipairs(adj_is) do

						if is_riverW_cache[adj_i] ~= nil then

							if flow_graph[adj_i] > 0.0 then
								local a_flow = 1.0
								if l_to_coastal[land_graph[adj_i]] then
									a_flow = a_flow+a_mountain_base_coast
								else
									a_flow = a_flow+a_mountain_base
								end
								
								local hasRiver = false
								if loop2 == 2 then
									hasRiver = is_riverW_cache[i]
								elseif loop2 == 3 then
									hasRiver = is_riverNW_cache[i]
								elseif loop2 == 4 then
									hasRiver = is_riverNE_cache[i]
								elseif loop2 == 5 then
									hasRiver = is_riverW_cache[adj_i]
								elseif loop2 == 6 then
									hasRiver = is_riverNW_cache[adj_i]
								elseif loop2 == 1 then
									hasRiver = is_riverNE_cache[adj_i]
								end

								if hasRiver then
									a_flow = a_flow+2.0;
								end

								if plot_type_cache[i] == PlotTypes.PLOT_HILLS then
									a_flow = a_flow+1.0;
								end

							
								
								a_hit = true
								a_tot = a_tot + flow_graph[adj_i]
								a_num = a_num+1
								a_flow_tot = a_flow_tot + 1
								a_flows[a_flow_tot] = a_flow
								a_lands[a_flow_tot] = land_graph[adj_i]
								
							end
						end
					end

					local tot_flow = 0;
					local land_tots = {}
					for loop2 = 1, a_flow_tot do
						tot_flow = tot_flow+(1.0/a_flows[loop2])
						if land_tots[a_lands[loop2]] == nil then
							land_tots[a_lands[loop2]] = 0
						end
						land_tots[a_lands[loop2]] = land_tots[a_lands[loop2]]+(1.0/a_flows[loop2])
					end

					if a_hit then

						local best_land = -1
						local best_flow = -1.0
						for loop2 = 1, a_flow_tot do
							if land_tots[a_lands[loop2]] > best_flow then
								best_flow = land_tots[a_lands[loop2]]
								best_land = a_lands[loop2]
							end
						end

						temp_land_graph[i] = best_land

						if flow_graph[i] <= -1.0*(tot_flow) then
							temp_flow_graph[i] = flow_graph[i]+tot_flow
						else
							temp_flow_graph[i] = 0.99985*a_tot/a_num*0.9998^loop

						end
						if l_to_coastal[best_land] then
							if is_coastal_cache[i] then
								temp_flow_graph[i] = temp_flow_graph[i]*1.00005*1.0001^loop
							else
								temp_flow_graph[i] = temp_flow_graph[i]*0.99995*0.9999^loop
							end
						end
						temp_flow_graph[i] = temp_flow_graph[i]*1.0
					end
				end
			end
		end
		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				local i = xy_to_i(x, y, iW, iH);
				flow_graph[i] = temp_flow_graph[i]
				land_graph[i] = temp_land_graph[i]
			end
		end
	end

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH);
			if land_graph[i] < 0 then
				land_graph[i] = 0
			end
		end
	end

	local total_flow_graph = table.fill(-1.0, iW * iH)
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH);
			if land_graph[i] ~= 0 then

				local adj_is = adj_is_cache[i];
				local is_boundary = false
				local boundary_flows = {}
				local total_flows = 0
				local total_blocking = 0

				if plot_type_cache[i] == PlotTypes.PLOT_HILLS then
					total_blocking = total_blocking+1
				end

				for loop2, adj_i in ipairs(adj_is) do

					if land_graph[adj_i] ~= nil then
						if land_graph[adj_i] ~= 0 then
							if land_graph[adj_i] ~= land_graph[i] then
								is_boundary = true

								local boundary_flow = 1.0
								boundary_flow = boundary_flow+cs_resistance[adj_i]
								
								local hasRiver = false
								if loop2 == 2 then
									hasRiver = is_riverW_cache[i]
								elseif loop2 == 3 then
									hasRiver = is_riverNW_cache[i]
								elseif loop2 == 4 then
									hasRiver = is_riverNE_cache[i]
								elseif loop2 == 5 then
									hasRiver = is_riverW_cache[adj_i]
								elseif loop2 == 6 then
									hasRiver = is_riverNW_cache[adj_i]
								elseif loop2 == 1 then
									hasRiver = is_riverNE_cache[adj_i]
								end

								if hasRiver then
									boundary_flow = boundary_flow+2
								end



								total_flows = total_flows+1
								boundary_flows[total_flows] = boundary_flow

							end
						else
							total_blocking = total_blocking+2
						end
					end
				end








				if is_boundary then
					local total_flow = 0.0
					for loop2 = 1, total_flows do
						total_flow = total_flow + 1.0/(total_blocking+boundary_flows[loop2])
					end
					total_flow_graph[i] = total_flow
				else
					total_flow_graph[i] = 0.0

				end
			end
		end
	end



	
	return land_graph, total_flow_graph
end



function AssignStartingPlots:PlaceResourcesAndCityStates()


		





















		print("Map Generation - Assigning Luxury Resource Distribution");
		self:AssignLuxuryRoles()

		self:PlaceCityStates()




		self:GenerateGlobalResourcePlotLists()


		self:PlaceLuxuries()


		self:PlaceStrategicAndBonusResources()

		self:NormalizeCityStateLocations()	


		self:FixResourceGraphics()


		


		Map.RecalculateAreas();























		for cs_number = 1, self.iNumCityStates do
			if self.city_state_validity_table[cs_number] == true then
				local data_table = self.cityStatePlots[cs_number];
				local x = data_table[1];
				local y = data_table[2];
				local city_state_ID = cs_number + GameDefines.MAX_MAJOR_CIVS - 1
				local cityState = Players[city_state_ID];
				local plot = Map.GetPlot(x, y);
				if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
					plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				end



			else
				local data_table = self.cityStatePlots[cs_number];
				if data_table ~= nil then
					local x = data_table[1];
					local y = data_table[2];


					local city_state_ID = cs_number + GameDefines.MAX_MAJOR_CIVS - 1
					local cityState = Players[city_state_ID];
					local plot = Map.GetPlot(x, y);
					if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
						plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
					end



				end
			end
		end

	local accept_map = false
	local beta_isolation_rejection = Map.GetCustomOption(22) + 2

	

	if beta_isolation_rejection == 1 or beta_isolation_rejection == 2 then
		accept_map = true
	end

	if beta_isolation_rejection ~= 1 then

		local player_plots = {}
		local player_coastal_bias = {}
		for l = 1, self.iNumCivs do
			table.insert(player_plots, Players[self.player_ID_list[l]]:GetStartingPlot())
			table.insert(player_coastal_bias, CivNeedsCoastalStart(GameInfo.Civilizations[Players[self.player_ID_list[l]]:GetCivilizationType()].Type))
		end

		local cs_plots = {}
		for cs_number = 1, self.iNumCityStates do
			if self.city_state_validity_table[cs_number] == true then
				local data_table = self.cityStatePlots[cs_number];
				local x = data_table[1];
				local y = data_table[2];
				table.insert(cs_plots, Map.GetPlot(x, y))
			end
		end

		local land_graph, total_flow_graph = CalculateFlows(player_plots, 24, true, cs_plots, 2, 2.0)
		local land_graph_territory, total_flow_graph_territory = CalculateFlows(player_plots, 36, true, cs_plots, 8, 0.5)
















		local flow_from_to = {}
		local flow_first = {}
		local flow_second = {}
		local flow_third = {}
		local land_sums = {}
		for l = 1, self.iNumCivs do
			land_sums[l] = 0
			flow_from_to[l] = {}
			flow_first[l] = 0.0
			flow_second[l] = 0.0
			flow_third[l] = 0.0
			for p = 1, self.iNumCivs do
				flow_from_to[l][p] = 0.0
			end
		end

		
		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				local i = xy_to_i(x, y, iW, iH);
				if total_flow_graph[i] > 0.0 then
					local adj_is = adj_is_cache[i];

					for loop2, adj_i in ipairs(adj_is) do

						if land_graph[adj_i] ~= nil then
							if land_graph[adj_i] ~= 0 then
								if land_graph[adj_i] ~= land_graph[i] then
									flow_from_to[land_graph[i]][land_graph[adj_i]] = flow_from_to[land_graph[i]][land_graph[adj_i]]+total_flow_graph[i]

								end
							end
						end
					end
				end
				if total_flow_graph_territory[i] > -1.0 then
					land_sums[land_graph_territory[i]] = land_sums[land_graph_territory[i]]+1
				end
			end
		end

		

		local worst_ratio = 1.0
		
		if beta_isolation_rejection > 2 then
			for l = 1, self.iNumCivs do
				for p = 1, self.iNumCivs do
					local land_ratio = land_sums[l]/math.max(1, land_sums[p])
					if land_ratio < worst_ratio then
						worst_ratio = land_ratio
					end
				end
			end
		end


		local beta_prox_cs = 2
		local prox_cs = false
		if beta_prox_cs == 1 then
			prox_cs = true
		else

			local land_graph2, total_flow_graph2 = CalculateFlows(player_plots, 50, false, {}, 0, 0.5)
			local player_hits = {}
			local missed_player = false
			for cs_number = 1, self.iNumCityStates do
				if self.city_state_validity_table[cs_number] == true then
					local data_table = self.cityStatePlots[cs_number];
					local cs_x = data_table[1];
					local cs_y = data_table[2];
					player_hits[land_graph2[xy_to_i(cs_x, cs_y, iW, iH)]] = true
				end
			end


			for l = 1, self.iNumCivs do
				if player_hits[l] == nil then
					missed_player = true
				end
			end
			if missed_player == false then
				prox_cs = true
			end

		end




		local beta_min_distance = 6
		local min_dist = 99
		if beta_min_distance ~= 1 then
			min_dist = 5+beta_min_distance
		end
		if self.iNumCivs ~= 6 then
			min_dist = 99
		end
		local min_acceptable = true
		
		for l = 1, self.iNumCivs do
			for p = 1, self.iNumCivs do

				local player_plot1 = Players[self.player_ID_list[l]]:GetStartingPlot()
				local player_plot2 = Players[self.player_ID_list[p]]:GetStartingPlot()

				local deltaX = math.abs(player_plot1:GetX()-player_plot2:GetX())

				if deltaX > math.floor(iW / 2) and Map:IsWrapX() then 
					deltaX = math.floor(iW / 2) - (deltaX % math.floor(iW / 2))
				end

				local deltaY = math.abs(player_plot1:GetY()-player_plot2:GetY())

				local pDistance = math.sqrt( (deltaX)^2 + (deltaY)^2 );

				local iso_factor = (0.75-(pDistance/26.0))*2.8;
				local iso_factor = 1.0/(1+(math.exp(-iso_factor))^2);

				flow_from_to[l][p] = flow_from_to[l][p]*iso_factor*3.8

				if l ~= p then
					if pDistance < min_dist then
						min_acceptable = false
					end
				end

			end
		end

		local region_adjs = {}
		local topology_thresh = 0.0
		if beta_isolation_rejection == 3 then
			topology_thresh = 5.0
		elseif beta_isolation_rejection == 3 then
			topology_thresh = 5.75
		elseif beta_isolation_rejection == 5 then
			topology_thresh = 6.5
		end

		for l = 1, self.iNumCivs do
			region_adjs[l] = {}
			for p = 1, self.iNumCivs do
				if flow_from_to[l][p] > topology_thresh or l == p then
					region_adjs[l][p] = true
				end

				if flow_from_to[l][p] > flow_first[l] then
					flow_third[l] = flow_second[l]
					flow_second[l] = flow_first[l]
					flow_first[l] = flow_from_to[l][p]
				elseif flow_from_to[l][p] > flow_second[l] then
					flow_third[l] = flow_second[l]
					flow_second[l] = flow_from_to[l][p]
				elseif flow_from_to[l][p] > flow_third[l] then
					flow_third[l] = flow_from_to[l][p]
				end
			end
		end


		local topology_check = true

		if beta_isolation_rejection > 2 then
			for l = 1, self.iNumCivs do
				for p = 1, self.iNumCivs do
					if l ~= p then
						local reg_d = {}
						GraphSuspendedBFS(region_adjs[l], reg_d, region_adjs, 1, p)
						if tablelength(reg_d) < self.iNumCivs-1 then
							topology_check = false
						end
					end
				end
			end
		end








		

		























		local most_isolated = 1000.0
		local worst_player = 0

		for l = 1, self.iNumCivs do
			local isolation_score = flow_first[l]*0.1+flow_second[l]*0.85+flow_third[l]*0.85
			if isolation_score < most_isolated then
				most_isolated = isolation_score
				worst_player = l
			end
		end

		local iso_thresh = 0.0
		if beta_isolation_rejection == 3 then
			iso_thresh = 5.0
		elseif beta_isolation_rejection == 4 then
			iso_thresh = 7.0
		elseif beta_isolation_rejection == 5 then
			iso_thresh = 9.0
		end

		local iso_check  = most_isolated > iso_thresh





		local lux_ids = {}
		lux_ids[GameInfo.Resources.RESOURCE_WHALE.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_PEARLS.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_IVORY.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_FUR.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_SILK.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_DYE.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_SPICES.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_SUGAR.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_COTTON.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_WINE.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_INCENSE.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_GOLD.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_SILVER.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_GEMS.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_MARBLE.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_COPPER.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_SALT.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_CITRUS.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_TRUFFLES.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_CRAB.ID] = 1
		lux_ids[GameInfo.Resources.RESOURCE_COCOA.ID] = 1


		if beta_using_lekmod then
			lux_ids[GameInfo.Resources.RESOURCE_COFFEE.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_TEA.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_TOBACCO.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_AMBER.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_JADE.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_OLIVE.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_PERFUME.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_CORAL.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_LAPIS.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_OBSIDIAN.ID] = 1
			lux_ids[GameInfo.Resources.RESOURCE_COCONUT.ID] = 1
		end

		local spawn_validity = true
		if beta_spawn_validation == 2 and self.iNumCivs == 6 then
			for player_index, pPlot in ipairs(player_plots) do
				local needs_coast = player_coastal_bias[player_index]
				if pPlot:IsCoastalLand(50) ~= needs_coast then
					spawn_validity = false
				end
				p_i = xy_to_i(pPlot:GetX(), pPlot:GetY(), iW, iH)
				all_rings = {}
				ring1 = {}
				ring2 = {}
				ring3 = {}
				all_rings[p_i] = 1
				ring1[p_i] = 1

				local adj_is1 = adj_is_cache[p_i]
				for loop1, adj_i1 in ipairs(adj_is1) do
					if all_rings[adj_i1] == nil then
						all_rings[adj_i1] = 1
						ring1[adj_i1] = 1
					end
				end

				for adj_i1, dumby in pairs(ring1) do
					local adj_is2 = adj_is_cache[adj_i1]
					for loop2, adj_i2 in ipairs(adj_is2) do
						if all_rings[adj_i2] == nil then
							all_rings[adj_i2] = 1
							ring2[adj_i2] = 1
						end
					end
				end
				for adj_i2, dumby in pairs(ring2) do
					local adj_is3 = adj_is_cache[adj_i2]
					for loop3, adj_i3 in ipairs(adj_is3) do
						if all_rings[adj_i3] == nil then
							all_rings[adj_i3] = 1
							ring3[adj_i3] = 1
						end
					end
				end

				local block_total = 0.0
				local lux_count = 0
				local horse_count = 0
				local iron_count = 0
				local coal_count = 0
				local oil_count = 0

				for i, dumby in pairs(all_rings) do
					local x, y = i_to_xy(i, iW, iH)
					local plot = Map.GetPlot(x, y)
					if plot ~= nil then
						local plotType = plot:GetPlotType()
						local resourceType = plot:GetResourceType()
						if lux_ids[resourceType] ~= nil then
							lux_count = lux_count+1
						elseif resourceType == GameInfo.Resources.RESOURCE_HORSE.ID then
							horse_count = horse_count+1
						elseif resourceType == GameInfo.Resources.RESOURCE_IRON.ID then
							iron_count = iron_count+1
						elseif resourceType == GameInfo.Resources.RESOURCE_COAL.ID then
							coal_count = coal_count+1
						elseif resourceType == GameInfo.Resources.RESOURCE_OIL.ID then
							oil_count = oil_count+1
						end

						if plot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN then
							if ring1[i] ~= nil then
								block_total = block_total+5.0
							elseif ring2[i] ~= nil then
								block_total = block_total+3.0
							elseif ring3[i] ~= nil then
								block_total = block_total+2.0
							end
						end

						if plot:IsLake() then
							if ring1[i] ~= nil then
								block_total = block_total+4.0
							elseif ring2[i] ~= nil then
								block_total = block_total+2.0
							elseif ring3[i] ~= nil then
								block_total = block_total+1.0
							end
						end

					end
				end

				if block_total > 17.0 then 
					spawn_validity = false
				end
				if lux_count ~= 4 then
					spawn_validity = false
				end
				if horse_count < 1 then
					spawn_validity = false
				end
				if iron_count < 1 then
					spawn_validity = false
				end
				if coal_count < 1 then
					spawn_validity = false
				end
				if oil_count < 2 then
					spawn_validity = false
				end
			end
		end

		local biggest_area = Map.FindBiggestArea(false);
		local iBiggestAreaID = biggest_area:GetID();


		local final_choke_pass = true
		for y = math.ceil(iH * 2 / 5), math.ceil((iH - 1) * 3 / 5) do
			local entry_count = 0
			for x = 0, iW - 1 do
				local plot = Map.GetPlot(x, y);
				local i = xy_to_i(x, y, iW, iH)
				if plot:GetArea() == iBiggestAreaID and BETA_TECTONIC_LANDS[i] < 1 then
					entry_count = entry_count+1
				end
			end
			if entry_count < 10 then
				final_choke_pass = false
			end
		end

		
		for x =  math.ceil(iW * 2 / 5), math.ceil((iW - 1) * 3 / 5) do
			local entry_count = 0
			for y = 0, iH - 1 do
				local plot = Map.GetPlot(x, y);
				local i = xy_to_i(x, y, iW, iH)
				if plot:GetArea() == iBiggestAreaID and BETA_TECTONIC_LANDS[i] < 1 then
					entry_count = entry_count+1
				end
			end
			if entry_count < 10 then
				final_choke_pass = false
			end
		end



	

































		if iso_check and min_acceptable and prox_cs and topology_check and worst_ratio > 0.45 and spawn_validity and final_choke_pass then
			accept_map = true
			if beta_isolation_rejection == 2 then
				for id, player in pairs(Players) do
					player:AddNotification(NotificationTypes.NOTIFICATION_GENERIC, "Player " .. worst_player .. " Has the Worst Isolation Score of: " .. most_isolated, "Player Connectivity")
				end
			end
		end
	end


	self:PrintFinalResourceTotalsToLog()

	

	return accept_map

end



function AssignStartingPlots:PlaceSpecificNumberOfResources(resource_ID, quantity, amount,
	                         ratio, impact_table_number, min_radius, max_radius, plot_list)






















	

	

	if plot_list == nil then

		return
	end
	local bCheckImpact = false;
	local impact_table = {};
	if impact_table_number == 1 then
		bCheckImpact = true;
		impact_table = self.strategicData;
	elseif impact_table_number == 2 then
		bCheckImpact = true;
		impact_table = self.luxuryData;
	elseif impact_table_number == 3 then
		bCheckImpact = true;
		impact_table = self.bonusData;
	elseif impact_table_number == 4 then
		bCheckImpact = true;
		impact_table = self.fishData;
	elseif impact_table_number == 8 then
		bCheckImpact = true;
		impact_table = self.seaOilData;
	end

	local iNumLeftToPlace = amount;
	local iNumPlots = table.maxn(plot_list);
	local iNumResources = math.min(amount, math.ceil(ratio * iNumPlots));

	for place_resource = 1, iNumResources do
		for loop, plotIndex in ipairs(plot_list) do
			if bCheckImpact == false or impact_table[plotIndex] == 0 then
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y)
				local i = xy_to_i(x, y, iW, iH)
				if res_plot:GetResourceType(-1) == -1 then
					if beta_tectonic_mounts == 1 then
						res_plot:SetResourceType(resource_ID, quantity);
						self.amounts_of_resources_placed[resource_ID + 1] = self.amounts_of_resources_placed[resource_ID + 1] + quantity;

						self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
						iNumLeftToPlace = iNumLeftToPlace - 1;
						if bCheckImpact == true then
							local res_addition = 0;
							if max_radius > min_radius then
								res_addition = Map.Rand(1 + (max_radius - min_radius), "Resource Radius - Place Resource LUA");
							end
							local rad = min_radius + res_addition;
							if beta_flexy_lux == 2 and impact_table_number == 2 and rad == 3 then
								rad = 2+Map.Rand(3, "Flexy Lux Radius");
							elseif beta_flexy_lux == 3 and impact_table_number == 2 and rad == 3 then
								rad = 2+Map.Rand(2, "Flexy Lux Radius");
							elseif beta_flexy_lux == 4 and impact_table_number == 2 and rad == 3 then
								rad = 2+math.min(Map.Rand(2, "Flexy Lux Radius"), Map.Rand(2, "Flexy Lux Radius"));
							end
							self:PlaceResourceImpact(x, y, impact_table_number, rad);
							
						end
						break
					else
						if (res_plot:GetPlotType() == PlotTypes.PLOT_OCEAN and BETA_TECTONIC_LANDS[i] > 0) == false then
							res_plot:SetResourceType(resource_ID, quantity);
							self.amounts_of_resources_placed[resource_ID + 1] = self.amounts_of_resources_placed[resource_ID + 1] + quantity;

							self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
							iNumLeftToPlace = iNumLeftToPlace - 1;
							if bCheckImpact == true then
								local res_addition = 0;
								if max_radius > min_radius then
									res_addition = Map.Rand(1 + (max_radius - min_radius), "Resource Radius - Place Resource LUA");
								end
								local rad = min_radius + res_addition;
								if beta_flexy_lux == 2 and impact_table_number == 2 and rad == 3 then
									rad = 2+Map.Rand(3, "Flexy Lux Radius");
								elseif beta_flexy_lux == 3 and impact_table_number == 2 and rad == 3 then
									rad = 2+Map.Rand(2, "Flexy Lux Radius");
								elseif beta_flexy_lux == 4 and impact_table_number == 2 and rad == 3 then
									rad = 2+math.min(Map.Rand(2, "Flexy Lux Radius"), Map.Rand(2, "Flexy Lux Radius"));
								end
								self:PlaceResourceImpact(x, y, impact_table_number, rad);
								
							end
							break
						end
					end
				end
			end
		end
	end
	return iNumLeftToPlace
end




function AssignStartingPlots:ObtainNextSectionInRegion(incoming_west_x, incoming_south_y,
	                         incoming_width, incoming_height, iAreaID, force_it, ignore_collisions)












	local region_edge_range = 0.2
	if beta_cs_placement == 2 then
		region_edge_range = 0.35
	end

	local iW, iH = Map.GetGridSize();
	local reached_middle = false;
	if incoming_width <= 0 or incoming_height <= 0 then
		return {}, {}, -1, -1, -1, -1, true;
	end
	if incoming_width < 4 or incoming_height < 4 then
		reached_middle = true;
	end
	local bTaller = false;
	local rows_to_check = math.ceil(region_edge_range * incoming_width);
	if incoming_height > incoming_width then
		bTaller = true;
		rows_to_check = math.ceil(region_edge_range * incoming_height);
	end

	local coastal_plots, inland_plots = {}, {};
	for section_y = incoming_south_y, incoming_south_y + incoming_height - 1 do
		for section_x = incoming_west_x, incoming_west_x + incoming_width - 1 do
			if reached_middle then
				local x = section_x % iW;
				local y = section_y % iH;
				if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) == true then
					local i = y * iW + x + 1;
					if self.plotDataIsCoastal[i] == true then
						table.insert(coastal_plots, i);
					else
						table.insert(inland_plots, i);
					end
				end
			else
				if bTaller == false then
					if section_x < incoming_west_x + rows_to_check or section_x >= incoming_west_x + incoming_width - rows_to_check then
						local x = section_x % iW;
						local y = section_y % iH;
						if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) == true then
							local i = y * iW + x + 1;
							if self.plotDataIsCoastal[i] == true then
								table.insert(coastal_plots, i);
							else
								table.insert(inland_plots, i);
							end
						end
					end
				else
					if section_y < incoming_south_y + rows_to_check or section_y >= incoming_south_y + incoming_height - rows_to_check then
						local x = section_x % iW;
						local y = section_y % iH;
						if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) == true then
							local i = y * iW + x + 1;
							if self.plotDataIsCoastal[i] == true then
								table.insert(coastal_plots, i);
							else
								table.insert(inland_plots, i);
							end
						end
					end
				end
			end
		end
	end
	local new_west_x, new_south_y, new_width, new_height;
	if bTaller then
		new_west_x = incoming_west_x + rows_to_check;
		new_south_y = incoming_south_y;
		new_width = incoming_width - (2 * rows_to_check);
		new_height = incoming_height;
	else
		new_west_x = incoming_west_x;
		new_south_y = incoming_south_y + rows_to_check;
		new_width = incoming_width;
		new_height = incoming_height - (2 * rows_to_check);
	end		

	return coastal_plots, inland_plots, new_west_x, new_south_y, new_width, new_height, reached_middle;
end



function AssignStartingPlots:GetRandomLuxuriesTargetNumber()


	local beta_lux_variety = 2




















	local LandXY = iW * iH
	local maxRandoms = 30
	local baseLuxCount = 4

	if LandXY < 6700 and beta_lux_variety == 1 then
		maxRandoms = (LandXY-720)/((2560-720)/8)+baseLuxCount
	end

	print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ LUX COUNT @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	print("Random Lux Count: " .. maxRandoms);
	print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

	return maxRandoms
end



function AssignStartingPlots:PlaceStrategicAndBonusResources()









	

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()
	

	local bonus_multiplier = 0.65;

	if self.resource_setting == 1 then
		bonus_multiplier = 1;
	elseif self.resource_setting == 2 then
		bonus_multiplier = 0.90;
	elseif self.resource_setting == 3 then
		bonus_multiplier = 0.80;
	elseif self.resource_setting == 4 then
		bonus_multiplier = 0.75;
	elseif self.resource_setting == 6 then
		bonus_multiplier = 0.55;
	elseif self.resource_setting == 7 then
		bonus_multiplier = 0.45;
	elseif self.resource_setting == 8 then
		bonus_multiplier = 0.35;
	elseif self.resource_setting == 9 then
		bonus_multiplier = 0.25;
	elseif self.resource_setting == 10 then
		bonus_multiplier = 0.15;
	end


	print("Map Generation - Placing Strategics");
	local resources_to_place = {
	{self.oil_ID, oil_amt, 65, 1, 4},
	{self.uranium_ID, uran_amt, 35, 1, 4} };
	self:ProcessResourceList(7, 1, self.marsh_list, resources_to_place)

	local resources_to_place = {
	{self.oil_ID, oil_amt, 55, 1, 5},
	{self.aluminum_ID, alum_amt, 15, 1, 2},
	{self.iron_ID, iron_amt, 35, 1, 2} };
	self:ProcessResourceList(16, 1, self.tundra_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.oil_ID, oil_amt, 65, 1, 5},
	{self.aluminum_ID, alum_amt, 15, 1, 2},
	{self.iron_ID, iron_amt, 20, 1, 2} };
	self:ProcessResourceList(15, 1, self.snow_flat_list, resources_to_place)

	local resources_to_place = {
	{self.oil_ID, oil_amt, 70, 1, 2},
	{self.iron_ID, iron_amt, 30, 1, 2} };
	self:ProcessResourceList(11, 1, self.desert_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.iron_ID, iron_amt, 26, 1, 3},
	{self.coal_ID, coal_amt, 35, 1, 3},
	{self.aluminum_ID, alum_amt, 39, 1, 3} };
	self:ProcessResourceList(22, 1, self.hills_list, resources_to_place)

	local resources_to_place = {
	{self.coal_ID, coal_amt, 30, 1, 2},
	{self.uranium_ID, uran_amt, 70, 1, 2} };
	self:ProcessResourceList(33, 1, self.jungle_flat_list, resources_to_place)
	local resources_to_place = {
	{self.coal_ID, coal_amt, 25, 1, 2},
	{self.oil_ID, oil_amt, 25, 1, 5},
	{self.uranium_ID, uran_amt, 50, 10, 0} };
	self:ProcessResourceList(39, 1, self.forest_flat_list, resources_to_place)

	local resources_to_place = {
	{self.horse_ID, horse_amt, 100, 1, 5} };
	self:ProcessResourceList(10, 1, self.dry_grass_flat_no_feature, resources_to_place)
	local resources_to_place = {
	{self.horse_ID, horse_amt, 100, 1, 5} };
	self:ProcessResourceList(10, 1, self.plains_flat_no_feature, resources_to_place)

	self:AddModernMinorStrategicsToCityStates()
	
	self:PlaceSmallQuantitiesOfStrategics(23 * bonus_multiplier, self.land_list);
	
	self:PlaceOilInTheSea();

	

	if self.amounts_of_resources_placed[self.iron_ID + 1] < 8 then

		local resources_to_place = { {self.iron_ID, iron_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.iron_ID + 1] < 4 * self.iNumCivs then

		local resources_to_place = { {self.iron_ID, iron_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs then
		print("Map has very low horse, adding another.");
		local resources_to_place = { {self.horse_ID, horse_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place)
		

		local resources_to_place = { {self.horse_ID, horse_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.dry_grass_flat_no_feature, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.coal_ID + 1] < 8 then

		local resources_to_place = { {self.coal_ID, coal_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.coal_ID + 1] < 4 * self.iNumCivs then

		local resources_to_place = { {self.coal_ID, coal_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.oil_ID + 1] < 4 * self.iNumCivs then
		print("Map has very low oil, adding another.");
		local resources_to_place = { {self.oil_ID, oil_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.aluminum_ID + 1] < 4 * self.iNumCivs then

		local resources_to_place = { {self.aluminum_ID, alum_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	
	while self.amounts_of_resources_placed[self.uranium_ID + 1] < 5 * self.iNumCivs do
		print("Map has very low uranium, adding another.");
		local resources_to_place = { {self.uranium_ID, uran_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	
	

	print("Map Generation - Placing Bonuses");
	
	if self._lek_coastal_refish then



		local plotDataImmediateCoast = {};
		local plotDataNextToImmediateCoast ={};
		local plotDataIsThreeFromMainland = {};

		plotDataImmediateCoast, plotDataNextToImmediateCoast = GenerateMainlandExpandedCoastData();
		plotDataIsThreeFromMainland = GenerateThreeFromMainlandCoast(plotDataImmediateCoast, plotDataNextToImmediateCoast);

		table.fill(self.mainland_coast_list, false, iW * iH);
		table.fill(self.mainland_coast_list_inner, false, iW * iH);
		table.fill(self.mainland_coast_list_second, false, iW * iH);
		table.fill(self.mainland_coast_list_outer, false, iW * iH);

		local temp_list_pangaea = {};
		local temp_list_pangaea_inner = {};
		local temp_list_pangaea_second = {};
		local temp_list_pangaea_outer = {};


		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				local test_plot = Map.GetPlot(x, y)
				if plotDataImmediateCoast[i] == true then
					self.mainland_coast_list[i] = true;
					self.mainland_coast_list_inner[i] = true;

					table.insert(temp_list_pangaea, i);
					table.insert(temp_list_pangaea_inner, i);
				elseif plotDataNextToImmediateCoast[i] == true then
					self.mainland_coast_list[i] = true;
					self.mainland_coast_list_second[i] = true;

					table.insert(temp_list_pangaea, i);
					table.insert(temp_list_pangaea_second, i);
				elseif plotDataIsThreeFromMainland[i] == true then
					self.mainland_coast_list[i] = true;
					self.mainland_coast_list_outer[i] = true;

					table.insert(temp_list_pangaea, i);
					table.insert(temp_list_pangaea_outer, i);
				end
			end
		end

		local fish_coast_inner = GetShuffledCopyOfTable(temp_list_pangaea_inner)
		local fish_coast_second = GetShuffledCopyOfTable(temp_list_pangaea_second)
		local fish_coast_outer = GetShuffledCopyOfTable(temp_list_pangaea_outer)

		if beta_tectonic_mounts == 1 then
			self:PlaceFishMainland(3 * bonus_multiplier, fish_coast_inner);
			self:PlaceFishMainland(8 * bonus_multiplier, fish_coast_second);
			self:PlaceFishMainland(15 * bonus_multiplier, fish_coast_outer);
		else
			self:PlaceFishMainland(3 * bonus_multiplier + (0.15*1.75), fish_coast_inner);
			self:PlaceFishMainland(8 * bonus_multiplier + (0.4*1.75), fish_coast_second);
			self:PlaceFishMainland(15 * bonus_multiplier + (0.75*1.75), fish_coast_outer);
		end

	elseif self.method == 1 then


		local plotDataImmediateCoast = {};
		local plotDataNextToImmediateCoast ={};
		local plotDataIsThreeFromMainland = {};

		plotDataImmediateCoast, plotDataNextToImmediateCoast = GenerateMainlandExpandedCoastData();
		plotDataIsThreeFromMainland = GenerateThreeFromMainlandCoast(plotDataImmediateCoast, plotDataNextToImmediateCoast);

		table.fill(self.mainland_coast_list, false, iW * iH);


		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				if plotDataImmediateCoast[i] == true or plotDataNextToImmediateCoast[i] == true or plotDataIsThreeFromMainland[i] == true then
					self.mainland_coast_list[i] = true;
				end
			end
		end

		local temp_list_panagaea = {};

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				local test_plot = Map.GetPlot(x, y);
				if self.mainland_coast_list[i] == true then	
					table.insert(temp_list_panagaea, i);
				end
			end
		end

		local fish_list_b = {}

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				local test_plot = Map.GetPlot(x, y);
				if self.mainland_coast_list[i] == true or (beta_lake_fish == 2 and test_plot:IsLake() == true) then	
					table.insert(fish_list_b, i);
					if((beta_lake_fish == 2 and test_plot:IsLake() == true)) then
						table.insert(fish_list_b, i);
						table.insert(fish_list_b, i);
						table.insert(fish_list_b, i);
						table.insert(fish_list_b, i);
					end
				end
			end
		end

		self.coast_list_panagaea = GetShuffledCopyOfTable(temp_list_panagaea)
		


		if beta_tectonic_mounts == 1 then
			self:PlaceFishMainland(10 * bonus_multiplier, GetShuffledCopyOfTable(fish_list_b));
		else
			self:PlaceFishMainland(10 * bonus_multiplier + (0.5*1.75), GetShuffledCopyOfTable(fish_list_b));
		end
	end

	if self._lek_coastal_refish then

		if beta_tectonic_mounts == 1 then
			self:PlaceFish(16 * bonus_multiplier, self.coast_list);
		else
			self:PlaceFish(16 * bonus_multiplier + (0.8*1.75), self.coast_list);
		end
	else
		if beta_tectonic_mounts == 1 then
			self:PlaceFish(8 * bonus_multiplier, self.coast_list);
		else
			self:PlaceFish(8 * bonus_multiplier + (0.4*1.75), self.coast_list);
		end
	end


	self:PlaceSexyBonusAtCivStarts()
	self:AddExtraBonusesToHillsRegions()
	
	local resources_to_place = {
	{self.deer_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(6 * bonus_multiplier, 3, self.extra_deer_list, resources_to_place)

	local resources_to_place = {
	{self.wheat_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(6 * bonus_multiplier, 3, self.desert_wheat_list, resources_to_place)

	local resources_to_place = {
	{self.deer_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(8 * bonus_multiplier, 3, self.tundra_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.banana_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(10 * bonus_multiplier, 3, self.banana_list, resources_to_place)

	local resources_to_place = {
	{self.wheat_ID, 1, 100, 1, 3} };
	self:ProcessResourceList(30 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.bison_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(15 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.cow_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.cow_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.grass_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.stone_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(20 * bonus_multiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.bison_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(20 * bonus_multiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.sheep_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(20 * bonus_multiplier, 3, self.hills_open_list, resources_to_place)

	local resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(10 * bonus_multiplier, 3, self.tundra_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(16 * bonus_multiplier, 3, self.desert_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.deer_ID, 1, 100, 3, 4} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.forest_flat_that_are_not_tundra, resources_to_place)
	
	local resources_to_place = {
	{self.hardwood_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.hills_covered_list, resources_to_place)

	local resources_to_place = {
	{self.hardwood_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.flat_covered, resources_to_place)

	local resources_to_place = {
	{self.hardwood_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.tundra_flat_forest, resources_to_place)
	
	local resources_to_place = {
	{self.maize_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(35 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)
	
	
end



function AssignStartingPlots:PlaceFishMainland(frequency, plot_list)





	if plot_list == nil then

		return
	end


	local iNumTotalPlots = tablelength(plot_list)
	local iNumFishToPlace = math.ceil(iNumTotalPlots / frequency);
	

	local current_index = 1;
	for place_resource = 1, iNumFishToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];

				if self.fishData[plotIndex] == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y)

					local featureType = res_plot:GetFeatureType()

					if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then

						if res_plot:GetResourceType(-1) == -1 then

							local fish_radius = Map.Rand(0, "Fish Radius - Place Fish LUA") + 1;



							if beta_tectonic_mounts == 1 then
								res_plot:SetResourceType(self.fish_ID, 1);






								self:PlaceResourceImpact(x, y, 4, fish_radius);
								placed_this_res = true;
								self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
							else
								if BETA_TECTONIC_LANDS[plotIndex] < 1 then 
									res_plot:SetResourceType(self.fish_ID, 1);






									self:PlaceResourceImpact(x, y, 4, fish_radius);
									placed_this_res = true;
									self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
								end
							end
						end
					end
				end
			end
		end
	end
end



function AssignStartingPlots:PlaceFish(frequency, plot_list)

	if plot_list == nil then

		return
	end
	local iW, iH = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumFishToPlace = math.ceil(iNumTotalPlots / frequency);
	local bMainlandCoast = false;


	local current_index = 1;
	for place_resource = 1, iNumFishToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				bMainlandCoast = false;

				if self.method == 1 then
					bMainlandCoast = self.mainland_coast_list[plotIndex];
				end

				if self.fishData[plotIndex] == 0 and bMainlandCoast == false then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y)
					if res_plot:GetResourceType(-1) == -1 then

						local fish_radius = Map.Rand(4, "Fish Radius - Place Fish LUA") + 1;



						if beta_tectonic_mounts == 1 then
							res_plot:SetResourceType(self.fish_ID, 1);
							self:PlaceResourceImpact(x, y, 4, fish_radius);
							placed_this_res = true;
							self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
						else
							if BETA_TECTONIC_LANDS[plotIndex] < 1 then
								res_plot:SetResourceType(self.fish_ID, 1);
								self:PlaceResourceImpact(x, y, 4, fish_radius);
								placed_this_res = true;
								self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
							end
						end
					end
				end
			end
		end
	end
end



function AssignStartingPlots:AttemptToPlaceBonusResourceAtPlot(x, y, bAllowOasis, Fish_Count)


	local plot = Map.GetPlot(x, y);
	local maxNumGranary = 4
	local maxFishPlace = Fish_Count
	if plot == nil then

		return false
	end
	if plot:GetResourceType(-1) ~= -1 then

		return false
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_SNOW then

		return false
	end
	local featureType = plot:GetFeatureType()
	if featureType == FeatureTypes.FEATURE_OASIS then

		return false
	end
	local plotType = plot:GetPlotType()




	if plotType == PlotTypes.PLOT_OCEAN then
		if maxNumGranary > 0 and maxFishPlace > 0 then
			if terrainType == TerrainTypes.TERRAIN_COAST and featureType == FeatureTypes.NO_FEATURE then
				if plot:IsLake() == false or beta_lake_fish == 2 then
					if beta_tectonic_mounts == 1 then
						plot:SetResourceType(self.fish_ID, 1);
						print("Placed Fish.");
						self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
						maxFishPlace = maxFishPlace - 1;
						return true, false, true
					else
						local i = xy_to_i(x, y, iW, iH)
						if BETA_TECTONIC_LANDS[i] < 1 then
							plot:SetResourceType(self.fish_ID, 1);
							print("Placed Fish.");
							self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
							maxFishPlace = maxFishPlace - 1;
							return true, false, true
						end
					end
				end
			end
		end
	end
	if featureType == FeatureTypes.FEATURE_JUNGLE then
		if maxNumGranary > 0 then
		plot:SetResourceType(self.banana_ID, 1);
		print("Placed Banana.");
		self.amounts_of_resources_placed[self.banana_ID + 1] = self.amounts_of_resources_placed[self.banana_ID + 1] + 1;
		maxNumGranary = maxNumGranary - 1;
		return true, false, false
		else
		return false
		end
	elseif featureType == FeatureTypes.FEATURE_FOREST then
		if maxNumGranary > 0 then
		plot:SetResourceType(self.deer_ID, 1);
		print("Placed Deer.");
		self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1; 
		maxNumGranary = maxNumGranary - 1;
		return true, false, false
		else
		return false
		end
	elseif featureType == FeatureTypes.FEATURE_FOREST then
		plot:SetResourceType(self.hardwood_ID, 1);
		print("Placed Hardwood.");
		self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
		return true, false, false
	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE and terrainType ~= TerrainTypes.TERRAIN_DESERT then

		if maxNumGranary > 0 then
			plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
			plot:SetResourceType(self.deer_ID, 1);
			print("Placed Deer xx.");
			self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
			maxNumGranary = maxNumGranary - 1;
			return true, false, false
		else
			return false
		end

	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE and terrainType ~= TerrainTypes.TERRAIN_DESERT then
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
		plot:SetResourceType(self.hardwood_ID, 1);
		print("Placed Hardwood.");
		self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
		return true, false, false
	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE and terrainType ~= TerrainTypes.TERRAIN_DESERT then
		plot:SetResourceType(self.sheep_ID, 1);
		print("Placed Sheep xx.");
		self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
		return true, false, false
		

	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_GRASS then
		local placethis = Map.Rand(100, "");
		if placethis < 50 then
			if maxNumGranary > 0 then	
				plot:SetResourceType(self.bison_ID, 1);
				print("Placed Bison.");
				self.amounts_of_resources_placed[self.bison_ID + 1] = self.amounts_of_resources_placed[self.bison_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		elseif placethis > 50 and placethis < 90 then
			if maxNumGranary > 0 then
				plot:SetResourceType(self.sheep_ID, 1);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				print("Placed Sheep.");
				self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		else
			if maxNumGranary > 0 then	
				plot:SetResourceType(self.stone_ID, 1);
				print("Placed Stone.");
				self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_GRASS then
		local placethis = Map.Rand(100, "");
		if placethis < 67 then
			if maxNumGranary > 0 then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
				plot:SetResourceType(self.deer_ID, 1);
				print("Placed Deer xx.");
				self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		else
			if maxNumGranary > 0 then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
				plot:SetResourceType(self.deer_ID, 1);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				print("Placed Deer xx.");
				self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_GRASS then
			plot:SetResourceType(self.cow_ID, 1);
			print("Placed Cow.");
			self.amounts_of_resources_placed[self.cow_ID + 1] = self.amounts_of_resources_placed[self.cow_ID + 1] + 1;
		return true, false, false
	

	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		local placethis = Map.Rand(100, "");
		if placethis < 75 then
			if maxNumGranary > 0 then
				plot:SetResourceType(self.wheat_ID, 1);
				print("Placed Wheat.");
				self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		else
			if maxNumGranary > 0 then
				plot:SetResourceType(self.bison_ID, 1);
				print("Placed Bison.");
				self.amounts_of_resources_placed[self.bison_ID + 1] = self.amounts_of_resources_placed[self.bison_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		if maxNumGranary > 0 then
			plot:SetResourceType(self.bison_ID, 1);
			print("Placed Bison.");
			self.amounts_of_resources_placed[self.bison_ID + 1] = self.amounts_of_resources_placed[self.bison_ID + 1] + 1;
			maxNumGranary = maxNumGranary - 1;
			return true, false, false
		else
			return false
		end

	elseif terrainType == TerrainTypes.TERRAIN_DESERT and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then

		local placethis = Map.Rand(100, "");
		if placethis < 25 then
			plot:SetResourceType(self.wheat_ID, 1);
			print("Placed Wheat.");
			self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
			return true, false, false
		elseif placethis > 25 and placethis < 75 then
			plot:SetResourceType(self.sheep_ID, 1);
			plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
			plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
			print("Placed Sheep.");
			self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
			maxNumGranary = maxNumGranary - 1;
			return true, false, false
		else
			if maxNumGranary > 0 then
				plot:SetResourceType(self.stone_ID, 1);
				print("Placed Stone.");
				self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
		
		
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		plot:SetResourceType(self.cow_ID, 1);
		print("Placed Cow.");
		self.amounts_of_resources_placed[self.cow_ID + 1] = self.amounts_of_resources_placed[self.cow_ID + 1] + 1;
		return true, false, false
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
		plot:SetResourceType(self.hardwood_ID, 1);
		print("Placed Hardwood.");
		self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
		return true, false, false
		

	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE then

					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
					plot:SetResourceType(self.deer_ID, 1);
					print("Placed Deer.");
					self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
					return true, false, false
	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE then

					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
					plot:SetResourceType(self.hardwood_ID, 1);
					print("Placed Hardwood.");
					self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
					return true, false, false

	elseif terrainType == TerrainTypes.TERRAIN_DESERT and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE then 
		if plot:IsFreshWater() then

			plot:SetResourceType(self.wheat_ID, 1);
			print("Placed Wheat.");
			self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
			return true, false, false
		elseif bAllowOasis then
					plot:SetFeatureType(FeatureTypes.FEATURE_OASIS, -1);
					print("Placed Oasis.");
					return true, true, false
		else
					print("Not allowed to place any more Oasis help at this site.");
		end
	end
	

	return false, false, false
end



function AssignStartingPlots:PlaceResourceImpactCoastalMod(x, y, impact_table_number, radius, radiusCoastal)




	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local impactPlotIndex = y * iW + x + 1;

	self.cityStateData[impactPlotIndex] = impact_value;

	if radius == 0 then
		return
	end



	local radiusCoastal = radiusCoastal
	if beta_coastal_deadzone == 1 then
		radiusCoastal = 4
	elseif beta_coastal_deadzone == 2 then
		radiusCoastal = 5
	elseif beta_coastal_deadzone == 3 then
		radiusCoastal = 6
	else
		radiusCoastal = 7
	end


	if radius > 0 and radius < iH / 2 then
		for ripple_radius = 1, radius do
			local ripple_value = radius - ripple_radius + 1;





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

						local ringPlotIndex = realY * iW + realX + 1;

						self.cityStateData[ringPlotIndex] = 1;
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported Radius length of ", radius, " passed to PlaceResourceImpact()");
	end

	if radiusCoastal > 0 and radiusCoastal < iH / 2 then
		for ripple_radius = 1, radiusCoastal do
			local ripple_value = radiusCoastal - ripple_radius + 1;





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

						local plot = Map.GetPlot(realX, realY);
						if plot:IsCoastalLand(50) then
							local ringPlotIndex = realY * iW + realX + 1;

							self.cityStateData[ringPlotIndex] = 1;


							self:ExpandCoastalRing(realX, realY, 3)
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported RadiusCoastal length of ", radiusCoastal, " passed to PlaceResourceImpact()");
	end
end








































































							




















	













		

























				









				



					





				







				




				




					





				






















				
















					


			





















					










function AssignStartingPlots:BalanceAndAssign(args)






	self.player_to_region = {};




	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number)
	end



	local bDisableStartBias = Game.GetCustomOption("GAMEOPTION_DISABLE_START_BIAS");
	if bDisableStartBias == 1 then

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
			local i = y * iW + x + 1;
			player:SetStartingPlot(start_plot)
			self.player_to_region[player_ID] = region_number
		end



		if self.bTeamGame == true then
			self:NormalizeTeamLocations()
		end

		return
	end



	local all_regions = {};
	for loop = 1, self.iNumCivs do
		table.insert(all_regions, loop);
	end
	local regions_still_available = GetShuffledCopyOfTable(all_regions)

	local civs_needing_coastal_start = {};
	local civs_priority_coastal_start = {};
	local civs_needing_river_start = {};
	local civs_needing_region_priority = {};
	local civs_needing_region_avoid = {};
	local regions_with_coastal_start = {};
	local regions_with_lake_start = {};
	local regions_with_river_start = {};
	local regions_with_near_river_start = {};
	local civ_status = table.fill(false, GameDefines.MAX_MAJOR_CIVS);
	local region_status = table.fill(false, self.iNumCivs);
	local priority_lists = {};
	local avoid_lists = {};
	local iNumCoastalCivs, iNumRiverCivs, iNumPriorityCivs, iNumAvoidCivs = 0, 0, 0, 0;
	local iNumCoastalCivsRemaining, iNumRiverCivsRemaining, iNumPriorityCivsRemaining, iNumAvoidCivsRemaining = 0, 0, 0, 0;
	

	


	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop];
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		print("Player", playerNum, "of Civ Type", civType);
		local bNeedsCoastalStart = CivNeedsCoastalStart(civType)
		if args.MixedBias and Map.Rand(100, "") >= 0 and CivNeedsPlaceFirstCoastalStart(civType) then 
			bNeedsCoastalStart = false;
		end
		if bNeedsCoastalStart == true then
			print("- - - - - - - needs Coastal Start!"); print("-");
			iNumCoastalCivs = iNumCoastalCivs + 1;
			iNumCoastalCivsRemaining = iNumCoastalCivsRemaining + 1;
			table.insert(civs_needing_coastal_start, playerNum);
			local bPlaceFirst = CivNeedsPlaceFirstCoastalStart(civType);
			if bPlaceFirst then
				print("- - - - - - - needs to Place First!"); print("-");
				table.insert(civs_priority_coastal_start, playerNum);
			end
		else
			local bNeedsRiverStart = CivNeedsRiverStart(civType)
			if bNeedsRiverStart == true then

				iNumRiverCivs = iNumRiverCivs + 1;
				iNumRiverCivsRemaining = iNumRiverCivsRemaining + 1;
				table.insert(civs_needing_river_start, playerNum);
			else
				local iNumRegionPriority = GetNumStartRegionPriorityForCiv(civType)
				if iNumRegionPriority > 0 then

					local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
					iNumPriorityCivs = iNumPriorityCivs + 1;
					iNumPriorityCivsRemaining = iNumPriorityCivsRemaining + 1;
					table.insert(civs_needing_region_priority, playerNum);
					priority_lists[playerNum] = table_of_this_civs_priority_needs;
				else
					local iNumRegionAvoid = GetNumStartRegionAvoidForCiv(civType)
					if iNumRegionAvoid > 0 then

						local table_of_this_civs_avoid_needs = GetStartRegionAvoidListForCiv_GetIDs(civType)
						iNumAvoidCivs = iNumAvoidCivs + 1;
						iNumAvoidCivsRemaining = iNumAvoidCivsRemaining + 1;
						table.insert(civs_needing_region_avoid, playerNum);
						avoid_lists[playerNum] = table_of_this_civs_avoid_needs;
					end
				end
			end
		end
	end
	
	print("Civs with Coastal Bias:", iNumCoastalCivs);
	print("Civs with River Bias:", iNumRiverCivs);
	print("Civs with Region Priority:", iNumPriorityCivs);
	print("Civs with Region Avoid:", iNumAvoidCivs); print("-");
	

	if iNumCoastalCivs > 0 then

		local iNumRegionsWithCoastalStart, iNumRegionsWithLakeStart, iNumUnassignableCoastStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][1] == true then
					print("Region#", region_number, "has a Coastal Start.");
					iNumRegionsWithCoastalStart = iNumRegionsWithCoastalStart + 1;
					table.insert(regions_with_coastal_start, region_number);
				end
			end
		end
		if iNumRegionsWithCoastalStart < iNumCoastalCivs then
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][2] == true and
					   self.startLocationConditions[region_number][1] == false then
						print("Region#", region_number, "has a Lake Start.");
						iNumRegionsWithLakeStart = iNumRegionsWithLakeStart + 1;
						table.insert(regions_with_lake_start, region_number);
					end
				end
			end
		end
		if iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart < iNumCoastalCivs then
			iNumUnassignableCoastStarts = iNumCoastalCivs - (iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart);
		end

		print("iNumCoastalCivs: " .. iNumCoastalCivs);
		print("iNumUnassignableCoastStarts: " .. iNumUnassignableCoastStarts);
		if iNumCoastalCivs - iNumUnassignableCoastStarts > 0 then

			local non_priority_coastal_start = {};
			for loop1, iPlayerNum1 in ipairs(civs_needing_coastal_start) do
				local bAdd = true;
				for loop2, iPlayerNum2 in ipairs(civs_priority_coastal_start) do
					if (iPlayerNum1 == iPlayerNum2) then
						bAdd = false;
					end
				end
				if bAdd then
					table.insert(non_priority_coastal_start, iPlayerNum1);
				end
			end
			
			local shuffled_priority_coastal_start = GetShuffledCopyOfTable(civs_priority_coastal_start);
			local shuffled_non_priority_coastal_start = GetShuffledCopyOfTable(non_priority_coastal_start);
			local shuffled_coastal_civs = {};
			

			for loop, iPlayerNum in ipairs(shuffled_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end
			

			for loop, iPlayerNum in ipairs(shuffled_non_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end			
			
			for loop, iPlayerNum in ipairs(shuffled_coastal_civs) do
				print("shuffled_coastal_civs[" .. loop .. "]: " .. iPlayerNum);
			end
			
			local shuffled_coastal_regions, shuffled_lake_regions;
			local current_lake_index = 1;
			if iNumRegionsWithCoastalStart > 0 then
				shuffled_coastal_regions = GetShuffledCopyOfTable(regions_with_coastal_start);
			end
			if iNumRegionsWithLakeStart > 0 then
				shuffled_lake_regions = GetShuffledCopyOfTable(regions_with_lake_start);
			end
			for loop, playerNum in ipairs(shuffled_coastal_civs) do
				if loop > iNumCoastalCivs - iNumUnassignableCoastStarts then

					break
				end

				if loop <= iNumRegionsWithCoastalStart then

					local choose_this_region = shuffled_coastal_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[playerNum] = choose_this_region


					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, b, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum)
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else

					local choose_this_region = shuffled_lake_regions[current_lake_index];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[playerNum] = choose_this_region

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, b, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum)
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
					current_lake_index = current_lake_index + 1;
				end
			end


		end
	end
	

	if iNumRiverCivs > 0 or iNumCoastalCivsRemaining > 0 then

		local iNumRegionsWithRiverStart, iNumRegionsNearRiverStart, iNumUnassignableRiverStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][3] == true then
					iNumRegionsWithRiverStart = iNumRegionsWithRiverStart + 1;
					table.insert(regions_with_river_start, region_number);
				end
			end
		end
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][4] == true and
				   self.startLocationConditions[region_number][3] == false then
					iNumRegionsNearRiverStart = iNumRegionsNearRiverStart + 1;
					table.insert(regions_with_near_river_start, region_number);
				end
			end
		end
		if iNumRegionsWithRiverStart + iNumRegionsNearRiverStart < iNumRiverCivs then
			iNumUnassignableRiverStarts = iNumRiverCivs - (iNumRegionsWithRiverStart + iNumRegionsNearRiverStart);
		end


		if iNumRiverCivs - iNumUnassignableRiverStarts > 0 then
			local shuffled_river_civs = GetShuffledCopyOfTable(civs_needing_river_start);
			local shuffled_river_regions, shuffled_near_river_regions;
			if iNumRegionsWithRiverStart > 0 then
				shuffled_river_regions = GetShuffledCopyOfTable(regions_with_river_start);
			end
			if iNumRegionsNearRiverStart > 0 then
				shuffled_near_river_regions = GetShuffledCopyOfTable(regions_with_near_river_start);
			end
			for loop, playerNum in ipairs(shuffled_river_civs) do
				if loop > iNumRiverCivs - iNumUnassignableRiverStarts then

					break
				end

				if loop <= iNumRegionsWithRiverStart then

					local choose_this_region = shuffled_river_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[playerNum] = choose_this_region

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else

					local choose_this_region = shuffled_near_river_regions[loop - iNumRegionsWithRiverStart];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[playerNum] = choose_this_region

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end

		if iNumCoastalCivsRemaining > 0 and iNumRiverCivs < iNumRegionsWithRiverStart + iNumRegionsNearRiverStart then
			local iNumFallbacksWithRiverStart, iNumFallbacksNearRiverStart = 0, 0;
			local fallbacks_with_river_start, fallbacks_with_near_river_start = {}, {};
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][3] == true then
						iNumFallbacksWithRiverStart = iNumFallbacksWithRiverStart + 1;
						table.insert(fallbacks_with_river_start, region_number);
					end
				end
			end
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][4] == true and
					   self.startLocationConditions[region_number][3] == false then
						iNumFallbacksNearRiverStart = iNumFallbacksNearRiverStart + 1;
						table.insert(fallbacks_with_near_river_start, region_number);
					end
				end
			end
			if iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart > 0 then
			
				local shuffled_coastal_fallback_civs = GetShuffledCopyOfTable(civs_needing_coastal_start);
				local shuffled_river_fallbacks, shuffled_near_river_fallbacks;
				if iNumFallbacksWithRiverStart > 0 then
					shuffled_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_river_start);
				end
				if iNumFallbacksNearRiverStart > 0 then
					shuffled_near_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_near_river_start);
				end
				for loop, playerNum in ipairs(shuffled_coastal_fallback_civs) do
					if loop > iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart then

						break
					end

					if loop <= iNumFallbacksWithRiverStart then

						local choose_this_region = shuffled_river_fallbacks[loop];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);
						self.player_to_region[playerNum] = choose_this_region

						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
						if a then
							table.remove(regions_still_available, c[1]);
						end
					else

						local choose_this_region = shuffled_near_river_fallbacks[loop - iNumRegionsWithRiverStart];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);
						self.player_to_region[playerNum] = choose_this_region

						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
						if a then
							table.remove(regions_still_available, c[1]);
						end
					end
				end
			end
		end
	end
	

	if iNumPriorityCivs > 0 then
		print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
		local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority = 0, 0, 0;
		local single_priority, multi_priority, fallback_priority = {}, {}, {};
		local single_sorted, multi_sorted = {}, {};

		for playerNum, priority_needs in pairs(priority_lists) do
			local len = table.maxn(priority_needs)
			if len == 1 then
				print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
				local priority_data = {playerNum, priority_needs[1]};
				table.insert(single_priority, priority_data)
				iNumSinglePriority = iNumSinglePriority + 1;
			else
				print("Player#", playerNum, "has multiple Region Priority, this many types:", len);
				local priority_data = {playerNum, len};
				table.insert(multi_priority, priority_data)
				iNumMultiPriority = iNumMultiPriority + 1;
			end
		end

		if iNumSinglePriority > 0 then

			for region_type = 1, 9 do
				for loop, data in ipairs(single_priority) do
					if data[2] == region_type then

						table.insert(single_sorted, data);
					end
				end
			end

			for loop, data in ipairs(single_sorted) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				print("* Attempting to assign Player#", iPlayerNum, "to a region of Type#", iPriorityType);
				local bFoundCandidate, candidate_regions = false, {};
				for test_loop, region_number in ipairs(regions_still_available) do
					if self.regionTypes[region_number] == iPriorityType then
						table.insert(candidate_regions, region_number);
						bFoundCandidate = true;

					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[iPlayerNum] = choose_this_region
					print("Player Number", iPlayerNum, "with single Region Priority assigned to Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					table.insert(fallback_priority, data)
					iNumNeedFallbackPriority = iNumNeedFallbackPriority + 1;

				end
			end
		end

		if iNumMultiPriority > 0 then
			for iNumPriorities = 2, 8 do
				for loop, data in ipairs(multi_priority) do
					if data[2] == iNumPriorities then

						table.insert(multi_sorted, data);
					end
				end
			end

			for loop, data in ipairs(multi_sorted) do
				local iPlayerNum = data[1];
				local iNumPriorityTypes = data[2];

				local bFoundCandidate, candidate_regions = false, {};
				for test_loop, region_number in ipairs(regions_still_available) do
					for inner_loop = 1, iNumPriorityTypes do
						local region_type_to_test = priority_lists[iPlayerNum][inner_loop];
						if self.regionTypes[region_number] == region_type_to_test then
							table.insert(candidate_regions, region_number);
							bFoundCandidate = true;

						end
					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[iPlayerNum] = choose_this_region

					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end


				end
			end
		end

		if iNumNeedFallbackPriority > 0 then
			for loop, data in ipairs(fallback_priority) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				print("* Attempting to assign Player#", iPlayerNum, "to a fallback region as similar as possible to Region Type#", iPriorityType);
				local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, regions_still_available)
				if choose_this_region == -1 then

				else
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					self.player_to_region[iPlayerNum] = choose_this_region

					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end
	end
	

	if iNumAvoidCivs > 0 then

		local avoid_sorted, avoid_unsorted, avoid_counts = {}, {}, {};

		for playerNum, avoid_needs in pairs(avoid_lists) do
			local len = table.maxn(avoid_needs)

			local avoid_data = {playerNum, len};
			table.insert(avoid_unsorted, avoid_data)
			table.insert(avoid_counts, len)
		end
		table.sort(avoid_counts)
		for loop, avoid_count in ipairs(avoid_counts) do
			for test_loop, avoid_data in ipairs(avoid_unsorted) do
				if avoid_count == avoid_data[2] then
					table.insert(avoid_sorted, avoid_data[1])
					table.remove(avoid_unsorted, test_loop)
				end
			end
		end

		for loop = iNumAvoidCivs, 1, -1 do
			local iPlayerNum = avoid_sorted[loop];
			local candidate_regions = {};
			for test_loop, region_number in ipairs(regions_still_available) do
				local bFoundCandidate = true;
				for inner_loop, region_type_to_avoid in ipairs(avoid_lists[iPlayerNum]) do
					if self.regionTypes[region_number] == region_type_to_avoid then
						bFoundCandidate = false;
					end
				end
				if bFoundCandidate == true then
					table.insert(candidate_regions, region_number);

				end
			end
			if table.maxn(candidate_regions) > 0 then
				local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
				local choose_this_region = candidate_regions[diceroll];
				local x = self.startingPlots[choose_this_region][1];
				local y = self.startingPlots[choose_this_region][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);
				self.player_to_region[iPlayerNum] = choose_this_region

				region_status[choose_this_region] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
				if a then
					table.remove(regions_still_available, c[1]);
				end


			end
		end
	end
				

	local playerList, regionList = {}, {};
	for loop = 1, self.iNumCivs do
		local player_ID = self.player_ID_list[loop];
		if civ_status[player_ID + 1] == false then
			table.insert(playerList, player_ID);
		end
		if region_status[loop] == false then
			table.insert(regionList, loop);
		end
	end
	local iNumRemainingPlayers = table.maxn(playerList);
	local iNumRemainingRegions = table.maxn(regionList);
	if iNumRemainingPlayers > 0 or iNumRemainingRegions > 0 then




		if iNumRemainingPlayers ~= iNumRemainingRegions then
			print("-"); print("ERROR: Number of civs remaining after handling biases does not match number of regions remaining!"); print("-");
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for index, player_ID in ipairs(playerListShuffled) do
			local region_number = regionList[index];
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];

			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
			self.player_to_region[player_ID] = region_number
		end
	end




	if self.bTeamGame == true then
		self:NormalizeTeamLocations()
	end

end




function AssignStartingPlots:MeasureStartPlacementFertilityOfLandmass(iAreaID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY)






	local xEnd, yEnd;



	if wrapsX then
		xEnd = iEastX + iW;
	else
		xEnd = iEastX;
	end
	if wrapsY then
		yEnd = iNorthY + iH;
	else
		yEnd = iNorthY;
	end

	local areaFertilityTable = {};
	local areaFertilityCount = 0;
	local plotCount = 0;
	for yLoop = iSouthY, yEnd do
		for xLoop = iWestX, xEnd do
			plotCount = plotCount + 1;
			local x = xLoop % iW;
			local y = yLoop % iH;
			local plot = Map.GetPlot(x, y);
			local thisPlotsArea = plot:GetArea()
			if thisPlotsArea ~= iAreaID then
				table.insert(areaFertilityTable, 0);
			else
				local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true);
				table.insert(areaFertilityTable, plotFertility);
				areaFertilityCount = areaFertilityCount + plotFertility;
			end
		end
	end
	


	return areaFertilityTable, areaFertilityCount, plotCount
end



function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()


	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 4, 7, 6, 7, 8;

	if self.resource_setting == 1 or self.resource_setting == 2 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 5, 4, 5, 6;
	elseif self.resource_setting == 3 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 6, 5, 6, 7;
	elseif self.resource_setting == 7 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 5, 8, 7, 8, 9;
	elseif self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 6, 9, 8, 9, 10;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt-1+beta_more_coal, alum_amt
end



function ZonePinches()
	local blocking_map = {}
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH)
			local plot = Map.GetPlot(x, y)
			local ptype = plot:GetPlotType()
			if ptype == PlotTypes.PLOT_OCEAN then
				blocking_map[i] = -1
			elseif ptype == PlotTypes.PLOT_MOUNTAIN then
				blocking_map[i] = 1
			else
				blocking_map[i] = 0
			end
		end
	end



	local blob_graph, blobs = get_blobs(blocking_map)

	for loop, blob in ipairs(blobs) do
		if tablelength(blob) > 1 then
			local coastal_check = false
			local inland_check = false
			local convert_canidates = {}
			for i, dumby in pairs(blob) do
				local x, y = i_to_xy(i, iW, iH)
				local plot = Map.GetPlot(x, y)
				if plot:IsCoastalLand(50) then
					coastal_check = true
					convert_canidates[i] = true
				else
					inland_check = true
				end
			end
			if inland_check and coastal_check then
				for i, dumby in pairs(convert_canidates) do
					local x, y = i_to_xy(i, iW, iH)
					local plot = Map.GetPlot(x, y)
					plot:SetPlotType(PlotTypes.PLOT_OCEAN)
					BETA_TECTONIC_LANDS[i] = 4
				end
			end
		end
	end

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH)
			local plot = Map.GetPlot(x, y)
			if BETA_TECTONIC_LANDS[i] > 0 and plot:GetPlotType() == PlotTypes.PLOT_OCEAN and beta_using_lekmod and plot:GetTerrainType() == TerrainTypes.TERRAIN_COAST then
				local adj_is = adj_is_cache[i]
				local land_adjacent = false
				for loop, adj_i in ipairs(adj_is) do
					local adj_x, adj_y = i_to_xy(adj_i, iW, iH)
					local adj_plot = Map.GetPlot(adj_x, adj_y)
					if adj_plot ~= nil then
						if adj_plot:GetPlotType() ~= PlotTypes.PLOT_OCEAN then
							land_adjacent = true
						end
					end
				end
				if land_adjacent then
					plot:SetFeatureType(FeatureTypes.FEATURE_SHALLOWS)
				end
			end
		end
	end
end



function ZoneSets(asp)
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH)
			local plot = Map.GetPlot(x, y)
			if BETA_TECTONIC_LANDS[i] > 0 and plot:GetPlotType() == PlotTypes.PLOT_OCEAN then
				asp:PlaceResourceImpact(x, y, 1, 0)
				asp:PlaceResourceImpact(x, y, 2, 0)
				asp:PlaceResourceImpact(x, y, 3, 0)
				asp:PlaceResourceImpact(x, y, 4, 0)
				asp:PlaceResourceImpact(x, y, 5, 0)
				asp:PlaceResourceImpact(x, y, 6, 0)
				asp:PlaceResourceImpact(x, y, 7, 0)
				asp:PlaceResourceImpact(x, y, 8, 0)
			end
		end
	end
end


function StartPlotSystem()



	local RegionalMethod = 1;


	local AllowInlandSea = Map.GetCustomOption(17)
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

	if beta_tectonic_mounts == 2 then
		ZonePinches()
		Map.RecalculateAreas()
	end

	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()

	     start_plot_database._lek_prioritize_center = true

	     start_plot_database.centerBias = 20
	     start_plot_database.middleBias = 50

	     start_plot_database._lek_collide_coastals = true


		start_plot_database._lek_coastal_refish = false

	if beta_tectonic_mounts == 2 then
		ZoneSets(start_plot_database)
	end

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
	elseif wonders == 15 then
		wonders = Map.Rand(3, "") + 3
	elseif wonders == 16 then
		wonders = Map.Rand(5, "") + 2
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
	local accept_map = start_plot_database:PlaceResourcesAndCityStates();

	return accept_map, start_plot_database.region_luxury_assignment, start_plot_database.player_to_region


end




function AssignStartingPlots:PlaceCityStateInRegion(city_state_number, region_number)

	local iW, iH = Map.GetGridSize();
	local placed_city_state = false;
	local reached_middle = false;
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];

	local eligible_coastal, eligible_inland = {}, {};


	local x, y;
	local curWX = iWestX;
	local curSY = iSouthY;
	local curWid = iWidth;
	local curHei = iHeight;
	while placed_city_state == false and reached_middle == false do

		local nextWX, nextSY, nextWid, nextHei;
		eligible_coastal, eligible_inland, nextWX, nextSY, nextWid, nextHei,
		  reached_middle = self:ObtainNextSectionInRegion(curWX, curSY, curWid, curHei, iAreaID, false, false)
		curWX, curSY, curWid, curHei = nextWX, nextSY, nextWid, nextHei;

		x, y, placed_city_state = self:PlaceCityState(eligible_coastal, eligible_inland, false, false)
	end







































































































	if placed_city_state == true then

		self.cityStatePlots[city_state_number] = {x, y, region_number};
		self.city_state_validity_table[city_state_number] = true;
		local city_state_ID = city_state_number + GameDefines.MAX_MAJOR_CIVS - 1;
		local cityState = Players[city_state_ID];
		local cs_start_plot = Map.GetPlot(x, y)

		cs_starts[city_state_ID] = cs_start_plot
		self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true)
		self:PlaceResourceImpact(x, y, 5, 4)
		self:PlaceResourceImpact(x, y, 2, 3)
		self:PlaceResourceImpact(x, y, 1, 0)
		self:PlaceResourceImpact(x, y, 3, 3)
		self:PlaceResourceImpact(x, y, 4, 3)
		self:PlaceResourceImpact(x, y, 7, 3)

		local impactPlotIndex = y * iW + x + 1;
		self.playerCollisionData[impactPlotIndex] = true;

	else

		self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
	end
end



function AssignStartingPlots:PlaceCityStates()
	print("Map Generation - Choosing sites for City States");




	self:AssignCityStatesToRegionsOrToUninhabited()
	


	local iW, iH = Map.GetGridSize();
	local iUninhabitedCandidatePlots = table.maxn(self.uninhabited_areas_coastal_plots) + table.maxn(self.uninhabited_areas_inland_plots);

	for cs_number, region_number in ipairs(self.city_state_region_assignments) do
		if cs_number <= self.iNumCityStates then
			if region_number == -1 and iUninhabitedCandidatePlots > 0 then

				iUninhabitedCandidatePlots = iUninhabitedCandidatePlots - 1;
				local cs_x, cs_y, success;
				cs_x, cs_y, success = self:PlaceCityState(self.uninhabited_areas_coastal_plots, self.uninhabited_areas_inland_plots, true, true)












				if success == true then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true;
					local city_state_ID = cs_number + GameDefines.MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y)

					cs_starts[city_state_ID] = cs_start_plot
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true)
					self:PlaceResourceImpact(cs_x, cs_y, 5, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 2, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 1, 0)
					self:PlaceResourceImpact(cs_x, cs_y, 3, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 4, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 7, 3)

					local impactPlotIndex = cs_y * iW + cs_x + 1;
					self.playerCollisionData[impactPlotIndex] = true;

				else

					self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
				end
			elseif region_number == -1 and iUninhabitedCandidatePlots <= 0 then
				local iRandRegion = 1 + Map.Rand(self.iNumCivs, "Emergency Redirect of CS placement, choosing Region - LUA");

				self:PlaceCityStateInRegion(cs_number, iRandRegion)
			else

				self:PlaceCityStateInRegion(cs_number, region_number)
			end
		end
	end
	

	if self.iNumCityStatesDiscarded > 0 then

		local cs_last_chance_plot_list = {};
		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				if self:CanPlaceCityStateAt(x, y, -1, false, false) == true then
					local i = y * iW + x + 1;
					table.insert(cs_last_chance_plot_list, i);
				end
			end
		end
		local iNumLastChanceCandidates = table.maxn(cs_last_chance_plot_list);

		if iNumLastChanceCandidates > 0 then
			print("-"); print("-"); print("ALERT: Some City States failed to be placed due to overcrowding. Attempting 'last chance' placement method.");
			print("Total number of remaining eligible candidate plots:", iNumLastChanceCandidates);
			local last_chance_shuffled = GetShuffledCopyOfTable(cs_last_chance_plot_list)
			local cs_list = {};
			for cs_num = 1, self.iNumCityStates do
				if self.city_state_validity_table[cs_num] == false then
					table.insert(cs_list, cs_num);

				end
			end
			for loop, cs_number in ipairs(cs_list) do
				local cs_x, cs_y, success;
				cs_x, cs_y, success = self:PlaceCityState(last_chance_shuffled, {}, true, true)
				if success == true then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true;
					local city_state_ID = cs_number + GameDefines.MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y)

					cs_starts[city_state_ID] = cs_start_plot
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true)
					self:PlaceResourceImpact(cs_x, cs_y, 5, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 2, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 1, 0)
					self:PlaceResourceImpact(cs_x, cs_y, 3, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 4, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 7, 3)

					local impactPlotIndex = cs_y * iW + cs_x + 1;
					self.playerCollisionData[impactPlotIndex] = true;
					self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded - 1;

				else

					break
				end
			end
			if self.iNumCityStatesDiscarded > 0 then
				print("-"); print("ALERT: No eligible city state sites remain. DISCARDING", self.iNumCityStatesDiscarded, "city states. BYE BYE!"); print("-");
			end
		else
			print("-"); print("-"); print("ALERT: No eligible city state sites remain. DISCARDING", self.iNumCityStatesDiscarded, "city states. BYE BYE!"); print("-");
		end
	end
end



function GenMap()

	local accept_map = false
	local attempts = 0
	local region_luxury_assignment = {}
	local iNumCivs, iNumCityStates, player_ID_list, bTeamGame, teams_with_major_civs, number_civs_per_team = GetPlayerAndTeamInfo()


	local attempt_cap = 15

	local beta_tectonic_mounts = 2

	beta_reroll_count = 4

	if beta_reroll_count == 2 then
		attempt_cap = 50
	elseif beta_reroll_count == 3 then
		attempt_cap = 100
	elseif beta_reroll_count == 4 then
		attempt_cap = 300
	elseif beta_reroll_count == 5 then
		attempt_cap = 99999
	end

	local time = os.clock()
	local elapsed_time = 0

	while accept_map == false and attempts < attempt_cap do
		attempts = attempts + 1
		print("Generating Map");




		







		nextRiverID = 0;
		_rivers = {};



		coast_cache = {};
		has_cached_coast = false;

		cs_starts = {}


		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				local plot = Map.GetPlot(x, y);
				plot:SetFeatureType(FeatureTypes.NO_FEATURE);
				plot:SetResourceType(-1, -1);
				plot:SetImprovementType(-1);
				plot:SetNWOfRiver(false);
				plot:SetNEOfRiver(false);
				plot:SetWOfRiver(false);
			end
		end


		



		GeneratePlotTypes();



		

		GenerateTerrain();


		

		
		

		Map.RecalculateAreas();


		


		

		AddRivers();

		


		

		AddLakes();

		


		


		AddFeatures();
		




		Map.RecalculateAreas();












		accept_map, region_luxury_assignment, player_to_region = StartPlotSystem();
















	end

	local elapsed_time = os.clock()-time






	for city_state_ID, cs_start_plot in pairs(cs_starts) do
		local cityState = Players[city_state_ID];
		cityState:SetStartingPlot(cs_start_plot)
	end


	Map.RecalculateAreas();






















	
	



	
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			
			local plot = Map.GetPlot(x, y)
			local res_ID = plot:GetResourceType(-1)
			local featureType = plot:GetFeatureType()
			local terrainType = plot:GetTerrainType()
			local plotType = plot:GetPlotType()

			

			if beta_lake_fish == 2 then
				if plot:IsLake() and res_ID == GameInfo.Resources.RESOURCE_FISH.ID then
					Game.SetPlotExtraYield(x, y, YieldTypes.YIELD_FOOD, -1)
					Game.SetPlotExtraYield(x, y, YieldTypes.YIELD_GOLD, 1)
				end
			end













		end
	end


	if accept_map then
		for id, player in pairs(Players) do
			player:AddNotification(NotificationTypes.NOTIFICATION_TRADE_ROUTE, "The map was made " .. tostring(attempts) .. " times across " .. string.format("%.3f", elapsed_time) .. " seconds. (" .. string.format("%.4f", elapsed_time/attempts) .. " Seconds/Map)", "Remakes")
		end
	else
		for id, player in pairs(Players) do
			player:AddNotification(NotificationTypes.NOTIFICATION_TRADE_ROUTE_BROKEN, "The map was made the Maximum (" .. attempt_cap .. ") number of times - using the last map made.", "Remakes Failed")
		end
	end


	AddGoodies();


	DetermineContinents();

	local beta_errors = Map.GetCustomOption(19)
	if beta_errors == 3 then
		local bad_var = {}
		if bad_var > 5 then
			print("THIS SHOULD NEVER WORK")
		end
	end

end

function GenerateMap()

	beta_using_lekmod = false
	if GameInfo.Resources.RESOURCE_CORAL ~= nil then
		beta_using_lekmod = true
	end



	beta_flexy_lux = 2

	beta_no_ice = 2

	beta_bay_diffusion =  2
	ripple_decider = Map.GetCustomOption(6);
	beta_cliffs = Map.GetCustomOption(24);

	beta_coast_zone = Map.GetCustomOption(23);


	beta_coastal_deadzone = 4

	beta_more_coal = 1

	beta_cs_placement = 2


	beta_tectonic_mounts = 2

	beta_spawn_validation = 2


	beta_lake_fish = 1


	beta_inland_prox_penalty = 2

	iW, iH = Map.GetGridSize();

	wrapX = Map:IsWrapX();
	wrapY = Map:IsWrapY();

	adj_is_cache = {}

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = xy_to_i(x, y, iW, iH);
			local adj_is = GetAdjacent(x, y);
			adj_is_cache[i] = adj_is;
		end
	end

	local beta_errors = Map.GetCustomOption(19)

	 local status, err = xpcall(GenMap, debug.traceback)
	 if status == false then
		print(err)
		if beta_errors ~= 1 then
			for id, player in pairs(Players) do
				player:AddNotification(NotificationTypes.NOTIFICATION_SPY_WAS_KILLED, err, "Error Report - Please Screenshot and Send to Jacobian.")
			end
		end
	else
		if beta_errors ~= 1 then
			for id, player in pairs(Players) do
				player:AddNotification(NotificationTypes.NOTIFICATION_SPY_RIG_ELECTION_SUCCESS, "The map completed generation. You are good to play. Thank you for playing Lekmap v6.0!", "The Map Script Completed Successfully")
			end
		end
	end
end

function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)

	if(plot:CanHaveFeature(self.featureIce)) then
		if Map.IsWrapX() and (iY == 0 or iY == self.iGridH - 1) then
			if beta_no_ice == 1 then
				plot:SetFeatureType(self.featureIce, -1)
			elseif beta_no_ice == 2 and plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
				plot:SetFeatureType(self.featureIce, -1)
			elseif beta_no_ice == 4 then
				plot:SetFeatureType(FeatureTypes.FEATURE_FALLOUT, -1)
			end
		else
			local rand = Map.Rand(200, "Add Ice Lua")/100.0;

			if(rand < 8 * (lat - 0.875)) then
				if beta_no_ice == 1 then
					plot:SetFeatureType(self.featureIce, -1)
				elseif beta_no_ice == 2 and plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
					plot:SetFeatureType(self.featureIce, -1)
				elseif beta_no_ice == 4 then
					plot:SetFeatureType(FeatureTypes.FEATURE_FALLOUT, -1)
				end
			elseif(rand < 4 * (lat - 0.75)) then
				if beta_no_ice == 1 then
					plot:SetFeatureType(self.featureIce, -1)
				elseif beta_no_ice == 2 and plot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
					plot:SetFeatureType(self.featureIce, -1)
				elseif beta_no_ice == 4 then
					plot:SetFeatureType(FeatureTypes.FEATURE_FALLOUT, -1)
				end
			end
		end
	end
end

function AddRivers()
	local riverSourceRangeDefault = 1.5;
	local seaWaterRangeDefault = 0.75;
	local plotsPerRiverEdge =  9;
	local riverRnd = 210;
	
	if beta_tectonic_mounts == 2 then
		riverSourceRangeDefault = 1.48;
		seaWaterRangeDefault = 0.75;
		plotsPerRiverEdge =  9;
		riverRnd = 212;
	end

	local rivers_level = Map.GetCustomOption(9)

	if rivers_level == 1 then
		riverSourceRangeDefault = 3;
		seaWaterRangeDefault = 2;
		plotsPerRiverEdge =  12;
		riverRnd = 180;
	elseif rivers_level	== 3 then
		riverSourceRangeDefault = 1;
		seaWaterRangeDefault = 0.5;
		plotsPerRiverEdge =  8;
		riverRnd = 230;
	end

	print("Map Generation - Adding Rivers");
	
	local passConditions = {
		function(plot)
			return (plot:IsHills() or plot:IsMountain());
		end,
		
		function(plot)
			return (not plot:IsCoastalLand()) and (Map.Rand(8, "MapGenerator AddRivers") == 0);
		end,
		
		function(plot)
			local area = plot:Area();
			return (plot:IsHills() or plot:IsMountain()) and (area:GetNumRiverEdges() <	((area:GetNumTiles() / plotsPerRiverEdge) + 1));
		end,
		
		function(plot)
			local area = plot:Area();
			return (area:GetNumRiverEdges() < (area:GetNumTiles() / plotsPerRiverEdge) + 1);
		end
	}
	
	for iPass, passCondition in ipairs(passConditions) do
					
		if (iPass <= 2) then
			riverSourceRange = riverSourceRangeDefault;
			seaWaterRange = seaWaterRangeDefault;
		else
			riverSourceRange = (riverSourceRangeDefault / 2);
			seaWaterRange = (seaWaterRangeDefault / 2);
		end
			
		local iW, iH = Map.GetGridSize();

		for i = 0, (iW * iH) - 1, 1 do
			plot = Map.GetPlotByIndex(i);
			if(not plot:IsWater()) then
				if(passCondition(plot)) then
					if (not Map.FindWater(plot, riverSourceRange, true)) then
						if (not Map.FindWater(plot, seaWaterRange, false)) then
							local inlandCorner = plot:GetInlandCorner();
							if(inlandCorner) then
								local riverChance = Map.Rand(riverRnd, "MapGenerator AddRivers");
								if riverChance > 178 then
									DoRiver(inlandCorner);
								end
							end
						end
					end
				end			
			end
		end
	end		
end
