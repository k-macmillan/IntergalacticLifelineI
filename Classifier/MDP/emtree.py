import numpy as np
from pprint import pprint
from numpy.random import permutation as permute
from random import random
from networkx import DiGraph
from networkx import dfs_postorder_nodes as dfs
import networkx as nx
import matplotlib.pyplot as plt


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

    @staticmethod
    def __opt(data):
        return np.average(data, axis=0)

    def __init__(self, data, dist=None, opt=None, guess=None, loss=None, base_cluster_size=2, mways=2):
        super().__init__()
        self.icount = 0
        self.dist = dist or EMTree.__dist
        self.guess = guess or EMTree.__getx0
        self.loss = loss or (lambda x, recs: EMTree.__loss(self.dist, x, recs))
        self.opt = opt or EMTree.__opt
        self.data = data
        self._s = base_cluster_size
        self._m = mways
        if self._s < self._m:
            raise AttributeError("base cluster size must be greater than number of children in node (mways)")
        self.build()

    def build(self):
        idx = 0
        a = permute(len(self.data))
        self.add_node(0, recs=list(), depth=0)
        for i in a:
            idx = self.__build_insert(0, idx, self.data[i])
        self.icount = idx

    def train(self, its=100):
        for _ in range(its):
            self.__maximization()
            self.__expectation()

            # Rebuild nodes that lost a child
            # get list of nodes with one or fewer children and more than 's' data points
            tobuild = []
            tocut = []
            for nbr, data in self.nodes(data=True):
                if len(data['recs']) > self._s and len(self.succ[nbr]) == 0:
                    tobuild.append(nbr)
                if len(self.succ[nbr]) == 1:
                    tocut.append(nbr)

            # rebuild the nodes in tobuild
            for nbr in tobuild:
                if self.has_node(nbr):
                    data = self.nodes[nbr]
                    recs = data['recs']
                    data['recs'] = []
                    T = nx.dfs_tree(self, nbr)
                    for subn in list(T.nodes):
                        if subn != nbr:
                            self.remove_node(subn)
                    for rec in recs:
                        self.icount = self.__build_insert(nbr, self.icount, rec)

                        # pprint(list(self.nodes(data=True)))
                        # pprint(list(self.edges(data=True)))
                        # if input("Ok? ") == 'quit':
                        #   break

            for nbr in tocut:
                if not self.has_node(nbr):
                    continue
                if len(self.succ[nbr]) == 1:
                    prevn, _ = list(self.pred[nbr].items())[0]
                    nextn, _ = list(self.succ[nbr].items())[0]
                    self.remove_node(nbr)
                    self.add_edge(prevn, nextn, center=None, depth=self.nodes[nextn]['depth'])

        self.__maximization()

    def __build_insert(self, node, idx, rec):
        self.nodes[node]['recs'].append(rec)

        if len(self.nodes[node]['recs']) < self._s:
            return idx

        if len(self.nodes[node]['recs']) == self._s:
            for r in self.nodes[node]['recs'][:self._m]:
                idx += 1
                self.add_node(idx, recs=[r, ], depth=self.nodes[node]['depth'] + 1)
                self.add_edge(node, idx, center=None, depth=self.nodes[node]['depth'] + 1)
            return idx

        succ = self.successors(node)
        nextnode = min(succ, key=lambda x: len(self.nodes[x]['recs']))
        return self.__build_insert(nextnode, idx, rec)

    def __maximization(self):
        self.nodes[0]['recs'] = []
        removed = set()
        for idx in dfs(self):
            if idx == 0:
                continue
            if len(self.nodes[idx]['recs']) == 0:
                removed.add(idx)
            for nbr, pred in self.pred[idx].items():
                pred['center'] = self.opt(self.nodes[idx]['recs'])
            self.nodes[idx]['recs'] = []
        for idx in removed:
            self.remove_node(idx)

    def __expectation(self):
        for rec in self.data:
            self.__expect_insert(0, rec)

    def __expect_insert(self, node, rec):
        self.nodes[node]['recs'].append(rec)
        scores = {}
        for nbr, edge in self.succ[node].items():
            scores[nbr] = self.dist(rec, edge['center'])
        if len(scores) == 0:
            return
        best = min(scores.keys(), key=lambda k: scores[k])
        self.__expect_insert(best, rec)

    def classify(self, rec, node=0):
        if len(self.succ[node]) == 0:
            return []
        scores = {}
        for nbr, edge in self.succ[node].items():
            scores[nbr] = self.dist(rec, edge['center'])
        best = min(scores.keys(), key=lambda k: scores[k])
        return [(self.succ[node][best]['depth'], best), ] + self.classify(rec, best)


def __debug():
    data = np.array([[random(), random()] for _ in range(256)])

    emtree = EMTree(data, dist=lambda x, y: np.sqrt(sum((x - y) ** 2)), base_cluster_size=2, mways=2)
    emtree.train()

    pprint(list(emtree.edges(data=True)))

    centers = np.array([n['center'] for _, _, n in emtree.edges(data=True)])
    depths = np.array([n['depth'] for _, _, n in emtree.edges(data=True)])
    md = max(depths)

    n = 256
    x, y = np.mgrid[min(data[:, 0]):max(data[:, 0]):complex(0, n), min(data[:, 1]):max(data[:, 1]):complex(0, n)]
    zl = [np.zeros((n, n)) for _ in range(md + 1)]
    for i in range(n):
        for j in range(n):
            a = emtree.classify([x[i, j], y[i, j]])
            a += a[-1:] * (md + 1 - len(a))

            for ii, (d, best) in enumerate(a):
                zl[ii][i, j] = best

    touni = [{} for _ in range(len(zl))]
    for i in range(len(zl)):
        uni = np.unique(zl[i])
        touni[i] = dict(zip(uni, range(len(uni))))
        op = np.vectorize(lambda x: touni[i][x])
        zl[i] = op(zl[i])

    for i in range(1, len(zl)):
        fig = plt.figure(figsize=(12, 12))
        data = np.array(data)
        plt.imshow(zl[i], extent=[0, 1, 1, 0])
        plt.scatter(data[:, 0], data[:, 1], color='k')
        fig.savefig("pics/t{}".format(i))

    # nx.draw(emtree, pos=None, with_labels=False, arrows=False)
    # plt.savefig('pics/nx_test')

    fig = plt.figure(figsize=(12, 12))
    plt.imshow(zl[md - 1], extent=[0, 1, 1, 0])
    plt.scatter(centers[:, 0], centers[:, 1], alpha=0.7, s=[36 * (md + 1 - d) for d in depths],
                color=["C{}".format(d % 10) for d in depths])
    fig.savefig("pics/a0")
