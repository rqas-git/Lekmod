include("Lekmod_utilities.lua")
include("PlotIterators.lua")

local this_civ = GameInfoTypes["CIVILIZATION_VENEZ"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)
local compassTech = GameInfoTypes["TECH_COMPASS"]
local saved = Modding.OpenSaveData()



function lekmod_venice_route_compass(team_id, tech_id)

	if tech_id ~= compassTech then return end
	for player_id = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		local player = Players[player_id]
		if player and player:IsAlive() and player:GetTeam() == team_id and player:GetCivilizationType() == this_civ then
			local key = "lekmod_venice_compass_" .. player_id
			if saved.GetValue(key) ~= 1 then
				player:ChangeNumMiscTradeRoutes(1)
				saved.SetValue(key, 1)
			end
		end
	end
end




if LekmodUtilities:is_civilization_active(this_civ) then
	GameEvents.TeamTechResearched.Add(lekmod_venice_route_compass)
end
