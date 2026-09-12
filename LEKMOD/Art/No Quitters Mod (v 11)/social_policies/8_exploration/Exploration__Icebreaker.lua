function NQMP_Exploration_PlayerDoTurn(iPlayer)
	local player = Players[iPlayer]
	if (player:IsEverAlive()) then
		if (player:HasPolicy(GameInfo.Policies["POLICY_EXPLORATION"].ID)) then
			local icebreakerID = GameInfo.UnitClasses{Type="UNITCLASS_ICEBREAKER"}().ID
			local featureIceID = FeatureTypes["FEATURE_ICE"]
			for unit in player:Units() do
				if (unit:GetUnitClassType() == icebreakerID) then 
					local plot = unit:GetPlot()
					if (plot:GetFeatureType() == featureIceID) then
						plot:SetFeatureType(FeatureTypes["NO_FEATURE"], -1)


					end
				end
			end
		end
	end
end
