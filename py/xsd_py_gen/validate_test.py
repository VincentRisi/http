import GENDS_UTILITY as xsd_check
from XSD_LTDS_SCHEMA import *

def run():
    valid = xsd_check.validate
    OrigSystem = valid(OrigSystemType, 'ABCDEF')
    TranRefNo  = valid(TranRefNoType, '123')
    AccNo      = valid(AccNoType, '123456789012345')
    BranchNo   = valid(BranchNoType, '1203')
    FECNo      = valid(FECNoType, '1234567890123456')
    Amount     = valid(AmountType, '1.50')
    


run()