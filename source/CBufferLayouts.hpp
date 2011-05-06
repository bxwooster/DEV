#ifndef __Devora_CBufferLayouts__
#define __Devora_CBufferLayouts__

#include "Matrix.h"

namespace Devora {

namespace CBufferLayouts {

typedef Vector2f float2;
typedef Vector3f float3;
typedef Vector4f float4;
typedef Matrix2f float2x2;
typedef Matrix3f float3x3;
typedef Matrix4f float4x4;

#pragma pack(push)
#pragma pack(4)

struct object
#include "shaders/cbuffer/object"

struct object_z
#include "shaders/cbuffer/object_z"

struct object_cube_z
#include "shaders/cbuffer/object_cube_z"

struct light
#include "shaders/cbuffer/light"

struct frame
#include "shaders/cbuffer/frame"

#pragma pack(push)

}

} // namespace Devora

#endif