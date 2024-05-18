from dbapi_util import *

class OciSqlReturning():
    def __init__(self, table, field):
        self.head = ''
        self.output = ''
        self.sequence = f'  {table}Seq.nextval'
        self.tail = 'returning {field} into :{field} '
        self.dropField = ''
        self.usesPlSql = True
    def check_use(self, value):
        return value

returning = OciSqlReturning

class Dutil_sequence(object):
    def _make(self): return Dutil_sequence()
    __slots__ = ['seq', 'tableSeq']
    def __init__(self):
        self.seq = ''
        self.tableSeq = ''
    def _fields(self):
        return Dutil_sequence.__slots__

class util_sequence(Dutil_sequence):
    def _get_output(self, _result):
        self.seq = _result[0]
        return 1
    def _copy_input(self, record):
        record.tableSeq = self.tableSeq
    def execute(self, connect):
        _command = f'select {self.tableSeq}.nextval from dual'
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
