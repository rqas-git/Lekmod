










FractalWorld = {};

function FractalWorld.Create(fracXExp, fracYExp)
	
	local gridWidth, gridHeight = Map.GetGridSize();
	
	local data = {
		InitFractal = FractalWorld.InitFractal,
		ShiftPlotTypes = FractalWorld.ShiftPlotTypes,
		ShiftPlotTypesBy = FractalWorld.ShiftPlotTypesBy,
		DetermineXShift = FractalWorld.DetermineXShift,
		DetermineYShift = FractalWorld.DetermineYShift,
		GenerateCenterRift = FractalWorld.GenerateCenterRift,
		GeneratePlotTypes = FractalWorld.GeneratePlotTypes,
		
		iFlags = Map.GetFractalFlags(),
		
		fracXExp = fracXExp,
		fracYExp = fracYExp,
		
		iNumPlotsX = gridWidth,
		iNumPlotsY = gridHeight,
		plotTypes = table.fill(PlotTypes.PLOT_OCEAN, gridWidth * gridHeight)
	};
		
	return data;
end	

function FractalWorld:InitFractal(args)
	if(args == nil) then args = {}; end
	
	print("=============== USING NQ PANGAEA FRACTAL ===============");


	local continent_grain = 1;

	local rift_grain = 1;

	local invert_heights = false;

	local polar = true;
	local ridge_flags = args.ridge_flags or self.iFlags;
	
	local fracFlags = {};
	
	if(invert_heights) then
		fracFlags.FRAC_INVERT_HEIGHTS = true;
	end
	
	if(polar) then
		fracFlags.FRAC_POLAR = true;
	end
	
	if(rift_grain > 0 and rift_grain < 4) then
		self.riftsFrac = Fractal.Create((self.iNumPlotsX), self.iNumPlotsY, rift_grain, {}, self.fracXExp, self.fracYExp);
		self.continentsFrac = Fractal.CreateRifts((self.iNumPlotsX * 1.1), self.iNumPlotsY, continent_grain, fracFlags, self.riftsFrac, self.fracXExp, self.fracYExp);
	else
		self.continentsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, continent_grain, fracFlags, self.fracXExp, self.fracYExp);	
	end









	local WorldSizeTypes = {};
	for row in GameInfo.Worlds() do
		WorldSizeTypes[row.Type] = row.ID;
	end
	local sizekey = Map.GetWorldSize();
	local sizevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]     = 4,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 8,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 8,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 20,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 24,
		[WorldSizeTypes.WORLDSIZE_HUGE]		= 32
	}

	local numPlates = sizevalues[sizekey] or 4


	self.continentsFrac:BuildRidges(numPlates, ridge_flags, 1, 4);
end

function FractalWorld:ShiftPlotTypes()
	local stripRadius = self.stripRadius;
	local shift_x = 0; 
	local shift_y = 0;

	shift_x = self:DetermineXShift();	
	shift_y = self:DetermineYShift();	
	
	self:ShiftPlotTypesBy(shift_x, shift_y);
end

function FractalWorld:ShiftPlotTypesBy(xshift, yshift)
	if(xshift > 0 or yshift > 0) then
		local iWH = self.iNumPlotsX * self.iNumPlotsY
		local buf = {};
		for i = 1, iWH + 1 do
			buf[i] = self.plotTypes[i];
		end
		
		for iDestY = 0, self.iNumPlotsY do
			for iDestX = 0, self.iNumPlotsX do
				local iDestI = self.iNumPlotsX * iDestY + iDestX;
				local iSourceX = (iDestX + xshift) % self.iNumPlotsX;
				local iSourceY = (iDestY + yshift) % self.iNumPlotsY;
				
				local iSourceI = self.iNumPlotsX * iSourceY + iSourceX
				self.plotTypes[iDestI] = buf[iSourceI]
			end
		end
	end
end

function FractalWorld:DetermineXShift()









	local land_totals = {};
	for x = 0, self.iNumPlotsX - 1 do
		local current_column = 0;
		for y = 0, self.iNumPlotsY - 1 do
			local i = y * self.iNumPlotsX + x + 1;
			if (self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN) then
				current_column = current_column + 1;
			end
		end
		table.insert(land_totals, current_column);
	end
	

	local column_groups = {};

	local group_radius = math.floor(self.iNumPlotsX / 10);

	for column_index = 1, self.iNumPlotsX do
		local current_group_total = 0;
		for current_column = column_index - group_radius, column_index + group_radius do
			local current_index = current_column % self.iNumPlotsX;
			if current_index == 0 then
				current_index = self.iNumPlotsX;
			end
			current_group_total = current_group_total + land_totals[current_index];
		end
		table.insert(column_groups, current_group_total);
	end
	

	local best_value = self.iNumPlotsY * (2 * group_radius + 1);
	local best_group = 1;
	for column_index, group_land_plots in ipairs(column_groups) do
		if group_land_plots < best_value then
			best_value = group_land_plots;
			best_group = column_index;
		end
	end
	

	local x_shift = best_group - 1;
	return x_shift;
