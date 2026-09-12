
import io
from pathlib import Path
import subprocess
import tarfile

ROOT = Path(__file__).resolve().parents[1]
BASELINE = '1bc2ff42'


def original(path, revision=BASELINE):
    return subprocess.check_output(['git', 'show', f'{revision}:{path}'], cwd=ROOT)


def extract_lua(destination, revision=BASELINE):
    archive = subprocess.check_output(['git', 'archive', revision, 'LEKMOD/Lua'], cwd=ROOT)
    with tarfile.open(fileobj=io.BytesIO(archive)) as files:
        for member in files:
            if member.isfile():
                path = Path(destination) / member.name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(files.extractfile(member).read())
