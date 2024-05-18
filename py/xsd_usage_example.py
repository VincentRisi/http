from lxml import etree
import sys, io, os.path
from xml.dom import minidom
import XSD_LTDS_REQUEST as LTDS
import PYXB_LTDS_REQUEST as PLR

schema_file_name = r'C:\nedbank\custom\python3\bbd-stdfront\source\putty3\gends\xsd\eximbills\LTDSRequestSchema.xsd'
test_files = [ r'C:\nedbank\custom\python3\bbd-stdfront\source\mcpe\c\eximbillsmqin\transaction.xml',
               r'C:\nedbank\custom\python3\bbd-stdfront\source\mcpe\c\eximbillsmqin\transaction2.xml',
               r'C:\nedbank\custom\python3\bbd-stdfront\source\misc-tests\xmlrecord-test\eximbills.xml',
               r'C:\nedbank\custom\python3\bbd-stdfront\source\mcpe\c\eximbillsmqin\moreandless.xml'
             ]
def extract(ifilename):
    _, name =os.path.split(ifilename)
    return name

def gends_example(ifilename):
    print (f'\n---- inf ---- Processing gends {extract(ifilename)}')
    # This is similar to using a readmessage_raw()
    with open(ifilename, 'rb') as ifile: 
        message = ifile.read()
        etree_message(message)

def etree_message(message):
    print ('\n---- inf ---- LTDS.parseString loading')
    transaction = LTDS.parseString(message, silence=True, print_warnings=False)
    # If there are messages we can assume not valid
    if len(transaction.gds_collector_.messages) > 0:
        for message in transaction.gds_collector_.messages:
            print (f'---- err ---- {message}')
        print (f'---- inf ---- failed')
        return
    print (f'---- inf ---- passed')
    # Details is simple
    details = transaction.Details
    print (f'''\n---- inf ---- Printing some fields
OrigSystem  :{details.OrigSystem}
TranId      :{details.TranId}
TxnRefNo    :{details.TxnRefNo}
TxnDate     :{details.TxnDate}
TxnBranchNo :{details.TxnBranchNo}
FinBranchNo :{details.FinBranchNo}
''')
    # Financial is arrayed
    for financial in transaction.Financials.Financial:
        print (f'FinancialNo:{financial.FinancialNo}')
        financial.Code = 'PRINCIPAL'
    details.FinBranchNo='5464'
    # This demonstrates validating an LTDS_REQUEST using a memfile
    with io.StringIO() as memfile:
        transaction.export(memfile, 0, pretty_print=False)
        print ('\n---- inf ---- transaction.export no pretty print')
        print (memfile.getvalue())
        # This just is here to show how to pretty print using minidom
        print ('\n---- inf ---- Pretty print using xml.dom.minidom example')
        xmlstr = minidom.parseString(memfile.getvalue()).toprettyxml(indent=' ')
        print(xmlstr)
        print ('\n---- inf ---- LTDS.parseString validation')
        transaction = LTDS.parseString(memfile.getvalue(), silence=True, print_warnings=False)
        if len(transaction.gds_collector_.messages) > 0:
            for message in transaction.gds_collector_.messages:
                print (f'---- err ---- {message}')
            print (f'---- inf ---- failed')
            return
        print (f'---- inf ---- passed')

def pyxb_example(ifilename):
    print (f'\n---- inf ---- Processing pyxb {extract(ifilename)}')
    # This is similar to using a readmessage_raw()
    with open(ifilename, 'rb') as ifile: 
        message = ifile.read()
        transaction = PLR.CreateFromDocument(message)
        # Details is simple
        details = transaction.Details
        print (f'''\n---- inf ---- Printing some fields
OrigSystem  :{details.OrigSystem}
TranId      :{details.TranId}
TxnRefNo    :{details.TxnRefNo}
TxnDate     :{details.TxnDate}
TxnBranchNo :{details.TxnBranchNo}
FinBranchNo :{details.FinBranchNo}
''')
        # Financial is arrayed
        for financial in transaction.Financials.Financial:
            print (f'FinancialNo:{financial.FinancialNo}')

def schema_validate(ifilename, schema):
    print (f'\n---- inf ---- validating {extract(ifilename)}')
    # This is similar to using a readmessage_raw()
    with open(ifilename, 'rb') as ifile: 
        doc = etree.parse(ifile)
        if schema.validate(doc) == False:
            print (schema.error_log)

def main(): 
    for ifilename in test_files:
        try:
            gends_example(ifilename)
        except Exception as x:
            print (f'{extract(ifilename)} {x}')

    for ifilename in test_files:
        try:
            pyxb_example(ifilename)
        except Exception as x:
            print (f'{extract(ifilename)} {x}')
    #return    
    with open(schema_file_name) as f_schema:
        schema_doc = etree.parse(f_schema)
        schema = etree.XMLSchema(schema_doc)

    for ifilename in test_files:
        try:
            schema_validate(ifilename, schema)
        except Exception as x:
            print (f'{extract(ifilename)} {x}')
        

if __name__ == '__main__':
    main()
