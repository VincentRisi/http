from dbapi_util import *

class Lite3Returning():
    def __init__(self, table, field):
        self.head = ''
        self.output = ''
        self.sequence = ''
        self.tail = '; select last_insert_rowid()'
        self.dropField = ''
        self.doesGeneratedKeys = False
        self.usesPlSql = False
    def check_use(self, value):
        return ''

returning = Lite3Returning
class DUtil_sequence(object):
    def _make(self): return DUtil_sequence()
    __slots__ = ['seq', 'tableSeq']
    def __init__(self):
        self.seq = ''
        self.tableSeq = ''
    def _fields(self):
        return DUtil_sequence.__slots__

class util_sequence(DUtil_sequence):
    def __init__(self, table):
        self.table = table
    def _get_output(self, _result):
        self.seq = _result[0]
        return 1
    def _copy_input(self, record):
        record.tableSeq = self.tableSeq
    def execute(self, connect):
        _command = f'select last_insert_rowid()+1 from {self.table}'
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
    query = util_sequence(table)
    query.tableSeq = f'{table}Seq'
    query = query.execute(connect)
    seq = query.seq
    return seq
