#!/usr/bin/env python3

"""Compare the renderer's 8-bit P3 PPM output using only the standard library."""

import argparse
import math
import sys
from itertools import zip_longest
from pathlib import Path
from typing import Iterator, TextIO


def tokens(stream: TextIO) -> Iterator[str]:
    for line in stream:
        yield from line.split("#", 1)[0].split()


def integer(token: str | None, path: Path) -> int:
    if token is None or not token.isascii() or not token.isdecimal():
        raise ValueError(f"{path}: missing or invalid integer")
    return int(token)


def header(values: Iterator[str], path: Path) -> tuple[int, int]:
    if next(values, None) != "P3":
        raise ValueError(f"{path}: expected a P3 PPM file")
    width = integer(next(values, None), path)
    height = integer(next(values, None), path)
    maximum = integer(next(values, None), path)
    if width <= 0 or height <= 0:
        raise ValueError(f"{path}: image dimensions must be positive")
    if maximum != 255:
        raise ValueError(f"{path}: expected maximum channel value 255")
    return width, height


def compare(reference: Path, candidate: Path) -> tuple[tuple[int, int], float, float, int, int]:
    with reference.open(encoding="ascii") as ref_file, candidate.open(encoding="ascii") as out_file:
        ref_values = tokens(ref_file)
        out_values = tokens(out_file)
        dimensions = header(ref_values, reference)
        if header(out_values, candidate) != dimensions:
            raise ValueError("image dimensions do not match")
        expected = 3 * dimensions[0] * dimensions[1]
        count = 0
        # Eight-bit channels have only 256 possible absolute errors. Accumulate
        # their frequencies instead of updating all statistics per channel.
        errors = [0] * 256
        for ref_token, out_token in zip_longest(ref_values, out_values):
            if count >= expected:
                raise ValueError("extra pixel data after the expected image")
            # Files are decoded as ASCII; channel parsing needs no Unicode check.
            if ref_token is None or out_token is None:
                raise ValueError("incomplete image: channel counts do not match")
            if not ref_token.isdecimal() or not out_token.isdecimal():
                raise ValueError("channel values must be nonnegative integers")
            ref_value = int(ref_token)
            out_value = int(out_token)
            if not (0 <= ref_value <= 255 and 0 <= out_value <= 255):
                raise ValueError("channel values must be within [0, 255]")
            count += 1
            if ref_value != out_value:
                errors[abs(ref_value - out_value)] += 1
        if count != expected:
            raise ValueError(f"incomplete image: expected {expected} channels, got {count}")
    squared_error = sum(error * error * frequency for error, frequency in enumerate(errors))
    different = sum(errors)
    maximum_error = next((error for error in range(255, 0, -1) if errors[error]), 0)
    mse = squared_error / expected
    psnr = math.inf if mse == 0 else 10 * math.log10(255 * 255 / mse)
    return dimensions, mse, psnr, different, maximum_error


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("reference", type=Path, help="baseline P3 PPM")
    parser.add_argument("candidate", type=Path, help="rendered P3 PPM")
    parser.add_argument("--threshold", type=float, default=45.0, help="minimum PSNR in dB (default: 45)")
    args = parser.parse_args()
    if not math.isfinite(args.threshold) or args.threshold <= 0:
        parser.error("threshold must be a finite positive number")
    try:
        dimensions, mse, psnr, different, maximum_error = compare(args.reference, args.candidate)
    except (OSError, UnicodeError, ValueError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 2
    passed = psnr >= args.threshold
    print(f"Image: {dimensions[0]} x {dimensions[1]}")
    print(f"MSE: {mse:.6f}; RMSE: {math.sqrt(mse):.6f}")
    print(f"Different channels: {different}; maximum channel error: {maximum_error}")
    print(f"PSNR: {psnr:.6f} dB; threshold: {args.threshold:g} dB")
    print("PASS" if passed else "FAIL")
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
