import sctokenizer
import numpy as np
import hashlib
import random
import bisect

def fileTokenizer(file, langauge):
    tokens = sctokenizer.tokenize_file(filepath=file, lang=langauge)
    readfile = open(file, 'r')
    Lines = readfile.readlines()
    countLine = 0
    countTokens=0
    replacedVariables=np.array([])
    tokensperLine=[0]
    nrTokens=[0]*len(Lines)
    for token in tokens:
        nrTokens[token.line-1]=nrTokens[token.line-1]+1
        #if(token.token_type==token.token_type.IDENTIFIER):
            #replacedVariables=np.append(replacedVariables,'v')
        if(token.token_type==token.token_type.STRING):
            replacedVariables=np.append(replacedVariables,'s')
        else:
            replacedVariables=np.append(replacedVariables,token.token_value)
    sum=0
    for i in nrTokens:
        sum+=i
        tokensperLine.append(sum)
    return [replacedVariables.tolist(), tokensperLine]


def fileHashKgrammer(replacedVariables,k):# k defines the number of grams
    i=0 #begein of string
    kElements=np.array([]) # filed with every k elements from the vector with replaces variables so i
    kElementsVector=np.array([])
    max=len(replacedVariables)-k+1
    while i<max:
        for j in range(i,k+i):
            kElements=np.append(kElements,replacedVariables[j])
        kElementsVector=np.append(kElementsVector,''.join(kElements))
        i+=1
        kElements=np.array([])
    hashkElementsVector=np.array([])
    for i in kElementsVector:
        hashkElementsVector=np.append(hashkElementsVector,int(hashlib.md5(i.encode()).hexdigest(),16))
    return hashkElementsVector

def winnowing (hashkElementsVector,w):
    arr=np.array([])
    p=hashkElementsVector.size
    poz=0
    while(poz<p-w+1):
        arr=np.append(arr,hashkElementsVector[poz:poz+w])
        poz+=1
    #min val for first row
    arr=arr.reshape(-1,w)
    fingerprints=np.array([])
    #stailesc minF ca fiind min din prima fereastra
    rowMin=0
    colMin=0
    minLocal=arr[0][0]
    for l in range(0,len(arr[0])):
            if(minLocal>arr[0][l]):
                minLocal=arr[0][l]
                colMin=l
    minF=minLocal
    fingerprints=np.append(fingerprints,minF)
    fingerprints=np.append(fingerprints,colMin)
    for k in range(1,len(arr)):
        minTimes=0
        rowMin=0
        colMin=0
        minLocal=arr[k][0]
        for l in range(0,len(arr[k])):
                if(minLocal>arr[k][l]):
                    minLocal=arr[k][l]
                    minTimes=0 #se schimba minimul
                    rowMin=k
                    colMin=l
                if(minLocal==arr[k][l]):
                    minLocal=arr[k][l]
                    minTimes+=1
                    rowMin=k
                    colMin=l
        if(minLocal==minF):
            if(minTimes>1):
                minF=minLocal
                fingerprints=np.append(fingerprints,minF)
                fingerprints=np.append(fingerprints,rowMin+colMin)
        else:
            minF=minLocal
            fingerprints=np.append(fingerprints,minF)
            fingerprints=np.append(fingerprints,rowMin+colMin)
    fingerprints=fingerprints.reshape(-1,2)
    return fingerprints.tolist()

#cod care returneaza hasu urile semnificative ale fisierelor, alaturi de indexul lor
def getFingerprints(file,langauge,K,w):
    rez,tokensPerLine=fileTokenizer(file, langauge)
    fingerprintsFile=winnowing(fileHashKgrammer(rez,K),w)  #toate amprentele selectate de algoritm
    return [tokensPerLine,fingerprintsFile]

def calculateLine(fingerIndex,tokensPerLine,K):
    linesVector=[]
    lista=[]
    for i in fingerIndex:
        start=bisect.bisect_right(tokensPerLine,i[0])
        end=bisect.bisect_right(tokensPerLine,i[0]+K)
        trueInv=[x for x in list(range(start,end+1)) if x not in lista]
        lista.extend(trueInv)
        interval=[[x,i[1]] for x in trueInv]
        linesVector.extend(interval)
    return linesVector 

##functia care apeleaza toate functionalitatile
def compare2Files(fingerprint1, fingerprint2,K):
    tokensPerLine1,fingerprintsFile1=fingerprint1
    tokensPerLine2,fingerprintsFile2=fingerprint2
    fingerindexFile1=[]
    fingerindexFile2=[]
    corespondence=0
    corespond_map={}
    infolist=[]
    for i in range(0,int(np.array(fingerprintsFile1).size/2.0)):
        if fingerprintsFile1[i][1] not in corespond_map:
                    corespond_map.update({fingerprintsFile1[i][1]:corespondence})
        for j in range(i+1,int(np.array(fingerprintsFile1).size/2.0)):
            if(fingerprintsFile1[i][0]==fingerprintsFile1[j][0]):
                #print("S-a gasit un fingerprint comun",i,j)
                if fingerprintsFile1[j][1] not in corespond_map:
                    corespond_map.update({fingerprintsFile1[j][1]:corespondence})
        corespondence+=1
        
    for i in range(0,int(np.array(fingerprintsFile1).size/2.0)): #impart la 2 pt ca rez e valoare si pozitie
        for j in range(0,int(np.array(fingerprintsFile2).size/2.0)):
            if(fingerprintsFile1[i][0]==fingerprintsFile2[j][0]):
                fingerindexFile1.append([fingerprintsFile1[i][1],corespond_map.get(fingerprintsFile1[i][1])])
                fingerindexFile2.append([fingerprintsFile2[j][1],corespond_map.get(fingerprintsFile1[i][1])])
                infolist.append([fingerprintsFile1[i][1],fingerprintsFile2[j][1],corespond_map.get(fingerprintsFile1[i][1])])
    return [calculateLine(fingerindexFile1,tokensPerLine1,K), calculateLine(fingerindexFile2,tokensPerLine2,K)]


def removeBlankLines(filename):
    new_lines = []
    with open(filename, 'r') as file:
        for line in file:
            if not line.isspace():
                new_lines.append(line)
    with open(filename, 'w') as file:
        file.writelines(new_lines)


def getTextFromFile(filename):
    with open(filename, 'r') as file:
        return file.readlines()


def randomRGB():
    r = random.randint(30,255)
    g = random.randint(30,255)
    b = random.randint(30,255)
    return f'rgb({r},{g},{b})'


def getLanguage(filename):
    languages = {'c': 'c', 'cpp': 'cpp', 'py': 'python'}
    return languages[filename.split('.')[-1]]


def getPercentage(lines1, lines2, filename1, filename2):
    text1 = getTextFromFile(filename1)
    text2 = getTextFromFile(filename2)
    return int(min(len(lines1), len(lines2)) / min(len(text1), len(text2)) * 100)