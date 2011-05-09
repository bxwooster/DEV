import struct
import io
import os
import collections

def write(filename, mesh):
    packer = struct.Struct('fff')
    pbuffer = io.BytesIO()
    nbuffer = io.BytesIO()

    for face in mesh.faces:
        indices = face.vertices
        if len(indices) == 3: pass
        elif len(indices) == 4:
            indices = [indices[0], indices[1], indices[2],
                       indices[2], indices[3], indices[0]]
        else:
            raise("WHAT!? 5 or more vertices in a face.")
        for i in indices:
            vertex = mesh.vertices[i]
            N = vertex.normal if face.use_smooth else face.normal
            V = vertex.co
            pbuffer.write( packer.pack(V.x, V.y, V.z) )
            nbuffer.write( packer.pack(N.x, N.y, N.z) )

    try:
        os.mkdir(filename)
    except OSError: pass # already exists
            
    file = open(filename + '\\position', 'wb')     
    file.write( pbuffer.getvalue() )        
    file.close()
    file = open(filename + '\\normal', 'wb')     
    file.write( nbuffer.getvalue() )        
    file.close()    
