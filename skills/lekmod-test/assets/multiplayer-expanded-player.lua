-- Temporary synchronized two-client smoke test. Every action uses game networking.
local mpLedger=nil
local function mpMark(k,v)
 if not mpLedger then mpLedger=Modding.OpenUserData("LekmodCrossplayTurns",1) end
 mpLedger.SetValue(k,tostring(v))
end
local mpOldUpdate=OnUpdate
local mpElapsed=0
local mpReady=false
local mpUnitActions={}
local mpCityOrders={}
local mpTargetSince=nil
local mpSaved=false
local mpQuickRequested=false
local mpCommand=nil

local function mpOrder(unit,mission,x,y)
 local unitID=unit:GetID()
 UI.SelectUnit(unit)
 Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION,mission,x,y,0,false,false)
 mpMark("action."..Game.GetGameTurn().."."..unitID,mission.." | "..x..","..y)
end
local function mpMoveTowards(unit,target)
 local best=nil
 local distance=Map.PlotDistance(unit:GetX(),unit:GetY(),target:GetX(),target:GetY())
 for dir=0,5 do
  local plot=Map.PlotDirection(unit:GetX(),unit:GetY(),dir)
  if plot and not plot:IsWater() and unit:CanMoveOrAttackInto(plot) then
   local d=Map.PlotDistance(plot:GetX(),plot:GetY(),target:GetX(),target:GetY())
   if d<distance and (plot:GetNumUnits()==0 or plot:IsCity() or plot:GetOwner()~=unit:GetOwner()) then best=plot;distance=d end
  end
 end
 if best then mpOrder(unit,MissionTypes.MISSION_MOVE_TO,best:GetX(),best:GetY());return true end
 if not target:IsCity() or Teams[unit:GetTeam()]:IsAtWar(target:GetTeam()) then
  mpOrder(unit,MissionTypes.MISSION_MOVE_TO,target:GetX(),target:GetY());return true
 end
 return false
end
local function mpSpecialOrder(unit,p)
 local row=GameInfo.Units[unit:GetUnitType()]
 if row.Found then
  if unit:CanFound(unit:GetPlot()) then
   local unitID=unit:GetID()
   for actionID,action in pairs(GameInfoActions) do
    if action.Type=="MISSION_FOUND" then
     if Game.CanHandleAction(actionID) then
      UI.SelectUnit(unit)
      Game.HandleAction(actionID)
      mpMark("foundRequest."..Game.GetGameTurn().."."..unitID,true)
     end
     return true
    end
   end
   return false
  end
  local capital=p:GetCapitalCity()
  if capital then
   local target=nil
   for i=0,Map.GetNumPlots()-1 do
    local plot=Map.GetPlotByIndex(i)
    if not plot:IsWater() and unit:CanFound(plot) then
     local d=Map.PlotDistance(capital:GetX(),capital:GetY(),plot:GetX(),plot:GetY())
     if d>=4 and d<=6 then target=plot;break end
    end
   end
   if target then return mpMoveTowards(unit,target) end
  end
  return false
 end
 if row.WorkRate and row.WorkRate>0 then
  if unit:GetBuildType()>=0 then return true end
  for _,name in ipairs({"BUILD_FARM","BUILD_MINE","BUILD_PASTURE","BUILD_PLANTATION","BUILD_CAMP"}) do
   local build=GameInfo.Builds[name]
   if build and unit:CanBuild(unit:GetPlot(),build.ID) then
    mpOrder(unit,MissionTypes.MISSION_BUILD,build.ID,-1);return true
   end
  end
  for i=0,Map.GetNumPlots()-1 do
   local plot=Map.GetPlotByIndex(i)
   if plot:GetOwner()==p:GetID() and plot:GetImprovementType()<0 and not plot:IsWater() then
    for _,name in ipairs({"BUILD_FARM","BUILD_MINE","BUILD_PASTURE","BUILD_PLANTATION","BUILD_CAMP"}) do
     local build=GameInfo.Builds[name]
     if build and unit:CanBuild(plot,build.ID) then return mpMoveTowards(unit,plot) end
    end
   end
  end
  return false
 end
 if unit:IsCombatUnit() then
  local target=nil
  for id=0,GameDefines.MAX_MAJOR_CIVS-1 do
   local other=Players[id]
   if other and other:IsAlive() and other:GetTeam()~=p:GetTeam() then
    local capital=other:GetCapitalCity()
    if capital then
     target=target or capital
     if Teams[p:GetTeam()]:IsAtWar(other:GetTeam()) then target=capital;break end
    end
   end
  end
  if target then
   local team=Teams[p:GetTeam()]
   if team:IsAtWar(target:GetTeam()) and unit:CanRangeStrikeAt(target:GetX(),target:GetY()) then
    mpOrder(unit,MissionTypes.MISSION_RANGE_ATTACK,target:GetX(),target:GetY());return true
   end
   return mpMoveTowards(unit,target:Plot())
  end
 end
 return false
