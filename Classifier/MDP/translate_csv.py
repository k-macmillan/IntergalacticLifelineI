import pandas as pd
from pprint import pprint


def translate(fname):
    data = pd.read_csv(fname)
    time_by_unit = {}
    for _, entry in data.iterrows():
        key = (entry[1], entry[2])
        time_by_unit[key] = time_by_unit.get(key, []) + [entry[0]]
    return time_by_unit