import sys
import os
import argparse

options_service = '''
{Service}%(service)s
{Timeout}%(timeout)s
{Metrics}%(metrics)s'''

options_procs = '''
{NoProcs}%(noprocs)s
{Debug}%(debug)s'''

options_log = '''
{Log}%(logdir)s/%(name)shttp.log
{LogLevel}%(loglevel)s
{LogReceive}%(logreceive)s
{LogTransmit}%(logtransmit)s
{LogDisplay}%(logdisplay)s'''

options_restart = '''
{WaitForChildren}%(waitforchildren)s
{RestartCount}%(restartcount)s'''

options_ssl = '''
{UseSSL}%(usessl)s
{CertFile}%(certfile)s
{CAPath}%(capath)s
{CAFile}%(cafile)s
{KeyFile}%(keyfile)s
{KeyPassword}%(keypassword)s'''

databasesection = '''
[DataBase]
{BinFile}%(bindir)s/%(name)s.bin
{Connection}%(connection)s
{Timeout}%(dbtimeout)s'''

db1section = '''
[%(db1)s]
{BinFile}%(bindir)s/%(db1name)s.bin
{Connection}%(db1connect)s'''

db2section = '''
[%(db2)s]
{BinFile}%(bindir)s/%(db2name)s.bin
{Connection}%(db2connect)s'''

db3section = '''
[%(db3)s]
{BinFile}%(bindir)s/%(db3name)s.bin
{Connection}%(db3connect)s'''

db4section = '''
[%(db4)s]
{BinFile}%(bindir)s/%(db4name)s.bin
{%Connection}%(db4connect)s'''

parser = argparse.ArgumentParser()
parser.add_argument('--platform', type=str)
parser.add_argument('--name', type=str)
parser.add_argument('--service', type=str)
parser.add_argument('--logdir', type=str)
parser.add_argument('--bindir', type=str)
parser.add_argument('--connect', type=str)
parser.add_argument('--outfile', type=str, default='')
parser.add_argument('--timeout', type=str, default='30000')
parser.add_argument('--noprocs', type=str, default='$(NoProcs)')
parser.add_argument('--debug', type=str, default='$(Debug)')
parser.add_argument('--loglevel', type=str, default='$(LogLevel)')
parser.add_argument('--logreceive', type=str, default='$(LogReceive)')
parser.add_argument('--logtransmit', type=str, default='$(LogTransmit)')
parser.add_argument('--logdisplay', type=str, default='$(LogDisplay)')
parser.add_argument('--metrics', type=str, default='$(Metrics)')
parser.add_argument('--waitforchildren', type=str, default='$(WaitForChildren)')
parser.add_argument('--restartcount', type=str, default='$(RestartCount)')
parser.add_argument('--usessl', type=str, default='0')
parser.add_argument('--certfile', type=str, default='')
parser.add_argument('--capath', type=str, default='')
parser.add_argument('--cafile', type=str, default='')
parser.add_argument('--keyfile', type=str, default='')
parser.add_argument('--keypassword', type=str, default='')
parser.add_argument('--dbtimeout', type=str, default='0')
parser.add_argument('--db1', type=str, default='')
parser.add_argument('--db1name', type=str, default='')
parser.add_argument('--db1connect', type=str, default='')
parser.add_argument('--db2', type=str, default='')
parser.add_argument('--db2name', type=str, default='')
parser.add_argument('--db2connect', type=str, default='')
parser.add_argument('--db3', type=str, default='')
parser.add_argument('--db3name', type=str, default='')
parser.add_argument('--db3connect', type=str, default='')
parser.add_argument('--db4', type=str, default='')
parser.add_argument('--db4name', type=str, default='')
parser.add_argument('--db4connect', type=str, default='')
parser.add_argument('--extrafile', type=str, default='')
try:
    args = parser.parse_args()
except ArgumentError as ae:
    print (ae.args)
    print (sys.argv)
    exit (1)

platform  = args.platform

if not platform in ['aix', 'linux', 'win32', 'python']:
    print (f'Invalid platform {platform} not aix, linux, win32 or python.')
    exit (1)

named = dict()
named['name'] = args.name
named['service'] = args.service
named['timeout'] = args.timeout
named['noprocs'] = args.noprocs
named['debug'] = args.debug
named['logdir'] = args.logdir
named['loglevel'] = args.loglevel
named['logreceive'] = args.logreceive
named['logtransmit'] = args.logtransmit
named['logdisplay'] = args.logdisplay
named['metrics'] = args.metrics
named['waitforchildren'] = args.waitforchildren
named['restartcount'] = args.restartcount
named['usessl'] = args.usessl
named['certfile'] = args.certfile
named['capath'] = args.capath
named['cafile'] = args.cafile
named['keyfile'] = args.keyfile
named['keypassword'] = args.keypassword
named['bindir'] = args.bindir
named['connection'] = args.connect
named['dbtimeout'] = args.dbtimeout
named['db1'] = args.db1
named['db1name'] = args.db1name
named['db1connect'] = args.db1connect
named['db2'] = args.db2
named['db2name'] = args.db2name
named['db2connect'] = args.db2connect
named['db3'] = args.db3
named['db3name'] = args.db3name
named['db3connect'] = args.db3connect
named['db4'] = args.db4
named['db4name'] = args.db4name
named['db4connect'] = args.db4connect

if len(args.outfile) > 0:
    ofile = open(args.outfile, 'wt')
    sys.stdout = ofile

print ('[Server Options]')
if platform in ['aix', 'linux', 'win32']:
    print (options_service % named)
if platform in ['aix', 'linux']:
    print (options_procs % named)
if platform in ['aix', 'linux', 'win32', 'python']:
    print (options_log % named)
if platform  in ['aix', 'linux']:
    print (options_restart % named)
if platform  in ['aix', 'linux', 'win32']:
    if args.usessl == '1':
        print (options_ssl % named)
if platform in ['aix', 'linux', 'win32']:
    print (databasesection % named)
if args.db1 != '':
    print (db1section % named)
if args.db2 != '':
    print (db2section % named)
if args.db3 != '':
    print (db3section % named)
if args.db4 != '':
    print (db4section % named)
if args.extrafile != '':
    with open(args.extrafile, 'rt') as extrafile:
        lines = extrafile.read()
        print (lines)    
