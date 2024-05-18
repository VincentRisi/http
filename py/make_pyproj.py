import sys
import glob
import os
import os.path
import stat
import uuid
import argparse
import importlib

top_format = r'''
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="4.0">
  <PropertyGroup>
    <Configuration Condition=" '$(Configuration)' == '' ">Debug</Configuration>
    <SchemaVersion>2.0</SchemaVersion>
'''

guid_format = r'''
    <ProjectGuid>{%(guid)s}</ProjectGuid>
    <ProjectHome>.</ProjectHome>
    <StartupFile>Router36.py</StartupFile>
    <SearchPath>%(search)s</SearchPath>
    <WorkingDirectory>.</WorkingDirectory>
    <OutputPath>.</OutputPath>
    <Name>%(name)s</Name>
    <RootNamespace>%(name)s</RootNamespace>
    <IsWindowsApplication>False</IsWindowsApplication>
    <InterpreterId>Global|PythonCore|3.6-32</InterpreterId>
    <LaunchProvider>Standard Python launcher</LaunchProvider>
    <EnableNativeCodeDebugging>False</EnableNativeCodeDebugging>
    <Environment>NPUDBLOGON=npud00/control@mn29
PYTHONPATH=./build/pyd/dev;./intrsys;./build/py
PUTTYBIN=./build/bin
PUTTYLOG=./build/bin
ROUTE_PLAN_TYPE=Test
LOG_LEVEL=Debug
LOG_DISPLAY=1</Environment>
    <InterpreterArguments>
    </InterpreterArguments>
    <CommandLineArguments>-c router3.ini -q %(queue)s -p %(script)s</CommandLineArguments>
  </PropertyGroup>
  <PropertyGroup Condition=" '$(Configuration)' == 'Debug' ">
    <DebugSymbols>true</DebugSymbols>
    <EnableUnmanagedDebugging>false</EnableUnmanagedDebugging>
  </PropertyGroup>
  <PropertyGroup Condition=" '$(Configuration)' == 'Release' ">
    <DebugSymbols>true</DebugSymbols>
    <EnableUnmanagedDebugging>false</EnableUnmanagedDebugging>
  </PropertyGroup>
  <ItemGroup>
'''

depend_format = r'''
    <Compile Include="%(folder)s\%(file)s.py" />
'''

link_format = r'''
    <Compile Include="build\py\%(link_name)s.py">
      <Link>intrsys\%(link_name)s.py</Link>
    </Compile>
'''

source_format = r'''
    <Compile Include="Router36.py" />
  </ItemGroup>
  <ItemGroup>
    <InterpreterReference Include="Global|PythonCore|3.6-32" />
  </ItemGroup>
  <ItemGroup>
'''

folders_format = r'''
    <Folder Include="%(folder)s\" />
'''

bottom_format = r'''
  </ItemGroup>
  <Import Project="$(MSBuildExtensionsPath32)\Microsoft\VisualStudio\v$(VisualStudioVersion)\Python Tools\Microsoft.PythonTools.targets" />
  <!-- Uncomment the CoreCompile target to enable the Build command in
       Visual Studio and specify your pre- and post-build commands in
       the BeforeBuild and AfterBuild targets below. -->
  <!--<Target Name="CoreCompile" />-->
  <Target Name="BeforeBuild">
  </Target>
  <Target Name="AfterBuild">
  </Target>
</Project>
'''

parser = argparse.ArgumentParser()
parser.add_argument('-a', '--all_genned', action='store_true', help='Specifies whether to include ALL the generated py files from portal')
parser.add_argument('-i', '--include_path', type=str, default='../router3', help='List of include paths to add to the project')
parser.add_argument('-g', '--include_genned', type=str, default='../build/py', help='Path to find the generated py files')
parser.add_argument('-q', '--queue', action='store_true', help='Use the queue name from the script queues definition to identify all dependancies')
parser.add_argument('script', type=str, required=True, help='Name of the folder to use when generating the project')
parser.add_argument('rest', nargs=argparse.REMAINDER)
args = parser.parse_args()

lines = []
genned_used = []

def extract_used(file):
    ifile = open(file, 'rt')
    lines = ifile.readlines()
    ifile.close()
    for line in lines:
        if ' DB_' in line:
            p = line.find(' DB_')
            next = line[p:].split()
            if not next[0] in genned_used:
                genned_used.append(next[0])
            continue
        if ' I3_' in line:
            p = line.find(' I3_')
            next = line[p:].split()
            if not next[0] in genned_used:
                genned_used.append(next[0])
            continue
        if ' XSD_' in line:
            p = line.find(' XSD_')
            next = line[p:].split()
            if not next[0] in genned_used:
                genned_used.append(next[0])
            continue

def reformat(name):
    arr = name.lower().split('_')
    if len(arr) == 1:
        return name.capitalize()
    result = ''
    for entry in arr:
        result += entry.capitalize()
    return result

def main():
    print('importing module {0}'.format(args.script))
    mod = importlib.import_module('{0}.DEPENDS'.format(args.script))   
    depends = mod.depends()
    lines.extend(top_format.splitlines()[1:])
    guid = dict()
    guid['guid'] = uuid.uuid4()
    search = ""
    for i, depend in enumerate(depends.systems()):
        if i > 0:
            search += ';'
        search += '.\\' + depend
    guid['search'] = search
    if args.queue == True:
        guid['name'] = reformat(script.queue_name.replace('-','_'))
    else:
        guid['name'] = reformat(depends.name())
    project_name = guid['name']
    # print ('project: {0}\nname: {1}\nroute_plan: {2}\nqueue: {3}\ndepends: {4}\nuuid: {5}'.format(project_name, script.name, script.route_plan_name, script.queue_name, script.depends, guid['guid']))
    
    guid['start'] = args.script
    firstQueue = depends.queues()[0]
    guid['script'] = depends.queue_mappings()[firstQueue]
    guid['queue'] = firstQueue

    expand = guid_format % (guid)
    lines.extend(expand.splitlines()[1:])
    include_path = args.include_path.replace('/','\\')
    for depend in depends.systems():
        depend_files = glob.glob(r'%s\%s\*.py' % (include_path, depend))
        for file in depend_files:
            extract_used(file)
            vars = dict()
            d, b = os.path.split(file)
            n, e = os.path.splitext(b)
            vars['folder'] = depend
            vars['file'] = n
            expand = depend_format % vars
            lines.extend(expand.splitlines()[1:])
    include_genned = args.include_genned.replace('/','\\')
    genned_files = glob.glob(r'%s\*.py' % (include_genned))
    for file in genned_files:
        link = dict()
        d, b = os.path.split(file)
        n, e = os.path.splitext(b)
        if args.all_genned == False:
            if not n in genned_used:
                continue
        link['link_name'] = n
        expand = link_format % link
        lines.extend(expand.splitlines()[1:])
    lines.extend(source_format.splitlines()[1:])
    for depend in depends.systems():
        vars = dict()
        vars['folder'] = depend
        expand = folders_format % vars
        lines.extend(expand.splitlines()[1:])
    lines.extend(bottom_format.splitlines()[1:])
    ofile = open(r'%s\%s.pyproj' % (include_path, project_name), 'wt')
    for line in lines:
        ofile.write('%s\n' % (line))
    print ('done')

if __name__ == '__main__':
    main()
