

#ifndef VERTEX_STRUCT
#define VERTEX_STRUCT

struct Vertex
{
    Vector4 Pos;
    Vector2 Tex;
};

struct VertexCol
{
    VertexCol(Vector4 _Pos, Color _Col) : Pos(_Pos), Col(_Col) {}
    Vector4 Pos;
    Color Col;
};

#endif