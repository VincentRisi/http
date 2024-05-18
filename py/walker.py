import os, os.path, shutil, filecmp, sys

import argparse

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('-w', '--win64bit',  type=str, default=r'D:\sars\sandbox\win64bit')
arg_parser.add_argument('-p', '--python37',  type=str, default=r'D:\sars\sandbox\python37')
arg_parser.add_argument('-l', '--logfile',   type=str, default='walker.log')
arg_parser.add_argument('-c', '--log2to3',   type=str, default='changes.log')
args = arg_parser.parse_args()

outlog = open(args.logfile, 'wt')
sys.stdout = outlog

win64bit = args.win64bit
python37 = args.python37

def rem_dirs():
    print ('rem_dirs()')
    for dir in ['changes', 'win64bit', 'python37']:
        shutil.rmtree(dir)

def loadup(fromdir, ext_list, file_list):
    print(f'loadup({fromdir}, {ext_list}, {file_list})');outlog.flush()
    for root, dirs, files in os.walk(fromdir):
        for file in files:
            if 'release' in root or 'release' in file:
                continue
            _, ext = os.path.splitext(file)
            if ext in ext_list:
                newdir = root[16:]
                os.makedirs(newdir, exist_ok=True)
                shutil.copyfile(f'{root}\\{file}', f'{newdir}\\{file}')

def runload():
    print ("runload()");outlog.flush()
    ext_list = ['.si','.py', '.im', '.ic', '.ib', '.idl', '.idl2', 'cmake']
    file_list = ['CMakeLists.txt']
    rem_dirs()
    loadup(win64bit, ext_list, file_list)
    loadup(python37, ext_list, file_list)

def makedirs(changes3dir, changesdir, scriptsdir):
    print (f'makedirs({changes3dir}, {changesdir}, {scriptsdir})');outlog.flush()
    os.makedirs(scriptsdir, exist_ok=True)
    os.makedirs(changes3dir, exist_ok=True)
    os.makedirs(changesdir, exist_ok=True)

def run2to3(fromfile, file, changes3dir, changesdir, scriptsdir):
    print (f'run2to3({fromfile}, {file}, {changes3dir}, {changesdir}, {scriptsdir})');outlog.flush()
    command = rf'c:\Python37\python c:\Python37\tools\scripts\2to3.py -W -n -o {changes3dir} {fromfile} >> {args.log2to3}'
    makedirs(changes3dir, changesdir, scriptsdir)
    os.system(command)
    shutil.copyfile(fromfile, f'{changesdir}//{file}')
    shutil.copyfile(fromfile, f'{scriptsdir}//{file}')

def gen_source(fromfile, gendir, tofile, genfile):
    shutil.copyfile(fromfile, tofile)
    os.makedirs(gendir, exist_ok=True)
    shutil.copyfile(fromfile, genfile)

def check2to3(fromdir='win64bit', todir='python37'):
    print (f'check2to3({fromdir}, {todir})');outlog.flush()
    for root, dirs, files in os.walk(fromdir):
        for file in files:
            _, ext = os.path.splitext(file)
            if ext == '.py' and '\\scripts\\' in root:
                scriptsdir = root.replace(fromdir, todir)
                changesdir = root.replace(fromdir, 'changes')
                change3dir = changesdir.replace('\\scripts\\', '\\scripts3\\')
                fromfile = f'{root}\\{file}'
                tofile = fromfile.replace(fromdir, todir)
                if os.path.exists(tofile):
                    if filecmp.cmp(fromfile, tofile, False) == False:
                        run2to3(fromfile, file, change3dir, changesdir, scriptsdir)
                else:
                    run2to3(fromfile, file, change3dir, changesdir, scriptsdir)
            elif ext in ['.si', '.ib', '.idl', '.idl2', '.cmake']:
                gendir = root.replace(fromdir, 'changes')
                fromfile = f'{root}\\{file}'
                tofile = fromfile.replace(fromdir, todir)
                genfile = fromfile.replace(fromdir, 'changes')
                if os.path.exists(tofile):
                    if filecmp.cmp(fromfile, tofile, False) == False:
                        gen_source(fromfile, gendir, tofile, genfile)
                else:
                    gen_source(fromfile, gendir, tofile, genfile)

def list_changes():
    for root, dirs, files in os.walk('changes'):
        for file in files:
            print (f'{root}\\{file}');outlog.flush()

runload()
check2to3()
list_changes()


