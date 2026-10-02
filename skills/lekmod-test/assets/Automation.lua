-- This native startup state exposes Automation and Events, not gameplay APIs.
Automation.SetGameCoreInit({worldSize=2, climate=2, seaLevel=0, era=0,
    autorunTurnLimit=__TURN_TARGET__, autorunTurnDelay=0.1});
print("LEKMOD_VALIDATE starting native quick-play; target=__TURN_TARGET__");
Events.SerialEventStartGame(0);
