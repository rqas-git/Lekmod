import sys

import launcher
import saves


def main():
    commands = {'launcher': launcher.main, 'saves': saves.main}
    if len(sys.argv) < 2 or sys.argv[1] not in commands:
        raise SystemExit('Expected launcher or saves command')
    command = sys.argv.pop(1)
    return commands[command]()


if __name__ == '__main__':
    raise SystemExit(main())
