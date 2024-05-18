import zlib
from datetime import datetime, date, time

class Binary():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Binary'
        self.length = length
        self.null = null
        self.pkey = pkey

class Blob():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Blob'
        self.length = length
        self.null = null
        self.pkey = pkey

class Boolean():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Boolean'
        self.length = length
        self.null = null
        self.pkey = pkey

class Char():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Char'
        self.length = length
        self.null = null
        self.pkey = pkey

class Clob():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Clob'
        self.length = length
        self.null = null
        self.pkey = pkey

class Date():
    def __init__(self, null=False, pkey=False):
        self.name = 'Date'
        self.null = null
        self.pkey = pkey

class DateTime():
    def __init__(self, null=False, pkey=False):
        self.name = 'DateTime'
        self.null = null
        self.pkey = pkey

class Float():
    def __init__(self, length, scale, null=False, pkey=False):
        self.name = 'Float'
        self.length = length
        self.scale = scale
        self.null = null
        self.pkey = pkey

class HugeCHAR():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'HugeCHAR'
        self.length = length
        self.null = null
        self.pkey = pkey

class Image():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Image'
        self.length = length
        self.null = null
        self.pkey = pkey

class Int():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Int'
        self.length = length
        self.null = null
        self.pkey = pkey

class LongInt():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'LongInt'
        self.length = length
        self.null = null
        self.pkey = pkey

class Money():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Money'
        self.length = length
        self.null = null
        self.pkey = pkey

class NChar():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'NChar'
        self.length = length
        self.null = null
        self.pkey = pkey

class SmallInt():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'SmallInt'
        self.length = length
        self.null = null
        self.pkey = pkey

class Status():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'Status'
        self.length = length
        self.null = null
        self.pkey = pkey

class Time():
    def __init__(self, null=False, pkey=False):
        self.name = 'Time'
        self.null = null
        self.pkey = pkey

class TimeStamp():
    def __init__(self, null=False, pkey=False):
        self.name = 'TimeStamp'
        self.null = null
        self.pkey = pkey

class TinyInt():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'TinyInt'
        self.length = length
        self.null = null
        self.pkey = pkey

class Unhandled():
    def __init__(self, name, length, null=False, pkey=False):
        self.name = name
        self.length = length
        self.null = null
        self.pkey = pkey

class UserStamp():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'UserStamp'
        self.length = length
        self.null = null
        self.pkey = pkey

class VarNum():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'VarNum'
        self.length = length
        self.null = null
        self.pkey = pkey

class XMLTYPE():
    def __init__(self, length, null=False, pkey=False):
        self.name = 'XMLTYPE'
        self.length = length
        self.null = null
        self.pkey = pkey
