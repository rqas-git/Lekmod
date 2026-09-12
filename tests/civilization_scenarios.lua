local directory, scenario = arg[1], arg[2]

local function iterator(values)
   local index = 0
   return function() index = index + 1; return values[index] end
end

local function load_civ(name, env)
   setmetatable(env, {__index=_G})
   env.include = function() end
   env.print = function() end
   env.LekmodUtilities = {
      is_civilization_active=function() return true end,
      get_random_between=function() return 3 end,
      get_round=function(_, n) return math.floor(n + 0.5) end,
   }
   env.GameEvents = setmetatable({}, {__index=function(events, key)
      local callbacks = {}
      local event = {
         Add=function(callback) table.insert(callbacks, callback) end,
         fire=function(...) for _, callback in ipairs(callbacks) do callback(...) end end,
      }
      events[key] = event
      return event
   end})
   env.Locale = {ConvertTextKey=function() return 'alert' end}
   env.alerts = 0
   env.Events = {GameplayAlertMessage=function() env.alerts=env.alerts+1 end}
   setfenv(assert(loadfile(directory .. '/' .. name .. '.lua')), env)()
   return env
end

local scenarios = {}

function scenarios.italy()
   for _, golden in ipairs({true, false}) do
      for _, human in ipairs({true, false}) do
         for _, active in ipairs({0, 2}) do
            local reward = 0
            local player = {
               IsEverAlive=function() return true end, GetCivilizationType=function() return 1 end,
               HasPolicy=function() return true end, IsPolicyBranchFinished=function() return true end,
               IsGoldenAge=function() return golden end, IsHuman=function() return human end,
               ChangeGoldenAgeTurns=function(_, n) assert(golden); reward=reward+n end,
               ChangeGoldenAgeProgressMeter=function(_, n) assert(not golden); reward=reward+n end,
            }
            local env = load_civ('Lekmod_italy', {
               Players={[2]=player}, GameInfoTypes={CIVILIZATION_ITALY=1},
               GameInfo={Policies={[7]={PolicyBranchType='branch'}},
                  PolicyBranchTypes={branch={ID=4}}, GameSpeeds={[0]={GoldenAgePercent=100}}},
               Game={GetGameSpeedType=function() return 0 end, GetActivePlayer=function() return active end},
            })
            env.GameEvents.PlayerAdoptPolicy.fire(2, 7)
            assert(reward == (golden and 5 or 312.5), 'Italy reward depends on local player')
            assert(env.alerts == ((human and active==2) and 1 or 0), 'Alert leaked to another player')
         end
      end
   end
end

function scenarios.science()
   for _, selected in ipairs({-1, 7}) do
      for _, initial in ipairs({0, 10000}) do
         local overflow, science = initial, 0
         local techs = {ChangeResearchProgress=function(_, tech, amount, owner)
            assert(tech==7 and owner==1, 'Science was applied to an invalid technology')
            science=science+amount
         end}
         local player = {
            IsAlive=function() return true end, GetCivilizationType=function() return 1 end,
            GetCurrentResearch=function() return selected end, GetOverflowResearch=function() return overflow end,
            ChangeOverflowResearch=function(_, n) overflow=overflow+n end,
            GetTeam=function() return 0 end, GetID=function() return 1 end, IsHuman=function() return false end,
         }
         local env = load_civ('Lekmod_newzealand', {GameInfoTypes={CIVILIZATION_NEW_ZEALAND=1},
            Teams={[0]={GetTeamTechs=function() return techs end}}})
         env.lekmod_new_zealand_ua_award_bonus(player, {})
         env.lekmod_new_zealand_ua_award_bonus(player, {})
         assert(overflow==initial+(selected==-1 and 24 or 0), 'Overflow reward lost or overwritten')
         assert(science==(selected==7 and 24 or 0))
      end
   end
end

function scenarios.defender()
   for _, radius_owner in ipairs({0, 1, 2, -1}) do
      for _, friendship in ipairs({true, false}) do
         local promotions = {[286]=true, [287]=false}
         local plot = {IsPlayerCityRadius=function(_, id)
            assert(type(id)=='number', 'Radius needs a player ID, not an object')
            return id==radius_owner
         end, GetOwner=function() return -1 end}
         local unit = {IsHasPromotion=function(_, id) return promotions[id] or false end,
            GetPlot=function() return plot end,
            SetHasPromotion=function(_, id, value) promotions[id]=value end}
         local player = {IsAlive=function() return true end, IsBarbarian=function() return false end,
            GetTeam=function() return 8 end, Units=function() return iterator({unit}) end,
            IsDoF=function() error('Must exclude self from friendship checks') end}
         local friend = {IsAlive=function() return true end, IsDoF=function(_, id)
            assert(id==1, 'Friendship requires player ID, not team ID'); return friendship
         end}
         local env = load_civ('Lekmod_newzealand', {
            GameInfoTypes={CIVILIZATION_NEW_ZEALAND=1,PROMOTION_JFD_DEFENDER=286,PROMOTION_JFD_DEFENDER_ACTIVE=287},
            GameDefines={MAX_MAJOR_CIVS=3}, Players={[0]={IsAlive=function() return false end},[1]=player,[2]=friend},
         })
         env.GameEvents.PlayerDoTurn.fire(1)
         assert(promotions[287]==(radius_owner==1 or (friendship and radius_owner==2)))
         plot.IsPlayerCityRadius=function() return false end
         env.GameEvents.PlayerDoTurn.fire(1)
         assert(not promotions[287] and promotions[286], 'Bonus must expire after leaving city radius')
      end
   end
