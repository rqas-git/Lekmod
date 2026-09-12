------------------------------------------------------------------------------
--	FILE:	 MapGenerator.lua
--	AUTHORS: Shaun Seckman
--	         Bob Thomas
--	PURPOSE: Base logic for map generation.
------------------------------------------------------------------------------
--	Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

-- Shared generation steps; rectangular maps retain their coast and lake rules.
include("HBMapGenerator");
local generateCoasts = GenerateCoasts;
function GenerateCoasts(args)
    return generateCoasts(args, {2, 2});
end

function AddLakes()

	print("Map Generation - Adding Lakes");
	largeLakes = 3;

	local numLakesAdded = 0;
	local numLargeLakesAdded = 0;
	local coolDown = 0;
	local lakePlotRand = 25;
	local coolDownIni = 96;

	local lakes_level = Map.GetCustomOption(18)

	if lakes_level == 1 then
		largeLakes = 0;
		lakePlotRand = 40;
	elseif lakes_level	== 3 then
		largeLakes = 6;
		lakePlotRand = 10;
		coolDownIni = 0;
	end

	local iW, iH = Map.GetGridSize();
	print("******************************************************************************************************");
	for i = 0, (iW * iH) - 1, 1 do
		plot = Map.GetPlotByIndex(i);
		if(plot) then
			if (plot:IsWater() == false) then
				if (plot:IsCoastalLand() == false) then
					if (plot:IsRiver() == false) then
						coolDown = coolDown - 1;
						--if (AdjacentToNaturalWonder(plot) == false) then
							local r = Map.Rand(lakePlotRand, "MapGenerator AddLakes");
							if r == 0 then
								numLakesAdded = numLakesAdded + 1;
								print("NumlargeLakes: " .. tostring(numLargeLakesAdded));
								print("NumlargeLakes Allowed: " .. tostring(largeLakes));
								if(largeLakes > numLargeLakesAdded) and (coolDown < 1) then
									local bLakes = AddMoreLake(plot);
									if(bLakes == true) then
										numLargeLakesAdded = numLargeLakesAdded + 1;
										coolDown = 96;
									end
								end

								plot:SetPlotType(PlotTypes.PLOT_OCEAN);
							end
						--end
					end
				end
			end
		end
	end
	print("******************************************************************************************************");

	-- this is a minimalist update because lakes have been added
	if numLakesAdded > 0 then
		print(tostring(numLakesAdded).." lakes added")
		Map.CalculateAreas();
	end
end

----------------------------------------------------------------------------------------------------------------------
function AddMoreLake(plot)
	local largeLake = 0;
	lakePlots = {};

	for direction = 0, DirectionTypes.NUM_DIRECTION_TYPES - 1, 1 do
		local adjacentPlot = Map.PlotDirection(plot:GetX(), plot:GetY(), direction);
		if (adjacentPlot) then
			if (adjacentPlot:IsWater() == false)  then
				if (adjacentPlot:IsCoastalLand() == false) then
					if (adjacentPlot:IsRiver() == false) then
						--if (AdjacentToNaturalWonder(adjacentPlot) == false) then
							local r = Map.Rand(7 + largeLake, "MapGenerator AddLakes");
							if r < 3 then
								table.insert(lakePlots, adjacentPlot);
								largeLake = largeLake + 1;
							end
						--end
					end
				end
			end
		end
	end

	for iLake, lakePlot in ipairs(lakePlots) do
		lakePlot:SetPlotType(PlotTypes.PLOT_OCEAN);
	end

	if (largeLake > 2) then
		return true;
	else 
		return false;
	end
end

----------------------------------------------------------------------------------------------------------------------
