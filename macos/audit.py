#!/usr/bin/env python3

from pathlib import Path
import struct

DEFAULT_APP = Path.home() / "Library/Application Support/Steam/steamapps/common/Sid Meier's Civilization V/Civilization V.app"


def read_macho(path):
    data = Path(path).read_bytes()
    try:
        if struct.unpack_from('<I', data)[0] != 0xfeedfacf:
            raise RuntimeError('Expected a thin 64-bit Mach-O binary')
        segments = []
        symbol_table = None
        offset = 32
        for _ in range(struct.unpack_from('<I', data, 16)[0]):
            command, size = struct.unpack_from('<II', data, offset)
            if size < 8 or offset + size > len(data):
                raise RuntimeError('Invalid Mach-O load command')
            if command == 0x19:
                vmaddr, _, fileoff, filesize = struct.unpack_from('<QQQQ', data, offset + 24)
                segments.append((vmaddr, fileoff, filesize))
            elif command == 2:
                symbol_table = struct.unpack_from('<IIII', data, offset + 8)
            offset += size
        if symbol_table is None:
            raise RuntimeError('Mach-O symbol table is missing')
        symoff, count, stroff, strsize = symbol_table
        if symoff + count * 16 > len(data) or stroff + strsize > len(data):
            raise RuntimeError('Invalid Mach-O symbol table')
        strings = data[stroff:stroff + strsize]
        symbols, exports, imports = {}, set(), []
        for index in range(count):
            name_offset, kind, _, description, address = struct.unpack_from('<IBBHQ', data, symoff + index * 16)
            if kind & 0xe0 or not name_offset:
                continue
            end = strings.find(b'\0', name_offset)
            if end < 0:
                raise RuntimeError('Invalid Mach-O symbol name')
            name = strings[name_offset:end].decode('utf-8')
            undefined = kind & 0x0e == 0
            if not undefined:
                symbols[name] = address
                if kind & 1:
                    exports.add(name)
            elif description >> 8 == 0xfe:
                imports.append(name)
        return data, symbols, exports, imports, segments
    except (struct.error, UnicodeError) as error:
        raise RuntimeError(f'Invalid Mach-O binary: {path}') from error


def check_imports(library, app=DEFAULT_APP):
    host = app / "Contents/MacOS/Civilization V"
    exports = read_macho(host)[2]
    parsed = read_macho(library)
    imports = parsed[3]
    missing = sorted(set(imports) - exports)
    if missing:
        raise RuntimeError("Imports absent from the Mac host:\n" + '\n'.join(missing))
    print(f"Verified {len(imports)} engine imports against {host}")
    check_pregame_abi(library, parsed)
    return imports


def check_pregame_abi(library, parsed=None):
    data, symbols, _, _, segments = parsed or read_macho(library)
    address = symbols['__ZTV12CvDllPreGame'] + 16
    table = next(fileoff + address - vmaddr for vmaddr, fileoff, filesize in segments
                 if vmaddr <= address < vmaddr + filesize)
    expected = {
        28: '__ZN12CvDllPreGame6eraKeyEv',
        29: '__ZN12CvDllPreGame20findPlayerByNicknameEPKc',
        185: '__ZN12CvDllPreGame16setVersionStringERKNSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE',
        197: '__ZN12CvDllPreGame13versionStringEv',
        200: '__ZN12CvDllPreGame5writeER11FDataStream',
        217: '__ZN12CvDllPreGame22ReseatConnectedPlayersEv',
    }
    for slot, name in expected.items():
        actual = struct.unpack_from('<Q', data, table + slot * 8)[0]
        if actual != symbols.get(name):
            raise RuntimeError(f'Mac pre-game ABI mismatch at slot {slot}: expected {name}')
    print(f'Verified {len(expected)} Mac pre-game vtable anchors')


if __name__ == "__main__":
    check_imports(Path(__file__).parent / "build/libCvGameCoreDLL_Expansion2_DLL.dylib")
