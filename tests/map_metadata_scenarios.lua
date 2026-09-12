
assert(_VERSION == 'Lua 5.1')
local directory = assert(arg[1])
local function serialize(value)
    if type(value) == 'string' then return string.format('%q', value) end
    if type(value) ~= 'table' then return tostring(value) end
    local keys, parts = {}, {}
    for key in pairs(value) do keys[#keys + 1] = key end
    table.sort(keys, function(a, b) return tostring(a) < tostring(b) end)
    for _, key in ipairs(keys) do parts[#parts + 1] = serialize(key)..'='..serialize(value[key]) end
    return '{'..table.concat(parts, ',')..'}'
end
local seen = {}
local function assertFresh(value)
    if type(value) ~= 'table' then return end
    assert(not seen[value], 'Menu metadata shares a mutable table')
    seen[value] = true
    for _, child in pairs(value) do assertFresh(child) end
end
for _, name in ipairs({
    'LekmapArchipelagoLegacy', 'LekmapContinentsLegacy', 'LekmapDonutLegacy',
    'LekmapFourCornersv1.2', 'LekmapInlandSeaLegacy', 'LekmapLakesLegacy',
    'LekmapOvalLegacy', 'LekmapPangaeaFractalv6.2', 'LekmapPangaeaRectangularv4',
    'LekmapSmallContinentsLegacy', 'LekmapTeamerMapLegacy', 'LekmapTinyIslandsLegacy',
}) do
    local env = setmetatable({AssignStartingPlots={}, MultilayeredFractal={},
        FractalWorld={}, TerrainGenerator={}, FeatureGenerator={}}, {__index=_G})
    local coreCalls = 0
    env.GetCoreMapOptions = function()
        coreCalls = coreCalls + 1
        return {}, {}, {}, {}, {}
    end
    env.include = function(module)
        if module == 'HBMapOptions' then
            setfenv(assert(loadfile(directory..'/'..module..'.lua')), env)()
        end
    end
    setfenv(assert(loadfile(directory..'/'..name..'.lua')), env)()
    local info = env.GetMapScriptInfo()
    local expected = serialize(info)
    assertFresh(info)
    for _, option in ipairs(info.CustomOptions) do
        option.Values[1], option.DefaultValue = 'mutated', -1
    end
    info.Name = 'mutated'
    local again = env.GetMapScriptInfo()
    assertFresh(again)
    assert(serialize(again) == expected, name..' retained a previous menu edit')
    assert(coreCalls == 2, name..' changed calls to GetCoreMapOptions')
    print(name..':'..expected)
end
