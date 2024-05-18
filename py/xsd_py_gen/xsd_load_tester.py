import sys
sys.path.append(r'D:\vlab\python\pytools\xsd_py_gen\build\example\py')
from gends_util import *
from io import BytesIO, StringIO
import os, pickle

def test_gps_payment():
    from XSD_GPSPayment import Payment
    for filename in [
        r'D:\vlab\python\pytools\xsd_py_gen\data\GPSPayment.xml',
        r'D:\vlab\python\pytools\xsd_py_gen\data\GPSPayment2.xml',
        r'D:\vlab\python\pytools\xsd_py_gen\data\GPSPayment3.xml',
        r'D:\vlab\python\pytools\xsd_py_gen\data\GPSPayment4.xml',
        r'D:\vlab\python\pytools\xsd_py_gen\data\GPSPayment5.xml',
        ]:
        with open(filename,'rb') as ifile:
            message = ifile.read()
        payment = Payment()
        with StringIO() as logfile:
            print (filename)
            load_message(payment, message, logfile)
            print (logfile.getvalue())
        xml = as_xml(payment)
        print (xml)
        
test_gps_payment()