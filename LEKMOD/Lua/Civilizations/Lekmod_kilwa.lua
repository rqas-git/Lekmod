
include("Lekmod_utilities.lua")

local this_civ = GameInfoTypes["CIVILIZATION_KILWA"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)
local trait_building = GameInfoTypes["BUILDING_KILWA_TRAIT"]




function lekmod_kilwa_ua_food(player_id)

	local player = Players[player_id]
	if player:GetCivilizationType() ~= this_civ or not player:IsAlive() then return end
    for city in player:Cities() do
        local number_of_routes
        if player.GetNumInternationalTradeRoutesFromCity then
            number_of_routes = player:GetNumInternationalTradeRoutesFromCity(city)
        else
            number_of_routes = LekmodUtilities:get_number_trade_routes_from_city(player, city, true, false)
        end
        if city then
            city:SetNumRealBuilding(trait_building, number_of_routes)
        end
    end

end

if is_active then
   GameEvents.PlayerDoTurn.Add(lekmod_kilwa_ua_food)
   GameEvents.UnitPrekill.Add(lekmod_kilwa_ua_food)
end
