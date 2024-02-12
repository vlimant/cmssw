import sys
filein=sys.argv[1]
fileout='out.md'
release = 'CMSSW_13_X_0'

lxr_url='https://cmssdt.cern.ch/lxr/ident?v+{release}&_i={keyword}'
with open(filein) as inputMarkDown, open(fileout,'w') as outputMarkDown:
    for inline in inputMarkDown:
        for word in inline.split():
            reword = word
            ### Rules to replace cms doc specific syntax
            #linking another class documentation
            
            #LXR linking : @LXR_<ClassName> or `<ClassName>`
            if word.startswith('@LXR_') or (word.startswith('`') and word.endswith('`')):
                keyword = word.replace('@LXR_','')
                keyword = word.replace('`','')
                reword = '[{}]({})'.format( keyword , lxr_url.format( release = release, keyword = keyword))
            #a parameter of the module: __parameter__
            if word.startswith('__') and word.endswith('__'):
                paramerName = word.replace('__','')
                reword = '[{}](aniceurlorsomething)'.format(paramerName)
            outputMarkDown.write(reword+' ')
    outputMarkDown.write("\n")

