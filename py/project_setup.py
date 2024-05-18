#!/usr/bin/env python
import sys
import os, os.path, stat
system = os.system
environ = os.environ
exists = os.path.exists
getcwd = os.getcwd

from optparse import OptionParser

RED = '\033[31m\033[%dm'
GREEN = '\033[32m\033[%dm'
ORANGE = '\033[33m\033[%dm'
BLUE = '\033[34m\033[%dm'
PURPLE = '\033[35m\033[%dm'
CYAN = '\033[36m\033[%dm'

 
def red(*args):
  text = '%s ' * len(args) % args
  if sys.platform == 'win32':
    print (text)
    return
  print ('%s%s%s' % (RED%1, text, RED%0))

def green(*args):
  text = '%s ' * len(args) % args
  if sys.platform == 'win32':
    print (text)
    return
  print ('%s%s%s' % (GREEN%1, text, GREEN%0))

def orange(*args):
  text = '%s ' * len(args) % args
  if sys.platform == 'win32':
    print (text)
    return
  print ('%s%s%s' % (ORANGE%1, text, ORANGE%0))

def blue(*args):
  text = '%s ' * len(args) % args
  if sys.platform == 'win32':
    print (text)
    return
  print ('%s%s%s' % (BLUE%1, text, BLUE%0))

def purple(*args):
  text = '%s ' * len(args) % args
  if sys.platform == 'win32':
    print (text)
    return
  print ('%s%s%s' % (PURPLE%1, text, PURPLE%0))

def cyan(*args):
  text = '%s ' * len(args) % args
  if sys.platform == 'win32':
    print (text)
    return
  print ('%s%s%s' % (CYAN%1, text, CYAN%0))

class Options(object):
  def __init__(self):
    self.platform  = sys.platform
    parser = OptionParser()
    parser.add_option('-b', '--basedir',     dest='basedir',  default='')
    parser.add_option('-f', '--filename',    dest='filename', default='')
    parser.add_option('-H', '--http',        dest='http',     default='http://%(user)s:%(password)s@168.142.186.250:7990/scm/%(project)s/%(repo)s.git')
    parser.add_option('-s', '--ssh',         dest='ssh',      default='ssh://git@168.142.186.250:7990/%(project)s/%(repo)s.git')
    if self.platform == 'win32':
      parser.add_option('-p', '--password',  dest='password', default='')
      parser.add_option('-u', '--user',      dest='user',     default='')
      parser.add_option('-d', '--depth',     dest='depth',    default=True, action="store_false", help='depth full')
    else:
      parser.add_option('-p', '--password',  dest='password', default='Gps%40ned468') # bloody stupid @ sign in pissword
      parser.add_option('-u', '--user',      dest='user',     default='gpsservice')
      parser.add_option('-d', '--depth',     dest='depth',    default=False, action="store_true", help='depth 1')
    parser.add_option('-v', '--verbose',     dest='verbose',  default=False, action="store_true", help='verbose')
    parser.add_option('-t', '--testing',     dest='testing',  default=False, action="store_true", help='testing')
    (options, args) = parser.parse_args()
    self.args      = args
    self.basedir   = options.basedir.replace('\\', '/')
    self.filename  = options.filename.replace('\\', '/')
    self.password  = options.password
    self.user      = options.user
    self.url       = options.http if len(options.user) > 0 else options.ssh
    self.depth     = options.depth
    self.verbose   = options.verbose
    self.testing   = options.testing
    if len(self.basedir) == 0:
      if self.platform == 'win32':
        self.basedir = 'c:/nedbank'
      elif self.platform == 'linux2':
        self.basedir = '/main/nedbank/bbd/common'  
      else:
        self.basedir = '/main/nedcor/bbd/common'

class Project(object):
  def __init__(self, opts):
    infile = open(opts.filename, 'rt')
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
      fields = line.split()
      if len(fields) >= 2:
        if fields[0] in ['feature', 'bugfix', 'hotfix', 'release', 'custom']:
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

testing = False
verbose = False

def mkdir(dir):
  global testing, verbose
  orange('making dir %s' % (dir))
  if testing == True: return
  if not exists(dir):
    os.makedirs(dir, 0o775)

def chdir(dir):
  global testing, verbose
  purple('changing to dir %s' % (dir))
  if testing == True: return
  os.chdir(dir)

