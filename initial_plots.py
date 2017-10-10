import sys
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

descrip = "MasterLvl"
dataspec = "UnitCounts"
coords = {
    'x': 'Probe',
    'y': 'Stalker',
    'z': 'Zealot',
}
select_type = 'lines'
select_fun = 'all'

funs = {
    'max': lambda arr: [0,arr.max()],
    'mean': lambda arr: [0,arr.mean()],
    'all': lambda arr: arr,
}

files = sys.argv[1:]
units = list( pd.read_csv(files[0]) )
for c in coords.values():
    assert( c in units )
    
fig = plt.figure()
ax = fig.gca(projection='3d')

ptype = {
    'scatter': ax.scatter,
    'lines': ax.plot,
    'vector': ax.quiver,
}

for fi in files:
    x,y,z = list( coords[i] for i in ['x','y','z'] )
    f, ty = select_fun, select_type
    data = pd.read_csv( fi )
    ptype[ty]( funs[f](data[x]) , funs[f](data[y]), funs[f](data[z]) )
    ax.set_xlabel( x )
    #ax.set_xlim3d([0,60])
    #ax.set_ylim3d([0,20])
    #ax.set_zlim3d([0,40])
    ax.set_ylabel( y )
    ax.set_zlabel( z )

fig.savefig( "_".join( [descrip,dataspec,x,y,z,select_type,select_fun] ) )
plt.show()
    
