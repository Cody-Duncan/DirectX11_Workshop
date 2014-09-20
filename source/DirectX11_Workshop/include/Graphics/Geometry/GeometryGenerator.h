#pragma once

class Geometry
{
public:
	std::vector<DirectX::VertexPositionNormalTexture> m_Vertices;
	std::vector<unsigned int> m_Indices;

	Geometry(std::vector<DirectX::VertexPositionNormalTexture>&& vertices, std::vector<unsigned int>&& indices) :
		m_Vertices(vertices),
		m_Indices(indices)
	{}
};

class GeometryGenerator
{
public:
	static std::unique_ptr<Geometry> CreateCube(float size, bool rhcoords = true);
	static std::unique_ptr<Geometry> CreateSphere(float diameter = 1, size_t tessellation = 16, bool rhcoords = true);
	static std::unique_ptr<Geometry> CreateTeapot(float size = 1, size_t tessellation = 8, bool rhcoords = true);
	
};