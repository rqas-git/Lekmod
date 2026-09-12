function run_kilwa(script, utilities, seed, native)
    local env = setmetatable({}, {__index = _G})
    local writes, tableCalls = {}, 0
    env.GameInfoTypes = {CIVILIZATION_KILWA=7, BUILDING_KILWA_TRAIT=19}
    env.GameDefines = {MAX_MAJOR_CIVS=1}
    env.SlotStatus = {SS_TAKEN=1, SS_COMPUTER=2}
    env.PreGame = {GetSlotStatus=function() return 1 end, GetCivilization=function() return 7 end}
    local handlers = {}
    local function event(name) return {Add=function(fn) handlers[name]=fn end} end
    env.GameEvents = {PlayerDoTurn=event('turn'), UnitPrekill=event('kill')}
    local cities, routes = {}, {}
    for i=1,seed%9 do
        local city = {id=i, owner=i%3}
        function city:GetOwner() return self.owner end
        function city:SetNumRealBuilding(building, count)
            writes[#writes+1]=self.id .. ':' .. building .. ':' .. count
        end
        cities[i]=city
    end
    local player = {}
    function player:GetID() return 0 end
    function player:GetCivilizationType() return 7 end
    function player:IsAlive() return true end
    function player:Cities()
        local i=0
        return function() i=i+1; return cities[i] end
    end
    function player:GetTradeRoutes() tableCalls=tableCalls+1; return routes end
    if native then
        function player:GetNumInternationalTradeRoutesFromCity(city)
            local count=0
            for _,route in ipairs(routes) do
                if route.FromCity==city and route.ToCity and route.ToCity:GetOwner()~=self:GetID() then count=count+1 end
            end
            return count
        end
    end
    env.Players = {[0]=player}
    env.include=function() end
    setfenv(assert(loadstring(utilities)),env)()
    setfenv(assert(loadstring(script)),env)()
    for phase=1,4 do
        routes={}
        if #cities>0 then
            for i=1,seed%23 do
                routes[i]={FromCity=cities[(i*3+phase)%#cities+1],ToCity=cities[(i*7+seed)%#cities+1],Domain=i%2}
            end
        end
        for _,city in ipairs(cities) do city.owner=(city.id+phase)%3 end
        handlers.turn(0)
        handlers.kill(0,123,0,0,0)
    end
    return table.concat(writes,'|'), tableCalls
end

function compare_trade_text(size, before, after)
    local function run(fragment)
        local log={}
        local env=setmetatable({table_concat=table.concat,ourTradeItems={},theirTradeItems={}}, {__index=_G})
        for i=1,size do env.ourTradeItems[i]='our'..i;env.theirTradeItems[i]='their'..i end
        local function control(name) return {SetText=function(_,text) log[#log+1]=name..':'..text end} end
        env.instance={TheirTradeItems=control('their'),OurTradeItems=control('our')}
        env.instanceL={TheirTradeItems=control('theirL'),OurTradeItems=control('ourL')}
        setfenv(assert(loadstring(fragment)),env)()
        return table.concat(log,'|'),table.concat(env.ourTradeItems,'|')
    end
    local a,b=run(before)
    local c,d=run(after)
    return a==c and b==d
end

function compare_unit_filter(before, after)
    local function run(script)
        local env=setmetatable({}, {__index=_G})
        env.GameInfoTypes={UNIT_WORKBOAT=4,PROMOTION_MOVE_ALL_TERRAIN=1,PROMOTION_EMBARKATION=2}
        local noop={Add=function() end}
        env.GameEvents={CityCanTrain=noop,UnitCreated=noop,PlayerDoTurn=noop}
        local human=false
        env.Players={[0]={IsHuman=function() return human end}}
        setfenv(assert(loadstring(script)),env)()
        local results={}
        for mode=0,1 do
            human=mode==1
            for unit=-1,10 do results[#results+1]=tostring(env.lekmod_block_unit_construction(0,0,unit)) end
        end
        return table.concat(results,',')
    end
    return run(before)==run(after)
end
