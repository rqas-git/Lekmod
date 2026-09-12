
include("Lekmod_utilities.lua")

local this_civ = GameInfoTypes["CIVILIZATION_POLYNESIA"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)




function lekmod_polynesia_ua_ocean_impassable_fix(player_id)

	local player = Players[player_id]
	if not player:IsAlive() or player:GetCivilizationType() ~= this_civ then return end

   for unit in player:Units() do
      if unit:IsHasPromotion(GameInfoTypes["PROMOTION_OCEAN_IMPASSABLE"]) then
         unit:SetHasPromotion(GameInfoTypes["PROMOTION_OCEAN_IMPASSABLE"], false)
      end
   end

end

if is_active then

   GameEvents.UnitCreated.Add(lekmod_polynesia_ua_ocean_impassable_fix)
end
