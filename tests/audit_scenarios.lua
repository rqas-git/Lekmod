local root, scenario = arg[1], arg[2]
assert(_VERSION == 'Lua 5.1')
local function iter(values)
   local index = 0
   return function() index = index + 1; return values[index] end
end
local function events()
   return setmetatable({}, {__index=function(self, name)
      local handlers = {}
      local event = {
         Add=function(fn) handlers[fn]=true end,
         Remove=function(fn) handlers[fn]=nil end,
         fire=function(...) for fn in pairs(handlers) do fn(...) end end,
      }
      self[name]=event; return event
   end})
end
local function loadscript(path, env)
   env = env or {}
   setmetatable(env, {__index=_G})
   env.include = env.include or function() end
   env.print = function() end
   env.GameEvents, env.Events = events(), events()
   env.GameDefines = env.GameDefines or {MAX_MAJOR_CIVS=4}
   env.LekmodUtilities = {is_civilization_active=function() return true end, get_random_between=function() return 3 end}
   env.saved = env.saved or {}
   env.Modding = {OpenSaveData=function() return {
      GetValue=function(key) return env.saved[key] end,
      SetValue=function(key, value) env.saved[key]=value end,
   } end}
   setfenv(assert(loadfile(root .. '/' .. path)), env)()
   return env
end
local function civ(name, env) return loadscript('LEKMOD/Lua/Civilizations/Lekmod_'..name..'.lua', env) end
local scenarios = {}

function scenarios.science_legacy()
   local selected, amount = -1, 0
   local player = {IsAlive=function() return true end, GetCivilizationType=function() return 1 end,
      GetID=function() return 1 end, GetTeam=function() return 0 end, IsHuman=function() return false end,
      GetCurrentResearch=function() return selected end}
   local input = {Players={[1]=player}, GameInfoTypes={CIVILIZATION_NEW_ZEALAND=1},
      Teams={[0]={GetTeamTechs=function() return {ChangeResearchProgress=function(_, tech, n, id)
         assert(tech==7 and id==1); amount=amount+n
      end} end}}}
   local env = civ('newzealand', input)
   env.lekmod_new_zealand_ua_award_bonus(player, {})
   env.lekmod_new_zealand_ua_award_bonus(player, {})
   env.lekmod_new_zealand_apply_pending_science(1)
   assert(amount==0 and input.saved.lekmod_new_zealand_pending_science_1==24)
   env = civ('newzealand', input) -- saved awards survive a Lua-context reload
   selected=7
   env.lekmod_new_zealand_apply_pending_science(1)
   env.lekmod_new_zealand_apply_pending_science(1)
   assert(amount==24 and input.saved.lekmod_new_zealand_pending_science_1==0)
end

function scenarios.team_reward()
   local amounts, players = {}, {}
   for id=0,3 do
      local pid=id
      players[id]={IsAlive=function() return true end, GetCivilizationType=function() return pid==0 and 2 or 1 end,
         GetTeam=function() return pid<2 and 0 or 1 end, GetID=function() return pid end,
         GetCurrentResearch=function() return -1 end, IsHuman=function() return false end,
         ChangeOverflowResearch=function(_, n) amounts[pid]=(amounts[pid] or 0)+n end}
   end
   local env=civ('newzealand',{Players=players,GameInfoTypes={CIVILIZATION_NEW_ZEALAND=1},
      Teams={[0]={GetLeaderID=function() return 0 end},[1]={GetLeaderID=function() return 2 end}}})
   env.GameEvents.TeamMeet.fire(1,0)
   assert(amounts[0]==nil and amounts[1]==12 and amounts[2]==12 and amounts[3]==12)
end

function scenarios.one_time_players()
   local players, counts, saved = {}, {}, {}
   for id=0,1 do
      local pid=id
      players[id]={IsAlive=function() return true end, GetCivilizationType=function() return 1 end,
         GetTeam=function() return pid end, ChangeNumMiscTradeRoutes=function(_, n) counts[pid]=(counts[pid] or 0)+n end}
   end
   local input={Players=players,saved=saved,GameInfoTypes={CIVILIZATION_VENEZ=1,TECH_COMPASS=6}}
   local env=civ('venice',input)
   for id=0,1 do env.GameEvents.TeamTechResearched.fire(id,6) end
   env=civ('venice',input)
   for id=0,1 do env.GameEvents.TeamTechResearched.fire(id,6) end
   assert(counts[0]==1 and counts[1]==1)
   input.GameInfoTypes={CIVILIZATION_MEXICO=1}
   env=civ('mexico',input)
   local reveals={}
   env.lekmod_mexico_ua_do_discover=function(id) reveals[id]=(reveals[id] or 0)+1 end
   for id=0,1 do env.GameEvents.PlayerDoTurn.fire(id) end
   env=civ('mexico',input)
   env.lekmod_mexico_ua_do_discover=function() error('Repeated a persisted one-time ability') end
   for id=0,1 do env.GameEvents.PlayerDoTurn.fire(id) end
   assert(reveals[0]==1 and reveals[1]==1)
