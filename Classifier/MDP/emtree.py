import numpy as np
from pprint import pprint
from numpy.random import permutation as permute
from random import random
from networkx import DiGraph
from networkx import dfs_postorder_nodes as dfs
import networkx as nx
import matplotlib.pyplot as plt


def __accum_gamma__(edge, d, weight):
    edge['sx'] += d * weight
    edge['slx'] += np.log(d) * weight
    edge['sxlx'] += np.log(d) * weight


def __get_gamma_params__(sx, slx, sxlx):
    beta = sxlx - slx * sx
    alpha = sx / beta
    return alpha, beta


def __getx0__(recs):
    return recs[0]


def __dist__(x, y):
    return sum(abs(x - y))


def __loss__(f, x, data):
    return sum([f(x, y) for y in data])


def __accum__(accum, rec, weight=1):
    accum['sum'] += weight * rec
    accum['count'] += weight


def __aver__(succ, recs, data):
    wrkspace = dict(sum=0, count=0)
    if len(succ) > 0:
        for nbr, edge in succ.items():
            wrkspace['sum'] += edge['wrkspace']['sum']
            wrkspace['count'] += edge['wrkspace']['count']
    else:
        for rec in recs:
            __accum__(wrkspace, data[rec])

    if wrkspace['count'] < 1E-6:
        return None, wrkspace
    return wrkspace['sum'] / wrkspace['count'], wrkspace


class EMTree(DiGraph):
    def __init__(self, data, dist=None, averager=None, base_cluster_size=2, mways=2):
        super().__init__()
        self.tobuild = data
        self.icount = 0
        self.dist = dist or __dist__
        self.aver = averager or __aver__
        self.data = data
        self._s = base_cluster_size
        self._m = mways
        if self._s < self._m:
            raise AttributeError("base cluster size must be greater than number of children in node (mways)")
        self.build()

    def build(self):
        idx = 0
        a = permute(len(self.data))
        self.add_node(0, hit=set(), recs=[], depth=0)
        for i in a:
            idx = self.__build_insert(0, idx, i)
        self.icount = idx

    def train(self):
        modified = 1
        counter = 0
        while modified > 0:
            counter += 1
            self.__maximization()
            self.__clear_nodes()
            self.__expectation()
            self.__restructure()
            modified = self.__count_modified_nodes()
            print("Iteration", counter, "changed", modified, "nodes.")

        self.__maximization()
        self.__fix_depth()

    def __fix_depth(self):
        for nbr, data in sorted(self.nodes(data=True)):
            if len(self.pred[nbr].items()) == 0:
                data['depth'] = 0
            else:
                for prev, edge in self.pred[nbr].items():
                    d = self.nodes[prev]['depth']
                    data['depth'] = d + 1
                    edge['depth'] = d + 1

    def __count_modified_nodes(self):
        counts = 0
        for nbr, data in self.nodes(data=True):
            curr = set(data['recs'])
            if curr != data['hit']:
                data['hit'] = curr
                counts += 1
        return counts

    def __restructure(self):
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
                self.add_edge(prevn, nextn, wrkspace={}, count=0, center=None, depth=self.nodes[prevn]['depth'] + 1)

    def __build_insert(self, node, idx, rec):
        self.nodes[node]['recs'].append(rec)
        if len(self.nodes[node]['recs']) < self._s:
            return idx

        if len(self.nodes[node]['recs']) == self._s:
            for r in self.nodes[node]['recs'][:self._m]:
                idx += 1
                self.add_node(idx, hit=set(), recs=[r, ], depth=self.nodes[node]['depth'] + 1)
                self.add_edge(node, idx,
                              center=None,
                              depth=self.nodes[node]['depth'] + 1,
                              worksapce={},
                              count=0)
            return idx

        succ = self.successors(node)
        nextnode = min(succ, key=lambda x: len(self.nodes[x]['recs']))
        return self.__build_insert(nextnode, idx, rec)

    def __maximization(self):
        removed = set()
        for idx in dfs(self):
            if idx == 0:
                continue
            if len(self.nodes[idx]['recs']) == 0:
                removed.add(idx)
            for nbr, pred in self.pred[idx].items():
                pred['center'], pred['wrkspace'] = self.aver(self.succ[idx], self.nodes[idx]['recs'], self.data)
                pred['count'] = len(self.nodes[idx]['recs'])
        for idx in removed:
            self.remove_node(idx)

    def __clear_nodes(self):
        for node in self.nodes.values():
            node['recs'] = []

    def __expectation(self):
        for rec in range(len(self.data)):
            self.__expect_insert(0, rec)

    def __expect_insert(self, node, rec):
        self.nodes[node]['recs'].append(rec)
        scores = {}
        for nbr, edge in self.succ[node].items():
            scores[nbr] = self.dist(self.data[rec], edge['center'])
        if len(scores) == 0:
            return
        best = min(scores.keys(), key=lambda k: scores[k])
        # self.accum(self.edges[node][best], rec)
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
    data = np.array([[random(), random()] for _ in range(1000)])

    emtree = EMTree(data, dist=lambda x, y: np.sqrt(sum((x - y) ** 2)), base_cluster_size=4, mways=4)
    emtree.train()

    with open("out.txt", "w") as f:
        for nbr in sorted(emtree.nodes()):
            out = []
            for k, v in emtree.succ[nbr].items():
                out.append("{}:{}".format(k, v['wrkspace']['count']))
            print("Node", nbr, "at depth", emtree.nodes[nbr]['depth'],
                  "with", len(emtree.nodes[nbr]['recs']),
                  "records connects to", ", ".join(out),
                  file=f)

    centers = np.array([n['center'] for _, _, n in emtree.edges(data=True)])
    depths = np.array([n['depth'] for _, _, n in emtree.edges(data=True)])
    md = max(depths)

    n = 256
    y, x = np.mgrid[min(data[:, 0]):max(data[:, 0]):complex(0, n), min(data[:, 1]):max(data[:, 1]):complex(0, n)]
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
        plt.close(fig)

    # nx.draw(emtree, pos=None, with_labels=False, arrows=False)
    # plt.savefig('pics/nx_test')

    fig = plt.figure(figsize=(12, 12))
    plt.imshow(zl[md - 1], extent=[0, 1, 1, 0])
    plt.scatter(centers[:, 0], centers[:, 1], alpha=0.7, s=[36 * (md + 1 - d) for d in depths],
                color=["C{}".format(d % 10) for d in depths])
    fig.savefig("pics/a0")
    plt.close(fig)
