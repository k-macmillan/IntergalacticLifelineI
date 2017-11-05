import numpy as np
from pprint import pprint
from numpy.random import permutation as permute
import pandas as pd
from random import random
from networkx import DiGraph
from networkx import dfs_postorder_nodes as dfs
import networkx as nx
import matplotlib.pyplot as plt
from scipy.optimize import fmin
from fastdtw import fastdtw


class EMTree(DiGraph):
    @staticmethod
    def __getx0(recs):
        return recs[0]

    @staticmethod
    def __dist(x, y):
        return sum(abs(x - y))

    @staticmethod
    def __loss(f, x, data):
        return sum([f(x, y) for y in data])

    def __init__(self, data, dist=None, opt=fmin, guess=None, loss=None, base_cluster_size=2):
        super().__init__()
        self.dist = dist or EMTree.__dist
        self.guess = guess or EMTree.__getx0
        self.loss = loss or (lambda x, recs: EMTree.__loss(self.dist, x, recs))
        self.opt = opt
        self.data = data
        self._s = base_cluster_size
        self.build()

    def build(self):
        idx = 0
        a = permute(len(self.data))
        self.add_node(0, recs=list())
        for i in a:
            idx = self.__build_insert(0, idx, self.data[i])

    def __build_insert(self, node, idx, rec):
        _, data = zip(*self.nodes(data=True))
        data[node]['recs'].append(rec)

        if len(data[node]['recs']) < self._s:
            return idx

        if len(data[node]['recs']) == self._s:
            for r in data[node]['recs']:
                idx += 1
                self.add_node(idx, recs=[r, ])
                self.add_edge(node, idx, center=None)
            return idx

        succ = self.successors(node)
        nextnode = min(succ, key=lambda x: len(data[x]['recs']))
        return self.__build_insert(nextnode, idx, rec)

    def __maximization(self):
        _, data = zip(*self.nodes(data=True))
        for idx in dfs(self):
            if idx == 0:
                continue
            _, _, pred = self.in_edges([idx, ], data=True)[0]
            pred['center'] = self.opt(self.loss, self.guess(data[idx]['recs']))
            data[idx]['recs'] = []

    def __expectation(self):
        pass

    def __expect_insert(self, node, idx, rec):
        _, nodes = self.nodes(data=True)


def __debug():
    data = [[random(), random()] for _ in range(16)]
    emtree = EMTree(data)
    nodes = emtree.nodes(data=True)
    for i,n in nodes:
        print(i,":")
        pprint(n)
        print()
    nx.draw_circular(emtree)
    plt.show()


__debug()
