-- Temporary multiplayer smoke-test player, using normal draft/ready paths.
local mpStageLedger=nil
local function stageMark(k,v)
 if not mpStageLedger then mpStageLedger=Modding.OpenUserData("LekmodCrossplayStage",1) end
 mpStageLedger.SetValue(k,tostring(v))
end
local stageOldUpdate=OnStagingUpdate
local stageElapsed=0
local stageRules=false
local stageSelected=false
local stageStable=0
local stageBroadcast=0
function OnStagingUpdate(dt)
 if ContextPtr:IsHidden() then return end
 stageOldUpdate(dt)
 stageElapsed=stageElapsed+dt
 if stageElapsed<2 then return end
 stageElapsed=0
 local ok,err=pcall(function()
  local id=Matchmaking.GetLocalID()
  stageMark("localID",id)
  local connected=0
  for i=0,GameDefines.MAX_MAJOR_CIVS-1 do
   if Network.IsPlayerConnected(i) then connected=connected+1 end
  end
  stageMark("connected",connected)
  stageMark("everyoneConnected",Network.IsEveryoneConnected())
  stageMark("locked",Draft_IsDraftLocked())
  if connected<2 or not Network.IsEveryoneConnected() then stageStable=0;return end
  stageStable=stageStable+2
  if stageStable<10 then stageMark("phase","waiting for lobby initialization");return end
  if PreGame.GetLoadFileName()~="" then
   if not PreGame.IsReady(id) then OnReadyCheck(true) end
   stageMark("phase","checkpoint ready")
   if Matchmaking.IsHost() and not stageRules and PreGame.IsReady(0) and PreGame.IsReady(1) then
    stageRules=true;LaunchGame();stageMark("phase","checkpoint launch requested")
   end
   return
  end
  if Matchmaking.IsHost() and not stageRules then
   stageRules=true
   stageMark("rules","default bans and draft pool")
  end
  if not Draft_IsDraftLocked() then
   Draft_OnLocalBanReady(true)
   if Matchmaking.IsHost() then Draft_OnCreateDraft() end
   stageMark("phase","drafting")
   return
  end
  if Matchmaking.IsHost() then
   stageBroadcast=stageBroadcast+2
   if stageBroadcast>=10 and not PreGame.IsReady(1) then Draft_BroadcastPools();stageBroadcast=0 end
  end
  local pool=Draft_GetPoolForPlayer(id)
  stageMark("pool",pool and table.concat(pool,",") or "none")
  if not pool or #pool==0 then return end
  if not stageSelected then
   PreGame.SetCivilization(id,pool[1])
   Network.BroadcastPlayerInfo()
   stageSelected=true
   stageMark("civilization",GameInfo.Civilizations[pool[1]].Type)
   stageMark("phase","civilization selected")
   return
  end
  if not PreGame.IsReady(id) then OnReadyCheck(true) end
  stageMark("phase","ready")
 end)
 if not ok then stageMark("error",err);print("LEKMOD_MP_TEST staging error",err) end
end
-- The normal staging-show path installs OnStagingUpdate through EnsureStagingUpdate.

local stageChats=0
local stageStockChat=OnChat
Events.GameMessageChat.Remove(stageStockChat)
function OnChat(fromPlayer,toPlayer,msg,eTargetType)
 local ok,err=pcall(stageStockChat,fromPlayer,toPlayer,msg,eTargetType)
 if not ok then stageMark("stockChatError",err) end
 if type(msg)=="string" and string.sub(msg,1,8)=="#LDRAFT#" then
  stageChats=stageChats+1
  stageMark("protocolCount",stageChats)
  stageMark("protocol."..stageChats,tostring(fromPlayer).." | "..msg)
  stageMark("lastProtocol",tostring(fromPlayer).." | "..msg)
  stageMark("lockedAfterChat",Draft_IsDraftLocked())
 end
end
Events.GameMessageChat.Add(OnChat)