end

function FractalWorld:DetermineYShift()



	local land_totals = {};
	for y = 0, self.iNumPlotsY - 1 do
		local current_row = 0;
		for x = 0, self.iNumPlotsX - 1 do
			local i = y * self.iNumPlotsX + x + 1;
			if (self.plotTypes[i] ~= PlotTypes.PLOT_OCEAN) then
				current_row = current_row + 1;
			end
		end
		table.insert(land_totals, current_row);
	end
	

	local row_groups = {};

	local group_radius = math.floor(self.iNumPlotsY / 15);

	for row_index = 1, self.iNumPlotsY do
		local current_group_total = 0;
		for current_row = row_index - group_radius, row_index + group_radius do
			local current_index = current_row % self.iNumPlotsY;
			if current_index == 0 then
				current_index = self.iNumPlotsY;
			end
			current_group_total = current_group_total + land_totals[current_index];
		end
		table.insert(row_groups, current_group_total);
	end
	

	local best_value = self.iNumPlotsX * (2 * group_radius + 1);
	local best_group = 1;
	for row_index, group_land_plots in ipairs(row_groups) do
		if group_land_plots < best_value then
			best_value = group_land_plots;
			best_group = row_index;
		end
	end
	

	local y_shift = best_group - 1;
	return y_shift;
end

