#include "Graphics\Geometry\GeometryGenerator.h"
#include "DXTK\Bezier.h"                           //Bezier
#include <DirectXMath.h>						   //XMVECTOR


typedef std::vector<DirectX::VertexPositionNormalTexture> VertexCollection;
typedef std::vector<unsigned int> IndexCollection;



//--------------------------------------------------------------------------------------
// Cube (aka a Hexahedron)
//--------------------------------------------------------------------------------------


// Creates a cube primitive.
std::unique_ptr<Geometry> GeometryGenerator::CreateCube(float size, bool rhcoords)
{
	using namespace DirectX;

	// A cube has six faces, each one pointing in a different direction.
	const int FaceCount = 6;

	static const XMVECTORF32 faceNormals[FaceCount] =
	{
		{ 0, 0, 1 },
		{ 0, 0, -1 },
		{ 1, 0, 0 },
		{ -1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, -1, 0 },
	};

	static const XMVECTORF32 textureCoordinates[4] =
	{
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ 0, 0 },
	};

	VertexCollection vertices;
	IndexCollection indices;

	size /= 2;

	// Create each face in turn.
	for (int i = 0; i < FaceCount; i++)
	{
		XMVECTOR normal = faceNormals[i];

		// Get two vectors perpendicular both to the face normal and to each other.
		XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;

		XMVECTOR side1 = XMVector3Cross(normal, basis);
		XMVECTOR side2 = XMVector3Cross(normal, side1);

		// Six indices (two triangles) per face.
		size_t vbase = vertices.size();
		indices.push_back(vbase + 0);
		indices.push_back(vbase + 1);
		indices.push_back(vbase + 2);

		indices.push_back(vbase + 0);
		indices.push_back(vbase + 2);
		indices.push_back(vbase + 3);

		// Four vertices per face.
		vertices.push_back(VertexPositionNormalTexture((normal - side1 - side2) * size, normal, textureCoordinates[0]));
		vertices.push_back(VertexPositionNormalTexture((normal - side1 + side2) * size, normal, textureCoordinates[1]));
		vertices.push_back(VertexPositionNormalTexture((normal + side1 + side2) * size, normal, textureCoordinates[2]));
		vertices.push_back(VertexPositionNormalTexture((normal + side1 - side2) * size, normal, textureCoordinates[3]));
	}

	// Create the primitive object.
	std::unique_ptr<Geometry> primitive(new Geometry(std::move(vertices), std::move(indices)));

	return primitive;
}




//--------------------------------------------------------------------------------------
// Sphere
//--------------------------------------------------------------------------------------

// Creates a sphere primitive.
std::unique_ptr<Geometry> GeometryGenerator::CreateSphere(float diameter, size_t tessellation, bool rhcoords)
{
	using namespace DirectX;

	VertexCollection vertices;
	IndexCollection indices;

	if (tessellation < 3)
		throw std::out_of_range("tesselation parameter out of range");

	size_t verticalSegments = tessellation;
	size_t horizontalSegments = tessellation * 2;

	float radius = diameter / 2;

	// Create rings of vertices at progressively higher latitudes.
	for (size_t i = 0; i <= verticalSegments; i++)
	{
		float v = 1 - (float)i / verticalSegments;

		float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
		float dy, dxz;

		XMScalarSinCos(&dy, &dxz, latitude);

		// Create a single ring of vertices at this latitude.
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			float u = (float)j / horizontalSegments;

			float longitude = j * XM_2PI / horizontalSegments;
			float dx, dz;

			XMScalarSinCos(&dx, &dz, longitude);

			dx *= dxz;
			dz *= dxz;

			XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
			XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

			vertices.push_back(VertexPositionNormalTexture(normal * radius, normal, textureCoordinate));
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	size_t stride = horizontalSegments + 1;

	for (size_t i = 0; i < verticalSegments; i++)
	{
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			size_t nextI = i + 1;
			size_t nextJ = (j + 1) % stride;

			indices.push_back(i * stride + j);
			indices.push_back(nextI * stride + j);
			indices.push_back(i * stride + nextJ);

			indices.push_back(i * stride + nextJ);
			indices.push_back(nextI * stride + j);
			indices.push_back(nextI * stride + nextJ);
		}
	}

	// Create the primitive object.
	std::unique_ptr<Geometry> primitive(new Geometry(std::move(vertices), std::move(indices)));

	return primitive;
}





//--------------------------------------------------------------------------------------
// Teapot
//--------------------------------------------------------------------------------------

// Include the teapot control point data.
namespace
{
	#include "Graphics\Geometry\TeapotData.inc"
}

// Tessellates the specified bezier patch.
static void XM_CALLCONV TessellatePatch(VertexCollection& vertices, IndexCollection& indices, TeapotPatch const& patch, size_t tessellation, DirectX::FXMVECTOR scale, bool isMirrored)
{
	using namespace DirectX;

	// Look up the 16 control points for this patch.
	XMVECTOR controlPoints[16];

	for (int i = 0; i < 16; i++)
	{
		controlPoints[i] = TeapotControlPoints[patch.indices[i]] * scale;
	}

	// Create the index data.
	size_t vbase = vertices.size();
	Bezier::CreatePatchIndices(tessellation, isMirrored, [&](size_t index)
	{
		indices.push_back(vbase + index);
	});

	// Create the vertex data.
	Bezier::CreatePatchVertices(controlPoints, tessellation, isMirrored, [&](FXMVECTOR position, FXMVECTOR normal, FXMVECTOR textureCoordinate)
	{
		vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
	});
}

std::unique_ptr<Geometry> GeometryGenerator::CreateTeapot(float size, size_t tessellation, bool rhcoords)
{
	using namespace DirectX;

	VertexCollection vertices;
	IndexCollection indices;

	if (tessellation < 1)
		throw std::out_of_range("tesselation parameter out of range");

	XMVECTOR scaleVector = XMVectorReplicate(size);

	XMVECTOR scaleNegateX = scaleVector * g_XMNegateX;
	XMVECTOR scaleNegateZ = scaleVector * g_XMNegateZ;
	XMVECTOR scaleNegateXZ = scaleVector * g_XMNegateX * g_XMNegateZ;

	for (int i = 0; i < sizeof(TeapotPatches) / sizeof(TeapotPatches[0]); i++)
	{
		TeapotPatch const& patch = TeapotPatches[i];

		// Because the teapot is symmetrical from left to right, we only store
		// data for one side, then tessellate each patch twice, mirroring in X.
		TessellatePatch(vertices, indices, patch, tessellation, scaleVector, false);
		TessellatePatch(vertices, indices, patch, tessellation, scaleNegateX, true);

		if (patch.mirrorZ)
		{
			// Some parts of the teapot (the body, lid, and rim, but not the
			// handle or spout) are also symmetrical from front to back, so
			// we tessellate them four times, mirroring in Z as well as X.
			TessellatePatch(vertices, indices, patch, tessellation, scaleNegateZ, true);
			TessellatePatch(vertices, indices, patch, tessellation, scaleNegateXZ, false);
		}
	}

	// Create the primitive object.
	std::unique_ptr<Geometry> primitive(new Geometry(std::move(vertices), std::move(indices)));

	return primitive;
}