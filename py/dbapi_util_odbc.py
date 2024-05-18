from dbapi_util import *

has_sequence = list()
class ODBCReturning():
    def __init__(self, table, field):
        self.head = ''
        self.output = f'  output Inserted.{field}'
        if table in has_sequence:
            self.sequence = f'  next value for {table}Seq,'
            self.doesGeneratedKeys = False
        else:
            self.sequence = ''
            self.doesGeneratedKeys = True
        self.tail = ''
        self.dropField = ''
        self.usesPlSql = False
    def checkUse(self, value):
        if self.sequence != '':
            return value
        else:
            return ''

returning = ODBCReturning

class DUtil_sequence(object):
    def _make(self): return DUtil_sequence()
    __slots__ = ['seq', 'tableSeq']
    def __init__(self):
        self.seq = ''
        self.tableSeq = ''
    def _fields(self):
        return DUtil_sequence.__slots__

class util_sequence(DUtil_sequence):
    def _get_output(self, _result):
        self.seq = _result[0]
        return 1
    def _copy_input(self, record):
        record.tableSeq = self.tableSeq
    def execute(self, connect):
        _command = f'select next value for {self.tableSeq}'
        cursor = connect.cursor()
        cursor.execute(_command)
        record = util_sequence()
        self._copy_input(record)
        result = cursor.fetchone()
        if result == None:
            return None
        record._get_output(result)
        return record

def get_sequence(connect, table):
    query = util_sequence()
    query.tableSeq = f'{table}Seq'
    query = query.execute(connect)
    seq = query.seq
    return seq
