import sys
from gends_util import as_xml
from XSD_ExternalFndtMsg import *

tablesType = MsgTablesType()
tablesType.id = '1234'
tableType = MsgTableType()
columns = ['first','second','third','fourth','fifth']
for i, column in enumerate(columns):
  recType = RecordType()
  recType.isNew = 'Y'
  recType.column = column
  tableType.record.append(recType)
tablesType.mtable.append(tableType)
tableType = MsgTableType()
columns = ['node','number','name','address','whodone','whendone']
for i, column in enumerate(columns):
  recType = RecordType()
  recType.isNew = 'Y'
  recType.column = column
  tableType.record.append(recType)
tablesType.mtable.append(tableType)
data = as_xml(tablesType, namespace='https/fred.nerk')
print(data)
