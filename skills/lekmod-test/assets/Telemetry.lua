-- Injected only into the disposable LEKMOD InGame context.
local validationTarget = __TURN_TARGET__;
local ledger = Modding.OpenUserData("LekmodBackgroundValidation", 1);
local lastTurn = -1;
local function record(key, value)
    ledger.SetValue(key, tostring(value));
    print("LEKMOD_VALIDATE " .. key .. "=" .. tostring(value));
end
local function snapshot()
    if not Game.IsFinalInitialized() then return; end
    local turn = Game.GetGameTurn();
    if turn == lastTurn then return; end
    lastTurn = turn;
    record("year", Game.GetGameTurnYear());
    for i = 0, GameDefines.MAX_MAJOR_CIVS-1 do
        local p = Players[i];
        if p and p:IsAlive() then
            local prefix = "turn." .. turn .. ".player." .. i .. ".";
            local civ = GameInfo.Civilizations[p:GetCivilizationType()];
            record(prefix .. "civilization", civ and civ.Type or "unknown");
            record(prefix .. "alive", p:IsAlive());
            record(prefix .. "cities", p:GetNumCities());
            record(prefix .. "units", p:GetNumUnits());
            record(prefix .. "population", p:GetTotalPopulation());
            record(prefix .. "score", p:GetScore());
            record(prefix .. "gold", p:GetGold());
            record(prefix .. "science", p:GetScience());
            record(prefix .. "policies", p:GetNumPolicies());
            if p:GetTeam() >= 0 then
                record(prefix .. "technologies", Teams[p:GetTeam()]:GetTeamTechs():GetNumTechsKnown());
            end
        end
    end
    record("turn", turn); -- Publish the turn after its metrics.
    if turn == validationTarget then
        local saved, saveError = pcall(UI.QuickSave);
        record("quicksave", saved and "requested" or tostring(saveError));
        if saved then record("result", "completed-" .. validationTarget .. "-turns");
        else record("error", "quicksave failed: " .. tostring(saveError)); end
    elseif turn > validationTarget then
        record("error", "autoplay exceeded target turn");
    end
end
local function checkedSnapshot()
    local ok, failure = pcall(snapshot);
    if not ok then record("error", failure); end
end
Events.SerialEventGameDataDirty.Add(checkedSnapshot);
GameEvents.PlayerDoTurn.Add(function() checkedSnapshot(); end);
record("initialized", "in-game telemetry loaded");
record("target", validationTarget);
Events.SequenceGameInitComplete.Add(function()
    local ok, failure = pcall(function()
        Events.LoadScreenClose();
        Game.SetPausePlayer(-1);
        local width, height = Map.GetGridSize();
        record("map_script", PreGame.GetMapScript());
        record("map_width", width);
        record("map_height", height);
        record("game_speed", GameInfo.GameSpeeds[Game.GetGameSpeedType()].Type);
        if Game.GetGameTurn() ~= 0 then error("fresh game did not start at turn zero"); end
        checkedSnapshot();
        local activePlayer = Game.GetActivePlayer();
        if not Players[activePlayer] or not Players[activePlayer]:IsHuman() then
            error("native quick-play did not provide the expected human slot");
        end
        if Game.GetAIAutoPlay() ~= 0 then error("unexpected autoplay already running"); end
        local observer;
        for i = 0, GameDefines.MAX_MAJOR_CIVS-1 do
            if i ~= activePlayer and Players[i] and not Players[i]:IsAlive() then
                observer = i;
                PreGame.SetSlotStatus(i, SlotStatus.SS_OBSERVER);
                PreGame.SetSlotClaim(i, SlotClaim.SLOTCLAIM_UNASSIGNED);
                record("observer_slot", i);
                break;
            end
        end
        if observer == nil then error("no safe observer slot; autoplay refused"); end
        Game.SetAIAutoPlay(validationTarget, activePlayer);
        record("autoplay", Game.GetAIAutoPlay());
        record("loadscreen", "closed and unpaused via game event");
    end);
    if not ok then record("error", failure); end
end);
