import argparse

def replaced(line):
    x = line.replace('(', ' ').replace(')', ' ').replace('{', ' ').replace('}', ' ').replace(';', ' ').replace(',', ' ')
    return x

def remove_comment(line):
    n = line.find('//')
    if n >= 0:
        line = '%s\n' % (line[:n])
    return line

class Class:
    pass

def get_ops(infile):
    cobble = Class()
    book = Class()
    cobble.infile = infile
    cobble.books = []
    book.ops = []
    file_in = open(infile, 'rt')
    lines = file_in.readlines()
    file_in.close()
    START = 0
    IN_CLASS = 1
    state = START
    for line in lines:
        line = remove_comment(line)
        if state == IN_CLASS:
            if line.find('};') == 0:
                state = START
                continue
            n = line.find('tCobbler(')
            if n > 0:
                cobble.size = int(line[n+9:].replace('){}', ''))
                continue
            n = line.find('{move')
            if n > 0:
                parts = replaced(line).split()
                if parts[4].find('move') == 0:
                    if int(parts[7]) == cobble.size:
                        continue
                    op = Class()
                    book.ops.append(op)
                    op.name = parts[1][1:]
                    op.type = parts[4][4:]
                    op.pos = parts[6:]
                else:
                    op = Class()
                    book.ops.append(op)
                    op.name = parts[1][1:]
                    op.type = parts[6][4:]
                    op.pos = parts[8:]
                continue
            continue
        if line.find('class') == 0 and line.find('tCobbler') > 0:
            cobble.books.append(book)
            book.ops = []
            parts = line.split()
            book.name = parts[1][1:]
            state = IN_CLASS
            continue
    return cobble

def get_header(cobble, bookname, delim=','):
    result = ''
    tween = ''
    maximum = 0
    if bookname == '':
        for b in cobble.books:
            no = len(b.ops)
            if no > maximum:
                book = b
                no = maximum
    if maximum == 0:
        for book in cobble.books:
            if book.name == bookname:
                break
    for op in book.ops:
        result += '%s%s' % (tween, op.name)
        tween = delim
    return result, book

def display(cobble):
    print (cobble.infile, cobble.size, len(cobble.books))
    for book in cobble.books:
        print ('', book.name, len(book.ops))
        for op in book.ops:
            print ('', '', op.name, op.type, op.pos)

def fix(data, oftype, scale):
    if oftype == 'X':
        data = data.rstrip()
    elif oftype == 'N' and scale > 0:
        while len(data) > scale+1 and data[0] == '0':
            data = data[1:]
        if scale > 0:
            n = len(data) - scale
            data = '%s.%s' % (data[:n], data[n:])
    return data

def check(data, delim):
    if len(data) > 0 and data[0] != '"' and delim in data:
        if delim != '"':
            return '"%s"' % (data)
        else:
            return "'%s'" % (data)
    return data

def process_data(cobble, datafile, class_name, delim):
    header, book = get_header(cobble, class_name, delim)
    print (header)
    file_in = open(datafile, 'rt')
    lines = file_in.readlines()
    file_in.close()
    for line in lines:
        result = ''
        tween = ''
        for op in book.ops:
            if len(op.pos) >= 3:
                offset = int(op.pos[0])
                size = int(op.pos[1])
                scale = int(op.pos[2])
                data = line[offset:offset+size]
                xx = fix(data, op.type, scale)
                result += '%s%s' % (tween, check(xx, delim))
                tween = delim
        print (result)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--delim',  type=str, default=',')
    parser.add_argument('-c', '--class_name',  type=str,
                        default='', help='class-name-without-t')
    parser.add_argument('cobbler_header', help='cobbler.h filename')
    parser.add_argument('datafile',       help='flat-datafile')
    args = parser.parse_args()
    cobbler_header = args.cobbler_header
    datafile = args.datafile
    class_name = args.class_name
    delim = args.delim
    cobble = get_ops(cobbler_header)
    process_data(cobble, datafile, class_name, delim)
    return 0

if __name__ == '__main__':
    rc = main()
    exit(rc)
