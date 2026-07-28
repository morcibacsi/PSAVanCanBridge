#!/usr/bin/env python3
"""Create a release-ready firmware archive from PlatformIO build outputs."""

from __future__ import annotations

import argparse
import configparser
import datetime as dt
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import zipfile


RELEASE_ENVIRONMENTS = (
    "esp32_v13_can",
    "esp32_v14_can",
    "esp32_v15_can",
    "esp32_v15_van_ulp",
    "esp32c6_v16",
)
VERSION_RE = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+(?:-(?:alpha|beta|rc)(?:[.-]?[0-9A-Za-z.-]+)?)?$")
FLASH_ARGUMENT_RE = re.compile(
    r"(0x[0-9A-Fa-f]+)\s+\$BUILD_DIR[/\\]([A-Za-z0-9_.-]+\.bin)"
)


def repository_root() -> Path:
    return Path(__file__).resolve().parent.parent


def platformio_config(root: Path) -> configparser.ConfigParser:
    config = configparser.ConfigParser(interpolation=None)
    config.read(root / "platformio.ini", encoding="utf-8")
    return config


def flash_layout(config: configparser.ConfigParser, environment: str) -> list[tuple[str, str]]:
    section = f"env:{environment}"
    if not config.has_section(section):
        raise ValueError(f"PlatformIO environment does not exist: {environment}")
    command = config.get(section, "upload_command", fallback="")
    layout = [(filename, offset.lower()) for offset, filename in FLASH_ARGUMENT_RE.findall(command)]
    if not layout:
        raise ValueError(f"No flash files/offsets found in [{section}] upload_command")
    return layout


def git_value(root: Path, *arguments: str) -> str:
    return subprocess.check_output(
        ["git", *arguments], cwd=root, text=True, encoding="utf-8"
    ).strip()


def build_time_utc(root: Path) -> dt.datetime:
    source_date_epoch = os.environ.get("SOURCE_DATE_EPOCH")
    if source_date_epoch:
        return dt.datetime.fromtimestamp(int(source_date_epoch), tz=dt.timezone.utc)
    return dt.datetime.now(tz=dt.timezone.utc)


def metadata_text(
    version: str,
    commit_sha: str,
    environment: str,
    build_time: dt.datetime,
    layout: list[tuple[str, str]],
) -> str:
    file_lines = "\n".join(f"  {offset}  {filename}" for filename, offset in layout)
    flash_args = " ".join(f"{offset} {filename}" for filename, offset in layout)
    return (
        "PSA VAN-CAN Bridge firmware package\n"
        f"Release version: {version}\n"
        f"Git commit SHA: {commit_sha}\n"
        f"PlatformIO environment: {environment}\n"
        f"Build date (UTC): {build_time.strftime('%Y-%m-%dT%H:%M:%SZ')}\n"
        "\nFlash layout:\n"
        f"{file_lines}\n"
        "\nBasic flashing command (replace CHIP and PORT for your board):\n"
        "  esptool --chip CHIP --port PORT --baud 921600 write-flash "
        f"{flash_args}\n"
        "\nSee wiki/update-firmware-from-a-release.md for installation details.\n"
        "For OTA updates, upload firmware.bin from the device setup page.\n"
    )


def zip_info(name: str, build_time: dt.datetime) -> zipfile.ZipInfo:
    timestamp = build_time.astimezone(dt.timezone.utc).replace(tzinfo=None)
    if timestamp.year < 1980:
        timestamp = dt.datetime(1980, 1, 1)
    info = zipfile.ZipInfo(name, timestamp.timetuple()[:6])
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = 0o100644 << 16
    return info


def package(environment: str, version: str, output_dir: Path, commit_sha: str) -> Path:
    if environment not in RELEASE_ENVIRONMENTS:
        raise ValueError(
            f"Unsupported release environment {environment!r}; "
            f"expected one of: {', '.join(RELEASE_ENVIRONMENTS)}"
        )
    if not VERSION_RE.fullmatch(version):
        raise ValueError(
            f"Invalid release version {version!r}; expected 1.2.3 or a supported "
            "prerelease such as 1.2.3-rc.1"
        )

    root = repository_root()
    layout = flash_layout(platformio_config(root), environment)
    build_dir = root / ".pio" / "build" / environment
    missing = [filename for filename, _ in layout if not (build_dir / filename).is_file()]
    if missing:
        raise FileNotFoundError(
            f"Missing required build output(s) for {environment}: {', '.join(missing)}"
        )

    build_time = build_time_utc(root)
    archive_name = f"psa_van_can_bridge_{environment}_v{version}.zip"
    output_dir.mkdir(parents=True, exist_ok=True)
    archive_path = output_dir / archive_name
    metadata = metadata_text(version, commit_sha, environment, build_time, layout)

    with zipfile.ZipFile(archive_path, "w") as archive:
        for filename, _ in layout:
            archive.writestr(zip_info(filename, build_time), (build_dir / filename).read_bytes())
        archive.writestr(zip_info("FLASHING.txt", build_time), metadata.encode("utf-8"))

    print(archive_path.resolve())
    return archive_path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--environment", choices=RELEASE_ENVIRONMENTS)
    parser.add_argument("--version")
    parser.add_argument("--output-dir", type=Path, default=repository_root() / "firmware")
    parser.add_argument("--commit-sha")
    parser.add_argument(
        "--list-environments",
        action="store_true",
        help="print the release environment list as JSON",
    )
    args = parser.parse_args()

    if args.list_environments:
        print(json.dumps(RELEASE_ENVIRONMENTS))
        return 0
    if not args.environment or not args.version:
        parser.error("--environment and --version are required unless --list-environments is used")

    root = repository_root()
    commit_sha = args.commit_sha or git_value(root, "rev-parse", "HEAD")
    try:
        package(args.environment, args.version.removeprefix("v"), args.output_dir, commit_sha)
    except (FileNotFoundError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
