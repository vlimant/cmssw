import sys,os,glob
from collections import defaultdict
#filein=sys.argv[1]
#fileout='out.md'

release='_'.join(os.environ['CMSSW_VERSION'].split('_')[0:4])
docs= glob.glob('{}/src/*/*/doc/*.md'.format(os.environ['CMSSW_BASE']))

def LXR(keyword):
    global release
    lxr_url='https://cmssdt.cern.ch/lxr/ident?v+{release}&_i={keyword}'
    return '[{}]({})'.format( keyword , lxr_url.format( release = release, keyword = keyword))

def docObjectAndFile(inDoc):
    return inDoc.split('/')[-1].replace('.md',''),'.'.join(inDoc.split('/')[-4:]).replace('.doc','')

doc_src={}
doc_path={}
doc_done={}
for doc in docs:
    object_name,doc_out = docObjectAndFile(doc)
    doc_path[object_name] = doc_out
    doc_done[object_name] = False
    doc_src[object_name] = doc

out_path='/afs/cern.ch/user/v/vlimant/cernbox/DOCMD/'

allowed_punct=[',','.']
def processMD(doc):
    global doc_path,doc_done,out_path
    object_name,doc_out = docObjectAndFile(doc)
    if doc_done[object_name]: return
    doc_out = out_path+doc_out
    print('Compiling documentation for '+object_name)
    with open(doc) as inputMarkDown, open(doc_out,'w') as outputMarkDown:
        for inline in inputMarkDown:
            for word in inline.split():
                reword = word
                ### Rules to replace cms doc specific syntax
                #linking another class documentation : **<Object name>** for which there is a MD file
                #insert another class documentation : **<Object name>**: for which there is a MD file
                if word.startswith('**') and word.endswith('**:'):
                    objectName = word.replace('**:','').replace('**','')
                    if objectName in doc_path:
                        ##an @@include anchor would be great!!
                        ##reword = '{}:\n@@include[{}.md]({})'.format( LXR(objectName), objectName, doc_path[objectName])
                        reword = '{}:\n'.format(LXR(objectName))
                        ## process the MD if not already there
                        processMD( doc_src[objectName] )
                        # plain insert it here
                        with open(out_path+doc_path[objectName]) as sub_doc:
                            reword += sub_doc.read()
                    else:
                        reword = LXR(objectName)
                #LXR linking : @LXR_<ClassName> or `<ClassName>`
                if word.startswith('@LXR_') or (word.startswith('`') and any([word.endswith('`'+punct) for punct in ['']+allowed_punct])):
                    if any([word.endswith(punct) for punct in allowed_punct]):
                        word = word[:-1]
                    keyword = word.replace('@LXR_','')
                    keyword = word.replace('`','')
                    reword = LXR(keyword)
                #a parameter of the module: __parameter__
                if word.startswith('__') and any ([word.endswith('__'+punct) for punct in ['']+allowed_punct]):
                    if any([word.endswith(punct) for punct in allowed_punct]):
                        word = word[:-1]
                    parameterName = word.replace('__','')
                    ## include the actual value from the module description ?
                    reword = '[{}](aniceurlorsomething)'.format(parameterName)
                    reword = '_{}_'.format( parameterName)
                outputMarkDown.write(reword+' ')
        outputMarkDown.write("\n")
    print('\tDone with ',object_name)
    doc_done[object_name]=True

for doc in docs:
    processMD(doc)

