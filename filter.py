import numpy
def smooth(x, window_len=11, window='hanning'):
#output length is not equal to input length

    if window_len<2:
        return x
    
    if not window in ['flat', 'hanning', 'bartlett', 'blackman']:
        raise ValueError

    s= numpy.r_[x[window_len-1:0:-1],x,x[-2:-window_len-1:-1]]

    print(len(s))
    
    w=numpy.ones(window_len,'d')
    y=numpy.convolve(w/w.sum(),s,mode='valid')
    return y

from numpy import *
#from matplotlib import pylab
#from pylab import *
from pylab import *
import matplotlib

def smooth_demo():

    t=linspace(-4,4,100)
    x=sin(t)
    xn=x+randn(len(t))*0.1
    y=smooth(x)

    ws=31

    subplot(211)
    plot(ones(ws))

    windows=['flat', 'hanning', 'hamming', 'bartlett', 'blackman']

    hold(True)
    for w in windows[1:]:
        eval('plot('+w+'(ws) )')

    axis([0,30,0,1.1])

    legend(windows)
    title("The smoothing windows")
    subplot(212)
    plot(x)
    plot(xn)
    for w in windows:
        plot(smooth(xn,10,w))
    l=['original signal', 'signal with noise']
    l.extend(windows)

    legend(l)
    title("Smoothing a noisy signal")
    show()


if __name__=='__main__':
    smooth_demo()


