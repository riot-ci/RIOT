#!/usr/bin/env python3

import sys
import secrets

if len(sys.argv) >= 2:
    maxbits = int(sys.argv[1])
else:
    maxbits = 64

maxval = 2 ** maxbits
print(hex(secrets.randbelow(maxval)))
