from espresso import pmi
import _espresso 
import espresso
from espresso.esutil import cxxinit

class VerletListLocal(_espresso.VerletList):
    'The (local) verlet list.'

    def __init__(self, system, cutoff):
        'Local construction of a verlet list'
        if pmi.workerIsActive():
            cxxinit(self, _espresso.VerletList, system, cutoff)
            
    def totalSize(self):
        'count number of pairs in VerletList, involves global reduction'
        if pmi.workerIsActive():
            return self.cxxclass.totalSize(self)

if pmi.isController:
    class VerletList(object):
        __metaclass__ = pmi.Proxy
        pmiproxydefs = dict(
            cls = 'espresso.VerletListLocal',
            pmiproperty = [ 'builds' ],
            pmicall = [ 'totalSize' ]
            )
