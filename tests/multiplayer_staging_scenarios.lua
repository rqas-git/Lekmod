local hook=assert(arg[1])
local values={};local started=true;local ready=false;local readyCalls=0;local draftCalls=0
ContextPtr={IsHidden=function() return false end}
Modding={OpenUserData=function() return {SetValue=function(k,v) values[k]=v end} end}
OnStagingUpdate=function() end
OnChat=function() end
Events={GameMessageChat={Remove=function() end,Add=function() end}}
Matchmaking={GetLocalID=function() return 1 end,IsHost=function() return false end}
PreGame={GameStarted=function() return started end,IsReady=function() return ready end,GetLoadFileName=function() return "checkpoint" end}
OnReadyCheck=function(v) assert(v);ready=true;readyCalls=readyCalls+1 end
GameDefines={MAX_MAJOR_CIVS=4}
Network={IsPlayerConnected=function(i) return i<2 end,IsEveryoneConnected=function() return true end}
Draft_IsDraftLocked=function() assert(not started,"redrafting a started game");return false end
Draft_OnLocalBanReady=function() draftCalls=draftCalls+1 end
assert(loadfile(hook))()
OnStagingUpdate(2)
assert(readyCalls==1 and ready,"reconnect must use the normal ready action")
assert(values.phase=="hot join ready" and not values.error)
OnStagingUpdate(2)
assert(readyCalls==1 and draftCalls==0,"ready reconnect must not redraft")
started=false;ready=false
for i=1,5 do OnStagingUpdate(2) end
assert(values.phase=="checkpoint ready" and readyCalls==2,"checkpoint loading must remain supported")
PreGame.GetLoadFileName=function() return "" end
OnStagingUpdate(2)
assert(values.phase=="drafting" and draftCalls==1,"fresh games must retain normal drafting")
print("multiplayer staging scenarios passed")
