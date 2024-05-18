import sys

def process_aix(x):
    sys.stdout = open('aix%s.err' % (x), 'wt')

    with open('aix%s.log' % (x)) as ifile:
        lines = ifile.readlines()

    savelines = []

    skipnext = False
    for line in lines:
        line = line.strip()
        if '(continuing)' in line:
            continue
        if skipnext == True:
            skipnext = False
            continue
        if len(line) > 20:
            if line[:8] == 'Target "':
                if '" is up to date.' in line:
                    savelines = []
                    skipnext = True
                    continue
                elif '" did not make' in line:
                    for saved in savelines:
                        print (saved)
                    print (line)
                    print ('===***=== === === === === === === === === ===\n')
                    savelines = []
                    skipnext = True
                    continue
        savelines.append(line)
    sys.stdout.flush()

def process_linux(x):
    sys.stdout = open('linux%s.err' % (x), 'wt')

    with open('linux%s.log' % (x)) as ifile:
        lines = ifile.readlines()

    savelines = []

    for line in lines:
        line = line.strip()
        if len(line) > 20:
            if '%] Built target' in line:
                savelines = []
                continue
            elif 'not remade because of errors.' in line:
                for saved in savelines:
                    print (saved)
                print (line)
                print ('===***=== === === === === === === === === ===\n')
                savelines = []
                continue
            savelines.append(line)
    sys.stdout.flush()

#for x in ['25','27','37']:
#    process_aix (x)
#    process_linux (x)
process_linux('37')

