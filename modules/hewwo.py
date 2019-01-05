import math
import json
import main
import os
import config

li = [2, 4, 5, 6, 3, 1]
li.sort()
print li

print json.dumps([{"hewwo": "hewwo", "mistew": "mistew", "pwesident": "pwesident"}]);
print math.sin(math.pi * 3 / 4) ** 2

main.printHewwo(config.globvar, 5)
