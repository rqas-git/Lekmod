------------------------------------------------------------------------------
--	FILE:	 Lekmapv2.2.lua (Modified Pangaea_Plus.lua)
--	AUTHOR:  Original Bob Thomas, Changes HellBlazer, lek10, EnormousApplePie, Cirra, Meota
--	PURPOSE: Global map script - Simulates a Pan-Earth Supercontinent, with
--           numerous tectonic island chains.
------------------------------------------------------------------------------
--	Copyright (c) 2011 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

include("HBMapGenerator");
include("HBFractalWorld");
include("HBFeatureGenerator");
include("HBTerrainGenerator");
include("IslandMaker");
include("MultilayeredFractal");

------------------------------------------------------------------------------
include("HBMapOptions");

function GetMapScriptInfo()
	local world_age, temperature, rainfall, sea_level, resources = GetCoreMapOptions()
	return {
		Name = "Lekmap: Donut (Bonus Map)",
		Description = "A map script made for Lekmod based on HB's Mapscript v8.1. Donut",
		IsAdvancedMap = false,
		IconIndex = 18,
		SortIndex = 2,
		SupportsMultiplayer = true,
		CustomOptions = LekmapOptions.Create({
			[11] = { -- Land Size X
				DefaultValue = 8,
			},
			[12] = { -- Land Size Y
				DefaultValue = 13,
			},
		}, {
			{ -- 15
				Name = "Coastal Spawns",
				Values = {
					"Coastal Civs Only",
					"Random",
					"Random+ (~2 coastals)",
				},
				DefaultValue = 1,
				SortPriority = -85,
			},
			{ -- 16
				Name = "Coastal Luxes",
				Values = {
					"Guaranteed",
					"Random",
				},
				DefaultValue = 1,
				SortPriority = -84,
			},
			{ -- 17
				Name = "Inland Sea Spawns",
				Values = {
					"Allowed",
					"Not Allowed for Coastal Civs",
				},
				DefaultValue = 2,
				SortPriority = -83,
			},
			{ -- 18
				Name = "Radius Size",
				Values = {
					"0 - to the edge",
					"1",
					"2",
					"3",
					"4 - default",
				},
				DefaultValue = 5,
				SortPriority = -82,
			},
			{ -- 19
				Name = "Holy Radius Factor",
				Values = {
					"1,5",
					"2 - default",
					"3",
					"4",
				},
				DefaultValue = 2,
				SortPriority = -81,
			},
			{ -- 20
				Name = "Outside Region",
				Values = {
					"TXT_KEY_MAP_OPTION_HILLS",
					"TXT_KEY_MAP_OPTION_MOUNTAINS",
					"TXT_KEY_MAP_OPTION_OCEAN",
					"TXT_KEY_MAP_OPTION_STANDARD",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 3,
				SortPriority = -80,
			},
			{ -- 21
				Name = "Desert Size",
				Values = {
					"sparse",
					"average",
					"plentiful",
				},
				DefaultValue = 2,
				SortPriority = -79,
			},
			{ -- 22
				Name = "TXT_KEY_MAP_OPTION_CENTER_REGION",
				Values = {
					"TXT_KEY_MAP_OPTION_HILLS",
					"TXT_KEY_MAP_OPTION_MOUNTAINS",
					"TXT_KEY_MAP_OPTION_OCEAN",
					"TXT_KEY_MAP_OPTION_DESERT",
					"TXT_KEY_MAP_OPTION_STANDARD",
					"TXT_KEY_MAP_OPTION_RANDOM",
				},
				DefaultValue = 3,
				SortPriority = -78,
			},
		}),
	};
end
------------------------------------------------------------------------------
function GetMapInitData(worldSize)
	
	local LandSizeX = 28 + (Map.GetCustomOption(11) * 2);
	local LandSizeY = 18 + (Map.GetCustomOption(12) * 2);

	local worldsizes = {};

	worldsizes = {

		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {LandSizeX, LandSizeY}, -- 720
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {LandSizeX, LandSizeY}, -- 1664
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {LandSizeX, LandSizeY}, -- 2480
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {LandSizeX, LandSizeY}, -- 3900
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {LandSizeX, LandSizeY}, -- 6076
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {LandSizeX, LandSizeY} -- 9424
		}
		
	local grid_size = worldsizes[worldSize];
	--
	local world = GameInfo.Worlds[worldSize];
	if (world ~= nil) then
		return {
			Width = grid_size[1],
			Height = grid_size[2],
			WrapX = false,
		}; 
	end

end
------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function MultilayeredFractal:GeneratePlotsByRegion()
	-- Sirian's MultilayeredFractal controlling function.
	-- You -MUST- customize this function for each script using MultilayeredFractal.
	--
	-- This implementation is specific to Donut.
	local iW, iH = Map.GetGridSize();
	local fracFlags = {FRAC_WRAP_X = false, FRAC_POLAR = true};

	-- Get user input.
	hole_type = Map.GetCustomOption(22) -- Global
	
	-- Get user input.
	radiusSize = Map.GetCustomOption(18) -- Global
	radiusSize = radiusSize - 1;
	-- Get user input.
	holyRadiusFactor = Map.GetCustomOption(19) -- Global
	if holyRadiusFactor == 1 then
		holyRadiusFactor = 1.5;
	end
	
		-- Get user input.
	outsideTerrainType = Map.GetCustomOption(20) -- Global
	
	if outsideTerrainType == 5 then
		outsideTerrainType = 1 + Map.Rand(4, "Random terrain type for outside region - Donut Lua");
	end
	
	if hole_type == 6 then
		hole_type = 1 + Map.Rand(5, "Random terrain type for center region - Donut Lua");
	end

	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 6,
		};
	local grain = worldsizes[Map.GetWorldSize()];

	local terrainFrac = Fractal.Create(iW, iH, grain, fracFlags, -1, -1);
	local iHillsThreshold = terrainFrac:GetHeight(91);
	local iPeaksThreshold = terrainFrac:GetHeight(96);
	local iHillsClumps = terrainFrac:GetHeight(4);

	local hillsFrac = Fractal.Create(iW, iH, grain, fracFlags, -1, -1);
	local iHillsBottom1 = hillsFrac:GetHeight(20);
	local iHillsTop1 = hillsFrac:GetHeight(30);
	local iHillsBottom2 = hillsFrac:GetHeight(70);
	local iHillsTop2 = hillsFrac:GetHeight(80);

	local iCenterX = math.floor(iW / 2);
	local iCenterY = math.floor(iH / 2);
	local iRadius = iCenterX - radiusSize;
	local iHoleRadius = math.floor(iRadius / holyRadiusFactor);

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local fDistance = 0;
			if x ~= iCenterX or y ~= iCenterY then
				fDistance = math.sqrt(((x - iCenterX) ^ 2) + ((y - iCenterY) ^ 2));
			end
			if fDistance > iRadius then
				if outsideTerrainType == 1 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif outsideTerrainType == 2 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				elseif outsideTerrainType == 3 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
				else -- standard type
					local val = terrainFrac:GetHeight(x, y);
					local hillsVal = hillsFrac:GetHeight(x, y);
					if val >= iPeaksThreshold then
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
					elseif val >= iHillsThreshold or val <= iHillsClumps then
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
					elseif hillsVal >= iHillsBottom1 and hillsVal <= iHillsTop1 then
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
					elseif hillsVal >= iHillsBottom2 and hillsVal <= iHillsTop2 then
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
					else
						self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
					end
				end
			elseif fDistance < iHoleRadius and hole_type < 4 then -- Plot is in hole of donut.
				if hole_type == 1 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif hole_type == 2 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				else
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_OCEAN;
				end
			else -- standard type
				local val = terrainFrac:GetHeight(x, y);
				local hillsVal = hillsFrac:GetHeight(x, y);
				if val >= iPeaksThreshold then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_MOUNTAIN;
				elseif val >= iHillsThreshold or val <= iHillsClumps then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif hillsVal >= iHillsBottom1 and hillsVal <= iHillsTop1 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				elseif hillsVal >= iHillsBottom2 and hillsVal <= iHillsTop2 then
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_HILLS;
				else
					self.wholeworldPlotTypes[i] = PlotTypes.PLOT_LAND;
				end
			end
		end
	end

	-- Plot Type generation completed. Return global plot array.
	return self.wholeworldPlotTypes
