"""Give legacy Windows compilers an ASCII cwd without copying the project."""
from contextlib import contextmanager
from pathlib import Path
import os
import subprocess


@contextmanager
def compiler_root(root):
    root = Path(root)
    if os.name != 'nt' or str(root).isascii():
        yield root
        return
    drive = None
    for letter in 'RQPVUTS':
        candidate = letter + ':'
        if Path(candidate + '/').exists():
            continue
        result = subprocess.run(['subst', candidate, str(root)], capture_output=True)
        if result.returncode == 0:
            drive = candidate
            break
    if drive is None:
        raise RuntimeError('No free temporary drive for the compiler; use an ASCII project path.')
    try:
        yield Path(drive + '/')
    finally:
        subprocess.run(['subst', drive, '/d'], check=True, capture_output=True)
