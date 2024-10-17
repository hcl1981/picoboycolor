import pbc    
pb = pbc.PBC()

for i in range(0,240):
    pb.tft.line(0,0,240,int(i*1.166),pbc.color565(255-i,255,i))