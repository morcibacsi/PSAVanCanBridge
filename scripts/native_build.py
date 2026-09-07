Import("env")

import os
import subprocess


def load_visual_studio_environment():
    if os.name != "nt":
        return

    vswhere = os.path.join(
        os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
        "Microsoft Visual Studio",
        "Installer",
        "vswhere.exe",
    )
    if not os.path.isfile(vswhere):
        return

    installation = subprocess.check_output(
        [
            vswhere,
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-property",
            "installationPath",
        ],
        text=True,
    ).strip()
    if not installation:
        return

    vcvars = os.path.join(installation, "VC", "Auxiliary", "Build", "vcvars64.bat")
    output = subprocess.check_output(
        'call "{}" -vcvars_ver=14.44 >nul && set'.format(vcvars),
        text=True,
        shell=True,
    )
    for line in output.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            env["ENV"][key] = value


load_visual_studio_environment()

env.PrependENVPath("PATH", os.path.join(env.subst("$PROJECT_DIR"), "tools", "native"))

# PlatformIO's native platform defaults to GNU tools. This repository's Windows
# workflow also supports the LLVM toolchain installed by Visual Studio/LLVM.
env.Replace(
    CC="clang",
    CXX="clang++",
    AR="llvm-ar",
    RANLIB="llvm-ranlib",
    LINK="clang++",
)

# The native platform loads SCons' GNU tools before post scripts; some Windows
# SCons versions bake the executable name into command templates.
for command_name, old, new in (
    ("CCCOM", "gcc", "clang"),
    ("CXXCOM", "g++", "clang++"),
    ("LINKCOM", "g++", "clang++"),
    ("ARCOM", "ar", "llvm-ar"),
):
    command = env.get(command_name)
    if isinstance(command, str):
        env.Replace(**{command_name: command.replace(old, new, 1)})
