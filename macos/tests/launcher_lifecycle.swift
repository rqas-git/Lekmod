
func require(_ condition: @autoclosure () -> Bool, _ message: String) {
    if !condition() { fatalError(message) }
}

func model(launched: Bool = false) -> LauncherModel {
    let model = LauncherModel()
    let event: [String: Any] = [
        "event": "result", "app": "/game/Civilization V.app", "version": "test",
        "core_sha256": "abc123", "crossplay": true,
        "checks": [["id": "core", "title": "Native Lekmod library", "state": "ok",
                     "detail": "Installed library recognized"]],
        "ready": true, "repairable": true, "running": false, "launched": launched,
        "eui_enabled": false, "lekmod_installed": true, "lekmap_installed": true,
        "steam_session": ["state": "ok", "label": "Logged in"],
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
let diagnostics = model().diagnostics(at: Date(timeIntervalSince1970: 0))
require(diagnostics.contains("Native library SHA-256: abc123"), "Diagnostics must contain the inspected core hash")
require(diagnostics.contains("UI: Standard"), "Diagnostics must show the selected UI")
require(diagnostics.contains("[ok] Native Lekmod library: Installed library recognized"),
        "Diagnostics must contain the actual check results")
require(diagnostics.contains("Generated: 1970-01-01T00:00:00Z"), "Diagnostics must be timestamped")
require(LauncherModel().diagnostics().contains("No installation report"),
        "Diagnostics must explain when Check has not run")
print("Launcher lifecycle checks passed")
