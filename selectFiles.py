def calculateLine(fingerIndex,tokensPerLine,K):
    linesVector=[]
    lista=[]
    assurance=K*0.25
    for i in fingerIndex:
        start=bisect.bisect_right(tokensPerLine,int(i[0]-assurance))
        end=bisect.bisect_right(tokensPerLine,int(i[0]+K-assurance))
        trueInv=[x for x in list(range(start,end+1)) if x not in lista]
        lista.extend(trueInv)
        interval=[[x,i[1]] for x in trueInv]
        linesVector.extend(interval)
    return linesVector 