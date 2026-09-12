









TerrainGenerator = {};

function TerrainGenerator.Create(args)








	local args = args or {};
	local temperature = args.temperature or 2;
	local fracXExp = args.fracXExp or -1;
	local fracYExp = args.fracYExp or -1;
	local grain_amount = args.grain_amount or 3;
	local grassMoist = args.iGrassMoist or 2;



	local temperature_shift = args.temperature_shift or 0.1;
	local desert_shift = args.desert_shift or 16;


	local iDesertPercent = args.iDesertPercent or 32;
	local iPlainsPercent = args.iPlainsPercent or 65;

	if grassMoist == 1 then
		iPlainsPercent = 50;
	elseif grassMoist == 3 then
		iPlainsPercent = 80;
	end

	local fSnowLatitude  = args.fSnowLatitude  or 0.90;

	local tundra_level = Map.GetCustomOption(10)

	local fTundraLatitude = args.fTundraLatitude or 0.59;

	if tundra_level	== 1 then
		fTundraLatitude = 0.65;
	elseif tundra_level	== 3 then
		fTundraLatitude = 0.35;
	end

	local fGrassLatitude = args.fGrassLatitude or 0.1;

	if grassMoist == 3 then
		fGrassLatitude = 0.05;
	end

	local fDesertBottomLatitude = args.fDesertBottomLatitude or 0.2;
	local fDesertTopLatitude = args.fDesertTopLatitude or 0.5;

	if temperature == 1 then
		iDesertPercent = iDesertPercent - desert_shift;
		fTundraLatitude = fTundraLatitude - (temperature_shift * 1.5);
		fDesertTopLatitude = fDesertTopLatitude - temperature_shift;
		fGrassLatitude = fGrassLatitude - (temperature_shift * 0.5);
	elseif temperature == 3 then
		iDesertPercent = iDesertPercent + desert_shift;
		fSnowLatitude  = fSnowLatitude + (temperature_shift * 0.5);
		fTundraLatitude = fTundraLatitude + temperature_shift;
		fDesertTopLatitude = fDesertTopLatitude + temperature_shift;
		fGrassLatitude = fGrassLatitude - (temperature_shift * 0.5);
	else
	end












	local gridWidth, gridHeight = Map.GetGridSize();
	local world_info = GameInfo.Worlds[Map.GetWorldSize()];

	local data = {


		InitFractals			= TerrainGenerator.InitFractals,
		GetLatitudeAtPlot		= TerrainGenerator.GetLatitudeAtPlot,
		GenerateTerrain			= TerrainGenerator.GenerateTerrain,
		GenerateTerrainAtPlot	= TerrainGenerator.GenerateTerrainAtPlot,


		grain_amount	= grain_amount,
		fractalFlags	= Map.GetFractalFlags(),
		iWidth			= gridWidth,
		iHeight			= gridHeight,

		iDesertPercent	= iDesertPercent,
		iPlainsPercent	= iPlainsPercent,

		iDesertTopPercent		= 100,
		iDesertBottomPercent	= math.max(0, math.floor(100-iDesertPercent)),
		iPlainsTopPercent		= 100,
		iPlainsBottomPercent	= math.max(0, math.floor(100-iPlainsPercent)),

		fSnowLatitude			= fSnowLatitude,
		fTundraLatitude			= fTundraLatitude,
		fGrassLatitude			= fGrassLatitude,
		fDesertBottomLatitude	= fDesertBottomLatitude,
		fDesertTopLatitude		= fDesertTopLatitude,

		fracXExp		= fracXExp,
		fracYExp		= fracYExp,

	}

	data:InitFractals();

	return data;
end

function TerrainGenerator:InitFractals()
	self.deserts = Fractal.Create(	self.iWidth, self.iHeight,
									self.grain_amount, self.fractalFlags,
									self.fracXExp, self.fracYExp);

	self.iDesertTop = self.deserts:GetHeight(self.iDesertTopPercent);
	self.iDesertBottom = self.deserts:GetHeight(self.iDesertBottomPercent);

	self.plains = Fractal.Create(	self.iWidth, self.iHeight,
									self.grain_amount, self.fractalFlags,
									self.fracXExp, self.fracYExp);

	self.iPlainsTop = self.plains:GetHeight(self.iPlainsTopPercent);
	self.iPlainsBottom = self.plains:GetHeight(self.iPlainsBottomPercent);

	self.variation = Fractal.Create(self.iWidth, self.iHeight,
									self.grain_amount, self.fractalFlags,
									self.fracXExp, self.fracYExp);

	self.terrainDesert	= GameInfoTypes["TERRAIN_DESERT"];
	self.terrainPlains	= GameInfoTypes["TERRAIN_PLAINS"];
	self.terrainSnow	= GameInfoTypes["TERRAIN_SNOW"];
	self.terrainTundra	= GameInfoTypes["TERRAIN_TUNDRA"];
	self.terrainGrass	= GameInfoTypes["TERRAIN_GRASS"];
end

function TerrainGenerator:GetLatitudeAtPlot(iX, iY)


	local lat = math.abs((self.iHeight / 2) - iY) / (self.iHeight / 2);


	lat = lat + (128 - self.variation:GetHeight(iX, iY))/(255.0 * 5.0);

	lat = math.clamp(lat, 0, 1);

	return lat;
end

function TerrainGenerator:GenerateTerrain()

	local terrainData = {};
	for x = 0, self.iWidth - 1 do
		for y = 0, self.iHeight - 1 do
			local i = y * self.iWidth + x;
			local terrain = self:GenerateTerrainAtPlot(x, y);
			terrainData[i] = terrain;
		end
	end

	return terrainData;
end

function TerrainGenerator:GenerateTerrainAtPlot(iX,iY)
	local lat = self:GetLatitudeAtPlot(iX,iY);

	local plot = Map.GetPlot(iX, iY);
	if (plot:IsWater()) then
		local val = plot:GetTerrainType();
		if val == TerrainTypes.NO_TERRAIN then
			val = self.terrainGrass;
			plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
		end
		return val;
	end

	local terrainVal = self.terrainGrass;

	if(lat >= self.fSnowLatitude) then
		terrainVal = self.terrainSnow;
	elseif(lat >= self.fTundraLatitude) then
		terrainVal = self.terrainTundra;
	elseif (lat < self.fGrassLatitude) then
		terrainVal = self.terrainGrass;
	else
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if ((desertVal >= self.iDesertBottom) and (desertVal <= self.iDesertTop) and (lat >= self.fDesertBottomLatitude) and (lat < self.fDesertTopLatitude)) then
			terrainVal = self.terrainDesert;
		elseif ((plainsVal >= self.iPlainsBottom) and (plainsVal <= self.iPlainsTop)) then
			terrainVal = self.terrainPlains;
		end
	end


	if (terrainVal == TerrainTypes.NO_TERRAIN) then
		return plot:GetTerrainType();
	end

	return terrainVal;
end
