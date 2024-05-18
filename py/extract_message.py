import sys
import os, os.path
import db
import zlib
from datetime import datetime, date, time
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom

def decompress(length, data):
    sign = data[:4]
    if sign == b'\xED\xAC\xAC\xED':
        return "".join(chr(x) for x in zlib.decompress(data[4:]))
    return "".join(chr(x) for x in data)

def load_args():
    import argparse
    try:
        parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter)
        parser.add_argument('-c', '--connect', type=str, help='Connection string in the format of user/pass@db', required=True)
        parser.add_argument('-i', '--type', type=str, default='Message', help='Type of message to extract, can be Message/Stream/Response/Reply')
        parser.add_argument('-f', '--from_date', type=str, help='Specifies to filter extracted messages with date created from a specified date, format is YYYY/MM/DD')
        parser.add_argument('-t', '--to_date', type=str, help='Specifies to filter extracted messages with date created to a specified date, format is YYYY/MM/DD')
        parser.add_argument('-m', '--message', type=int, help='used to specify to extract a specific message id, this will be the primary key based on the type')
        parser.add_argument('-q', '--queue', type=str, help='Use this to filter on the QueueId field on the table')
        parser.add_argument('-s', '--system', type=str, help='Use this to filter on the SourceSysId field on Message Table')
        parser.add_argument('-r', '--stream_type', type=str, help='Use this to filter on the StreamType field on the stream table')
        parser.add_argument('-o', '--output', type=str, help='Specifies the folder where to output the extracted messages')
        parser.description = 'Program to extract MCPE messages/streams/replies from the database into xml files'
        parser.epilog = '''Examples:
Extract all messages after 1 Jan 2018
    extract_messages -c npu/npu@dn29 -f 2018/01/01

Extract all messages from a specific system
    extract_messages -c npu/npu@dn29 -s BANCS

Extract all messages from a specific queue and output into a folder
    extract_messages -c npu/npu@dn29 -q QUEUEX -o extractFolder
                        '''   
        args = parser.parse_args()
        return args, parser
    except Exception as ex:
        print (ex.with_traceback)

def sanitizeString(msg):
    msg = msg.replace('\x00', '')
    return msg

def make_XML(type, id, relatedId, reference, queue, system, message, messageType):
    top = Element(type,{'Id': str(id), 'RelatedId': str(relatedId), 'Reference': reference, 'Queue': queue, 'System': system, 'MessageType':str(messageType)})

    top.text = sanitizeString(message)

    rough_string = tostring(top, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

    return '{0}'.format(tostring(top))

def output(args, row):
    print ('{0} - {1}'.format(row[0], row[1]))

    if args.output != None:
        strV = make_XML(args.type,
                    row[0], 
                    row[1], 
                    row[3],
                    row[4],
                    row[2],
                    decompress(row[5], row[6]),
                    row[7])

        fname = '{0}/{1}.{2}.{3}.xms'.format(args.output, args.type, row[1], row[2])
        print('creating file ' + fname)
        file1 = open(fname,'w') 
        file1.write(strV) 
        file1.close() #to change file access modes 
    else:
        strV = make_XML(args.type,
                    row[0], 
                    row[1], 
                    row[3],
                    row[4],
                    row[2],
                    decompress(row[5], row[6]),
                    row[7])
        print (strV)


def validate(args):
    if args.connect == None:
        print('connection must be specified')
        return None

    if args.type == None:
        print('type must be specified')
        return None

    if args.type != 'Message' and args.type != 'Stream' and args.type != 'Response' and args.type != 'Reply':
        print('type can only be one of the following types (Message, Stream, Response, Reply)')
        return None

    if args.from_date == None and args.to_date == None and args.message == None and args.queue == None:
        print('at least one type of filter must be specified')
        return None


def make_query(args):
    _command = ''
    _T = ''
    if args.type == 'Message':
        _command = 'SELECT M.Id, M.Id, M.SourceSysId, M.Reference, M.QueueId, M.MessageLen, M.MessageData, M.MessageType, \'_\', M.Status, M.DateCreated, M.UsId, M.TmStamp FROM Message M WHERE 1=1 '
        _T = 'M'

    if args.type == 'Stream':
        _command = 'SELECT S.Id, M.Id, M.SourceSysId, S.StreamDescr, S.QueueId, S.MessageLen, S.MessageData, S.MessageType, S.StreamType, S.Status, S.DateCreated, S.UsId, S.TmStamp FROM Message M JOIN Streams S ON S.MessageId = M.Id WHERE 1=1 '
        _T = 'S'

    if args.type == 'Response':
        _command = 'SELECT R.Id, M.Id, M.SourceSysId, R.StreamRef, R.QueueId, R.MessageLen, R.MessageData, R.MessageType, \'_\', R.Status, R.DateCreated, R.UsId, R.TmStamp FROM Message M JOIN Response R ON R.MessageId = M.Id WHERE 1=1 '
        _T = 'R'

    if args.type == 'Reply':
        _command = 'SELECT R.Id, M.Id, M.SourceSysId, \'_\', R.QueueId, R.MessageLen, R.MessageData, R.MessageType, \'_\', R.Status, R.DateCreated, R.UsId, R.TmStamp FROM Message M JOIN Reply R ON R.MessageId = M.Id WHERE 1=1 '
        _T = 'R'

    if args.from_date != None:
        _command += ' AND {0}.DateCreated >= To_Date(\'{1}\', \'YYYY/MM/DD\')'.format(_T, args.from_date)

    if args.to_date != None:
        _command += ' AND {0}.DateCreated <= To_Date(\'{1}\', \'YYYY/MM/DD\')'.format(_T, args.to_date)

    if args.message != None:
        _command += ' AND {0}.Id = \'{1}\''.format(_T, args.message)
                                                               
    if args.system != None:
        _command += ' AND SourceSysId = \'{0}\''.format(args.system)

    if args.queue != None:
        _command += ' AND {0}.QueueId = \'{1}\''.format(_T, args.queue)

    if args.stream_type != None:
        _command += ' AND S.StreamType = \'{0}\''.format(args.stream_type)

    return _command

def extractMessages(dbx, args):
        _command = make_query(args)

        print('Executing query {0}'.format(_command))
        cursor = dbx.connect.cursor()
        cursor.execute(_command, [])
        records = []
        for row in cursor:
            output(args, row)

def main():
    args, parser = load_args()
    if args == None or args.connect == None:
        parser.print_help()
        return 1

    dbx = db.Connection(args.connect)

    extractMessages(dbx, args)

if __name__ == '__main__':
    rc = main()
