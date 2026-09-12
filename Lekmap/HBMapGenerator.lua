








include("HBAssignStartingPlots");





function Shuffle(t)
	local len = #t;
	local random = Map.Rand;
	for i = 1, len, 1 do
		local k = random(len - 1, "Shuffling Values") + 1;
		t[i], t[k] = t[k], t[i];
	end
end

local _plots = {};
function Plots(sort)
	local _indices = {};
	for i = 1, Map.GetNumPlots(), 1 do
		_indices[i] = i - 1;
	end	
	
	if(sort) then
		sort(_indices);
	end
	
	local cur = 0;
	local it = function()
		cur = cur + 1;
		local index = _indices[cur];
		local plot;
		
		if(index) then
			plot = _plots[index] or Map.GetPlotByIndex(index);
			_plots[index] = plot;
		end
		return index, plot;
	end
	
	return it;
end


function GetCustomSizeMapInitData(worldSize)

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

function GetCoreMapOptions()




	local world_age = {
		Name = "TXT_KEY_MAP_OPTION_WORLD_AGE",
		Values = {
			"TXT_KEY_MAP_OPTION_THREE_BILLION_YEARS",
			"TXT_KEY_MAP_OPTION_FOUR_BILLION_YEARS",
			"TXT_KEY_MAP_OPTION_FIVE_BILLION_YEARS",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2,
		SortPriority = -99,
	};
	local temperature = {
		Name = "TXT_KEY_MAP_OPTION_TEMPERATURE",
		Values = {
			"TXT_KEY_MAP_OPTION_COOL",
			"TXT_KEY_MAP_OPTION_TEMPERATE",
			"TXT_KEY_MAP_OPTION_HOT",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2,
		SortPriority = -98,
	};
	local rainfall = {
		Name = "TXT_KEY_MAP_OPTION_RAINFALL",
		Values = {
			"TXT_KEY_MAP_OPTION_ARID",
			"TXT_KEY_MAP_OPTION_NORMAL",
			"TXT_KEY_MAP_OPTION_WET",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2,
		SortPriority = -97,
	};
	local sea_level = {
		Name = "TXT_KEY_MAP_OPTION_SEA_LEVEL",
		Values = {
			"TXT_KEY_MAP_OPTION_LOW",
			"TXT_KEY_MAP_OPTION_MEDIUM",
			"TXT_KEY_MAP_OPTION_HIGH",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2,
		SortPriority = -96,
	};
	local resources = {
		Name = "TXT_KEY_MAP_OPTION_RESOURCES",
		Values = {
			"TXT_KEY_MAP_OPTION_SPARSE",
			"TXT_KEY_MAP_OPTION_STANDARD",
			"TXT_KEY_MAP_OPTION_ABUNDANT",
			"TXT_KEY_MAP_OPTION_LEGENDARY_START",
			"TXT_KEY_MAP_OPTION_STRATEGIC_BALANCE",
			"TXT_KEY_MAP_OPTION_RANDOM",
		},
		DefaultValue = 2,
		SortPriority = -95,
	};
	return world_age, temperature, rainfall, sea_level, resources
end






if(FlowDirectionTypes ~= nil) then
	TurnRightFlowDirections = {
		[FlowDirectionTypes.FLOWDIRECTION_NORTH] 
			= FlowDirectionTypes.FLOWDIRECTION_NORTHEAST,
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHEAST] 
			= FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST,	
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST] 
			= FlowDirectionTypes.FLOWDIRECTION_SOUTH,		
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTH]	
			= FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST,	
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST] 
			= FlowDirectionTypes.FLOWDIRECTION_NORTHWEST,	
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHWEST]
			= FlowDirectionTypes.FLOWDIRECTION_NORTH,
	};

	TurnLeftFlowDirections = {
		[FlowDirectionTypes.FLOWDIRECTION_NORTH] 
			= FlowDirectionTypes.FLOWDIRECTION_NORTHWEST,
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHEAST] 
			= FlowDirectionTypes.FLOWDIRECTION_NORTH,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST] 
			= FlowDirectionTypes.FLOWDIRECTION_NORTHEAST,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTH] 
			= FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST] 
			= FlowDirectionTypes.FLOWDIRECTION_SOUTH, 
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHWEST] 
			= FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST,
	};
end




