"""Build/run the real LP C routines with a host GPIO/cycle simulator.

Usage: python test/van_lp/run_tests.py (Clang plus a host C/C++ toolchain).
This tests protocol decisions; it does not emulate ESP32-C6 instruction timing.
"""
import os
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[2]
os.chdir(root)
if os.name == "nt":
    vswhere = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio/Installer/vswhere.exe"
    installation = subprocess.check_output([
        str(vswhere), "-latest", "-products", "*", "-requires",
        "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"
    ], text=True).strip()
    vcvars = Path(installation) / "VC/Auxiliary/Build/vcvars64.bat"
    output = subprocess.check_output(
        f'call "{vcvars}" -vcvars_ver=14.44 >nul && set', shell=True, text=True)
    for line in output.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            os.environ[key] = value
    bundled_clang = Path(installation) / "VC/Tools/Llvm/x64/bin"
    if (bundled_clang / "clang.exe").exists():
        os.environ["PATH"] = str(bundled_clang) + os.pathsep + os.environ["PATH"]

build = root / ".pio/van_lp_tests"
build.mkdir(parents=True, exist_ok=True)
subprocess.run(["clang", "-O2", "-Wall", "-Wextra", "-Itest/van_lp/stubs", "-c",
                "test/van_lp/lp_under_test.c", "-o", str(build / "lp.obj")], check=True)
subprocess.run(["clang++", "-std=c++17", "-O2", "-Wall", "-Wextra",
                "-D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH",
                "test/van_lp/test_van_lp.cpp", "src/Helpers/VanCrcCalculator.cpp",
                str(build / "lp.obj"),
                "-o", str(build / "test.exe")], check=True)
subprocess.run([str(build / "test.exe")], check=True)
