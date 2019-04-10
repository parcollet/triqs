import numpy as np
from pytriqs.gf import *

def fermi(eps, beta):
    return 1./(1. + np.exp(beta * eps))

beta = 50.0

# -- Test Matsubara frequency density for free Gf

g_iw = GfImFreq(beta=beta, indices=[0], n_points=1000)

print "=============================================="

print g_iw.density()

print "OK"

#g_iw << inverse(iOmega_n)

#np.testing.assert_almost_equal(g_iw.density(), 0.5)

#np.random.seed(1337) # reproducible random values

# for eps in np.random.random(10):
    # print eps
    # #g_iw << inverse(iOmega_n - eps)
    # n = g_iw.density()
    # print "ok"
    # #n_ref = fermi(eps, beta)
    # #np.testing.assert_almost_equal(n, n_ref)

