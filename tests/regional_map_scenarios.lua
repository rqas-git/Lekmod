
assert(_VERSION == 'Lua 5.1')
local directory = assert(arg[1])
local function serialize(value)
    if type(value) == 'function' then return 'function' end
    if type(value) ~= 'table' then return tostring(value) end
    local keys, parts = {}, {}
    for key in pairs(value) do keys[#keys+1] = key end
    table.sort(keys, function(a,b) return tostring(a) < tostring(b) end)
    for _,key in ipairs(keys) do parts[#parts+1] = tostring(key)..'='..serialize(value[key]) end
    return '{'..table.concat(parts, ',')..'}'
end
local function environment(name, seed, options)
    local env = setmetatable({}, {__index=_G})
    local trace, state, plots, w, h = {}, seed, {}, 10, 8
    local function record(label, ...) trace[#trace+1] = label..serialize({...}) end
    env.print = function() end
    env.AssignStartingPlots, env.MultilayeredFractal, env.FeatureGenerator, env.TerrainGenerator = {}, {}, {}, {}
    env.PlotTypes = {PLOT_OCEAN=0, PLOT_LAND=1, NUM_PLOT_TYPES=4}
    env.FlowDirectionTypes = {NO_FLOWDIRECTION=-1, FLOWDIRECTION_NORTH=0, FLOWDIRECTION_NORTHEAST=1,
        FLOWDIRECTION_SOUTHEAST=2, FLOWDIRECTION_SOUTH=3, FLOWDIRECTION_SOUTHWEST=4, FLOWDIRECTION_NORTHWEST=5}
    env.DirectionTypes = {DIRECTION_NORTHEAST=0, DIRECTION_EAST=1, DIRECTION_SOUTHEAST=2,
        DIRECTION_SOUTHWEST=3, DIRECTION_WEST=4, DIRECTION_NORTHWEST=5, NUM_DIRECTION_TYPES=6}
    local worlds = {}
    for id, size in ipairs({'DUEL','TINY','SMALL','STANDARD','LARGE','HUGE'}) do
        local row = {ID=id-1, Type='WORLDSIZE_'..size}
        worlds[id-1], worlds[row.Type] = row, row
    end
    env.GameInfo = {Worlds=setmetatable(worlds, {__call=function()
        local i=-1
        return function() i=i+1; return worlds[i] end
    end})}
    local function plot(x,y) if x>=0 and x<w and y>=0 and y<h then return plots[y*w+x] end end
    local offsets = {{1,1},{1,0},{1,-1},{0,-1},{-1,0},{0,1}}
    for i=0,w*h-1 do
        local p = {x=i%w, y=math.floor(i/w), id=i}
        function p:GetX() return self.x end
        function p:GetY() return self.y end
        function p:IsWater() return seed%2==0 and (self.x==w-1 or self.y==0) end
        function p:GetPlotType() return self:IsWater() and 0 or 1 end
        for _,side in ipairs({'W','NW','NE'}) do
            p['Is'..side..'OfRiver'] = function(self) return self[side] ~= nil end
            p['Set'..side..'OfRiver'] = function(self, active, flow)
                self[side] = active and flow or nil
                record('river-edge', self.id, side, active, flow)
            end
        end
        plots[i] = p
    end
    env.Map = {
        GetGridSize=function() return w,h end,
        GetWorldSize=function() return seed%6 end,
        GetCustomOption=function(index) record('option',index); return options[index] or 2 end,
        PlotDirection=function(x,y,d) return plot(x+offsets[d+1][1], y+offsets[d+1][2]) end,
        Rand=function(bound,label)
            state=(state*25173+13849)%65521
            record('random',bound,label,state%bound)
            return state%bound
        end,
    }
    local includes = {HBMapGenerator=true, HBMapGeneratorRectangular=true, HBFractalWorld=true,
        HBRiverGenerator=true, HBRegionalStartPlotSystem=true}
    env.include = function(module)


        if module=='HBMapGeneratorMirrored' then module='HBMapGenerator' end
        if includes[module] then
            local chunk=assert(loadfile(directory..'/'..module..'.lua'))
            setfenv(chunk,env)()
        end
    end
    setfenv(assert(loadfile(directory..'/'..name..'.lua')),env)()
    env.nextRiverID, env._rivers = 0, {}
    env.GetOppositeFlowDirection = function(flow) return (flow+3)%6 end
    env.AssignStartingPlots.Create = function()
        record('create-starts')
        local database = {}
        for _,method in ipairs({'GenerateRegions','ChooseLocations','BalanceAndAssign',
                               'PlaceNaturalWonders','PlaceResourcesAndCityStates'}) do
            database[method] = function(_,args) record(method,args) end
        end
        return database
    end
    env.Fractal = {}
    for _,method in ipairs({'Create','CreateRifts'}) do
        env.Fractal[method] = function(...)
            record(method,...)
            return {BuildRidges=function(_,...) record('ridges',...) end}
        end
    end
    return env, plot, function() return table.concat(trace,';') end
end

for _,name in ipairs({'LekmapTeamerMapLegacy','LekmapFourCornersv1.2'}) do
    for seed=1,4 do
        for flow=-1,5 do
            for _,position in ipairs({{5,4},{5,7},{0,4},{9,0}}) do
                local env,plot,snapshot = environment(name,seed,{})
                env.DoRiver(plot(position[1],position[2]),flow)

                env._rivers[plot(5,4)]=99
                env.DoRiver(plot(5,4))
                print(name..':'..seed..':'..flow..':'..serialize(position)..':'..env.nextRiverID..':'..snapshot())
            end
        end
    end
end
for _,name in ipairs({'LekmapOvalLegacy','LekmapDonutLegacy'}) do
    for coast=1,3 do
        for lux=1,2 do
            for _,wonders in ipairs({1,5,14}) do
                local env,_,snapshot = environment(name,3,{[15]=coast,[16]=lux,[7]=wonders,[13]=4,[5]=7,[17]=1})
                env.StartPlotSystem()
                print(name..':starts:'..coast..':'..lux..':'..wonders..':'..snapshot()..':'..
                    serialize({env.OnlyCoastal,env.BalancedCoastal,env.CoastLux,env.MixedBias}))
            end
        end
    end
end
for _,name in ipairs({'LekmapOvalLegacy','LekmapArchipelagoLegacy','LekmapTinyIslandsLegacy','LekmapSmallContinentsLegacy'}) do
    for world=-1,6 do
        for size=1,6 do
            local env,_,snapshot = environment(name,1,{[11]=size,[12]=7-size})
            print(name..':size:'..world..':'..size..':'..serialize(env.GetMapInitData(world))..':'..snapshot())
        end
    end
end
for seed=1,8 do
    for _,custom in ipairs({false,true}) do
        local env,_,snapshot = environment('LekmapPangaeaFractalv6.2',seed,{})
        local instance = {iNumPlotsX=22+seed, iNumPlotsY=18, iFlags=7, fracXExp=6, fracYExp=5}
        env.FractalWorld.InitFractal(instance,custom and {ridge_flags=3,continent_grain=4,polar=false} or nil)
        print('fractal:'..seed..':'..tostring(custom)..':'..snapshot())
    end
end
