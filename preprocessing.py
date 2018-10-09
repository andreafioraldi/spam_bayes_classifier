#!/usr/bin/env python

import random

to_remove = ["the", "a", "of", ":", ".", "!", "-"]

random.seed()

with open("./dataset/SMSSpamCollection") as f:
    dataset = f.readlines()

dataset = set(map(lambda x: ",".join(filter(lambda x: x not in to_remove, x.replace(",", "").strip().split())), dataset))

validation = random.sample(dataset, len(dataset) / 3)
training = dataset.difference(validation)

with open("validation.csv", "w") as f:
    map(lambda x: f.write(x + "\n"), validation)
with open("training.csv", "w") as f:
    map(lambda x: f.write(x + "\n"), training)
