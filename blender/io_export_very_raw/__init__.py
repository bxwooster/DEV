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

if "bpy" in locals():
    from imp import reload
    if "exporter" in locals():
        reload(exporter)

import bpy
from . import exporter
from io_utils import ExportHelper

class Export(bpy.types.Operator, ExportHelper):
    '''Export a single object.'''
    bl_idname = "export.very_raw"
    bl_label = "Export"
    filename_ext = ""

    @classmethod
    def poll(cls, context):
        ob = context.active_object
        return (ob and ob.type == 'MESH')

    def execute(self, context):
        if not self.filepath:
            raise Exception("filename not set")
        if not context.active_object:
            raise Exception('no object selected')
        exporter.write(self.filepath, context.active_object.data)
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
