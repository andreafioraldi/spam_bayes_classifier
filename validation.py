#!/usr/bin/env python

import sys

if len(sys.argv) < 2:
    print "usage: validation.py <bernoulli|multinomial>"
    exit()

def naive_bayes_classify_text(d, Pc, Pwc):
    m = 0
    am = None #fail by default
    for c in Pc:
        def get_valids():
            for x in d:
                try: yield Pwc[(x, c)]
                except KeyError: pass
        try:
            r = Pc[c]*reduce(lambda x, y: x*y, list(get_valids()))
            if r > m:
                m = r
                am = c
        except: continue
    return am

with open("validation.csv") as f:
    validation = map(lambda x: x.strip().split(","), filter(lambda x: len(x) > 0, f.readlines()))

with open("%s_Pc.csv" % sys.argv[1]) as f:
    Pc_list = map(lambda x: x.strip().split(","), filter(lambda x: len(x) > 0, f.readlines()))
with open("%s_Pwc.csv" % sys.argv[1]) as f:
    Pwc_list = map(lambda x: x.strip().split(","), filter(lambda x: len(x) > 0, f.readlines()))

Pc = {}
for x in Pc_list:
    Pc[x[0]] = float(x[1])

Pwc = {}
for x in Pwc_list:
    Pwc[(x[0], x[1])] = float(x[2])
    if float(x[2]) > 1:
        print x

passed = 0
failed = 0

for test in validation:
    r = naive_bayes_classify_text(test[1:], Pc, Pwc)
    if r != test[0]:
        failed += 1
    else:
        passed += 1

print "passed:", passed
print "failed:", failed
print "rate: %d%%" % (passed * 100 / len(validation))