function SetPlotTypes(plotTypes)
	print("Setting Plot Types (MapGenerator.Lua)");
	



	for i, plot in Plots() do
		plot:SetPlotType(plotTypes[i + 1], false, false);
	end
end

function GenerateCoasts(args, defaultExpansionDice)
	print("Setting coasts and oceans (MapGenerator.Lua)");
	local args = args or {};
	local bExpandCoasts = args.bExpandCoasts or true;
	local expansion_diceroll_table = args.expansion_diceroll_table or defaultExpansionDice or {2, 4};
	
	local shallowWater = GameDefines.SHALLOW_WATER_TERRAIN;
	local deepWater = GameDefines.DEEP_WATER_TERRAIN;

	for i, plot in Plots() do
		if(plot:IsWater()) then
			if(plot:IsAdjacentToLand()) then
				plot:SetTerrainType(shallowWater, false, false);
			else
				plot:SetTerrainType(deepWater, false, false);
			end
		end
	end
	
	if bExpandCoasts == false then
		return
	end

	print("Expanding coasts (MapGenerator.Lua)");
	for loop, iExpansionDiceroll in ipairs(expansion_diceroll_table) do
		local shallowWaterPlots = {};
		for i, plot in Plots() do
			if(plot:GetTerrainType() == deepWater) then


				if(plot:IsAdjacentToShallowWater() and Map.Rand(iExpansionDiceroll, "add shallows") == 0) then
					table.insert(shallowWaterPlots, plot);
				end
			end
		end
		for i, plot in ipairs(shallowWaterPlots) do
			plot:SetTerrainType(shallowWater, false, false);
		end
	end
end

function SetTerrainTypes(terrainTypes)
	print("Setting Terrain Types (MapGenerator.Lua)");
	for i, plot in Plots() do
		if not plot:IsWater() then
			plot:SetTerrainType(terrainTypes[i], false, false);
		end		
	end
end

function GeneratePlotTypes()

	print("Generating Plot Types (MapGenerator.Lua)");
	

	local plotLand = PlotTypes.PLOT_LAND;
	local plotTypes = {};
	for i = 0, Map.GetNumPlots() - 1, 1 do
		plotTypes[i] = plotLand;
	end 
	
	SetPlotTypes(plotTypes);
	
	GenerateCoasts();
end

function GenerateTerrain()

	print("Generating Terrain Types (MapGenerator.Lua)");
	

	local terrainGrass = GameInfo.Terrains.TERRAIN_GRASS.ID;
	local terrainTypes = {};
	for i = 0, Map.GetNumPlots() - 1, 1 do
		terrainTypes[i] = terrainGrass;
	end 
	
	SetTerrainTypes(terrainTypes);	
end

function GetOppositeFlowDirection(dir)

	local numTypes = FlowDirectionTypes.NUM_FLOWDIRECTION_TYPES;
	return ((dir + 3) % numTypes);
end

function GetRiverValueAtPlot(plot)

	local numPlots = PlotTypes.NUM_PLOT_TYPES;
	local sum = (numPlots - plot:GetPlotType()) * 20;

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


