
import matplotlib.pyplot as plt
import sys
import numpy as np
from scipy.interpolate import make_interp_spline

def get_data(file):
    x = []
    y = []
    with open(file, 'r') as f:
        for line in f:
            values = [float(s) for s in line.split(',')]
            x.append(values[0])
            y.append(values[1])
    return x, y

def plot_graph(x,y,color,label):
    x = np.array(x)
    y = np.array(y)
    X_Y_Spline = make_interp_spline(x, y)
    X_ = np.linspace(x.min(), x.max(), 500)
    Y_ = X_Y_Spline(X_)
    plt.plot(X_, Y_,label=label,color=color)
    plt.xlabel("% hashing power of adversary node")
    plt.ylabel("MPU adversary node * 100")
    plt.title('')
    
def plot_graph_all(x,y,color,label):
    x = np.array(x)
    y = np.array(y)
    X_Y_Spline = make_interp_spline(x, y)
    X_ = np.linspace(x.min(), x.max(), 500)
    Y_ = X_Y_Spline(X_)
    plt.plot(X_, Y_,label=label,color=color)
    plt.xlabel("% hashing power of adversary node")
    plt.ylabel("MPU overall node * 100")
    plt.title('')

if __name__ == '__main__':
    x = []
    y = []
    x,y = get_data("data_selfish25.csv")
    plot_graph(x,y,'green',"$\zeta$=0.25")
    x,y = get_data("data_selfish55.csv")
    plot_graph(x,y,'blue',"$\zeta$=0.55")
    x,y = get_data("data_selfish85.csv")
    plot_graph(x,y,'red',"$\zeta$=0.85")
    plt.legend()
    plt.savefig('plot_selfish_zeta.png')
    plt.clf()
    
    x,y = get_data("data_all_selfish25.csv")
    plot_graph_all(x,y,'green',"$\zeta$=0.25")
    x,y = get_data("data_all_selfish55.csv")
    plot_graph_all(x,y,'blue',"$\zeta$=0.55")
    x,y = get_data("data_all_selfish85.csv")
    plot_graph_all(x,y,'red',"$\zeta$=0.85")
    plt.legend()
    plt.savefig('plot_selfish_zeta_all.png')
    plt.clf()

    x,y = get_data("data_stubborn25.csv")
    plot_graph(x,y,'green',"$\zeta$=0.25")
    x,y = get_data("data_stubborn55.csv")
    plot_graph(x,y,'blue',"$\zeta$=0.55")
    x,y = get_data("data_stubborn85.csv")
    plot_graph(x,y,'red',"$\zeta$=0.85")
    plt.legend()
    plt.savefig('plot_stubborn_zeta.png')
    plt.clf()

    x,y = get_data("data_all_stubborn25.csv")
    plot_graph_all(x,y,'green',"$\zeta$=0.25")
    x,y = get_data("data_all_stubborn55.csv")
    plot_graph_all(x,y,'blue',"$\zeta$=0.55")
    x,y = get_data("data_all_stubborn85.csv")
    plot_graph_all(x,y,'red',"$\zeta$=0.85")
    plt.legend()
    plt.savefig('plot_stubborn_zeta_all.png')