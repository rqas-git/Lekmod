local f=assert(io.open(arg[1]));local source=f:read('*a');f:close()
local special=assert(source:match('(local function mpSpecialOrder.-)\nlocal function mpSnapshot'))
local war=assert(source:match('( if id==0 and turn>=12 then.-)\n if not mpQuickRequested'))
local target
local env={GameInfo={Units={[0]={}}},Teams={},Players={},GameDefines={MAX_MAJOR_CIVS=4},ipairs=ipairs}
local function player(team,alive)
 return {GetID=function()return team end,GetTeam=function()return team end,IsAlive=function()return alive~=false end,GetCapitalCity=function()return {GetTeam=function()return team end} end}
end
env.Teams[0]={IsAtWar=function(_,team)return team==2 end,IsHasMet=function()return true end}
env.Players={[0]=player(0),[1]=player(0),[2]=player(1),[3]=player(2)}
env.mpMoveTowards=function(_,plot)target=plot;return true end
env.mpOrder=function()error('unexpected ranged order')end
for _,p in pairs(env.Players)do local capital=p.GetCapitalCity();capital.Plot=function()return capital end;capital.GetX=function()return 0 end;capital.GetY=function()return 0 end;p.GetCapitalCity=function()return capital end end
local unit={GetUnitType=function()return 0 end,IsCombatUnit=function()return true end,CanRangeStrikeAt=function()return false end}
local chunk=assert(loadstring(special..'\nreturn mpSpecialOrder'));setfenv(chunk,env);local act=chunk()
assert(act(unit,env.Players[0]));assert(target:GetTeam()==2,'must prefer a war opponent over teammate or peaceful opponent')
env.Players[3]=player(2,false);target=nil
assert(act(unit,env.Players[0]));assert(target:GetTeam()==1,'must ignore dead players and teammates')
env.Players[2]=player(0);assert(not act(unit,env.Players[0]),'no opposing player')
local requests={};env.id=0;env.turn=13;env.p=env.Players[0]
env.Teams[0].IsAtWar=function(_,team)return team~=0 end
env.Network={SendChangeWar=function(team,value)requests[#requests+1]={team,value}end};env.mpMark=function()end
env.Players[2]=player(1);env.Players[3]=player(1)
local orders=assert(loadstring(war));setfenv(orders,env);orders();assert(#requests==0,'must not declare war on own team')
env.Teams[0].IsAtWar=function()return false end;orders();assert(#requests==1 and requests[1][1]==1 and requests[1][2]==true)

local movement=assert(source:match('(local function mpMoveTowards.-)\nlocal function mpSpecialOrder'))
local moves={};local moveEnv={Map={PlotDistance=function()return 3 end,PlotDirection=function()return nil end},Teams=env.Teams,MissionTypes={MISSION_MOVE_TO=0},mpOrder=function(_,_,x,y)moves[#moves+1]={x,y}end}
local moveChunk=assert(loadstring(movement..'\nreturn mpMoveTowards'));setfenv(moveChunk,moveEnv);local move=moveChunk()
local mover={GetX=function()return 0 end,GetY=function()return 0 end,GetTeam=function()return 0 end}
local goal={GetX=function()return 4 end,GetY=function()return 5 end,IsCity=function()return false end,GetTeam=function()return 1 end}
assert(move(mover,goal) and #moves==1,'legal destination should use engine routing when greedy walk stalls')
goal.IsCity=function()return true end;moveEnv.Teams[0].IsAtWar=function()return false end
assert(not move(mover,goal) and #moves==1,'must not request entry into peaceful enemy city')
moveEnv.Teams[0].IsAtWar=function()return true end;assert(move(mover,goal) and #moves==2)

local commandBlock=assert(source:match('( local command=mpLedger.-)\n local turn=Game.GetGameTurn'))
local values={};local ledger={GetValue=function()return 'validationLedger.SetValue("probe",123); return "same handle"' end,SetValue=function(k,v)values[k]=v end}
local commandEnv={mpLedger=ledger,loadstring=loadstring,pcall=pcall,tostring=tostring,error=error,mpMark=function(k,v)values[k]=v end}
local commandChunk=assert(loadstring(commandBlock));setfenv(commandChunk,commandEnv);commandChunk()
assert(values.probe==123 and values.commandResult=='true | same handle','commands must reuse the existing ledger')
local founders=0;local ready=false
local foundEnv={GameInfo={Units={[0]={Found=true}}},GameInfoActions={[9]={Type="MISSION_FOUND"}},pairs=pairs,UI={SelectUnit=function()end},Game={CanHandleAction=function()return ready end,HandleAction=function(id)assert(id==9);founders=founders+1 end,GetGameTurn=function()return 0 end},mpMark=function()end}
local foundChunk=assert(loadstring(special..'\nreturn mpSpecialOrder'));setfenv(foundChunk,foundEnv);local found=foundChunk()
local settler={GetUnitType=function()return 0 end,CanFound=function()return true end,GetPlot=function()return {} end,GetID=function()return 123 end}
assert(found(settler,{}) and founders==0,'must wait for founding action readiness')
ready=true;assert(found(settler,{}) and founders==1,'must use normal founding action')
print('multiplayer team, movement, command and founding scenarios passed')
