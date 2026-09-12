-- Compare coastline/lake output, RNG traces, and feature defaults with the original scripts.
assert(_VERSION == 'Lua 5.1')
local directory = assert(arg[1])
local variants = {'', 'Rectangular'}
local function environment(seed, moisture, lakes)
    local env = setmetatable({}, {__index = _G})
    local w, h, plots, trace, state = 24, 18, {}, {}, seed
    env.print = function() end
    env.GameDefines = {SHALLOW_WATER_TERRAIN = 1, DEEP_WATER_TERRAIN = 2, LAKE_PLOT_RAND = 25}
    env.PlotTypes = {PLOT_OCEAN = 0, PLOT_LAND = 1}
    env.DirectionTypes = {NUM_DIRECTION_TYPES = 6}
    env.TerrainTypes = {TERRAIN_SNOW = 3, TERRAIN_TUNDRA = 4, TERRAIN_PLAINS = 5}
    env.FeatureTypes = {FEATURE_FLOOD_PLAINS = 0, FEATURE_ICE = 1, FEATURE_JUNGLE = 2,
                        FEATURE_FOREST = 3, FEATURE_OASIS = 4, FEATURE_MARSH = 5}
    env.GameInfo = {Worlds = {[0] = {FeatureGrainChange = seed % 3}}}
    local directions = {{1,0},{1,-1},{0,-1},{-1,0},{-1,1},{0,1}}
    local function getPlot(x,y)
        if x >= 0 and x < w and y >= 0 and y < h then return plots[y*w+x] end
    end
    local function adjacent(plot, condition)
        for _, offset in ipairs(directions) do
            local other = getPlot(plot.x+offset[1], plot.y+offset[2])
            if other and condition(other) then return true end
        end
        return false
    end
    for i=0,w*h-1 do
        local x,y = i%w, math.floor(i/w)
        local p = {x=x,y=y,id=i,water=x<3 or x>w-4 or y<2,terrain=5,area=0}
        function p:IsWater() return self.water end
        function p:IsAdjacentToLand() return adjacent(self,function(q) return not q.water end) end
        function p:IsAdjacentToShallowWater() return adjacent(self,function(q) return q.water and q.terrain==1 end) end
        function p:IsCoastalLand() return not self.water and adjacent(self,function(q) return q.water end) end
        function p:IsRiver() return (self.id+seed)%29==0 end
        function p:GetTerrainType() return self.terrain end
        function p:SetTerrainType(value) self.terrain=value end
        function p:SetPlotType(value) self.water=value==0 end
        function p:SetArea(value) self.area=value end
        function p:GetX() return self.x end
        function p:GetY() return self.y end
        plots[i]=p
    end
    env.Map = {
        GetNumPlots=function() return w*h end,
        GetGridSize=function() return w,h end,
        GetWorldSize=function() return 0 end,
        GetFractalFlags=function() return 7 end,
        GetCustomOption=function(index) return index==18 and lakes or moisture end,
        GetPlotByIndex=function(index) return plots[index] end,
        PlotDirection=function(x,y,d) return getPlot(x+directions[d+1][1],y+directions[d+1][2]) end,
        CalculateAreas=function() trace[#trace+1]='calculate-areas' end,
        Rand=function(bound,label)
            state=(state*25173+13849)%65521
            local value=state%bound
            trace[#trace+1]=bound..':'..label..':'..value
            return value
        end,
    }
    env.Fractal = {Create=function(...)
        local parameters={...}
        trace[#trace+1]='fractal:'..table.concat(parameters,',')
        return {GetHeight=function(_,percent) return percent*2+seed end}
    end}
    env.include = function(name)
        if name:match('^HBFeatureGenerator') or name:match('^HBMapGenerator') then
            local chunk=assert(loadfile(directory..'/'..name..'.lua'))
            setfenv(chunk,env)()
        end
    end
    local function snapshot(instance)
        local output={}
        if instance then
            local keys={}
            for key,value in pairs(instance) do
                if type(value)=='number' then keys[#keys+1]=key end
            end
            table.sort(keys)
            for _,key in ipairs(keys) do output[#output+1]=key..'='..instance[key] end
        else
            for i=0,w*h-1 do
                local p=plots[i]
                output[#output+1]=(p.water and 'w' or 'l')..p.terrain..':'..p.area
            end
        end
        output[#output+1]=table.concat(trace,';')
        return table.concat(output,'|')
    end
    return env,snapshot
end

for _,variant in ipairs(variants) do
    for seed=1,12 do
        for level=1,3 do
            for _,custom in ipairs({false,true}) do
                local env,snapshot=environment(seed,level,level)
                env.include('HBMapGenerator'..variant)
                local args=custom and {expansion_diceroll_table={3,5,2},bExpandCoasts=false} or nil
                env.GenerateCoasts(args)
                env.AddLakes()
                print(variant..':map:'..seed..':'..level..':'..tostring(custom)..':'..snapshot())
                local feature,featureSnapshot=environment(seed,level,level)
                feature.include('HBFeatureGenerator'..variant)
                local options={rainfall=level}
                if custom then options.iJunglePercent=0; options.iForestPercent=63; options.fMarshPercent=2 end
                local instance=feature.FeatureGenerator.Create(options)
                assert(options.iJunglePercent==(custom and 0 or nil), 'caller options were mutated')
                print(variant..':feature:'..seed..':'..level..':'..tostring(custom)..':'..featureSnapshot(instance))
            end
        end
    end
end