function FractalWorld:GenerateCenterRift()







	local riftLean = Map.Rand(2, "FractalWorld Center Rift Lean - Lua");
	

	local riftLine = {};
	local westOfRift = {};
	local eastOfRift = {};

	local primaryMaxLength = math.max(1, math.floor(self.iNumPlotsY / 8));
	local secondaryMaxLength = math.max(1, math.floor(self.iNumPlotsY / 11));
	local tertiaryMaxLength = math.max(1, math.floor(self.iNumPlotsY / 14));
	

	local startDistanceFromCenterColumn = math.floor(self.iNumPlotsY / 8);
	if riftLean == 0 then
		startDistanceFromCenterColumn = -(startDistanceFromCenterColumn);
	end
	local startX = math.floor(self.iNumPlotsX / 2) + startDistanceFromCenterColumn;
	local startY = 0;
	local startingDirection = DirectionTypes.DIRECTION_NORTHWEST;
	if riftLean == 0 then
		startingDirection = DirectionTypes.DIRECTION_NORTHEAST;
	end

	local riftXBoundary = math.floor(self.iNumPlotsX / 2) - startDistanceFromCenterColumn;
	







	local currentDirection = startingDirection;
	local currentX = startX;
	local currentY = startY;
	table.insert(riftLine, {currentX, currentY});

	local rowIndex = currentY + 1;
	westOfRift[rowIndex] = currentX - 1;
	eastOfRift[rowIndex] = currentX + 1;

	local plotIndex = currentX + 1;
	self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
	

	if riftLean == 0 then
		while currentY < self.iNumPlotsY - 1 do

			local nextDirection = 0;

			if currentDirection == DirectionTypes.DIRECTION_EAST then
				local segmentLength = Map.Rand(tertiaryMaxLength + 1, "FractalWorld Center Rift Segment Length - Lua");

				if currentX >= riftXBoundary then
					nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
				else
					local dice = Map.Rand(3, "FractalWorld Center Rift Direction - Lua");
					if dice == 1 then
						nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
					else
						nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
					end
				end

				local plotsToDo = segmentLength;
				while plotsToDo > 0 do
					currentX = currentX + 1;
					rowIndex = currentY;

					eastOfRift[rowIndex] = currentX + 1;
					plotIndex = currentY * self.iNumPlotsX + currentX + 1;
					self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
					plotsToDo = plotsToDo - 1;
				end

			elseif currentDirection == DirectionTypes.DIRECTION_NORTHWEST then
				local segmentLength = Map.Rand(secondaryMaxLength + 1, "FractalWorld Center Rift Segment Length - Lua");

				if currentX >= riftXBoundary then
					nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
				else
					local dice = Map.Rand(4, "FractalWorld Center Rift Direction - Lua");
					if dice == 2 then
						nextDirection = DirectionTypes.DIRECTION_EAST;
					else
						nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
					end
				end

				local plotsToDo = segmentLength;
				while plotsToDo > 0 and currentY < self.iNumPlotsY - 1 do



					local nextPlot = Map.PlotDirection(currentX, currentY, currentDirection);
					currentX = nextPlot:GetX();
					currentY = currentY + 1;
					rowIndex = currentY;
					westOfRift[rowIndex] = currentX - 1;
					eastOfRift[rowIndex] = currentX + 1;
					plotIndex = currentY * self.iNumPlotsX + currentX + 1;
					self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
					plotsToDo = plotsToDo - 1;
				end
				
			else
				local segmentLength = Map.Rand(primaryMaxLength + 1, "FractalWorld Center Rift Segment Length - Lua");

				if currentX >= riftXBoundary then
					nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
				else
					local dice = Map.Rand(2, "FractalWorld Center Rift Direction - Lua");
					if dice == 1 and currentY > self.iNumPlotsY * 0.28 then
						nextDirection = DirectionTypes.DIRECTION_EAST;
					else
						nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
					end
				end

				local plotsToDo = segmentLength;
				while plotsToDo > 0 and currentY < self.iNumPlotsY - 1 do



					local nextPlot = Map.PlotDirection(currentX, currentY, currentDirection);
					currentX = nextPlot:GetX();
					currentY = currentY + 1;
					rowIndex = currentY;
					westOfRift[rowIndex] = currentX - 1;
					eastOfRift[rowIndex] = currentX + 1;
					plotIndex = currentY * self.iNumPlotsX + currentX + 1;
					self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
					plotsToDo = plotsToDo - 1;
				end
			end
			

			currentDirection = nextDirection;
		end

	else
		while currentY < self.iNumPlotsY - 1 do

			local nextDirection = 0;

			if currentDirection == DirectionTypes.DIRECTION_WEST then
				local segmentLength = Map.Rand(tertiaryMaxLength + 1, "FractalWorld Center Rift Segment Length - Lua");

				if currentX <= riftXBoundary then
					nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
				else
					local dice = Map.Rand(3, "FractalWorld Center Rift Direction - Lua");
					if dice == 1 then
						nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
					else
						nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
					end
				end

				local plotsToDo = segmentLength;
				while plotsToDo > 0 do
					currentX = currentX - 1;
					rowIndex = currentY;
					westOfRift[rowIndex] = currentX - 1;

					plotIndex = currentY * self.iNumPlotsX + currentX + 1;
					self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
					plotsToDo = plotsToDo - 1;
				end

			elseif currentDirection == DirectionTypes.DIRECTION_NORTHEAST then
				local segmentLength = Map.Rand(secondaryMaxLength + 1, "FractalWorld Center Rift Segment Length - Lua");

				if currentX <= riftXBoundary then
					nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
				else
					local dice = Map.Rand(4, "FractalWorld Center Rift Direction - Lua");
					if dice == 2 then
						nextDirection = DirectionTypes.DIRECTION_WEST;
					else
						nextDirection = DirectionTypes.DIRECTION_NORTHWEST;
					end
				end

				local plotsToDo = segmentLength;
				while plotsToDo > 0 and currentY < self.iNumPlotsY - 1 do



					local nextPlot = Map.PlotDirection(currentX, currentY, currentDirection);
					currentX = nextPlot:GetX();
					currentY = currentY + 1;
					rowIndex = currentY;
					westOfRift[rowIndex] = currentX - 1;
					eastOfRift[rowIndex] = currentX + 1;
					plotIndex = currentY * self.iNumPlotsX + currentX + 1;
					self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
					plotsToDo = plotsToDo - 1;
				end
				
			else
				local segmentLength = Map.Rand(primaryMaxLength + 1, "FractalWorld Center Rift Segment Length - Lua");

				if currentX <= riftXBoundary then
					nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
				else
					local dice = Map.Rand(2, "FractalWorld Center Rift Direction - Lua");
					if dice == 1 and currentY > self.iNumPlotsY * 0.28 then
						nextDirection = DirectionTypes.DIRECTION_WEST;
					else
						nextDirection = DirectionTypes.DIRECTION_NORTHEAST;
					end
				end

				local plotsToDo = segmentLength;
				while plotsToDo > 0 and currentY < self.iNumPlotsY - 1 do



					local nextPlot = Map.PlotDirection(currentX, currentY, currentDirection);
					currentX = nextPlot:GetX();
					currentY = currentY + 1;
					rowIndex = currentY;
					westOfRift[rowIndex] = currentX - 1;
					eastOfRift[rowIndex] = currentX + 1;
					plotIndex = currentY * self.iNumPlotsX + currentX + 1;
					self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
					plotsToDo = plotsToDo - 1;
				end
			end
			

			currentDirection = nextDirection;
		end
	end

	westOfRift[self.iNumPlotsY] = currentX - 1;
	eastOfRift[self.iNumPlotsY] = currentX + 1;
	plotIndex = (self.iNumPlotsY - 1) * self.iNumPlotsX + currentX + 1;
	self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;


	local horizontalDrift = 3;
	local verticalDrift = 2;

	if riftLean == 0 then

		for y = self.iNumPlotsY - 1 - verticalDrift, 0, -1 do
			local thisRowX = westOfRift[y+1];
			for x = horizontalDrift, thisRowX do
				local sourcePlotIndex = y * self.iNumPlotsX + x + 1;
				local destPlotIndex = (y + verticalDrift) * self.iNumPlotsX + (x - horizontalDrift) + 1;
				self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
			end
		end

		for y = verticalDrift, self.iNumPlotsY - 1 do
			local thisRowX = eastOfRift[y+1];
			for x = thisRowX, self.iNumPlotsX - horizontalDrift - 1 do
				local sourcePlotIndex = y * self.iNumPlotsX + x + 1;
				local destPlotIndex = (y - verticalDrift) * self.iNumPlotsX + (x + horizontalDrift) + 1;
				self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
			end
		end


		for y = 0, verticalDrift - 1 do
			local thisRowX = westOfRift[y+1];
			for x = 0, thisRowX do
				local plotIndex = y * self.iNumPlotsX + x + 1;
				self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end

		for y = self.iNumPlotsY - verticalDrift, self.iNumPlotsY - 1 do
			local thisRowX = eastOfRift[y+1];
			for x = thisRowX, self.iNumPlotsX - 1 do
				local plotIndex = y * self.iNumPlotsX + x + 1;
				self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end

		for y = verticalDrift, self.iNumPlotsY - 1 - verticalDrift do
			local westX = westOfRift[y-verticalDrift+1] - horizontalDrift + 1;
			local eastX = eastOfRift[y+verticalDrift+1] + horizontalDrift - 1;
			for x = westX, eastX do
				local plotIndex = y * self.iNumPlotsX + x + 1;
				self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end

	else

		for y = verticalDrift, self.iNumPlotsY - 1 do
			local thisRowX = westOfRift[y+1];
			for x = horizontalDrift, thisRowX do
				local sourcePlotIndex = y * self.iNumPlotsX + x + 1;
				local destPlotIndex = (y - verticalDrift) * self.iNumPlotsX + (x - horizontalDrift) + 1;
				self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
			end
		end

		for y = self.iNumPlotsY - 1 - verticalDrift, 0, -1 do
			local thisRowX = eastOfRift[y+1];
			for x = thisRowX, self.iNumPlotsX - horizontalDrift - 1 do
				local sourcePlotIndex = y * self.iNumPlotsX + x + 1;
				local destPlotIndex = (y + verticalDrift) * self.iNumPlotsX + (x + horizontalDrift) + 1;
				self.plotTypes[destPlotIndex] = self.plotTypes[sourcePlotIndex]
			end
		end


		for y = self.iNumPlotsY - verticalDrift, self.iNumPlotsY - 1 do
			local thisRowX = westOfRift[y+1];
			for x = 0, thisRowX do
				local plotIndex = y * self.iNumPlotsX + x + 1;
				self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end

		for y = 0, verticalDrift - 1 do
			local thisRowX = eastOfRift[y+1];
			for x = thisRowX, self.iNumPlotsX - 1 do
				local plotIndex = y * self.iNumPlotsX + x + 1;
				self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end

		for y = verticalDrift, self.iNumPlotsY - 1 - verticalDrift do
			local westX = westOfRift[y+verticalDrift+1] - horizontalDrift + 1;
			local eastX = eastOfRift[y-verticalDrift+1] + horizontalDrift - 1;
			for x = westX, eastX do
				local plotIndex = y * self.iNumPlotsX + x + 1;
				self.plotTypes[plotIndex] = PlotTypes.PLOT_OCEAN;
			end
		end
	end
