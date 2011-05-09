from struct import Struct
from io import BytesIO
import os
import collections

def write(filename, mesh):
    ppack = Struct('fff') #float3
    npack = ppack
    ipack = Struct('HHH') #ushort3
    pbuffer = BytesIO()
    nbuffer = BytesIO()
    ibuffer = BytesIO()

    for face in mesh.faces:
        I = face.vertices
        if len(I) == 3:
            ibuffer.write( ipack.pack(I[0], I[1], I[2]) )
        elif len(I) == 4:
            ibuffer.write( ipack.pack(I[0], I[1], I[2]) )
            ibuffer.write( ipack.pack(I[2], I[3], I[0]) )
        else:
            raise("WHAT!? 5 or more vertices in a face.")
    for vertex in mesh.vertices:
        N = vertex.normal # if face.use_smooth else face.normal
        V = vertex.co     # non-smooth not supported so far with indexing
        pbuffer.write( ppack.pack(V.x, V.y, V.z) )
        nbuffer.write( npack.pack(N.x, N.y, N.z) )

    try:
        os.mkdir(filename)
    except OSError: pass # probably already exists
            
    file = open(filename + '\\position', 'wb')     
    file.write( pbuffer.getvalue() )        
    file.close()
    file = open(filename + '\\normal', 'wb')     
    file.write( nbuffer.getvalue() )        
    file.close()
    file = open(filename + '\\index', 'wb')     
    file.write( ibuffer.getvalue() )        
    file.close()       