end

local function bolivia()
   local saved, buildings = {}, {}
   local capital = {SetNumRealBuilding=function(_, id, n) buildings[id]=n end,IsHuman=function() return false end}
   local captured_buildings = {[10]=1,[11]=1}
   local captured = {SetNumRealBuilding=function(_,id,n) captured_buildings[id]=n end}
   local cities = {capital}
   local player = {GetCapitalCity=function() return capital end,IsAlive=function() return true end,
      GetCivilizationType=function() return 1 end,Cities=function() return iterator(cities) end}
   local other = {GetCapitalCity=function() return {} end,IsAlive=function() return true end,
      GetCivilizationType=function() return 2 end,Cities=function() return iterator({captured}) end}
   local env = load_civ('Lekmod_bolivia', {
      GameInfoTypes={CIVILIZATION_BOLIVIA=1,UNIT_ARTIST=3,UNIT_WRITER=147,UNIT_COLORADO=238,
         BUILDING_BOLIVIA_TRAIT_PRODUCTION=10,BUILDING_BOLIVIA_TRAIT_FOOD=11},
      Modding={OpenSaveData=function() return {GetValue=function(k) return saved[k] end,
         SetValue=function(k,v) saved[k]=v end} end}, Players={[1]=player,[2]=other},
   })
   return env, saved, buildings, captured_buildings, player
end

function scenarios.bolivia()
   local env, saved, buildings, captured, player = bolivia()
   env.GameEvents.PlayerCityFounded.fire(1,3,10)
   assert(saved.bolivia_last_expended==nil and buildings[10]==nil, 'Founding invented an artist expenditure')
   env.GameEvents.GreatPersonExpended.fire(1,147)
   assert(buildings[11]==1 and buildings[10]==0)
   env.GameEvents.PlayerCityFounded.fire(1,3,10)
   assert(saved.bolivia_last_expended=='1471' and buildings[11]==1 and buildings[10]==0)
   for _, was_capital in ipairs({true, false}) do
      env.GameEvents.CityCaptureComplete.fire(1,was_capital,3,10,2)
      assert(saved.bolivia_last_expended=='1471' and buildings[11]==1)
      assert(captured[10]==0 and captured[11]==0, 'Captured city retained Bolivia buildings')
      env.GameEvents.CityCaptureComplete.fire(2,was_capital,147,10,1)
      assert(saved.bolivia_last_expended=='1471' and buildings[11]==1)
   end
   env.GameEvents.GreatPersonExpended.fire(1,3)
   assert(buildings[10]==1 and buildings[11]==0)
   player.GetCapitalCity=function() return nil end
   env.GameEvents.CityCaptureComplete.fire(1,true,3,10,2)
end

function scenarios.colorado()
   local env, _, _, _, player = bolivia()
   local strength, happiness = 40, 10
   local unit = {GetUnitType=function() return 238 end,SetBaseCombatStrength=function(_,n) strength=n end}
   local other = {GetUnitType=function() return 1 end}
   player.GetUnitByID=function(_,id) if id==7 then return unit elseif id==238 then return other end end
   player.GetExcessHappiness=function() return happiness end
   player.Units=function() return iterator({unit}) end
   env.GameInfo={Units={[238]={Combat=40}}}
   env.GameEvents.UnitCreated.fire(1,238,3,10)
   assert(strength==40, 'Non-Colorado instance refreshed Colorado units')
   env.GameEvents.UnitCreated.fire(1,7,3,10)
   assert(strength==44, 'Created Colorado missed happiness bonus')
   happiness=0
   env.GameEvents.PlayerHappinessChanged.fire(1)
   assert(strength==40)
   env.GameEvents.UnitCreated.fire(1,999,3,10)
end

function scenarios.mughals()
   local buildings = {}
   local city = {IsHasBuilding=function(_,id) return buildings[id]==1 end,
      SetNumRealBuilding=function(_,id,n) buildings[id]=n end,GetReligiousMajority=function() return 4 end}
   local player = {IsAlive=function() return true end,GetCivilizationType=function() return 1 end,
      HasCreatedReligion=function() return false end,Cities=function() return iterator({city}) end}
   local env = load_civ('Lekmod_mughals', {GameInfoTypes={CIVILIZATION_MUGHALS=1,BUILDING_DUMMY_MUGHALS=10},
      Players={[2]=player}, GameDefines={MAX_MAJOR_CIVS=3},ReligionTypes={RELIGION_PANTHEON=0}})
   for _, was_capital in ipairs({true,false}) do
      buildings[10]=0
      env.GameEvents.CityCaptureComplete.fire(0,was_capital,10,20,2,6,true)
      assert(buildings[10]==1, 'Capture did not refresh new owner')
   end
   city.GetReligiousMajority=function() return -1 end
   env.GameEvents.CityConvertsReligion.fire(2,-1,10,20)
   assert(buildings[10]==0, 'Conversion did not remove obsolete city bonus')
   city.GetReligiousMajority=function() return 4 end
   env.GameEvents.CityConvertsReligion.fire(2,4,10,20)
   assert(buildings[10]==1, 'Conversion did not add city bonus')
end

assert(scenarios[scenario], 'Unknown scenario')()
print(scenario .. ' passed')