end

function FractalWorld:GeneratePlotTypes(args)








	local args = args or {};
	local sea_level = args.sea_level or 2;
	local world_age = args.world_age or 2;


	local sea_level_low = args.sea_level_low or 65;
	local sea_level_normal = args.sea_level_normal or 72;
	local sea_level_high = args.sea_level_high or 78;
	local world_age_old = args.world_age_old or 2;
	local world_age_normal = args.world_age_normal or 3;
	local world_age_new = args.world_age_new or 5;

	local extra_mountains = args.extra_mountains or 0;
	local grain_amount = args.grain_amount or 3;
	local adjust_plates = args.adjust_plates or 1.0;
	local shift_plot_types = args.shift_plot_types or true;
	local tectonic_islands = args.tectonic_islands or false;
	local hills_ridge_flags = args.hills_ridge_flags or self.iFlags;
	local peaks_ridge_flags = args.peaks_ridge_flags or self.iFlags;
	local has_center_rift = args.has_center_rift or false;
	

	local water_percent = sea_level_normal;
	if sea_level == 1 then
		water_percent = sea_level_low
	elseif sea_level == 3 then
		water_percent = sea_level_high
	else
	end


	local adjustment = world_age_normal;
	if world_age == 3 then
		adjustment = world_age_old;
		adjust_plates = adjust_plates * 0.75;
	elseif world_age == 1 then
		adjustment = world_age_new;
		adjust_plates = adjust_plates * 1.5;
	else
	end

	local hillsBottom1 = 28 - adjustment;
	local hillsTop1 = 28 + adjustment;
	local hillsBottom2 = 72 - adjustment;
	local hillsTop2 = 72 + adjustment;
	local hillsClumps = 1 + adjustment;
	local hillsNearMountains = 90 - (adjustment * 2) - extra_mountains;
	local mountains = 100 - adjustment - extra_mountains;


	local WorldSizeTypes = {};
	for row in GameInfo.Worlds() do
		WorldSizeTypes[row.Type] = row.ID;
	end
	local sizekey = Map.GetWorldSize();

	local sizevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]     = 3,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 3,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 4,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 4,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 5,
		[WorldSizeTypes.WORLDSIZE_HUGE]		= 5
	};
	local grain = sizevalues[sizekey] or 3;

	local platevalues = {
		[WorldSizeTypes.WORLDSIZE_DUEL]		= 6,
		[WorldSizeTypes.WORLDSIZE_TINY]     = 9,
		[WorldSizeTypes.WORLDSIZE_SMALL]    = 12,
		[WorldSizeTypes.WORLDSIZE_STANDARD] = 18,
		[WorldSizeTypes.WORLDSIZE_LARGE]    = 24,
		[WorldSizeTypes.WORLDSIZE_HUGE]     = 30
	};
	local numPlates = platevalues[sizekey] or 5;

	numPlates = numPlates * adjust_plates;


	self.hillsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);
	self.mountainsFrac = Fractal.Create(self.iNumPlotsX, self.iNumPlotsY, grain, self.iFlags, self.fracXExp, self.fracYExp);










	self.hillsFrac:BuildRidges(numPlates, hills_ridge_flags, 1, 2);

	self.mountainsFrac:BuildRidges((numPlates * 2) / 3, peaks_ridge_flags, 6, 1);


	local iWaterThreshold = self.continentsFrac:GetHeight(water_percent);
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
		
			local i = y * self.iNumPlotsX + x + 1;
			local val = self.continentsFrac:GetHeight(x, y);
			local mountainVal = self.mountainsFrac:GetHeight(x, y);
			local hillVal = self.hillsFrac:GetHeight(x, y);
	
			if(val <= iWaterThreshold) then
				self.plotTypes[i] = PlotTypes.PLOT_OCEAN;
				
				if tectonic_islands then
					if (mountainVal == iMountain100) then
						self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
					elseif (mountainVal == iMountain99) then
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					elseif (mountainVal == iMountain97) or (mountainVal == iMountain95) then
						self.plotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
					
			else
				if (mountainVal >= iMountainThreshold) then
					if (hillVal >= iPassThreshold) then
						self.plotTypes[i] = PlotTypes.PLOT_HILLS;
					else
						self.plotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
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
	end

	if(shift_plot_types) then
		self:ShiftPlotTypes();

		if(has_center_rift) then
			self:GenerateCenterRift()
		end
	end

	return self.plotTypes;
end
