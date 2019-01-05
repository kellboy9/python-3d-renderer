import os
__all__ = []
files = os.listdir('./modules')
for file in files:
    if file.endswith(".py") or file.endswith(".pyc"):
        __all__.append(os.path.splitext(file)[0])
