import os, pandas
from pprint import pprint
from emtree import EMTree

datadir = "/home/dgfaelan/Documents/SeniorDesign/PvZ_1stDeath/"


def fromdict(other):
    build_order = dict(timing={}, count={})
    build_order['count'] = {k: v for k, v in other['count'].items()}
    build_order['timing'] = {k: v for k, v in other['timing'].items()}
    return build_order


def fromlist(series):
    build_order = dict(timing={}, count={})
    for time, typ, knd in series:
        n = build_order['count'][(typ, knd)] = build_order['count'].get((typ, knd), 0) + 1
        build_order['timing'][(typ, knd, n)] = time
    return build_order


def translate(filename):
    fromfile = pandas.read_csv(filename)
    build_order = dict(timing={}, count={})
    for _, entry in fromfile.iterrows():
        typ = int(entry[1])
        knd = int(entry[2])
        time = entry[0]
        n = build_order['count'][(typ, knd)] = build_order['count'].get((typ, knd), 0) + 1
        build_order['timing'][(typ, knd, n)] = time
    return build_order


def element_wise(a, b, op):
    for k, v in b['count'].items():
        a['count'][k] = op(a['count'].get(k, 0), v)
    for k, v in b['timing'].items():
        a['timing'][k] = op(a['timing'].get(k, 0), v)
    return a


def dist_(query, baseline):
    distance = 0
    keys = query['timing'].keys()
    for k in keys:
        if k in baseline['timing']:
            distance += abs(query['timing'][k] - baseline['timing'][k])
        else:
            distance += query['timing'][k]
    return distance


def wdist_(query, baseline, weights):
    valid = True
    distance = 0.0
    keys = query['timing'].keys()
    for k in keys:
        if k in baseline:
            distance += weights[k] * abs(query[k] - baseline[k])
        else:
            valid = False
    if not valid:
        distance += 1E8
    return distance


class S2ClassifierData(dict):
    def __init__(self, *, series=None, other=None, filename=None):
        self.fname = filename or "*internal*"
        if other is not None and isinstance(other, dict):
            super().__init__(fromdict(other))
        elif filename is not None and type(filename) is str:
            super().__init__(translate(filename))
        elif series is not None and type(series) is list:
            super().__init__(fromlist(series))
        else:
            super().__init__(timing={}, count={})

    def __add__(self, other):
        result = S2ClassifierData(other=self)
        return element_wise(result, other, lambda a, b: a + b)

    def __iadd__(self, other):
        return element_wise(self, other, lambda a, b: a + b)

    def __truediv__(self, other):
        result = S2ClassifierData(other=self)
        return element_wise(result, other, lambda a, b: a / b)

    def __itruediv__(self, other):
        return element_wise(self, other, lambda a, b: a / b)


def averager(succ, rec_ids, datalist):
    wrkspace = dict(sum=S2ClassifierData(), count=S2ClassifierData())
    if len(succ) > 0:
        for edge in succ.values():
            wrkspace['sum'] += edge['wrkspace']['sum']
            wrkspace['count'] += edge['wrkspace']['count']

    for rec in rec_ids:
        wrkspace['sum'] += datalist[rec]
        element_wise(wrkspace['count'], datalist[rec], lambda a, b: a + 1)

    element_wise(wrkspace['count'], wrkspace['count'], lambda a, b: 1 if a < 1E-6 else a)
    return wrkspace['sum'] / wrkspace['count'], wrkspace


def run():
    data = []
    counter = 0
    for f in os.listdir(datadir):
        counter += 1
        fname = os.path.join(datadir, f)
        record = S2ClassifierData(filename=fname)
        data.append(record)

    s2classifier = EMTree(data, dist=dist_, averager=averager, base_cluster_size=3, mways=3)
    s2classifier.train()

    with open("out.txt", "w") as f:
        for nbr in sorted(s2classifier.nodes()):
            out = []
            for k, v in s2classifier.succ[nbr].items():
                out.append("{}:{}".format(k, v['count']))
            print("Node", nbr, "at depth", s2classifier.nodes[nbr]['depth'],
                  "with", len(s2classifier.nodes[nbr]['recs']),
                  "records connects to", ", ".join(out),
                  file=f)

    result = {}
    for nbr, node in sorted(s2classifier.nodes(data=True)):
        for rec in node['recs']:
            result[rec] = result.get(rec, []) + [nbr, ]

    with open("results.txt", "w") as f:
        for rec, path in sorted(result.items(), key=lambda x: (x[1], x[0])):
            print("Replay {}[{}]: {}".format(rec, data[rec].fname, ", ".join(str(i) for i in path)), file=f)
