import sys
import os
import os.path
import stat

system  = os.system
environ = os.environ
exists  = os.path.exists
getcwd  = os.getcwd

platform  = sys.platform

BLACK = '\033[30m\033[%dm'
RED = '\033[31m\033[%dm'
GREEN = '\033[32m\033[%dm'
ORANGE = '\033[33m\033[%dm'
BLUE = '\033[34m\033[%dm'
PURPLE = '\033[35m\033[%dm'
CYAN = '\033[36m\033[%dm'
WHITE = '\033[37m\033[%dm'

def printer(colour):
  def model(*args):
    text = '%s ' * len(args) % args
    if sys.platform == 'win32':
      print(text)
      return
    print('%s%s%s' % (colour % (1), text, colour % (0)))
  return model

red = printer(RED)
green = printer(GREEN)
orange = printer(ORANGE)
blue = printer(BLUE)
purple = printer(PURPLE)
cyan = printer(CYAN)

import argparse
arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('-b', '--basedir',     type=str, default='')
arg_parser.add_argument('-f', '--filename',    type=str, default='')
arg_parser.add_argument('-l', '--libfrontdir', type=str, default='fronts')
if platform[0:5] == 'linux': # while we dont have the ability to curl to nexus
    arg_parser.add_argument('-n', '--nexus',       type=str, default='nexus/repository/binaries-bbd')
else:
    arg_parser.add_argument('-n', '--nexus',       type=str, default='https://nexus.nednet.co.za/repository/binaries-bbd')
arg_parser.add_argument('-H', '--http',        type=str, default='http://%(user)s:%(password)s@168.142.186.250:7990/scm/%(project)s/%(repo)s.git')
arg_parser.add_argument('-s', '--ssh',         type=str, default='ssh://git@nedstash.bpdev2000.net:7999/%(project)s/%(repo)s.git')
if platform == 'win32':
  arg_parser.add_argument('-p', '--password',  type=str, default='')
  arg_parser.add_argument('-u', '--user',      type=str, default='')
  arg_parser.add_argument('-d', '--depth',     action='store_false')
else:
  arg_parser.add_argument('-p', '--password',  type=str, default='Gps%40ned468')
  arg_parser.add_argument('-u', '--user',      type=str, default='gpsservice')
  arg_parser.add_argument('-d', '--depth',     action='store_true')
arg_parser.add_argument('-V', '--version',     type=int)
arg_parser.add_argument('-v', '--verbose',     action='store_true')
arg_parser.add_argument('-t', '--testing',     action='store_true')
arg_parser.add_argument('-z', '--zipbased',    action='store_true')
args = arg_parser.parse_args()

if len(args.basedir) == 0:
  if platform == 'win32':
    args.basedir = 'c:/nedbank'
  elif platform[0:5] == 'linux':
    args.basedir = '/main/nedbank/bbd/common'  
  else:
    args.basedir = '/main/nedcor/bbd/common'

class Project(object):
  def __init__(self, args):
    infile = open(args.filename, 'rt')
    lines = infile.readlines()
    infile.close()
    self.error = False
    self.error_list = []
    self.urls = []
    for line in lines:
      line = line.strip().lower()
      n = line.find('#')
      if n >= 0:
        line = line[:n]
      if len(line) == 0:
        continue
      if 'bbd-stdfront' in line:
        green('libfront replaces bbd-stdfront and bbd-libs')
        continue
      if 'bbd-libs' in line:
        continue
      if 'bbd-cslibs' in line:
        green('*** consider using nuget to obtain cslibs stuff ***')
      fields = line.split()
      if len(fields) >= 2:
        if fields[0] in ['feature', 'bugfix', 'hotfix', 'release', 'custom', 'general']:
          self.type = fields[0]
          self.name = fields[1]
        else:
          self.error = True
          self.error_list.append('Invalid project line %s' % (repr(line)))
        continue
      fields = line.split('.')
      if not len(fields) in [2, 3]:
        self.error = True
        self.error_list.append('Invalid url line %s' % (repr(line)))
        continue
      self.urls.append(fields)

def mkdir(dir):
  testing, verbose = args.testing, args.verbose
  orange('making dir %s' % (dir))
  if testing == True: return
  if not exists(dir):
    os.makedirs(dir, 0o775)

def chdir(dir):
  mkdir(dir)
  testing, verbose = args.testing, args.verbose
  purple('changing to dir %s' % (dir))
  if testing == True: return
  os.chdir(dir)

def command(cmd):
  testing, verbose = args.testing, args.verbose
  green('%s' % (cmd))
  if testing == True: return
  rc = system(cmd)

