def payload_files():
    header = bytearray(68)
    header[:2] = b'MZ'
    header[60:64] = (64).to_bytes(4, 'little')
    header[64:] = b'PE\0\0'
    return {
        'CvGameCore_Expansion2.dll': bytes(header),
        'MPModsPack.Civ5Pkg': b'<Civ5Package><GUID>fixture</GUID></Civ5Package>',
        'Override/CIV5Units.xml': b'<GameData><Units><Row><Type>UNIT_FIXTURE</Type></Row></Units></GameData>',
        'Lua/UI/FrontEnd.lua': b'local LEKMOD_UI_CHECK_DONE = false',
        'Lua/UI/InGame.lua': b'print("fixture")',
        'Lua/tmp/fixture.lua.ignore': b'print("fixture")',
    }
