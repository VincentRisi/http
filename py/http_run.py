import glob, os.path

filepath = r'C:\nedbank\custom\python3\libfront\source\mcpe\putty3\idl2\accuity\httpserver\tests'

def main():
    files = glob.glob(f'{filepath}\\*.*')
    for file in files:
        _, base = os.path.split(file)
        name, ext  = os.path.splitext(base)
        ext = ext[1:]
        if ext in ['sh', 'post', 'patch', 'put', 'option', 'head']:
            with open(file, 'rt') as ifile:
                print (f"{name.replace('-','_')}_{ext} = '''\\n")
                lines = ifile.readlines()
                for line in lines:
                    line = line.rstrip()
                    print (line.replace('echo','#').replace('curl','##'))
                print ("'''\n")

if __name__ == '__main__':
    main()

