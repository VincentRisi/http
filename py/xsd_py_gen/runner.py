import sys, os, importlib

def pathed(dir):
    os.makedirs(dir, exist_ok=True)
    return dir

build_sets = '''\
simon|CommonTypes|CommonTypes
simon|ExternalFndtMsg|ExternalFndtMsg
simon|Weird|Weird
example|GPSPayment|GPSPayment
example|pacs.008.001.08|PACS_008
example|pain.001.001.09|PAIN_001
messageorch/Common|file_message|FILE_MESSAGE
'''.splitlines()

CMAKE_CURRENT_SOURCE_DIR = 'C:/vlab/python/pytools/xsd_py_gen'
CMAKE_CURRENT_BINARY_DIR = 'C:/vlab/python/pytools/xsd_py_gen/out'

def process(build_set):
    dir_name, base_name, make_name = build_set.split('|')
    outPath = pathed(f'{CMAKE_CURRENT_BINARY_DIR}/{dir_name}/py')
    logPath = pathed(f'{CMAKE_CURRENT_BINARY_DIR}/{dir_name}/log')
    inFile  = f'{CMAKE_CURRENT_SOURCE_DIR}/xsds/{dir_name}/{base_name}.xsd'
    inPath  = f'{CMAKE_CURRENT_SOURCE_DIR}/xsds/{dir_name}'
    python_front = f'{CMAKE_CURRENT_SOURCE_DIR}/python_front.py'
    outFile = f'{outPath}/XSD_{make_name}.py'
    logFile = f'{logPath}/{base_name}.log'
    sys.argv = ['xsd_py_gen.py', '-i', f'{inFile}', '-I', f'{inPath}', '-o', f'{outFile}', '-l', f'{logPath}', '-P', f'{python_front}']

import xsd_py_gen
for no in [0,1,2]:
    process(build_sets[no])
    xsd_py_gen.runtime_parms()
    xsd_py_gen.main()