def command(cmd):
  global testing, verbose
  green('%s' % (cmd))
  if testing == True: return
  rc = system(cmd)

def mklink(platform, source, target):
  global testing, verbose
  cyan('linking %s to %s' % (source, target))
  if not exists(target):
    if platform == 'win32':
      command('mklink /j %s %s' % (target.replace('/','\\'), source.replace('/','\\')))
    else:
      command('ln -sf %s %s' % (source, target))

def main():
  global testing, verbose
  kill = False
  opts = Options()
  if len(opts.basedir) == 0:
    red('BASEDIR is not defined')
    kill = True
  if len(opts.filename) == 0:
    red('usage build.py -f filename')
    kill = True
  if exists(opts.filename) == False:
    red('filename %s does not exists' % (opts.filename))
    kill = True
  if len(opts.user) == 0:
    green('using ssh url')
  elif len(opts.password) == 0:
    red('usage -p password as user supplied')
    kill = True
  if kill == True:
    exit(1)
  testing = opts.testing
  verbose = opts.verbose
  proj = Project(opts)
  if proj.error == True:
    for error in proj.error_list:
      red(error)
    exit(1)
  print_args = {}
  print_args['user'] = opts.user
  print_args['password'] = opts.password
  print_args['type'] = proj.type
  print_args['name'] = proj.name
  print_args['basedir'] = opts.basedir
  first = True
  green('## %s %s' % (proj.type, proj.name))
  here = os.getcwd()
  for url in proj.urls:
    is_svn = False
    project = url[0]
    repo = url[1].strip()
    print_args['project'] = project
    print_args['repo'] = repo
    if len(url) == 3:
      if url[2] == '=':
        if print_args['type'] == 'custom':
          branch = '%(name)s' % print_args
        else:  
          branch = '%(type)s/%(name)s' % print_args
      elif url[2] == 'svn':
        branch = 'master'
        is_svn = True
      else:
        branch = url[2]
    else:
      branch = 'master'
    print_args['branch'] = branch
    if repo == 'bbd-stdfront':
      print_args['sandbox'] = '%(basedir)s/sandbox/%(name)s' % print_args
      print_args['projdir'] = '%(basedir)s/%(type)s/%(name)s' % print_args
      print_args['front']   = '%(sandbox)s/%(repo)s' % print_args
      print_args['srcdir']  = '%(front)s/source' % print_args
      print_args['blddir']  = '%(front)s/build' % print_args
      mkdir("%(sandbox)s" % print_args)
      mkdir("%(projdir)s" % print_args)
      chdir("%(sandbox)s" % print_args)
    branch = '%(branch)s' % print_args
    url = opts.url % print_args
    if not exists(repo):
      depth = '--depth 1 ' if opts.depth == False else ''
      command("git clone -b %s %s%s" % (branch, depth, url))
    if repo == 'bbd-stdfront':
      mklink(opts.platform, '%(sandbox)s/%(repo)s' % print_args, '%(projdir)s/%(repo)s' % print_args)
    else:
      mklink(opts.platform, '%(sandbox)s/%(repo)s' % print_args, '%(srcdir)s/%(repo)s' % print_args)
    if first == True:
      first = False
      mkdir("%(srcdir)s"  % print_args)
      mkdir("%(blddir)s"  % print_args)
      if opts.platform == 'win32':
        pullfile = open("%(sandbox)s/pull.cmd" % print_args, "wt")
      else:
        pullfile = open("%(sandbox)s/pull.sh" % print_args, "wt")
        os.chmod("%(sandbox)s/pull.sh" % print_args, stat.S_IRWXG|stat.S_IRWXU|stat.S_IROTH)
    if is_svn == True and opts.platform == 'win32':
      pullfile.write('echo %(repo)s is an svn based repo\n')
    pullfile.write('pushd %(repo)s\ngit pull\npopd\n' % print_args)
  if opts.platform != 'win32':
    pullfile.write('cd %(projdir)s/bbd-stdfront/build\n' % print_args)
    pullfile.write('if [ -r CMakeCache.txt ]\n')
    pullfile.write('then\n')
    pullfile.write('  cmake ..\n')
    pullfile.write('else\n')
    pullfile.write('  cd ..\n')
    pullfile.write('  ./cmake.sh ..\n')
    pullfile.write('fi\n')
  else:
    pullfile.write('pause\n')
  pullfile.close()
  os.chdir(here)

if __name__ == '__main__':
  main()
