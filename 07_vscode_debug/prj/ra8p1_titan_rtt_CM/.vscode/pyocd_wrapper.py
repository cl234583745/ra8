import subprocess
import sys
import os

os.environ["PYTHONUNBUFFERED"] = "1"
cmd = [
    r"D:\Program Files\Python313\Scripts\pyocd.exe",
    *sys.argv[1:],
    r"--pack",
    r"D:\Program Files\pyocd\packs\Renesas.RA_DFP.6.1.0.pack",
]
p = subprocess.Popen(cmd, stdout=sys.stdout, stderr=subprocess.STDOUT)
sys.exit(p.wait())
