-- Author: EnormousApplePie

------------------------------------------------------------------------------------------------------------------------
-- Blocked units
------------------------------------------------------------------------------------------------------------------------

local workboat_unit = GameInfoTypes["UNIT_WORKBOAT"]

function lekmod_block_unit_construction(player_id, _, unit_type)

   if unit_type ~= workboat_unit then return true end
   local player = Players[player_id]

   if not player:IsHuman() then return true
   else return false end

end
GameEvents.CityCanTrain.Add(lekmod_block_unit_construction)
------------------------------------------------------------------------------------------------------------------------
-- Unit Embark bug fix
------------------------------------------------------------------------------------------------------------------------
local hover_promotion = GameInfoTypes["PROMOTION_MOVE_ALL_TERRAIN"]
local embark_promotion = GameInfoTypes["PROMOTION_EMBARKATION"]

function lekmod_embark_fix(player_id)

	local player = Players[player_id]
	if not player:IsAlive() then return end
	for unit in player:Units() do

		if unit:IsHasPromotion(hover_promotion) then
			unit:SetHasPromotion(embark_promotion, false)
		end

	end

end
-- Note: UnitCreated is a Lekmod Event! Not available in the base game
GameEvents.UnitCreated.Add(lekmod_embark_fix)
GameEvents.PlayerDoTurn.Add(lekmod_embark_fix)
