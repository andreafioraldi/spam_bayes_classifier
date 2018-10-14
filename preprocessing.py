#!/usr/bin/env python

import random
import re

to_remove = ["the", "a"]

random.seed()

with open("./dataset/SMSSpamCollection") as f:
    dataset = f.readlines()

dataset = set(map(lambda x: ",".join(filter(lambda x: x not in to_remove, re.sub("[^ \t\w\d]+", "", x.strip().lower()).split())), dataset))

validation = random.sample(dataset, len(dataset) / 3)
training = dataset.difference(validation)

with open("validation.csv", "w") as f:
    map(lambda x: f.write(x + "\n"), validation)
with open("training.csv", "w") as f:
    map(lambda x: f.write(x + "\n"), training)
