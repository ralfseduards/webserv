#!/usr/bin/python3

from datetime import datetime

date = datetime.today().strftime("%Y-%m-%d %H:%M:%S")

print("Content-Type: text/html\n")
print("<html><body>", date, "</body></html>")