def main():
  testing, verbose = args.testing, args.verbose
  kill = False
  if len(args.basedir) == 0:
    red('basedir is not defined')
    kill = True
  if len(args.libfrontdir) == 0:
    red('libfrontdir is not defined')
    kill = True
  if len(args.filename) == 0:
    red('usage:\n  python libfront-setup.py -f filename ...')
    kill = True
  if exists(args.filename) == False:
    red('filename %s does not exists' % (args.filename))
    kill = True
  if len(args.user) == 0:
    url = args.ssh
    green(f'using ssh url {args.ssh}')
  elif len(args.password) == 0:
    red(f'requires -p password as -u {args.user} supplied')
    kill = True
  else: 
    url = args.http
    green(f'using http url {args.http}')
  if args.version is None or args.version < 1:
    red('-V version is required and cannot be less than 1')
    kill = True
  if kill == True:
    exit(1)
  proj = Project(args)
  if proj.error == True:
    for error in proj.error_list:
      red(error)
    exit(1)
  print_args = {}
  print_args['user'] = args.user
  print_args['password'] = args.password
  print_args['type'] = proj.type
  print_args['name'] = proj.name
  print_args['basedir'] = args.basedir
  print_args['version'] = args.version
  print_args['libfrontdir'] = args.libfrontdir
  print_args['nexus'] = args.nexus
  first = True
  green('## %s %s' % (proj.type, proj.name))
  if platform == 'win32':
    print_args['os'] = 'w32'
  elif platform[0:5] == 'linux':
    print_args['os'] = 'linux'  
  else:
    print_args['os'] = 'aix'
  here = os.getcwd()
  if args.zipbased == True:
    print_args['zipname'] = 'libfront_v%(version)04d.zip' % print_args
    print_args['toolsdir'] = '%(basedir)s/%(type)s/%(name)s/libfront/build' % print_args
    mkdir('%(toolsdir)s/' % print_args)
    if platform[0:5] == 'linux':  # while we dont have the ability to curl to nexus
        command("cp %(casedir)/%(nexus)s/%(libfrontdir)s/libfront_%(os)s_v%(version)04d.zip %(toolsdir)s/%(zipname)s" % print_args)
    else:
        command("curl %(nexus)s/%(libfrontdir)s/libfront_%(os)s_v%(version)04d.zip --insecure -o %(toolsdir)s/%(zipname)s" % print_args)
    command('unzip %(toolsdir)s/%(zipname)s -d %(basedir)s/%(type)s/%(name)s' % (print_args))
  else:
    chdir('%(basedir)s/%(type)s/%(name)s' % print_args)
    if not exists('libfront'):
      print_args['project'] = 'com'
      print_args['repo'] = 'libfront'
      print_args['front'] = url % print_args
      command("git clone -b v%(version)04d --depth 1 %(front)s" % print_args)
  chdir('%(basedir)s/%(type)s/%(name)s/libfront/source' % print_args)
  for url_list in proj.urls:
    project = url_list[0]
    repo = url_list[1].strip()
    print_args['project'] = project
    print_args['repo'] = repo
    if len(url_list) == 3:
      if url_list[2] == '=':
        if print_args['type'] in ['custom', 'general']:
          branch = '%(name)s' % print_args
        else:  
          branch = '%(type)s/%(name)s' % print_args
      else:
        branch = url_list[2]
    else:
      branch = 'master'
    clone_url = url % print_args
    if not exists(repo):
      if print_args['type'] != 'general':
        depth = '--depth 1 ' if args.depth == False else ''
      else:
        depth = ''
      command("git clone -b %s %s%s" % (branch, depth, clone_url))
    print_args['branch'] = branch
    if first == True:
      first = False
      if platform == 'win32':
        pullfile = open("pull.cmd" % print_args, "wt")
      else:
        pullfile = open("pull.sh" % print_args, "wt")
        os.chmod("pull.sh" % print_args, stat.S_IRWXG|stat.S_IRWXU|stat.S_IROTH)
    pullfile.write('pushd %(repo)s\ngit pull\npopd\n' % print_args)
  if platform != 'win32':
    pullfile.write('cd %(basedir)s/%(type)s/%(name)s/libfront\n' % print_args)
    pullfile.write('mkdir -p build\n' % print_args)
    pullfile.write('cd build\n' % print_args)
    pullfile.write('if [ -r CMakeCache.txt ]\n')
    pullfile.write('then\n')
    pullfile.write('  cmake ../source\n')
    pullfile.write('else\n')
    pullfile.write('  cd ../source\n')
    pullfile.write('  ./cmake.sh\n')
    pullfile.write('fi\n')
  else:
    pullfile.write('pause\n')
  pullfile.close()
  os.chdir(here)

if __name__ == '__main__':
  main()

