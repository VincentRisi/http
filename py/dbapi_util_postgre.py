from dbapi_util import *

class PostgreSqlReturning():
    def __init__(self, table, field):
        self.head = ''
        self.output = ''
        self.sequence = f'  0'
        self.tail = 'returning {field} '
        self.dropField = ''
        self.usesPlSql = False
    def check_use(self, value):
        return value

returning = PostgreSqlReturning

def get_sequence(connect, table):
    seq = 0
    return seq
