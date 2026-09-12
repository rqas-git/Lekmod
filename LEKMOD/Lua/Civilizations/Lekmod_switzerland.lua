

include("Lekmod_utilities.lua")
include("PlotIterators.lua")

local this_civ = GameInfoTypes["CIVILIZATION_SWISS"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)




local mountaineer = GameInfoTypes["PROMOTION_SWISS_MOUNTAINEER"];
local mountaineer_active = GameInfoTypes["PROMOTION_SWISS_MOUNTAINEER_ACTIVE"];

function lekmod_switzerland_uu_mountain_bonus(player_id, unit_id)

	local player = Players[player_id]
	local unit = player:GetUnitByID(unit_id)
	if not unit:IsHasPromotion(mountaineer) then return end

   if unit:IsNearTerrainType(GameInfoTypes["TERRAIN_MOUNTAIN"], 1, false) then
      unit:SetHasPromotion(mountaineer_active, true);
   else
      unit:SetHasPromotion(mountaineer_active, false);
   end

end


GameEvents.UnitSetXY.Add(lekmod_switzerland_uu_mountain_bonus)

GameEvents.UnitCreated.Add(lekmod_switzerland_uu_mountain_bonus)
