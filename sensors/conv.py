#!/usr/bin/env python

import re,sys

def bits(i):
    while i != 0:
        yield i & 1
        i = i >> 1

def raw_to_degrees(raw):
    i = int(raw, 16)

    sign = (i & 0xF800) >> 11
    if sign == 0:
        value = i & 0x7FF
        sgn = 1
    elif sign == 0x1F:
        value = (~i + 1) & 0x7FF
        sgn = -1
    else:
        raise Exception("Malformed value")

    # print 'bits\t=\t', '\t'.join(map(str, bits(value)))
    # print 'coeff\t=', '\t'.join([str(pow(2,x)) for x in range(-4, 7)])

    return sgn * sum([y * pow(2, x)
                      for (x,y) in zip(range(-4, 7),
                                       bits(value))])
    
def main():
    print raw_to_degrees(sys.argv[1])

if __name__=="__main__":
    main()
