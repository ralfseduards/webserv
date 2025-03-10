#!/usr/bin/python3

import os
import sys

print("This is the whole environment: ")

for name, value in os.environ.items():
    print(f"{name}: {value}")

print("----------END----------")

print()

print("This was passed as stdin: ")

input_data = sys.stdin.read()
print(input_data)

print("----------END----------")

