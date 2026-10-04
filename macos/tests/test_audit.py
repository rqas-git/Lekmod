from pathlib import Path
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from audit import read_macho


class MachOTests(unittest.TestCase):
    def binary(self):
        strings = b'\0_export\0_dynamic\0_system\0_debug\0'
        entries = [(1, 0x0f, 0, 123), (9, 1, 0xfe00, 0),
                   (18, 1, 0x0100, 0), (26, 0xe1, 0, 456)]
        symbols = b''.join(struct.pack('<IBBHQ', name, kind, 1, description, address)
                           for name, kind, description, address in entries)
        header = struct.pack('<IIIIIIII', 0xfeedfacf, 0x01000007, 3, 6, 1, 24, 0, 0)
        command = struct.pack('<IIIIII', 2, 24, 56, len(entries), 56 + len(symbols), len(strings))
        return header + command + symbols + strings

    def test_exports_dynamic_imports_and_debug_symbols_are_distinguished(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / 'core'
            path.write_bytes(self.binary())
            _, symbols, exports, imports, _ = read_macho(path)
            self.assertEqual(symbols, {'_export': 123})
            self.assertEqual(exports, {'_export'})
            self.assertEqual(imports, ['_dynamic'])

    def test_truncated_and_invalid_binaries_are_rejected(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / 'core'
            for data in (b'', b'not Mach-O', self.binary()[:-3]):
                with self.subTest(length=len(data)):
                    path.write_bytes(data)
                    with self.assertRaises(RuntimeError):
                        read_macho(path)
