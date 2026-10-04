local file = assert(io.open(arg[1]))
local source = file:read('*a'); file:close()
local start = assert(source:find(' for city in p:Cities() do', source:find('local function mpTick()', 1, true), true))
local finish = assert(source:find(' local block=p:GetEndTurnBlockingType()', start, true))
local produce = assert(loadstring(source:sub(start, finish - 1)))
local function iterator(items)
 local i=0
 return function() i=i+1;return items[i] end
end
local units = {
 UNIT_SETTLER={ID=0,Type='UNIT_SETTLER',Found=true,WorkRate=0,Combat=0},
 UNIT_WORKER={ID=1,Type='UNIT_WORKER',Found=false,WorkRate=100,Combat=0},
 UNIT_ARCHER={ID=2,Type='UNIT_ARCHER',Found=false,WorkRate=0,Combat=5},
 UNIT_WARRIOR={ID=3,Type='UNIT_WARRIOR',Found=false,WorkRate=0,Combat=8}
}
for _,name in ipairs({"UNIT_SETTLER","UNIT_WORKER","UNIT_ARCHER","UNIT_WARRIOR"}) do local unit=units[name];units[unit.ID]=unit end
setmetatable(units,{__call=function() return iterator({units.UNIT_WARRIOR}) end})
GameInfo={Units=units,Buildings={BUILDING_MONUMENT={ID=5,Type='BUILDING_MONUMENT'}}}
OrderTypes={ORDER_TRAIN=0,ORDER_CONSTRUCT=1}
mpMark=function() end
local cases={
 {cities=1,population=2,units={3},expected=0},
 {cities=2,population=2,units={3},expected=1},
 {cities=2,population=2,units={1},expected=2},
 {cities=1,population=1,units={3},expected=5},
 {cities=1,population=2,units={3},queued=1}
}
for _,case in ipairs(cases) do
 local requests={}
 mpCityOrders={}
 local city={GetID=function() return 1 end,GetOrderQueueLength=function() return case.queued or 0 end,
  GetPopulation=function() return case.population end,
  CanTrain=function() return true end,CanConstruct=function() return true end}
 p={Cities=function() return iterator({city}) end,GetNumCities=function() return case.cities end,
  Units=function()
   local list={}
   for _,id in ipairs(case.units) do list[#list+1]={GetUnitType=function() return id end} end
   return iterator(list)
  end}
 Game={CityPushOrder=function(_,order,id) requests[#requests+1]={order,id} end}
 produce()
 produce()
 assert(#requests==(case.expected and 1 or 0),'production overwritten before acknowledgement')
 if case.expected then
  assert(requests[1][2]==case.expected,'wrong production priority')
  case.queued=1;produce();case.queued=0;produce()
  assert(#requests==2,'next completed queue did not allow new production')
 end
end
print('5 asynchronous production scenarios passed')
