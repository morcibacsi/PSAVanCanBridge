#!/usr/bin/env python3
"""Build and package one or all supported firmware environments."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

from package_firmware import RELEASE_ENVIRONMENTS, VERSION_RE, package, repository_root


def normalized_version(value: str) -> str:
    version = value.removeprefix("v")
    if not VERSION_RE.fullmatch(version):
        raise ValueError(
            f"Invalid release version {value!r}; expected 1.2.3 or a supported "
            "prerelease such as 1.2.3-rc.1"
        )
    return version


def platformio_command() -> str:
    for command in ("pio", "platformio"):
        executable = shutil.which(command)
        if executable:
            return executable

    windows_fallback = Path(r"C:\.platformio\penv\Scripts\pio.exe")
    if windows_fallback.is_file():
        return str(windows_fallback)
    raise FileNotFoundError(
        "PlatformIO was not found. Install PlatformIO Core or add pio to PATH."
    )


def choose_environment() -> str:
    print("Supported firmware environments:")
    for index, environment in enumerate(RELEASE_ENVIRONMENTS, start=1):
        print(f"  {index}. {environment}")

    while True:
        choice = input(f"Select an environment (1-{len(RELEASE_ENVIRONMENTS)}): ").strip()
        try:
            return RELEASE_ENVIRONMENTS[int(choice) - 1]
        except (ValueError, IndexError):
            print("Invalid selection.")


def build_and_package(
    environment: str,
    version: str,
    output_dir: Path,
    pio: str,
    commit_sha: str | None,
) -> Path:
    print(f"\nBuilding environment: {environment}")
    print(f"Firmware version: {version}")

    process_environment = os.environ.copy()
    process_environment["PLATFORMIO_BUILD_FLAGS"] = (
        f'-D FIRMWARE_VERSION=\\"{version}\\"'
    )
    subprocess.run(
        [pio, "run", "--environment", environment],
        cwd=repository_root(),
        env=process_environment,
        check=True,
    )
    return package(
        environment,
        version,
        output_dir,
        commit_sha or subprocess.check_output(
            ["git", "rev-parse", "HEAD"],
            cwd=repository_root(),
            text=True,
            encoding="utf-8",
        ).strip(),
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("environment", nargs="?", choices=RELEASE_ENVIRONMENTS)
    parser.add_argument("version", nargs="?")
    parser.add_argument(
        "--all",
        nargs="?",
        const="",
        metavar="VERSION",
        help="build every supported release environment",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=repository_root() / "firmware",
    )
    parser.add_argument("--commit-sha")
    args = parser.parse_args()

    if args.all is not None and args.environment:
        parser.error("environment cannot be combined with --all")

    environments = RELEASE_ENVIRONMENTS if args.all is not None else (
        args.environment or choose_environment(),
    )
    version_input = args.all or args.version or input("Enter the version number: ").strip()

    try:
        version = normalized_version(version_input)
        pio = platformio_command()
        for environment in environments:
            build_and_package(
                environment,
                version,
                args.output_dir,
                pio,
                args.commit_sha,
            )
    except (FileNotFoundError, ValueError, subprocess.CalledProcessError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
