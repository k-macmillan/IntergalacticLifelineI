import sys
import numpy as np
import pandas as pd
files = sys.argv[1:]
means = []
for fi in files:
    data = pd.read_csv( fi )
    print(type(data.mean(axis=0)))
    print(data.describe())
