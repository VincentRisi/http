ATTRIB, PSEUDO, ASIS = range(3)

class RecordType:
    isNew: str
    column: (str,ATTRIB)
    whizz: (str,PSEUDO)
    def __init__(self):
        self.isNew = ''
        self.whizz = ''
        self.column = ''

class MsgTableType:
    record: [RecordType]
    record2: list[RecordType]
    def __init__(self):
        self.record = []

rt = RecordType()
for annote in rt.__annotations__:
  print (annote, type(rt.__annotations__[annote]))

mtt = MsgTableType()
for annote in mtt.__annotations__:
  print (annote, mtt.__annotations__[annote], type(mtt.__annotations__[annote]))

class Banana:
    def __init__(self):
        self.fred: tuple[str, ATTRIB] = [1.5]
        self.kiss: (str, ATTRIB) = '1234'

b = Banana()
print (repr(b.fred), type(b.fred), repr(b.kiss), type(b.kiss))

