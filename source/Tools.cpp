#include "Matrix.h"
#include "ok.h"

#include "Geometries.hpp"
#include "DeviceState.hpp"

#include <D3DX11.h>
#include <fstream>

namespace Devora {

typedef unsigned int uint;

namespace Tools {

void CompileShader( char* file, char* profile, ID3D10Blob** code )
{
	IPtr<ID3D10Blob> info;

	unsigned int shader_flags = D3D10_SHADER_ENABLE_STRICTNESS |
	D3D10_SHADER_OPTIMIZATION_LEVEL0 |
	D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	HOK_EX( D3DX11CompileFromFile( file,
	NULL, NULL, "main", profile, shader_flags,
	0, NULL, code, ~info, NULL ),
	*&info ? (char*)info->GetBufferPointer() : "" );
}

void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near)
{
	float ys = 1 / tanf( y_fov * float(M_PI) / 180 * 0.5f );
	float xs = ys / aspect_ratio;
	proj << xs, 0, 0, 0,
			0, ys, 0, 0,
			0, 0, -1, -z_near,
			0, 0, -1, 0;
}

Geometry ReadGeometry(ID3D11Device* device, const std::string& filename)
{
	using std::ifstream;

	Geometry geometry;

	ifstream file( filename, ifstream::in | ifstream::binary );
	file.exceptions( ifstream::eofbit | ifstream::failbit | ifstream::badbit );

	char head[8];
	char ver[4];
    
	file.read( head, 8 );
	file.read( ver, 4 );

	uint elements;
	file.read( (char*)&elements, 4 );

	uint fmt, index, len;

	for ( uint i = 0; i < elements; i++ )
	{
		file.read( (char*)&fmt, 4 );
		file.read( (char*)&index, 4 );
		file.read( (char*)&len, 4 );
        
		char* semantic = new char[len + 1];
		file.read( (char*)semantic, len );
		delete[] semantic;
	}
    
	geometry.offset = 0;
	file.read( (char*)&geometry.stride, 4 );
    
	uint width;
	file.read( (char*)&width, 4 );
	geometry.count = width / geometry.stride;
    
	char* vertices = new char[width];
	file.read( vertices, width ); 

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.ByteWidth = width;

	HOK( device->CreateBuffer
		( &buffer_desc, &data, ~geometry.buffer ) );

	delete[] vertices;

	return geometry;
}

}

} // namespace Devora