end

local function mpSnapshot()
 local turn=Game.GetGameTurn()
 for i=0,GameDefines.MAX_MAJOR_CIVS-1 do
  local p=Players[i]
  if p and p:IsAlive() then
   local k="turn."..turn..".player."..i.."."
   mpMark(k.."human",p:IsHuman())
   mpMark(k.."minor",p:IsMinorCiv())
   mpMark(k.."team",p:GetTeam())
   mpMark(k.."civilization",GameInfo.Civilizations[p:GetCivilizationType()].Type)
   mpMark(k.."cities",p:GetNumCities())
   mpMark(k.."units",p:GetNumUnits())
   mpMark(k.."population",p:GetTotalPopulation())
   mpMark(k.."gold",p:GetGold())
   mpMark(k.."science",p:GetScience())
   mpMark(k.."score",p:GetScore())
   mpMark(k.."policies",p:GetNumPolicies())
   mpMark(k.."technologies",Teams[p:GetTeam()]:GetTeamTechs():GetNumTechsKnown())
   local unitIDs={}
   for unit in p:Units() do
    unitIDs[#unitIDs+1]=unit:GetID()
    mpMark(k.."unit."..unit:GetID()..".damage",unit:GetDamage())
    mpMark(k.."unit."..unit:GetID()..".type",GameInfo.Units[unit:GetUnitType()].Type)
    mpMark(k.."unit."..unit:GetID()..".position",unit:GetX()..","..unit:GetY())
   end
   table.sort(unitIDs)
   mpMark(k.."unitIDs",table.concat(unitIDs,","))
   mpMark(k.."culture",p:GetJONSCulture())
   mpMark(k.."faith",p:GetFaith())
   local improvements=0
   for index=0,Map.GetNumPlots()-1 do local plot=Map.GetPlotByIndex(index);if plot:GetOwner()==i and plot:GetImprovementType()>=0 then improvements=improvements+1 end end
   mpMark(k.."improvements",improvements)
   local cityIDs={}
   for city in p:Cities() do
    cityIDs[#cityIDs+1]=city:GetID()
    mpMark(k.."city."..city:GetID()..".population",city:GetPopulation())
    mpMark(k.."city."..city:GetID()..".damage",city:GetDamage())
    mpMark(k.."city."..city:GetID()..".owner",city:GetOwner())
    mpMark(k.."city."..city:GetID()..".position",city:GetX()..","..city:GetY())
   end
   table.sort(cityIDs)
   mpMark(k.."cityIDs",table.concat(cityIDs,","))
   for other=0,GameDefines.MAX_MAJOR_CIVS-1 do
    if Players[other] and Players[other]:IsAlive() and other~=i then mpMark(k.."war."..other,Teams[p:GetTeam()]:IsAtWar(Players[other]:GetTeam())) end
   end
  end
 end
 mpMark("quickCombat",PreGame.GetQuickCombat())
 mpMark("quickMovement",PreGame.GetQuickMovement())
 mpMark("simultaneousTurns",Game.IsOption("GAMEOPTION_SIMULTANEOUS_TURNS"))
 mpMark("noDestructiveRecapture",Game.IsOption("GAMEOPTION_NO_DESTRUCTIVE_RECAPTURE"))
 mpMark("turn",turn)
end
local function mpTick()
 if not mpReady or not Game.IsFinalInitialized() then return end
 local command=mpLedger and mpLedger.GetValue("command")
 if command and command~=mpCommand then
  mpCommand=command
  local chunk,err=loadstring("return function(validationLedger)\n"..command.."\nend")
  if not chunk then error(err) end
  local execute=chunk()
  local ok,result=pcall(execute,mpLedger)
  mpMark("commandResult",tostring(ok).." | "..tostring(result))
  mpMark("commandCompleted",command)
 end
 local turn=Game.GetGameTurn()
 local id=Game.GetActivePlayer()
 local p=Players[id]
 mpSnapshot()
 if turn>=30 then
  if turn>30 then error("exceeded target") end
  if not mpTargetSince then mpTargetSince=os.time() end
  if not mpSaved and os.time()-mpTargetSince>=3 then
   UI.SaveGame("Lekmod Expanded Crossplay Validation Turn 30")
   mpSaved=true
   mpMark("result","completed-30-turns")
  end
  return
 end
 mpMark("tickTime",os.time())
 mpMark("processing",Game.IsProcessingMessages())
 mpMark("turnActive",p and p:IsTurnActive())
 mpMark("sentComplete",Network.HasSentNetTurnComplete())
 mpMark("currentBlocking",p and p:GetEndTurnBlockingType())
 if not p or not p:IsAlive() or not p:IsTurnActive() or Game.IsProcessingMessages() then return end
 UI.SetDontShowPopups(true)
 if id==0 and turn>=12 then
  for other=1,GameDefines.MAX_MAJOR_CIVS-1 do
   local rival=Players[other]
   if rival and rival:IsAlive() and rival:GetTeam()~=p:GetTeam() and Teams[p:GetTeam()]:IsHasMet(rival:GetTeam()) then
    local war=turn<24
    if Teams[p:GetTeam()]:IsAtWar(rival:GetTeam())~=war then
     Network.SendChangeWar(rival:GetTeam(),war);mpMark("warRequest."..turn,war);return
    end
   end
  end
 end
 if not mpQuickRequested and p:GetNumCities()>0 and Matchmaking.IsHost() then
  Network.SendGameOptions({{"GAMEOPTION_QUICK_COMBAT",true},{"GAMEOPTION_QUICK_MOVEMENT",true}})
  mpQuickRequested=true
 end
 if p:GetCurrentResearch()<0 then
  local best=nil
  for tech in GameInfo.Technologies() do
   if p:CanResearch(tech.ID) and (not best or tech.Cost<best.Cost) then best=tech end
  end
  if best then Network.SendResearch(best.ID,0,-1,false);mpMark("lastResearch",best.Type) end
 end
 for city in p:Cities() do
  local cityID=city:GetID()
  if city:GetOrderQueueLength()>0 then mpCityOrders[cityID]=nil end
  if city:GetOrderQueueLength()==0 and not mpCityOrders[cityID] then
   local chosen=nil
   local settler,worker=false,false
   for unit in p:Units() do local r=GameInfo.Units[unit:GetUnitType()];settler=settler or r.Found;worker=worker or (r.WorkRate and r.WorkRate>0) end
   for _,name in ipairs({"UNIT_SETTLER","UNIT_WORKER","UNIT_ARCHER","UNIT_WARRIOR"}) do
    local u=GameInfo.Units[name]
    local wanted=(name=="UNIT_SETTLER" and p:GetNumCities()<2 and not settler and city:GetPopulation()>=2)
     or (name=="UNIT_WORKER" and p:GetNumCities()>=2 and not worker)
     or (p:GetNumCities()>=2 and worker and (name=="UNIT_ARCHER" or name=="UNIT_WARRIOR"))
    if wanted and u and city:CanTrain(u.ID) then
     chosen=u
     mpCityOrders[cityID]=true
     Game.CityPushOrder(city,OrderTypes.ORDER_TRAIN,u.ID,false,false,false)
     mpMark("lastProduction",u.Type);break
    end
   end
   if not chosen and city:GetOrderQueueLength()==0 then
   for _,name in ipairs({"BUILDING_MONUMENT","BUILDING_SHRINE","BUILDING_GRANARY","BUILDING_LIBRARY","BUILDING_WATERMILL","BUILDING_BARRACKS"}) do
    local b=GameInfo.Buildings[name]
    if b and city:CanConstruct(b.ID) then chosen=b;break end
   end
   if chosen then
    mpCityOrders[cityID]=true
    Game.CityPushOrder(city,OrderTypes.ORDER_CONSTRUCT,chosen.ID,false,false,false)
    mpMark("lastProduction",chosen.Type)
   else
    for unit in GameInfo.Units() do
     if city:CanTrain(unit.ID) and unit.Combat>0 then
      mpCityOrders[cityID]=true
      Game.CityPushOrder(city,OrderTypes.ORDER_TRAIN,unit.ID,false,false,false)
      mpMark("lastProduction",unit.Type);break
     end
    end
   end
   end
  end
 end
 local block=p:GetEndTurnBlockingType()
 mpMark("blocking",block)
 if block==EndTurnBlockingTypes.ENDTURN_BLOCKING_POLICY or block==EndTurnBlockingTypes.ENDTURN_BLOCKING_FREE_POLICY then
  local chosen=false
  for branch in GameInfo.PolicyBranchTypes() do
   if not p:IsPolicyBranchUnlocked(branch.ID) and p:CanUnlockPolicyBranch(branch.ID) then
    Network.SendUpdatePolicies(branch.ID,false,true);mpMark("policyRequest",branch.Type);return
   end
  end
  if not chosen then
   for policy in GameInfo.Policies() do
    if p:CanAdoptPolicy(policy.ID) then Network.SendUpdatePolicies(policy.ID,true,true);mpMark("policyRequest",policy.Type);return end
   end
  end
 end
 if block==EndTurnBlockingTypes.ENDTURN_BLOCKING_FOUND_PANTHEON then
  local beliefs=Game.GetAvailablePantheonBeliefs()
  if beliefs and #beliefs>0 then Network.SendFoundPantheon(id,beliefs[1]);mpMark("pantheonRequest",beliefs[1]);return end
 end
 local unitIDs={}
 for u in p:Units() do unitIDs[#unitIDs+1]=u:GetID() end
 for _,unitID in ipairs(unitIDs) do
  local unit=p:GetUnitByID(unitID)
  if unit and unit:MovesLeft()>0 and not unit:IsWaiting() and (mpUnitActions[unitID]~=turn or (turn==0 and p:GetNumCities()==0 and GameInfo.Units[unit:GetUnitType()].Found)) then
   UI.SelectUnit(unit)
   local ordered=mpSpecialOrder(unit,p)
   if not ordered and unit:GetPlot():GetNumUnits()>1 then
    for dir=0,5 do
     local plot=Map.PlotDirection(unit:GetX(),unit:GetY(),dir)
     if plot and plot:GetNumUnits()==0 and unit:CanMoveThrough(plot) then
      mpOrder(unit,MissionTypes.MISSION_MOVE_TO,plot:GetX(),plot:GetY());ordered=true;break
     end
    end
   end
   if not ordered and unit:CanHold(unit:GetPlot()) then
    mpOrder(unit,MissionTypes.MISSION_SKIP,-1,-1)
   end
   mpUnitActions[unitID]=turn
  end
 end
 if turn==0 and p:GetNumCities()==0 then return end
 if Game.CanDoControl(ControlTypes.CONTROL_ENDTURN) then
  Game.DoControl(ControlTypes.CONTROL_ENDTURN)
  mpMark("lastEndTurnRequest",turn)
 elseif ControlTypes.CONTROL_FORCEENDTURN and Game.CanDoControl(ControlTypes.CONTROL_FORCEENDTURN) then
  Game.DoControl(ControlTypes.CONTROL_FORCEENDTURN)
  mpMark("lastEndTurnRequest",turn)
 end
end
local function mpUpdate(dt)
 mpOldUpdate(dt)
 mpElapsed=mpElapsed+dt
 if mpElapsed<1 then return end
 mpElapsed=0
 local ok,err=pcall(mpTick)
 if not ok then mpMark("error",err);print("LEKMOD_MP_TEST gameplay error",err) end
end
Events.SequenceGameInitComplete.Add(function()
 local ok,err=pcall(function()
  UI.SetDontShowPopups(true)
  Events.LoadScreenClose()
  mpMark("localID",Game.GetActivePlayer())
  mpMark("map",PreGame.GetMapScript())
  local x,y=Map.GetGridSize();mpMark("mapSize",x.."x"..y)
  ContextPtr:SetUpdate(mpUpdate)
  mpReady=true
  mpMark("initialized",true)
  mpMark("initializationTime",os.time())
  mpMark("error","")
  mpSnapshot()
 end)
 if not ok then mpMark("error",err) end
end)
