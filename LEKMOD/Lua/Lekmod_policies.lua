


function lekmod_resettlements_policy_new_buildings(player_id, x, y)

    local player = Players[player_id]
    if not player:HasPolicy(GameInfo.Policies["POLICY_RESETTLEMENT"].ID) then return end

    for loopCity in player:Cities() do
        if (loopCity:GetX() == x and loopCity:GetY() == y) then

            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_WORKSHOP"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_GRANARY"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_AQUEDUCT"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_MONUMENT"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_LIBRARY"], 1)
        end
    end

end
GameEvents.PlayerCityFounded.Add(lekmod_resettlements_policy_new_buildings)



local policy_promotion_rules

function lekmod_policy_free_promotion_unit_combats(player_id, unit_id)

    local player = Players[player_id]
    local unit = player:GetUnitByID(unit_id)
    if not player:IsAlive() or not unit then return end



    if not policy_promotion_rules then
        policy_promotion_rules = {}
        for row in GameInfo.Policy_FreePromotionUnitCombats() do
            policy_promotion_rules[#policy_promotion_rules + 1] = {
                policy_id = GameInfoTypes[row.PolicyType],
                promotion_id = GameInfoTypes[row.PromotionType],
                combat_class_id = GameInfoTypes[row.UnitCombatType]
            }
        end
    end

    for _, rule in ipairs(policy_promotion_rules) do

        local policy_id = rule.policy_id
        local promotion_id = rule.promotion_id
        local combat_class_id = rule.combat_class_id

        if player:HasPolicy(policy_id)
        and unit:GetUnitCombatType() == combat_class_id
        and (not unit:IsHasPromotion(promotion_id)) then

            unit:SetHasPromotion(promotion_id, true)

        end

    end

end

function lekmod_policy_free_promotion_unit_combats_on_adopt(player_id)

    local player = Players[player_id]


    for unit in player:Units() do
        if unit then
            lekmod_policy_free_promotion_unit_combats(player_id, unit:GetID())
        end
    end

end
GameEvents.PlayerAdoptPolicy.Add(lekmod_policy_free_promotion_unit_combats_on_adopt)
GameEvents.UnitCreated.Add(lekmod_policy_free_promotion_unit_combats)

local function AwardConsulatesVotesForEra(player, eraID)
    if eraID >= GameInfoTypes["ERA_INDUSTRIAL"] then   player:ChangeNumPolicyLeagueVotes(1) end
    if eraID >= GameInfoTypes["ERA_MODERN"] then       player:ChangeNumPolicyLeagueVotes(1) end
    if eraID >= GameInfoTypes["ERA_POSTMODERN"] then   player:ChangeNumPolicyLeagueVotes(1) end
    if eraID >= GameInfoTypes["ERA_FUTURE"] then       player:ChangeNumPolicyLeagueVotes(1) end
end

function Lekmod_OnAdoptConsulates(playerID, policyID)
    if policyID ~= GameInfoTypes["POLICY_CONSULATES"] then return end
    local player = Players[playerID]
    if not player:IsAlive() then return end

    local currentEra = player:GetCurrentEra()
    AwardConsulatesVotesForEra(player, currentEra)
end

function Lekmod_OnEraChangeGiveConsulatesVote(teamID, newEraID)

    if     newEraID == GameInfoTypes["ERA_INDUSTRIAL"]
        or newEraID == GameInfoTypes["ERA_MODERN"]
        or newEraID == GameInfoTypes["ERA_POSTMODERN"]
        or newEraID == GameInfoTypes["ERA_FUTURE"]
    then
        for playerID = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
            local player = Players[playerID]
            if player:IsAlive() and player:GetTeam() == teamID
                and player:HasPolicy(GameInfoTypes["POLICY_CONSULATES"]) then
                player:ChangeNumPolicyLeagueVotes(1)
            end
        end
    end
end

GameEvents.PlayerAdoptPolicy.Add(Lekmod_OnAdoptConsulates)
GameEvents.TeamSetEra.Add(Lekmod_OnEraChangeGiveConsulatesVote)
