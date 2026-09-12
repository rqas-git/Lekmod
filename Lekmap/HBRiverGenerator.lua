

function DoRiver(startPlot, thisFlowDirection, originalFlowDirection, riverID)


	local iW, iH = Map.GetGridSize()
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


					elseif adjacentPlot == nil and riverPlotY == iH - 1 then
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHEAST then

							local value = Map.Rand(5, "River Rand");
							if (flowDirection == originalFlowDirection) then
								value = (value * 3) / 4;
							end
							if (value < bestValue) then
								bestValue = value;
								bestFlowDirection = flowDirection;
							end
						end


					elseif adjacentPlot == nil and riverPlotX == 0 then
						if flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTH or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTH or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_NORTHWEST or
						   flowDirection == FlowDirectionTypes.FLOWDIRECTION_SOUTHWEST then

							local value = Map.Rand(5, "River Rand");
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

		DoRiver(riverPlot, bestFlowDirection, originalFlowDirection, riverID);
	end
end
