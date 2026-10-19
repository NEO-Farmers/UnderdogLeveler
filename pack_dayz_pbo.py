#!/usr/bin/env python3
"""
Correct DayZ Standalone PBO packer (reverse-engineered from a known-good PBO).

DayZ PBO layout (little-endian):
    1. "Vers" product entry (21 bytes): 0x00 + b"sreV" + 16 zero bytes
    2. Prefix product entry: b"product\0" + b"dayz ugc\0" + b"prefix\0" + <prefix>\0
    3. 0x00 terminator
    4. File entries, each: <name>\0 + 5 x uint32 (packing, origSize, reserved, timestamp, dataSize)
       where packing=0, origSize=0, reserved=0
    5. 0x00 terminator
    6. File data (concatenated, uncompressed)
    7. Footer: 0x00 + SHA-1 of everything before this footer (21 bytes total)
"""
import os
import sys
import struct
import hashlib

EXCLUDE_NAMES = {"README.md", "mod.cpp", "meta.cpp", "_unpbo_manifest.json"}
EXCLUDE_EXTS = {".md", ".pyc", ".pyo", ".bak", ".pbo"}


def pack_pbo(src_dir, out_path, prefix):
    files = []
    for root, dirs, names in os.walk(src_dir):
        dirs[:] = [d for d in dirs if d not in ("__pycache__", ".git")]
        for name in names:
            ext = os.path.splitext(name)[1].lower()
            if name in EXCLUDE_NAMES or ext in EXCLUDE_EXTS:
                continue
            full = os.path.join(root, name)
            rel = os.path.relpath(full, src_dir).replace("/", "\\")
            files.append((rel, full))
    files.sort(key=lambda x: x[0].lower())

    buf = bytearray()

    # 1. Vers entry
    buf += b"\x00" + b"sreV" + b"\x00" * 16

    # 2. Prefix product entry
    buf += b"product\x00"
    buf += b"dayz ugc\x00"
    buf += b"prefix\x00"
    buf += prefix.encode("utf-8") + b"\x00"

    # 3. terminator
    buf += b"\x00"

    # 4. file entries
    contents = []
    for rel, full in files:
        with open(full, "rb") as f:
            content = f.read()
        contents.append(content)
        buf += rel.encode("utf-8") + b"\x00"
        buf += struct.pack("<IIIII", 0, 0, 0, int(os.path.getmtime(full)), len(content))

    # 5. terminating entry: empty filename + 5 zero fields (20 bytes)
    buf += b"\x00" + b"\x00" * 20

    # 6. file data
    for content in contents:
        buf += content

    # 7. footer: 0x00 + SHA1 of everything above
    digest = hashlib.sha1(bytes(buf)).digest()
    buf += b"\x00" + digest

    with open(out_path, "wb") as f:
        f.write(bytes(buf))

    return len(buf), len(files)


def parse_pbo(path):
    data = open(path, "rb").read()

    def read_cstr(pos):
        end = data.index(b"\x00", pos)
        return data[pos:end].decode("utf-8", "replace"), end + 1

    pos = 0
    assert data[pos] == 0, "missing Vers filename terminator"
    assert data[pos + 1:pos + 5] == b"sreV", "bad Vers magic"
    pos += 21

    product, pos = read_cstr(pos)
    ugc, pos = read_cstr(pos)
    ptype, pos = read_cstr(pos)
    prefix, pos = read_cstr(pos)
    assert product == "product" and ptype == "prefix", "bad prefix product entry"

    assert data[pos] == 0, "missing product-section terminator"
    pos += 1

    files = []
    while data[pos] != 0:
        name, pos = read_cstr(pos)
        packing, orig, reserved, ts, dsize = struct.unpack("<IIIII", data[pos:pos + 20])
        pos += 20
        files.append((name, packing, orig, reserved, ts, dsize))

    # terminating entry: empty filename byte + 5 zero fields (20 bytes)
    assert data[pos] == 0, "missing terminating entry filename"
    assert data[pos + 1:pos + 21] == b"\x00" * 20, "terminating entry fields must be zero"
    pos += 21

    data_start = pos
    body_len = len(data) - 21
    assert data[body_len] == 0, "missing footer null"
    stored = data[body_len + 1:]
    calc = hashlib.sha1(data[:body_len]).digest()
    ok = stored == calc
    return prefix, files, data_start, ok


if __name__ == "__main__":
    here = os.path.dirname(os.path.abspath(__file__))
    ref = os.path.join(here, "SF_Leaderboard.pbo")

    print("== Verify reference PBO ==")
    prefix, files, data_start, ok = parse_pbo(ref)
    print(f"prefix={prefix!r} files={len(files)} data_start={data_start} sha1_ok={ok}")
    for f in files[:4]:
        print("   ", f)

    src = os.path.join(here, "UnderdogLeveler")
    out = os.path.join(here, "UnderdogLeveler.pbo")
    size, count = pack_pbo(src, out, "UnderdogLeveler")
    print(f"\n== Packed {out} ==")
    print(f"size={size} bytes, files={count}")

    print("== Verify packed PBO ==")
    prefix2, files2, data_start2, ok2 = parse_pbo(out)
    print(f"prefix={prefix2!r} files={len(files2)} data_start={data_start2} sha1_ok={ok2}")
    for f in files2:
        print("   ", f)
    assert ok2, "PACKED PBO SHA1 FAILED"
    assert prefix2 == "UnderdogLeveler"
    print("\nALL CHECKS PASSED")
