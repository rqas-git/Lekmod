for index=1,#arg do
 local f=assert(io.open(arg[index]));local source=f:read('*a');f:close()
 local diagnostic=assert(source:match('( mpMark%("tickTime".-)\n UI.SetDontShowPopups'))
 local processing=true;local active=true;local marks={}
 local env={os={time=function()return 123 end},Game={IsProcessingMessages=function()return processing end},Network={HasSentNetTurnComplete=function()error('network getter can block the Windows UI thread')end},p={IsTurnActive=function()return active end,IsAlive=function()return true end,GetEndTurnBlockingType=function()return -1 end},mpMark=function(k,v)marks[k]=v end}
 local chunk=assert(loadstring(diagnostic));setfenv(chunk,env)
 chunk();assert(marks.processing==true and marks.turnActive==true)
 processing=false;active=false;chunk();assert(marks.turnActive==false)
 active=true;chunk();assert(marks.processing==false and marks.turnActive==true)
end
print('multiplayer diagnostics avoid the blocking network getter')
