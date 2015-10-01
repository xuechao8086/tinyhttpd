'''
Created on 2010-6-21
 
@author: maodouzi
'''
import random
 
def getMinSubKeys(leftArray, rightArray):
    if (sum(leftArray) > sum(rightArray)):
        subArray = leftArray
        minuendArray = rightArray
        subFlag = True
    elif (sum(leftArray) < sum(rightArray)):
        subArray = rightArray
        minuendArray = leftArray
        subFlag = False
    else:
        return None
     
    minSubNum = (sum(subArray) - sum(minuendArray)) / 2.0
     
    tmpMinSubResult = sum(subArray)
    subKey = 0
    minuendKey = 0
    for i_key, i in enumerate(subArray):
        for j_key, j in enumerate(minuendArray):
            if (abs(i - j - minSubNum) < tmpMinSubResult):
                tmpMinSubResult = abs(i - j - minSubNum)
                subKey = i_key
                minuendKey = j_key
                 
    if (subFlag == True):
        return (subKey, minuendKey)
    else:
        return (minuendKey, subKey)
 
if __name__ == '__main__':
    aa = range(20);
    bb = [random.randint(0, 100) for i in aa]
    bb.sort(reverse=True)
    print aa
    print bb
     
    leftArray = []
    rightArray = []
    while (len(bb)):
        tmpNumArray = bb[0:2]
        bb = bb[2:]
         
        if (sum(leftArray) <= sum(rightArray)):
            leftArray.append(tmpNumArray[0])
            rightArray.append(tmpNumArray[1])
        else :
            leftArray.append(tmpNumArray[1])
            rightArray.append(tmpNumArray[0])
             
        leftArray.sort()
        rightArray.sort()
        tmpKeyArray = getMinSubKeys(leftArray, rightArray)
        if (tmpKeyArray == None):
            continue
        else:
            orgSubNum = abs(sum(leftArray) - sum(rightArray));
            newSubNum = abs(orgSubNum + (rightArray[tmpKeyArray[1]] - leftArray[tmpKeyArray[0]]) * 2)
            if (newSubNum < orgSubNum):
                tmpNum = leftArray[tmpKeyArray[0]]
                leftArray[tmpKeyArray[0]] = rightArray[tmpKeyArray[1]]
                rightArray[tmpKeyArray[1]] = tmpNum
         
        leftArray.sort()
        rightArray.sort()
     
    print leftArray, sum(leftArray)
    print rightArray, sum(rightArray)
     
    print "The subNum is: %d" % abs(sum(leftArray) - sum(rightArray))
