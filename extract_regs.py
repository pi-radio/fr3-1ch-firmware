#!/usr/bin/env python3

import click

@click.command()
@click.argument('file', type=click.Path(exists=True))
def extract_regs(file):
    rv = {}
    
    with open(file, 'r') as f:
        for l in f:
            rn, v = l.split()

            rn = int(rn[1:])

            v = int(v, 16)

            assert (v >> 16) == rn

            v = v & 0xFFFF

            rv[rn] = v

    for i in range(max(rv.keys())+1):
        print(f"  0x{rv[i]:04x},")

            

extract_regs()
