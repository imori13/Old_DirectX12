#pragma once

class i_vertex
{
	virtual ~i_vertex();
};

struct basic_vertex
{
	vector3 position;
	vector4 color;
};

struct basic_mesh
{
	std::vector<basic_vertex> m_vertices;
	std::vector<uint32_t> m_indices;
};

struct basic_material
{
	float hoge;
};

/*  テンプレートではなく抽象化しても良い？  */
template<class Material, class Mesh> struct basic_model
{
	std::vector<std::pair<Material, std::vector<Mesh>>> renders;
};