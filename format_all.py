import pathlib
from c_formatter_42.run import run_all

for p in pathlib.Path(".").rglob("*"):
    if p.suffix in (".c", ".h"):
        content = p.read_text(encoding="utf-8")
        p.write_text(run_all(content), encoding="utf-8")
        print("OK:", p)