------------------------------------------------------------------------------------------------------------------------
-- Resettlements. Add a few buildings to newly founded cities if the player has the policy
------------------------------------------------------------------------------------------------------------------------
function lekmod_resettlements_policy_new_buildings(player_id, x, y)

    local player = Players[player_id]
    if not player:HasPolicy(GameInfo.Policies["POLICY_RESETTLEMENT"].ID) then return end

    for loopCity in player:Cities() do
        if (loopCity:GetX() == x and loopCity:GetY() == y) then
            --Note: SetNumRealBuildingClass is a lekmod method. It is not available in the base game
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_WORKSHOP"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_GRANARY"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_AQUEDUCT"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_MONUMENT"], 1)
            loopCity:SetNumRealBuildingClass(GameInfoTypes["BUILDINGCLASS_LIBRARY"], 1)
        end
    end

end
GameEvents.PlayerCityFounded.Add(lekmod_resettlements_policy_new_buildings)
------------------------------------------------------------------------------------------------------------------------
-- Policy_FreePromotionUnitCombats. Give a free promotion to units for specific combat classes as put in the xml table
------------------------------------------------------------------------------------------------------------------------
local policy_promotion_rules

function lekmod_policy_free_promotion_unit_combats(player_id, unit_id)

    local player = Players[player_id]
    local unit = player:GetUnitByID(unit_id)
    if not player:IsAlive() or not unit then return end

    -- The rules database is fixed for this Lua context. Resolve IDs on first use,
    -- retaining database order and checking each unit's current state below.
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

    -- Apply to all the right currently existing units once upon adopting the policy
    for unit in player:Units() do
        if unit then
            lekmod_policy_free_promotion_unit_combats(player_id, unit:GetID())
        end
    end

end
GameEvents.PlayerAdoptPolicy.Add(lekmod_policy_free_promotion_unit_combats_on_adopt)
GameEvents.UnitCreated.Add(lekmod_policy_free_promotion_unit_combats)
------------------------------------------------------------------------------------------------------------------------
local function AwardConsulatesVotesForEra(player, eraID)
    if eraID >= GameInfoTypes["ERA_INDUSTRIAL"] then   player:ChangeNumPolicyLeagueVotes(1) end
    if eraID >= GameInfoTypes["ERA_MODERN"] then       player:ChangeNumPolicyLeagueVotes(1) end
    if eraID >= GameInfoTypes["ERA_POSTMODERN"] then   player:ChangeNumPolicyLeagueVotes(1) end
    if eraID >= GameInfoTypes["ERA_FUTURE"] then       player:ChangeNumPolicyLeagueVotes(1) end
end
-- 1) On policy adoption: backfill votes for all eras you've already passed if applicable
function Lekmod_OnAdoptConsulates(playerID, policyID)
    if policyID ~= GameInfoTypes["POLICY_CONSULATES"] then return end
    local player = Players[playerID]
    if not player:IsAlive() then return end

    local currentEra = player:GetCurrentEra()
    AwardConsulatesVotesForEra(player, currentEra)
end
-- 2) On era change: if you already have Consulates, give +1 vote for that new era
function Lekmod_OnEraChangeGiveConsulatesVote(teamID, newEraID)
    -- Only give the incremental vote for the *new* era
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
