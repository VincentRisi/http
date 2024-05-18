#import sys
#sys.path.append(r'D:\vlab\python\ctools\audio\utility\py')
#print (sys.path)
#from aaxlist import books
#for key in books:
#    book = books[key]
#    print (key, book.name)
alias_name = []
alias_type = []
alias_name.append('ErrorLogMsg')
alias_type.append('ErrorLogMsgType')
alias_name.append('FndtMsg')
alias_type.append('FndtMsgType')
alias_name.append('FndtHeader')
alias_type.append('FndtBatchMsgHeaderType')
alias_name.append('FndtMsgBatch')
alias_type.append('FndtBatchMsgType')
alias_name.append('FndtBatchMsgResp')
alias_type.append('FndtBatchMsgType')
alias_name.append('Extn')
alias_type.append('FndtMsgExtensionType')
alias_name.append('PostingInfo')
alias_type.append('PostingInfoType')
alias_name.append('PaymentContact')
alias_type.append('PaymentContactType')
alias_name.append('BalanceCheckInf')
alias_type.append('BalanceCheckInfType')
alias_name.append('FndtMsgSubset')
alias_type.append('FndtMsgSubsetType')
alias_name.append('binary')
alias_type.append('Base64BinrayType')
alias_name.append('AnyType')
alias_type.append('anyType')
alias_name.append('mtables')
alias_type.append('MsgTablesType')
alias_name.append('mtable')
alias_type.append('MsgTableType')
alias_name.append('FreeFormTextPayment')
alias_type.append('FreeFormTextType')
alias_name.append('InternalRequest')
alias_type.append('InternalRequestType')
alias_name.append('InternalResponse')
alias_type.append('InternalResponseType')
alias_name.append('MessageProcessingRequest')
alias_type.append('MessageProcessingRequestType')
alias_name.append('WsResult')
alias_type.append('ResponseDetailsType')
alias_name.append('FndtFault')
alias_type.append('FndtFaultType')
alias_name.append('MIDListResponse')
alias_type.append('MIDListResponseType')
alias_name.append('RuleResult')
alias_type.append('RuleResultType')
alias_name.append('IBANResponse')
alias_type.append('IBANResponseType')
alias_name.append('IBANRequest')
alias_type.append('IBANRequestType')
alias_name.append('column')
alias_type.append('MsgTableEntryBaseType')
alias_name.append('CHAR')
alias_type.append('STRING')
alias_name.append('DECIMAL')
alias_type.append('DOUBLE')
alias_name.append('OBJECT')
alias_type.append('STRING')
alias_name.append('outputMode')
alias_type.append('ParseOutputModeType')
alias_name.append('fndtMsgSubset')
alias_type.append('FndtMsgSubsetType')

import XSD_ExternalFndtMsg as xx
import XSD_CommonTypes as ct

str_attrib = (str, 'attrib')
class RecordType:
    isNew: str_attrib
    column: str
    def __init__(self):
        self.isNew = ''
        self.column = ''

class MsgTableType:
    rtList = [RecordType]
    record: rtList ## RecordType
    def __init__(self):
        self.record = []

z = RecordType()
print ('RecordType', z.__annotations__)

tt = MsgTableType()
print ('MsgTableType', tt.__annotations__)

xyz = ct.BulkRequestSummaryType()
print ('BulkRequestSummaryType', xyz.__annotations__)

mft = ct.MsgFeesType()
print ('MsgFeesType', mft.__annotations__)

def get_alias_type(name):
    try:
        no = alias_name.index(name)
        return alias_type[no]
    except:
        return name

def get_alias_name(typeof):
    try:
        no = alias_type.index(typeof)
        return alias_name[no]
    except:
        return typeof

print (get_alias_type('FndtMsg'))
print (get_alias_name('FndtMsgType'))
