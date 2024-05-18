import sys
import GENDS_UTILITY as gu
from XSD_ExternalFndtMsg import *
class FndtMsg(FndtMsgType):pass

def load_message():
    with open(r'C:\vlab\pytools\xsd_py_gen\data\Message.xml', 'rb') as ifile:
        message = ifile.read()
    fbm = FndtMsgType()
    gu.load_message(fbm, message, use_name='FndtMsg')
    print (gu.as_xml(fbm, use_name='FndtMsg'))

def load_message2():
    with open(r'C:\vlab\pytools\xsd_py_gen\data\Message.xml', 'rb') as ifile:
        message = ifile.read()
    fbm = FndtMsg()
    print (dir(fbm))
    gu.load_message(fbm, message)
    print (gu.as_xml(fbm))

load_message2()

def as_xml_test1():
    uct = UserContextType()
    uct.sessionID = 'xxx'
    uct.Role = 'dfff'
    uct.UserID = 'xxy'

    brit = BulkReqInfoType()
    brit.BulkInterfaceReqFileId = 'makkers'

    brst = BulkRequestSummaryType()
    brst.BulkReqInfo.append(brit)

    fbm = FndtBatchMsgHeaderType()
    fbm.FndtMsgId = 'a'
    fbm.FndtMsgCreDtTm = 'b'
    fbm.NbOfMsgInBulk = 'c'
    fbm.FndtMsgCtrlSum = 'd'
    fbm.contextName = 'e'
    fbm.contextLocalName = 'f'
    fbm.credentials = uct
    fbm.D_SKIP_PERSIST_ON_ERROR = 'y'
    fbm.Workflow = 'wf'
    fbm.BulkRequestSummary = brst

    print (gu.as_xml(fbm,use_name='simon'))

as_xml_test1()

def check_annotations():
    class Fred(FndtMsgType):
        pass
    fred = Fred()
    print (dir(fred))
    print (fred.__annotations__)
    Freda = FndtMsgType
    freda = Freda()
    print (dir(freda))
    print (freda.__annotations__)

#check_annotations()