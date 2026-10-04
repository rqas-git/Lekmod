assert(_VERSION == 'Lua 5.1')
local file = assert(io.open(arg[1]))
local source = file:read('*a'); file:close()
local blob = assert(source:match('(function get_blobs%(map%).-)\n\n\n\n'))
local dfs = assert(source:match('(function PlotDFS%(i, plot_list, comp_list, comp_val%).-)\n%-%- function used'))
local env = setmetatable({}, {__index=_G})
env.tablelength = function(t) local n=0; for _ in pairs(t) do n=n+1 end; return n end
env.table = setmetatable({fill=function(value,n) local t={}; for i=1,n do t[i]=value end; return t end}, {__index=table})
env.xy_to_i = function(x,y,w) return y*w+x+1 end
setfenv(assert(loadstring(dfs)),env)()
setfenv(assert(loadstring(blob)),env)()
for _, size in ipairs({{24,18},{56,50},{80,60}}) do
    env.iW,env.iH = size[1],size[2]
    for wrap=0,1 do
        env.adj_is_cache={}
        for i=1,env.iW*env.iH do
            local x,y=(i-1)%env.iW,math.floor((i-1)/env.iW)
            local neighbors={}
            for _,offset in ipairs({{1,0},{0,1},{-1,0},{0,-1},{1,-1},{-1,1}}) do
                local nx,ny=x+offset[1],y+offset[2]
                if wrap==1 then nx=nx%env.iW end
                if nx>=0 and nx<env.iW and ny>=0 and ny<env.iH then neighbors[#neighbors+1]=env.xy_to_i(nx,ny,env.iW) end
            end
            env.adj_is_cache[i]=neighbors
        end
        for seed=1,12 do
            local map={}
            for i=1,env.iW*env.iH do map[i]=(i*seed+math.floor(i/env.iW)*7)%13 end
            local start=os.clock()
            local graph,blobs=env.get_blobs(map)
            if arg[2]=='benchmark' then io.stderr:write(size[1]..'x'..size[2]..':'..wrap..':'..seed..':'..(os.clock()-start)..'\n') end
            local out={}
            for i=1,#graph do
                out[#out+1]=graph[i]
                if graph[i]>0 then assert(blobs[graph[i]][i]==true) end
            end
            print(size[1]..'x'..size[2]..':'..wrap..':'..seed..':'..table.concat(out,','))
        end
    end
end
