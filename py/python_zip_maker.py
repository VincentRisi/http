import sys
import os
import os.path
import stat
import glob
import zipfile
import py_compile

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-a', '--always',  action='store_true')
parser.add_argument('-i', '--inputs',  type=str, default='inputs')
parser.add_argument('-l', '--logfile', type=str, default='')
parser.add_argument('-n', '--name',    type=str, default='name')
parser.add_argument('-o', '--outpath', type=str, default='outpath')
parser.add_argument('-s', '--source',  action='store_true')
args = parser.parse_args()
print('Python Zip Maker name:%s inputs:%s outpath:%s' % (args.name, args.inputs, args.outpath))
if len(args.logfile) > 0:
  print (f'See {args.logfile}')
  sys.stdout = open(args.logfile, 'wt')

def lastmod(name):
  if os.path.exists(name) == True:
    info = os.stat(name)
    return info[stat.ST_MTIME], info[stat.ST_SIZE]
  else:
    return 0, 0
    
class Manifest(object):
  def __init__(self, name):
    self.name = name
    print('Creating %s' % (name))
    self.zipper = zipfile.ZipFile(name, 'w', zipfile.ZIP_DEFLATED)

  def close(self):
    self.zipper.close()  
    print('Done')
    
  def compile(self, filename, compname):
    path, tail = os.path.split(compname)
    if os.path.exists(path) == False:
        os.makedirs(path)
    ctime, csize = lastmod(compname)
    ftime, fsize = lastmod(filename)
    if args.always == True or ctime == 0 or ctime < ftime:
      try:
        py_compile.compile(filename, compname)
      except  Exception as inst:
        print(type(inst), inst.args, inst)
        ofile = open(compname, 'wt')
        ofile.close
        return False
    return True
      
  def add_py_to_zip(self, filename):
    path, tail = os.path.split(filename)
    print(' storing %s %s' % (path, tail)) 
    self.zipper.write(filename, tail)

def main():
    zip_file = '%s/%s.zip' % (args.outpath, args.name)
    manifest = Manifest(zip_file)
    infile = open(args.inputs)
    lines = infile.readlines()
    infile.close()
    result = 0
    for line in lines:
        files = line.strip().split(';')
        for file_name in files:
            comp_name = file_name.replace('/source/','/build/').replace('.py', '.pyc')
            rc = manifest.compile(file_name, comp_name)
            if rc == True:
                manifest.add_py_to_zip(comp_name)
                if args.source == True:
                    manifest.add_py_to_zip(file_name)
            else:
                result = 1
                manifest.add_py_to_zip(file_name)
    return result

if __name__ == '__main__':
    exit(main())