end
------------------------------------------------------------------------------
function GeneratePlotTypes()
	print("Setting Plot Types (Lua Donut) ...");

	local layered_world = MultilayeredFractal.Create();
	local plotsDonut = layered_world:GeneratePlotsByRegion();
	
	SetPlotTypes(plotsDonut);

	GenerateCoasts();
end
------------------------------------------------------------------------------

----------------------------------------------------------------------------------
function TerrainGenerator:GenerateTerrainAtPlot(iX, iY)
	local plot = Map.GetPlot(iX, iY);
	if (plot:IsWater()) then
		local val = plot:GetTerrainType();
		if val == TerrainTypes.NO_TERRAIN then -- Error handling.
			val = self.terrainGrass;
			plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
		end
		return val;	 
	end

	local iW, iH = Map.GetGridSize();
	local iCenterX = math.floor(iW / 2);
	local iCenterY = math.floor(iH / 2);
	local iRadius = iCenterX - radiusSize;
	local iHoleRadius = math.floor(iRadius / holyRadiusFactor);
	local terrainVal = self.terrainGrass;

	local fDistance = 0;
	if iX ~= iCenterX or iY ~= iCenterY then
		fDistance = math.sqrt(((iX - iCenterX) ^ 2) + ((iY - iCenterY) ^ 2));
	end
	if fDistance < iHoleRadius and hole_type == 4 then -- Desert plot in center.
		terrainVal = self.terrainDesert;
	else
		local desertVal = self.deserts:GetHeight(iX, iY);
		local plainsVal = self.plains:GetHeight(iX, iY);
		if ((desertVal >= self.iDesertBottom) and (desertVal <= self.iDesertTop)) then
			terrainVal = self.terrainDesert;
		elseif ((plainsVal >= self.iPlainsBottom) and (plainsVal <= self.iPlainsTop)) then
			terrainVal = self.terrainPlains;
		end
	end
	
	return terrainVal;
end
----------------------------------------------------------------------------------
function GenerateTerrain()
	print("Generating Terrain (Lua Donut) ...");
	-- desertSize

	local desertSize = 2 + 10 * Map.GetCustomOption(21); -- desertSize 12/22/32
	local args = {
		iDesertPercent = desertSize,
	};

	local terraingen = TerrainGenerator.Create(args);

	terrainTypes = terraingen:GenerateTerrain();
	
	SetTerrainTypes(terrainTypes);
	
	-- FixIslands();
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
function FeatureGenerator:AddIceAtPlot(plot, iX, iY, lat)
	-- No ice.
end
------------------------------------------------------------------------------
function FeatureGenerator:AddJunglesAtPlot(plot, iX, iY, lat)
	-- No jungle.
end
------------------------------------------------------------------------------
function AddFeatures()
	print("Adding Features (Lua Donut) ...");

	local featuregen = FeatureGenerator.Create();

	-- False parameter removes mountains from coastlines.
	featuregen:AddFeatures(false);
end
------------------------------------------------------------------------------


------------------------------------------------------------------------------
include("HBRegionalStartPlotSystem");
------------------------------------------------------------------------------