end

function scenarios.bolivia_players()
   local players, capitals, saved = {}, {}, {}
   local function capital()
      local buildings={}
      return {buildings=buildings, IsHuman=function() return false end,
         IsHasBuilding=function(_, id) return buildings[id]==1 end,
         SetNumRealBuilding=function(_, id, n) buildings[id]=n end}
   end
   for id=1,2 do
      local pid=id
      capitals[id]=capital()
      players[id]={IsAlive=function() return true end, GetCivilizationType=function() return 1 end,
         GetCapitalCity=function() return capitals[pid] end, Cities=function() return iter({capitals[pid]}) end}
   end
   local input={Players=players,saved=saved,GameInfoTypes={CIVILIZATION_BOLIVIA=1,UNIT_ARTIST=3,UNIT_WRITER=147,
      BUILDING_BOLIVIA_TRAIT_PRODUCTION=10,BUILDING_BOLIVIA_TRAIT_FOOD=11}}
   local env=civ('bolivia',input)
   env.lekmod_bolivia_is_person_expended(1,3)
   env.lekmod_bolivia_is_person_expended(2,147)
   assert(saved.bolivia_last_expended_1==3 and saved.bolivia_last_expended_2==147)
   capitals[1], capitals[2]=capital(),capital()
   env=civ('bolivia',input)
   for id=1,2 do env.lekmod_bolivia_city_founded(id) end
   assert(capitals[1].buildings[10]==1 and capitals[2].buildings[11]==1)
   -- The legacy record restores its owner; an overwritten owner recovers its capital building.
   input.saved={bolivia_last_expended='1472'}
   env=civ('bolivia',input)
   for id=1,2 do env.lekmod_bolivia_city_founded(id) end
   assert(input.saved.bolivia_last_expended_1==3 and input.saved.bolivia_last_expended_2==147)
end

function scenarios.georgia_events()
   local promotion, golden = false, true
   local unit={GetUnitType=function() return 5 end,IsDead=function() return false end,
      IsHasPromotion=function() return promotion end,SetHasPromotion=function(_,_,value) promotion=value end}
   local player={IsAlive=function() return true end,GetCivilizationType=function() return 1 end,
      IsGoldenAge=function() return golden end,Units=function() return iter({unit}) end}
   local env=civ('georgia',{Players={[0]=player},GameInfoTypes={CIVILIZATION_GEORGIA=1,UNIT_GEORGIA_KHEVSUR=5,PROMOTION_GEORGIA_KHEVSUR_GA=6}})
   env.GameEvents.UnitCreated.fire(0,7); assert(promotion)
   golden=false; env.GameEvents.PlayerSetGoldenAge.fire(0); assert(not promotion)
   golden=true; env.GameEvents.PlayerSetGoldenAge.fire(0); assert(promotion)
   promotion=false; env.Events.SequenceGameInitComplete.fire(); assert(promotion)
end

function scenarios.mughal_original_owner()
   for _, original in ipairs({1,2}) do
      local holyBonus, ownBonus=0,0
      local city={GetOriginalOwner=function() return original end,GetReligiousMajority=function() return 3 end,
         IsHasBuilding=function() return false end,SetNumRealBuilding=function(_,_,n) ownBonus=n end}
      local holy={IsHolyCityForReligion=function(_,r) return r==3 end,
         IsHasBuilding=function() return false end,SetNumRealBuilding=function(_,_,n) holyBonus=n end}
      local env=civ('mughals',{GameInfoTypes={CIVILIZATION_MUGHALS=1,BUILDING_DUMMY_MUGHALS=9},
         ReligionTypes={RELIGION_PANTHEON=0},Players={
         [1]={IsAlive=function() return true end,GetCivilizationType=function() return 1 end,
            HasCreatedReligion=function() return false end,Cities=function() return iter({city}) end},
         [2]={IsAlive=function() return true end,Cities=function() return iter({holy}) end}}})
      env.lekmod_ua_mughals_foreign_religion_check(1)
      assert(ownBonus==1 and holyBonus==(original==1 and 1 or 0))
   end
