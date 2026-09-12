

include("Lekmod_utilities.lua")
include("PlotIterators.lua")

local this_civ = GameInfoTypes["CIVILIZATION_GEORGIA"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)




function lekmod_georgia_uu_golden_age(player_id)

	local player = Players[player_id]
	if player:GetCivilizationType() ~= this_civ or not player:IsAlive() then return end

    for loop_unit in player:Units() do
        if loop_unit:GetUnitType() == GameInfoTypes["UNIT_GEORGIA_KHEVSUR"] and not loop_unit:IsDead() then
            if player:IsGoldenAge() and not loop_unit:IsHasPromotion(GameInfoTypes["PROMOTION_GEORGIA_KHEVSUR_GA"]) then
                loop_unit:SetHasPromotion(GameInfoTypes["PROMOTION_GEORGIA_KHEVSUR_GA"], true);
            elseif not player:IsGoldenAge() and loop_unit:IsHasPromotion(GameInfoTypes["PROMOTION_GEORGIA_KHEVSUR_GA"])then
                loop_unit:SetHasPromotion(GameInfoTypes["PROMOTION_GEORGIA_KHEVSUR_GA"], false);
            end
        end
    end

end


GameEvents.PlayerDoTurn.Add(lekmod_georgia_uu_golden_age)
GameEvents.GreatPersonExpended.Add(lekmod_georgia_uu_golden_age)
