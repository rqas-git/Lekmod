// Compiled with the production launcher declarations by check_launcher_lifecycle.py.
func require(_ condition: @autoclosure () -> Bool, _ message: String) {
    if !condition() { fatalError(message) }
}

func model(launched: Bool = false) -> LauncherModel {
    let model = LauncherModel()
    let event: [String: Any] = [
        "event": "result", "app": "/game/Civilization V.app", "version": "test",
        "crossplay": true, "checks": [], "ready": true, "repairable": true,
        "running": false, "launched": launched,
    ]
    var data = try! JSONSerialization.data(withJSONObject: event)
    data.append(10)
    model.receive(data)
    return model
}

for launched in [false, true] {
    let launcher = model(launched: launched)
    require(!launcher.observeGame(true), "Game startup must not request a full scan")
    require(launcher.report?.running == true, "Both external and launcher starts must show running")
    require(launcher.report?.launched == false, "The pending launch must be cleared")
    require(launcher.report?.ready == false && launcher.report?.repairable == false,
            "A running game cannot launch or repair")
    require(launcher.buttonTitle == "Game is running", "Running controls must update")
    require(!launcher.observeGame(true), "Repeated process probes must not rescan")
    require(launcher.observeGame(false), "Game exit must request fresh validation")
}

let idle = model()
require(!idle.observeGame(false), "An idle launcher must not repeatedly scan")
idle.busy = true
require(!idle.observeGame(true), "A stale process probe must not interrupt an action")
require(idle.report?.running == false, "Busy state must ignore stale probes")

let pending = model(launched: true)
require(!pending.observeGame(false), "Steam must retain its launch grace period")
require(pending.gameActive, "Controls must stay blocked during the launch grace period")
require(pending.observeGame(false, at: Date().addingTimeInterval(16)),
        "A failed Steam launch must recover after the grace period")
require(!LauncherModel().observeGame(true), "No process transition without an initial report")
print("Launcher lifecycle checks passed")
