local file = assert(io.open(arg[1]))
local source = file:read('*a')
file:close()
local function loadFunction(name)
    local first = assert(source:find('local function '..name..'( flag )', 1, true))
    local last = assert(source:find('\n--==========================================================', first, true))
    return assert(loadstring(source:sub(first, last - 1)..'\nreturn '..name))()
end
local stale = setmetatable({}, {__index=function() error('removed unit was accessed') end})
local current, lookups = nil, 0
local player = {GetUnitByID=function(_, id)
    assert(id == 42)
    lookups = lookups + 1
    return current
end}
local flag = {m_Player=player, m_PlayerID=0, m_UnitID=42, m_Unit=stale}
local finish, update, destroy = loadFunction('FinishMove'), loadFunction('UpdateFlagType'), loadFunction('DestroyFlag')
finish(flag)
update(flag)
assert(lookups == 2)
local garrisonChecks, texture = 0, nil
current = {IsEmbarked=function() return false end, IsGarrisoned=function()
    garrisonChecks = garrisonChecks + 1
    return true
end, GetTransportUnit=function() return nil end, GetPlot=function() return nil end}
local control = setmetatable({}, {__index=function(_, method)
    return function(_, value) if method == 'SetTexture' then texture = value end end
end})
for _, name in ipairs({'UnitIconShadow', 'FlagShadow', 'FlagBase', 'FlagBaseOutline',
    'LightEffect', 'HealthBarBG', 'AlphaAnim', 'FlagHighlight', 'ScrollAnim'}) do flag[name] = control end
finish(flag)
update(flag)
assert(garrisonChecks == 1 and texture == 'UnitFlagBase.dds')
current = nil
flag.m_TransportUnit = stale
flag.Anchor = control
g_ScrapControls = {}
g_spareNewUnitFlags = {}
g_UnitFlags = {[0]={[42]=flag}}
table_insert = table.insert
destroy(flag)
assert(g_UnitFlags[0][42] == nil and g_spareNewUnitFlags[1] == flag)
assert(lookups == 5)
print('Removed-unit callbacks and live garrison updates passed')
