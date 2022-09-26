'''
Minimum Spanning Tree generation (SVG) for Prim's algorithm.
Firstly use this code to generate SVG frames.
Then transform to bitmaps and convert to GIF.
'''

# range size
N = 300
margin = 20

def norm(x, y):
    return (x*x+y*y)**.5

def saveToSVG(nFrames, points, firmed, trying):
    # f = open('demo_'+'0'*(3-len(str(nFrames)))+str(nFrames)+'.svg', 'w')
    # f.write("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n")
    # for p in points:
    #     f.write("<circle cx=\"" +str(p[0]+margin)+ "\" cy=\""+ str(N-p[1]+margin) +"\" r=\"5\" fill=\"white\" stroke=\"black\"/>\n")
    # for L in firmed:
    #     f.write("<line x1=\"" +str(L[0][0]+margin)+ "\" y1=\""+ str(N-L[0][1]+margin) +"\" x2=\"" + str(L[1][0]+margin) + "\" y2=\"" + str(N-L[1][1]+margin) + "\" stroke=\"red\" stroke-width=\"5\"/>\n")
    # for L in trying:
    #     f.write("<line x1=\"" +str(L[0][0]+margin)+ "\" y1=\""+ str(N-L[0][1]+margin) +"\" x2=\"" + str(L[1][0]+margin) + "\" y2=\"" + str(N-L[1][1]+margin) + "\" stroke=\"blue\" stroke-width=\"5\"/>\n")
    # f.write("</svg>\n")
    # f.close()
    pass

def generatePoints(n):
    import random as r
    r.seed(100)
    
    res = []
    for i in range(n):
        pt = [r.randint(0,N) for _ in [0, 1]]
        if [pt] not in res:
            res += [pt]
    return res

def prim(n, points):
    Lsum = 0
    n = len(points)
    mst = []
    S = set()
    import heapq
    heap = []
    nframe = 0
    while len(mst)<n-1:
        if len(S)==0:
            topV = 0
        else:
            while heap[0][2] in S:
                heapq.heappop(heap)
            Lsum += heap[0][0]
            topV = heap[0][2]
            saveToSVG(nframe, points, mst, [[points[heap[0][1]], points[heap[0][2]]]])
            nframe+=1
            mst.append([points[heap[0][1]], points[topV]])
            heapq.heappop(heap)
            saveToSVG(nframe, points, mst, [])
            nframe+=1
        S.add(topV)
        for i in range(n):
            if i not in S:
                L = norm(points[i][0]-points[topV][0], points[i][1]-points[topV][1])
                heapq.heappush(heap, [L, topV, i])
    return mst, Lsum

# test 30 points temporarily
n = 10
pts = generatePoints(n)
print('\n'.join('{} {} {}'.format(i+1, x, y) for i, (x, y) in enumerate(pts)))
print(prim(n, pts))
