
local function event()
    local listeners = {}
    return {
        Add = function(f) listeners[f] = true end,
        Remove = function(f) listeners[f] = nil end,
        Fire = function(...)
            for f in pairs(listeners) do f(...) end
        end
    }
end

local function events()
    return setmetatable({}, {__index = function(t, k)
        local e = event(); rawset(t, k, e); return e
    end})
end

local function iterator(rows)
    local i = 0
    return function() i = i + 1; return rows[i] end
end

local function environment()
    local e = setmetatable({}, {__index = _G})
    e.print = function() end
    e.GameEvents, e.Events = events(), events()
    e.GameDefines = {MAX_MAJOR_CIVS = 4}
    e.GameInfoTypes = {BUILDING_A=1, BUILDING_B=2, POLICY_A=3,
        POLICY_B=4, CIV_A=5, CIVILIZATION_UAE=5, TECH_A=6,
        BRANCH_A=7, PROMO_A=8, PROMO_B=9, COMBAT_A=10, COMBAT_B=11,
        POLICY_CONSULATES=12, ERA_INDUSTRIAL=4, ERA_MODERN=5,
        ERA_POSTMODERN=6, ERA_FUTURE=7}
    e.Players, e.Teams, e.GameInfo = {}, {}, {}
    for i=0,3 do
        local p = {id=i, alive=false, policy=false, cities={}, units={}, gold=0}
        function p:GetID() return self.id end
        function p:GetTeam() return self.id end
        function p:IsAlive() return self.alive end
        function p:IsEverAlive() return self.alive end
        function p:IsBarbarian() return false end
        function p:IsMinorCiv() return false end
        function p:GetCivilizationType() return 5 end
        function p:IsGoldenAge() return self.golden end
        function p:HasPolicy(_) return self.policy end
        function p:IsPolicyBranchFinished(_) return self.branch end
        function p:HasPolicyBranch(_) return self.branch end
        function p:Cities() return iterator(self.cities) end
        function p:Units() return iterator(self.units) end
        function p:GetUnitByID(id) return self.units[id] end
        function p:ChangeGold(n) self.gold=self.gold+n end
        e.Players[i] = p
        e.Teams[i] = {IsHasTech = function() return p.tech end}
    end
    return e
end

local function load(e, source)
    local f = assert(loadstring(source))
    setfenv(f, e); f()
end

function run_dummy(source, count, capital, flags, duplicate)
    local e = environment()
    local p = e.Players[0]
    p.alive=true; p.policy=flags%2==1; p.tech=math.floor(flags/2)%2==1
    p.golden=math.floor(flags/4)%2==1; p.branch=math.floor(flags/8)%2==1
    local writes=0
    for i=1,count do
        local c={id=i, buildings={}}
        function c:IsCapital() return self.id==capital end
        function c:SetNumRealBuilding(b,n) self.buildings[b]=n; writes=writes+1 end
        p.cities[i]=c
    end
    local rows={
        {BuildingType='BUILDING_A', IsCapitalOnly=false},
        {BuildingType='BUILDING_B', IsCapitalOnly=true, TechType='TECH_A'},
        {BuildingType='BUILDING_A', PolicyType='POLICY_A', RequiresGoldenAge=true},
        {BuildingType='BUILDING_B', PolicyBranchChosen='BRANCH_A'},
        {BuildingType='BUILDING_B', CivilizationType='CIV_A', PolicyBranchFinished='BRANCH_A'}
    }
    if duplicate then rows[#rows+1]=rows[1] end
    e.GameInfo.Global_Dummy_Buildings=function() return iterator(rows) end
    load(e,source)
    e.GameEvents.PlayerAdoptPolicy.Fire(0,3)
    local state={}
    for i,c in ipairs(p.cities) do
        state[#state+1]=(c.buildings[1] or 0)..':'..(c.buildings[2] or 0)
    end
    return table.concat(state,','), writes
end

function run_uae(source, count, mask, active, native_predicate)
    local e=environment(); local p=e.Players[0]; p.alive=true
    local calls, predicates, popups=0,0,{}
    e.include=function() end
    e.LekmodUtilities={is_civilization_active=function() return true end}
    e.Game={GetActivePlayer=function() return active and 0 or 1 end}
    e.ToHexFromGrid=function(p) return p end
    e.HexToWorld=function(p) return p end
    e.Events.AddPopupTextEvent=function(pos,text) popups[#popups+1]=pos.x..':'..text end
    function p:GetInternationalTradeRoutePlotToolTip(plot)
        calls=calls+1; return plot.route and {'route'} or {}
    end
    if native_predicate then
        function p:HasInternationalTradeRoutePlotToolTip(plot)
            predicates=predicates+1; return plot.route
        end
    end
    for i=1,count do
        local u={id=i, xp=0, combat=(i+mask)%3~=0}
        local plot={route=(i*7+mask)%4<2}
        function plot:GetX() return i end
        function plot:GetY() return 0 end
        function u:GetPlot() return plot end
        function u:IsCombatUnit() return self.combat end
        function u:ChangeExperience(n) self.xp=self.xp+n end
        p.units[i]=u
    end
    load(e,source)
    e.GameEvents.PlayerDoTurn.Fire(0)
    local state={tostring(p.gold)}
    for _,u in ipairs(p.units) do state[#state+1]=tostring(u.xp) end
    return table.concat(state,',')..'|'..table.concat(popups,','),calls,predicates
end

function run_policies(source, count, reordered, alternate_rules)
    local e=environment(); local p=e.Players[0]; p.alive=true
    local reads,mutations=0,{}
    local rows={
        {PolicyType='POLICY_A',PromotionType='PROMO_A',UnitCombatType='COMBAT_A'},
        {PolicyType='POLICY_B',PromotionType='PROMO_B',UnitCombatType='COMBAT_B'},
        {PolicyType='POLICY_A',PromotionType='PROMO_A',UnitCombatType='COMBAT_A'}
    }
    if reordered then rows[1],rows[2]=rows[2],rows[1] end
    if alternate_rules then rows={{PolicyType='POLICY_A',PromotionType='PROMO_B',UnitCombatType='COMBAT_A'}} end
    e.GameInfo.Policy_FreePromotionUnitCombats=function()
        reads=reads+1; return iterator(rows)
    end
    for i=1,count do
        local u={id=i, combat=i%2==0 and 10 or 11, promotions={}}
        function u:GetID() return self.id end
        function u:GetUnitCombatType() return self.combat end
        function u:IsHasPromotion(id) return self.promotions[id] end
        function u:SetHasPromotion(id,v)
            self.promotions[id]=v; mutations[#mutations+1]=self.id..':'..id
        end
        p.units[i]=u
    end
    load(e,source)

    e.GameEvents.UnitCreated.Fire(0,999)
    p.alive=false
    for i=1,count do e.GameEvents.UnitCreated.Fire(0,i) end
    assert(reads==0)
    p.alive=true
    for i=1,count do e.GameEvents.UnitCreated.Fire(0,i) end
    p.policy=true
    e.GameEvents.PlayerAdoptPolicy.Fire(0,3)

    for _,u in ipairs(p.units) do u.combat=10; u.promotions[8]=nil end
    for i=1,count do e.GameEvents.UnitCreated.Fire(0,i) end
    e.GameEvents.PlayerAdoptPolicy.Fire(0,3)
    return table.concat(mutations,','),reads
end
