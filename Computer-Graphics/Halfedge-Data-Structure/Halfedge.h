#ifndef  CPP_TOYS_HALFEDGE_DATA_STRUCTURE_H_
#define CPP_TOYS_HALFEDGE_DATA_STRUCTURE_H_ //<PROJECT>_<PATH>_<FILE>_H_
#endif // ! CPP_TOYS_HALFEDGE_DATA_STRUCTURE_H_

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <functional>

struct HalfedgePoint
{
	float x = 0;
	float y = 0;
	float z = 0;
	size_t index_in_edge = 0;
};

struct HalfedgeEdge
{
	size_t index_in_point;
	size_t index_pair_edge;//�Ա��ڱ�vector�е�����
	size_t index_in_face;//�ñ�����������face�е�����
	size_t index_next_edge;//���е���һ����
};

struct HalfedgeFace
{
	size_t index_any_edge;
};


struct HashPair
{
	template<class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p)const
	{
		size_t hash1 = std::hash<T1>{}(p.first);
		size_t hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ (hash2 << 1);
	}
};

class Halfedge
{
public:
	Halfedge() { num_edges_ = num_faces_ = num_points_ = 0; }

	void InitHalfedgeWithOffFile(const std::string filename);

	void add_point(float x, float y, float z);
	size_t add_edge(size_t index1, size_t index2, size_t index3, size_t index_face, bool inside);
	void add_face(size_t index1, size_t index2, size_t index3);
	void add_face(size_t index1, size_t index2, size_t index3, size_t index4);

	void add_face(std::vector<size_t>& PointIndex);

	void find_adj_verts(size_t index, std::vector<size_t>& res)const;
	void find_adj_edges(size_t index, std::vector<size_t>& res)const;

	void test();

	std::vector<float> ReturnPointCoord(size_t index)const;//���ݴ���ĵ������ ���ص������
	std::vector<size_t> ReturnPointIndexInFace(size_t index)const;//����face[index]�����е������
	std::vector<float> ReturnAdjEdges(size_t index)const;//����point[index]�����ڱ�
	std::vector<float> ReturnBoundingBox()const;//����AABB��Χ�е�����ֵ

	size_t ReturnFaceNum()const { return num_faces_; };

private:
	unsigned int num_points_;
	unsigned int num_faces_;
	unsigned int num_edges_;

	std::vector<HalfedgeEdge> edge_;
	std::vector<HalfedgeFace> face_;
	std::vector<HalfedgePoint> point_;

	//ʹ��map��ʶĳ������Ƿ����--�洢������ֵ
	std::unordered_map<std::pair<size_t, size_t>, size_t, HashPair> map_points_to_edge;
};