end

function scenarios.dummy_policies()
   local free, ever, owned=3,3,false
   local player={IsEverAlive=function() return true end,GetCivilizationType=function() return 4 end,
      HasPolicy=function() return owned end,GetNumFreePolicies=function() return free end,
      SetNumFreePolicies=function(_,n) ever=ever+math.max(0,n-free);free=n end,
      SetHasPolicy=function() owned=true end}
   local env=loadscript('LEKMOD/Lua/Lekmod_global_dummies.lua',{Players={[0]=player},GameDefines={MAX_MAJOR_CIVS=1},
      GameInfoTypes={CIVILIZATION_MAORI=4,POLICY_DUMMY_MAORI=9},GameInfo={Civilization_Dummy_Policies=function()
         return iter({{Type='CIVILIZATION_MAORI',PolicyType='POLICY_DUMMY_MAORI'}}) end}})
   env.Events.SequenceGameInitComplete.fire()
   env.Events.SequenceGameInitComplete.fire()
   assert(free==3 and ever==4 and owned, 'Dummy policy must preserve choices and compensate its policy cost once')
end

function scenarios.draft_guarantees()
   local env=loadscript('LEKMOD/Lua/Utilities/Lekmod_drafter.lua')
   local d=env.LekmodDrafter
   local index={}
   for id=1,12 do index[id]={tags=id<=4 and {'coastal'} or {}} end
   d.BuildIDIndex=function() return index end
   for seed=1,100 do
      for _, guaranteed in ipairs({{1,0},{0,2},{1,2}}) do
         math.randomseed(seed)
         local result=d.CreateDraft({picksPerPlayer=3,guaranteedCoastals=guaranteed[1],guaranteedInlands=guaranteed[2]}, {0,1,2,3},{})
         assert(result.ok)
         local seen={}
         for pid=0,3 do
            local coast,inland=0,0
            assert(#result.drafts[pid]==3)
            for _,id in ipairs(result.drafts[pid]) do
               assert(not seen[id]);seen[id]=true
               if id<=4 then coast=coast+1 else inland=inland+1 end
            end
            assert(coast>=guaranteed[1] and inland>=guaranteed[2])
         end
      end
   end
   local bad=d.CreateDraft({picksPerPlayer=3,guaranteedCoastals=2,guaranteedInlands=0},{0,1,2,3},{})
   assert(not bad.ok)
end

function scenarios.draft_protocol()
   local localID, options, sent = 0, {}, {}
   local env=loadscript('LEKMOD/Lua/Utilities/Lekmod_drafter.lua')
   local d=env.LekmodDrafter
   d.BuildIDIndex=function() return {[41]={},[42]={},[43]={}} end
   env=loadscript('LEKMOD/Lua/Utilities/Lekmod_staging_draft.lua',{
      LekmodDrafter=d,Controls={},m_SlotInstances={},
      Matchmaking={GetHostID=function() return 0 end,GetLocalID=function() return localID end,IsHost=function() return localID==0 end},
      SlotStatus={SS_TAKEN=1,SS_COMPUTER=2,SS_OPEN=3},SlotClaim={SLOTCLAIM_RESERVED=1},
      PreGame={GetSlotStatus=function(id) return id==3 and 2 or 1 end,GetSlotClaim=function() return 0 end,
         IsReady=function() return false end,IsHotSeatGame=function() return false end,GameStarted=function() return false end,
         GetGameOption=function(key) return options[key] or 0 end,SetGameOption=function(key,v) options[key]=v end,
         GetCivilization=function() return -1 end},
      Network={SendChat=function(message) sent[#sent+1]=message end},
   })
   for _,name in ipairs({'Draft_RefreshBanUI','Draft_RefreshDraftIconsAll','Draft_PersistToPreGame',
      'Draft_UpdateActionButtons','PopulateCivPulldown','Draft_PopulateRulesUI'}) do env[name]=function() end end
   env.g_DraftPools={[1]={41},[2]={42},[3]={43}}
   env.g_DraftLocked=true
   local function receive(sender,message) return env.Draft_HandleProtocol(sender,'#LDRAFT#'..message) end
   receive(2,'DRAFT|1|43'); assert(env.g_DraftPools[1][1]==41)
   receive(2,'LOCK|0'); assert(env.g_DraftLocked)
   receive(2,'RESET|1'); assert(env.g_DraftLocked)
   receive(2,'RULES|0|10|0|0|0|0'); assert(env.g_DraftRules.picksPerPlayer==3)
   receive(2,'SWAP|1|2|43|41'); assert(env.g_DraftPools[1][1]==41)
   receive(2,'SWAPREQ|1|2'); assert(env.g_DraftSwapDesire[1]==nil)
   receive(0,'DRAFT|1|43'); assert(env.g_DraftPools[1][1]==43)
   for _,invalid in ipairs({'DRAFT|99|41','DRAFT|1|999','DRAFT|1|41,41','DRAFT|1|41,,42',
      'DRAFT|1|-1','RULES|999999999|3|0|0|0|0','RULES|1|999999999|0|0|0|0',
      'BANREADY|99|1','BANREADY|1|2','READYMASK|999999999','LOCK|1junk','SWAPREQ|0|99'}) do
      local op,rest=string.match(invalid,'^([^|]+)|(.*)$')
      assert(not env.Draft_ValidateProtocol(0,op,rest),invalid)
   end
   -- Human swaps require both requests and are completed once by the host.
   receive(1,'SWAPREQ|1|2'); assert(env.g_DraftPools[1][1]==43)
   receive(2,'SWAPREQ|2|1'); assert(env.g_DraftPools[1][1]==42 and env.g_DraftPools[2][1]==43)
   assert(#sent==1 and string.find(sent[1],'#LDRAFT#SWAP|',1,true)==1)
   receive(1,'SWAPREQ|1|3'); assert(env.g_DraftPools[1][1]==43 and env.g_DraftPools[3][1]==42)
   assert(#sent==2)
   -- A client records requests but only applies host pool messages.
   localID=1
   receive(1,'SWAPREQ|1|2');receive(2,'SWAPREQ|2|1');assert(#sent==2)
   receive(0,'SWAP|1|2|41|42');assert(env.g_DraftPools[1][1]==41 and env.g_DraftPools[2][1]==42)
   env.g_DraftLocked=false
   receive(2,'BAN|1|41');assert(env.g_DraftBans[1]==nil)
   receive(1,'BAN|1|41');assert(env.g_DraftBans[1][1]==41)
   receive(2,'BANCTRL|1|1');assert(not env.g_DraftBanHostControl[1])
   receive(1,'BANCTRL|1|1');assert(env.g_DraftBanHostControl[1])
   receive(1,'BAN|1|42');assert(env.g_DraftBans[1][1]==41)
   receive(0,'BAN|1|42');assert(env.g_DraftBans[1][1]==42)
end

function scenarios.teamer_includes()
   local loaded={}
   local width,height=20,14
   local env={MultilayeredFractal={},Map={GetGridSize=function() return width,height end}}
   env.Map.GetPlot=function(x,y)
      assert(x>=0 and x<width and y>=0 and y<height)
      return {GetX=function() return x end,GetY=function() return y end}
   end
   env.include=function(module)
      if loaded[module] then return end
      loaded[module]=true
      if string.sub(module,1,2)=='HB' then
         setfenv(assert(loadfile(root..'/Lekmap/'..module..'.lua')),env)()
      else
         assert(module=='IslandMaker' or module=='MultilayeredFractal' or module=='NaturalWondersCustomMethods',module)
      end
   end
   loadscript('Lekmap/LekmapTeamerMapLegacy.lua',env)
   assert(loaded.HBMapGenerator and loaded.HBFeatureGenerator and loaded.HBAssignStartingPlots)
   assert(type(env.FeatureGenerator.Create)=='function' and type(env.AssignStartingPlots.Create)=='function')
   for w=20,54,2 do
      for h=14,40,2 do
         width,height=w,h
         for x=0,w-1 do
            for y=0,h-1 do
               local mirror=env.getMirroredPlot(env.Map.GetPlot(x,y))
               assert(mirror:GetX()==w-x-1 and mirror:GetY()==h-y-1)
               local twice=env.getMirroredPlot(mirror)
               assert(twice:GetX()==x and twice:GetY()==y)
            end
         end
      end
   end
end

assert(scenarios[scenario], 'Unknown scenario: '..tostring(scenario))()
print(scenario .. ' passed')
