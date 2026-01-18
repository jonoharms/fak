import json
import sys

with open(sys.argv[1], 'r') as f:
    data = json.load(f)

if 'defines' in data:
    for key, value in data['defines'].items():
        print(f'-D{key}={value}')
