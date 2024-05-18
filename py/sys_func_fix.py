import glob
import os.path
import shutil

def main():
    files = glob.glob(r'D:\nedbank\sandbox\python3\putty3\router3\*\*.py')
    for file in files:
        d,f = os.path.split(file)
        _,g = os.path.split(d)
        if g in ('intrsys','gps','wallstreet','swift'):
            continue
        ifile = open(file, 'rt')
        lines = ifile.readlines()
        ifile.close()
        line = lines[0]
        if line.find('from INTRINSIC') != 0:
            continue
        print (g, f)
        shutil.move(file, file.replace('.py','.~py'))
        ofile = open(file, 'wt')
        ofile.write('from SYS_FUNCTIONS import *\n')
        ofile.writelines(lines)
        ofile.close()


if __name__ == '__main__':
    main()
