

include("Lekmod_utilities.lua")

local this_civ = GameInfoTypes["CIVILIZATION_MAORI"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)





function lekmod_maori_ua(player_id)

   local maori_promotion_id = GameInfoTypes["PROMOTION_MAORI"]
   local maori_promotion_civilian_id = GameInfoTypes["PROMOTION_MAORI_CIVILIAN"]

	local player = Players[player_id]
	local game_turn = Game.GetGameTurn()

	if not player:IsAlive() or player:GetCivilizationType() ~= this_civ then return end

	for unit in player:Units() do

		if game_turn < 5 then
		elseif unit:IsHasPromotion(maori_promotion_id) then
		   unit:SetHasPromotion(maori_promotion_id, false)
		elseif unit:IsHasPromotion(maori_promotion_civilian_id) then
			unit:SetHasPromotion(maori_promotion_civilian_id, false)
		end

	end
end

if is_active then
   GameEvents.PlayerDoTurn.Add(lekmod_maori_ua)
end
