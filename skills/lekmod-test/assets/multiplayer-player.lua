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
local mpTargetSince=nil
local mpSaved=false
local mpCommand=nil
local function mpSnapshot()
 local turn=Game.GetGameTurn()
 for i=0,GameDefines.MAX_MAJOR_CIVS-1 do
  local p=Players[i]
  if p and p:GetCivilizationType()>=0 then
   local k="turn."..turn..".player."..i.."."
   mpMark(k.."alive",p:IsAlive())
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
  end
 end
 mpMark("turn",turn)
end
local function mpTick()
 if not mpReady or not Game.IsFinalInitialized() then return end
 local command=mpLedger and mpLedger.GetValue("command")
 if command and command~=mpCommand then
  mpCommand=command
  local chunk,err=loadstring(command)
  if not chunk then error(err) end
  local ok,result=pcall(chunk)
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
   UI.SaveGame("Lekmod Crossplay Validation Turn 30")
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
 if p:GetCurrentResearch()<0 then
  local best=nil
  for tech in GameInfo.Technologies() do
   if p:CanResearch(tech.ID) and (not best or tech.Cost<best.Cost) then best=tech end
  end
  if best then Network.SendResearch(best.ID,0,-1,false);mpMark("lastResearch",best.Type) end
 end
 for city in p:Cities() do
  if city:GetOrderQueueLength()==0 then
   local chosen=nil
   for _,name in ipairs({"BUILDING_MONUMENT","BUILDING_SHRINE","BUILDING_GRANARY","BUILDING_LIBRARY","BUILDING_WATERMILL","BUILDING_BARRACKS"}) do
    local b=GameInfo.Buildings[name]
    if b and city:CanConstruct(b.ID) then chosen=b;break end
   end
   if chosen then
    Game.CityPushOrder(city,OrderTypes.ORDER_CONSTRUCT,chosen.ID,false,false,false)
    mpMark("lastProduction",chosen.Type)
   else
    for unit in GameInfo.Units() do
     if city:CanTrain(unit.ID) and unit.Combat>0 then
      Game.CityPushOrder(city,OrderTypes.ORDER_TRAIN,unit.ID,false,false,false)
      mpMark("lastProduction",unit.Type);break
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
  if unit and unit:MovesLeft()>0 and not unit:IsWaiting() and mpUnitActions[unitID]~=turn then
   UI.SelectUnit(unit)
   if unit:CanFound(unit:GetPlot()) then
    Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION,MissionTypes.MISSION_FOUND,-1,-1,0,false,false)
    mpMark("founded",turn)
   elseif unit:CanFortify(unit:GetPlot()) then
    Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION,MissionTypes.MISSION_FORTIFY,-1,-1,0,false,false)
   elseif unit:CanSleep(unit:GetPlot()) then
    Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION,MissionTypes.MISSION_SLEEP,-1,-1,0,false,false)
   end
   mpUnitActions[unitID]=turn
  end
 end
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
