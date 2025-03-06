#!/usr/bin/python3

import os

try:
    if (os.environ['TEST'] == 'true'):
        print(os.environ)
except:
    print("not in test environment")

# execute the date script
os.system("date.py");

# this should not work
os.system("ls")