nextRiverID = 0;
_rivers = {};
function DoRiver(startPlot, thisFlowDirection, originalFlowDirection, riverID)

	thisFlowDirection = thisFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;
	originalFlowDirection = originalFlowDirection or FlowDirectionTypes.NO_FLOWDIRECTION;


	if (riverID == nil) then
		riverID = nextRiverID;
		nextRiverID = nextRiverID + 1;
	end

	local otherRiverID = _rivers[startPlot]
	if (otherRiverID ~= nil and otherRiverID ~= riverID and originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then
		return;
	end

	local riverPlot;
	
	local bestFlowDirection = FlowDirectionTypes.NO_FLOWDIRECTION;
	if (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH) then
	
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if ( adjacentPlot == nil or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() ) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_NORTHEAST);
		
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST) then
	
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if ( adjacentPlot == nil or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater() ) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);

	
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST) then
	
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if (riverPlot == nil) then
			return;
		end
		
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if (adjacentPlot == nil or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);

	
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH) then
	
		riverPlot = Map.PlotDirection(startPlot:GetX(), startPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		if (riverPlot == nil) then
			return;
		end
		
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_EAST);
		if (adjacentPlot == nil or riverPlot:IsWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end
		
		_rivers[riverPlot] = riverID;
		riverPlot:SetWOfRiver(true, thisFlowDirection);

	
	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST) then

		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHEAST);
		if (adjacentPlot == nil or riverPlot:IsNWOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end
		
		_rivers[riverPlot] = riverID;
		riverPlot:SetNWOfRiver(true, thisFlowDirection);


	elseif (thisFlowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST) then
		
		riverPlot = startPlot;
		local adjacentPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_SOUTHWEST);
		
		if ( adjacentPlot == nil or riverPlot:IsNEOfRiver() or riverPlot:IsWater() or adjacentPlot:IsWater()) then
			return;
		end

		_rivers[riverPlot] = riverID;
		riverPlot:SetNEOfRiver(true, thisFlowDirection);
		riverPlot = Map.PlotDirection(riverPlot:GetX(), riverPlot:GetY(), DirectionTypes.DIRECTION_WEST);

	else
	


		riverPlot = startPlot;		
	end

	if (riverPlot == nil or riverPlot:IsWater()) then

		return; 
	end


	local riverPlotX = riverPlot:GetX();
	local riverPlotY = riverPlot:GetY();
	

	local adjacentPlotFunctions = {
		[FlowDirectionTypes.FLOWDIRECTION_NORTH] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST); 
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHEAST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHEAST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHEAST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_EAST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTH] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_SOUTHWEST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_WEST);
		end,
		
		[FlowDirectionTypes.FLOWDIRECTION_NORTHWEST] = function() 
			return Map.PlotDirection(riverPlotX, riverPlotY, DirectionTypes.DIRECTION_NORTHWEST);
		end	
	}
	
	if(bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then


		local bestValue = math.huge;
		for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
			
			if (GetOppositeFlowDirection(flowDirection) ~= originalFlowDirection) then
				
				if (thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or 
					flowDirection == TurnLeftFlowDirections[thisFlowDirection]) then
				
					local adjacentPlot = getAdjacentPlot();
					
					if (adjacentPlot ~= nil) then
					
						local value = GetRiverValueAtPlot(adjacentPlot);
						if (flowDirection == originalFlowDirection) then
							value = (value * 3) / 4;
						end
						
						if (value < bestValue) then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end
					end
				end
			end
		end
		

		if(bestFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then
		
			local bestValue = math.huge;
			for flowDirection, getAdjacentPlot in pairs(adjacentPlotFunctions) do
			
				if (thisFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION or
					flowDirection == TurnRightFlowDirections[thisFlowDirection] or 
					flowDirection == TurnLeftFlowDirections[thisFlowDirection]) then
				
					local adjacentPlot = getAdjacentPlot();
					
					if (adjacentPlot ~= nil) then
						
						local value = GetRiverValueAtPlot(adjacentPlot);
						if (value < bestValue) then
							bestValue = value;
							bestFlowDirection = flowDirection;
						end
					end	
				end
			end
		end
		
	end
	

	if (bestFlowDirection ~= FlowDirectionTypes.NO_FLOWDIRECTION) then
		if  (originalFlowDirection == FlowDirectionTypes.NO_FLOWDIRECTION) then
			originalFlowDirection = bestFlowDirection;
		end
		
		DoRiver(riverPlot, bestFlowDirection, originalFlowDirection, riverID, riverLength);
	end
	
end


function AddRivers()
	local riverSourceRangeDefault = 1.5;
	local seaWaterRangeDefault = 0.75;
	local plotsPerRiverEdge =  9;
	local riverRnd = 210;
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


function AddLakes()

	print("Map Generation - Adding Lakes");
	
	local numLakesAdded = 0;
	local lakePlotRand = GameDefines.LAKE_PLOT_RAND;
	for i, plot in Plots() do
		if not plot:IsWater() then
			if not plot:IsCoastalLand() then
				if not plot:IsRiver() then
					local r = Map.Rand(lakePlotRand, "MapGenerator AddLakes");
					if r == 0 then
						plot:SetArea(-1);
						plot:SetPlotType(PlotTypes.PLOT_OCEAN);
						numLakesAdded = numLakesAdded + 1;
					end
				end
			end
		end
	end
	

	if numLakesAdded > 0 then
		print(tostring(numLakesAdded).." lakes added")
		Map.CalculateAreas();
	end
end


function AddFeatures()

	print("Map Generation - Adding Features");
	
	for i, plot in Plots() do
		for feature in GameInfo.Features() do
			if(plot:CanHaveFeature(feature.ID)) then
				local r = Map.Rand(10000, "MapGenerator AddFeatures");
				if(r < feature.AppearanceProbability) then
					plot:SetFeatureType(feature.ID);
				end 
			end
		end
	end
end

function CanPlaceGoodyAt(improvement, plot)

	local improvementID = improvement.ID;
	local NO_TEAM = -1;
	local NO_RESOURCE = -1;
	local NO_IMPROVEMENT = -1;
	
	if (not plot:CanHaveImprovement(improvementID, NO_TEAM)) then
		return false;
	end
	
	if (plot:GetImprovementType() ~= NO_IMPROVEMENT) then
		return false;
	end

	if (plot:GetResourceType() ~= NO_RESOURCE) then
		return false;
	end

	if (plot:IsImpassable()) then
		return false;
	end

	if (plot:IsMountain()) then
		return false;
	end
	

	local areaID = plot:GetArea();
	local area = Map.GetArea(areaID);	
	local numTiles = area:GetNumTiles();
	if (numTiles < 3) then
		return false;
	end


	local uniqueRange = improvement.GoodyRange;
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	for dx = -uniqueRange, uniqueRange - 1, 1 do
		for dy = -uniqueRange, uniqueRange - 1, 1 do
			local otherPlot = Map.PlotXYWithRangeCheck(plotX, plotY, dx, dy, uniqueRange);
			if(otherPlot and otherPlot:GetImprovementType() == improvementID) then
				return false;
			end
		end
	end 


	for dx = -3, 3 do
		for dy = -3, 3 do
			local otherPlot = Map.PlotXYWithRangeCheck(plotX, plotY, dx, dy, 4);
			if(otherPlot) then
				if otherPlot:IsStartingPlot() then
					for player_num = 0, GameDefines.MAX_CIV_PLAYERS - 1 do
						local player = Players[player_num];
						if player:IsEverAlive() then

							local playerStartPlot = player:GetStartingPlot();
							if otherPlot == playerStartPlot then
								return false;
							end
						end
					end
				end
			end
		end
	end

	return true;
end

function AddGoodies()

	print("-------------------------------");
	print("Map Generation - Adding Goodies");
	

	local startEra = Game.GetStartEra();
	if(GameInfo.Eras[startEra].NoGoodies) then
		print("** The Era specified NO GOODY HUTS");
		return;
	end

	if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_GOODY_HUTS)) then
		print("** The game specified NO GOODY HUTS");
		return false;
	end


	for improvement in GameInfo.Improvements() do
		local tilesPerGoody = improvement.TilesPerGoody;
		
		if(improvement.Goody and tilesPerGoody > 0) then
		
			local improvementID = improvement.ID;
			for index, plot in Plots(Shuffle) do
				if ( not plot:IsWater() ) then
					

					local area = plot:Area();
					local improvementCount = area:GetNumImprovements(improvementID);
					local scaler = (area:GetNumTiles() + (tilesPerGoody/2))/tilesPerGoody;	
					if (improvementCount < scaler) then
						
						if (CanPlaceGoodyAt(improvement, plot)) then
							plot:SetImprovementType(improvementID);
						end
					end
				end
			end
		end
	end
	print("-------------------------------");
end

function DetermineContinents()
	print("Determining continents for art purposes (MapGenerator.Lua)");



	

	






	










	


	Map.DefaultContinentStamper();
end

function StartPlotSystem()













	print("Creating start plot database.");
	local start_plot_database = AssignStartingPlots.Create()
	
	print("Dividing the map in to Regions.");
	start_plot_database:GenerateRegions();
	
	print("Choosing start locations for civilizations.");
	start_plot_database:ChooseLocations()
	
	print("Normalizing start locations and assigning them to Players.");
	start_plot_database:BalanceAndAssign()
	
	print("Placing Natural Wonders.");
	start_plot_database:PlaceNaturalWonders()

	print("Placing Resources and City States.");
	start_plot_database:PlaceResourcesAndCityStates()
end

function GenerateMap()
	print("Generating Map");




	

	GeneratePlotTypes();
	

	GenerateTerrain();
	

	Map.RecalculateAreas();
	

	AddRivers();
	

	AddLakes();
	

	AddFeatures();


	Map.RecalculateAreas();









	StartPlotSystem();


	AddGoodies();


	DetermineContinents();
end
