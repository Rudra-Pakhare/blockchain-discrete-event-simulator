#get x y corrdinates from the file graph.txt

import matplotlib.pyplot as plt
import sys
import numpy as np
from scipy.interpolate import make_interp_spline

l = []
with open('data.csv', 'r') as file:
    data = file.read()
    for line in data.splitlines():
        a = [float(x) for x in line.split(',')]
        l.append(a)

with open('data.csv', 'w') as file:
    for i in l:
        a = []
        a.append(str(i[0]))
        for j in range(1,len(i)):
            a.append(str(i[j]/15))
        file.write(','.join(a) + '\n')

def get_data():
    x = []
    y = []
    with open('data.csv', 'r') as f:
        for line in f:
            values = [float(s) for s in line.split(',')]
            x.append(values[0]*100)
            y.append(values[1]*100)
    return x, y

def get_data_all():
    x = []
    y = []
    with open('data.csv', 'r') as f:
        for line in f:
            values = [float(s) for s in line.split(',')]
            x.append(values[0]*100)
            y.append(values[2]*100)
    return x, y

def plot_graph(a):
    x, y = get_data()
    x = np.array(x)
    y = np.array(y)
    X_Y_Spline = make_interp_spline(x, y)
    X_ = np.linspace(x.min(), x.max(), 500)
    Y_ = X_Y_Spline(X_)
    plt.plot(X_, Y_)
    plt.xlabel("% hashing power of adversary node")
    plt.ylabel('MPU adversary node * 100')
    plt.title('zeta = ' + a + '')
    plt.savefig('plot_selfish_zeta' + a + '.png')
    with open('data_selfish'+a+'.csv', 'w') as file:
        for i in range(len(x)):
            b = []
            b.append(str(x[i]))
            b.append(str(y[i]))
            file.write(','.join(b) + '\n')

def plot_graph_all(a):
    x, y = get_data_all()
    x = np.array(x)
    y = np.array(y)
    X_Y_Spline = make_interp_spline(x, y)
    X_ = np.linspace(x.min(), x.max(), 500)
    Y_ = X_Y_Spline(X_)
    plt.plot(X_, Y_)
    plt.xlabel("% hashing power of adversary node")
    plt.ylabel('MPU overall node * 100')
    plt.title('zeta = ' + a + '')
    plt.savefig('plot_selfish_zeta_all' + a + '.png')
    with open('data_all_selfish'+a+'.csv', 'w') as file:
        for i in range(len(x)):
            b = []
            b.append(str(x[i]))
            b.append(str(y[i]))
            file.write(','.join(b) + '\n')

if __name__ == '__main__':
    
    plot_graph(sys.argv[1])
    plt.clf()
    plot_graph_all(sys.argv[1])
    