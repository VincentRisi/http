from dbapi_util import *

class Returning():
    def __init__(self, table, field):
        self.head = f"select {field} from new table ("
        self.output = ''
        self.sequence = f'  nextval for {table}Seq,'
        self.tail = ')'
        self.dropField = ''
        self.doesGeneratedKeys = False
        self.usesPlSql = False
    def check_use(self, value):
        return value

returning = Returning

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
        _command = f'select nextval for {self.tableSeq} from SYSIBM.SYSDUMMY1 WITH UR'
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
