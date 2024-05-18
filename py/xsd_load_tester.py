from GENDS_UTILITY import *
from io import BytesIO, StringIO
#from xml.dom import minidom
import os, pickle

def test_ltds_transaction():
    from XSD_LTDS_SCHEMA import Transaction, TranDetails, TranFinancials, TranFinancial
    transaction = Transaction()
    print (transaction.__dict__)
    print (transaction.__annotations__)
    detail = TranDetails()
    transaction.Details = detail
    detail.TranId = 25
    transaction.Details.OrigSystem = 'banana'
    item = TranFinancial()
    item.Amt = 12
    transaction.Financials.FinancialCount = 1
    transaction.Financials.Financial.append(item)
    with open(r'testfiles\xml\LTDSRequest.xml','rb') as ifile:
        message = ifile.read()
    load_message(transaction, message)
    value = amp_fix("<&>\'\"")
    print (value, amp_fix(value, undo=True), amp_fix(value, asis=True), amp_fix(value, False, True))
    print (transaction.Financials.Financial[0].FinancialNo)
    print (len(transaction.Financials.Financial))
    xml = as_xml(transaction, encoding='ascii')
    print (xml)

def test_gps_payment():
    from XSD_MCPE_GPS_PAYMENT import Payment
    for filename in [
        r'testfiles\xml\GPSPayment.xml',
        r'testfiles\xml\GPSPayment2.xml',
        r'testfiles\xml\GPSPayment3.xml',
        r'testfiles\xml\GPSPayment4.xml',
        r'testfiles\xml\GPSPayment5.xml',
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

def test_pacs_documents():
    from XSD_GES_PACS_008_001_08 import Document
    for filename in [
        r'testfiles\xml\Document.xml',
        ]:
        with open(filename,'rb') as ifile:
            message = ifile.read()
        docpacs = Document()
        with StringIO() as logfile:
            print (filename)
            load_message(docpacs, message, logfile)
            print (logfile.getvalue())
        xml = as_xml(docpacs)
        print (xml)

def test_pain_documents():
    from XSD_GES_PAIN_001_001_09 import Document
    for filename in [
        r'testfiles\xml\PainDoc1.xml',
        r'testfiles\xml\PainDoc2.xml',
        r'testfiles\xml\PainDoc3.xml',
        ]:
        with open(filename,'rb') as ifile:
            message = ifile.read()
        docpain = Document()
        with StringIO() as logfile:
            print (filename)
            load_message(docpain, message, logfile)
            print (logfile.getvalue())
        xml = as_xml(docpain)
        print (xml)

def test_mmti_transaction():
    from XSD_MCPE_MMTI_Schema_v1_0_0 import Transaction
    for filename in [
        r'testfiles\xml\m32372648.xml',
        ]:
        with open(filename,'rt') as ifile:
            messageraw = ifile.read()
        message = messageraw.encode()
        transaction = Transaction()
        with StringIO() as logfile:
            print (filename)
            load_message(transaction, message, logfile)
            print (logfile.getvalue())
        xml = as_xml(transaction)
        print (xml)

test_ltds_transaction()
#test_gps_payment()
#test_pacs_documents()
#test_pain_documents()
#test_mmti_transaction()
