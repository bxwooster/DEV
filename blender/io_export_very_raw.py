bl_info = {
    "name": "Very Raw",
    "author": "B. Wooster",
    "version": (0, 1),
    "blender": (2, 5, 7),
    "api": 36302,
    "location": "File > Export",
    "description": "Exports some data.",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"}

import struct
import io
import collections

import bpy
from io_utils import ExportHelper

Element = collections.namedtuple('Element', 'name index fmt')

def write(filename, mesh):
    packer = struct.Struct('fffffff')
    buffer = io.BytesIO()
    elements = []
    
    elements.append( Element(b'POSITION', 0, 2) )
    #DXGI_FORMAT_R32G32B32A32_FLOAT = 2
    elements.append( Element(b'NORMAL', 0, 6) )
    #DXGI_FORMAT_R32G32B32_FLOAT = 6

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
            data = (V.x, V.y, V.z, 1, N.x, N.y, N.z)
            buffer.write( packer.pack(*data) )
            
    out = open(filename, 'wb')
    out.write(b'the.geom' b'0003')
    out.write(struct.pack('I', len(elements)))
    for e in elements:
        out.write(struct.pack('III', e.fmt, e.index, len(e.name)))
        out.write(e.name)

    raw = buffer.getvalue()        
    out.write(struct.pack('II', packer.size, len(raw)))
    out.write( raw )
                
    out.close()

class Export(bpy.types.Operator, ExportHelper):
    '''Export a single object.'''
    bl_idname = "export.very_raw"
    bl_label = "Export"
    filename_ext = ".vraw"

    @classmethod
    def poll(cls, context):
        ob = context.active_object
        return (ob and ob.type == 'MESH')

    def execute(self, context):
        if not self.filepath:
            raise Exception("filename not set")
        if not context.active_object:
            raise Exception('no object selected')
        write(self.filepath, context.active_object.data)
        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager
        wm.fileselect_add(self)
        return {'RUNNING_MODAL'}

def menu_func(self, context):
    path = "C:\\projects\\Devora\\geometry\\" + context.active_object.name + ".vraw"
    self.layout.operator(Export.bl_idname, text="Very Raw").filepath = path

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
