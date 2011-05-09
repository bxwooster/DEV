from struct import Struct
from io import BytesIO
import os
import collections

def write(filename, mesh):
    ppack = Struct('fff') #float3
    npack = ppack
    ipack = Struct('H') #ushort
    pbuffer = BytesIO()
    nbuffer = BytesIO()
    ibuffer = BytesIO()

    reindex = {}
    total = 0

    vertices = mesh.vertices
    for face in mesh.faces:
        if len(face.vertices) == 3:        
            if not face.use_smooth:
                N = face.normal
                for index in face.vertices:
                    V = vertices[index].co
                    pbuffer.write( ppack.pack(V.x, V.y, V.z) )
                    nbuffer.write( npack.pack(N.x, N.y, N.z) )
                    ibuffer.write( ipack.pack(total) )
                    total += 1
            else:
                for index in face.vertices:
                    if index not in reindex:
                        reindex[index] = total
                        total += 1
                        V = vertices[index].co
                        N = vertices[index].normal
                        pbuffer.write( ppack.pack(V.x, V.y, V.z) )
                        nbuffer.write( npack.pack(N.x, N.y, N.z) )
                    ibuffer.write( ipack.pack( reindex[index] ) )
        else:
            raise Exception("Untriangulated face.")

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
