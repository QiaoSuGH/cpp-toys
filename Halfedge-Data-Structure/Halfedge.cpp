#pragma once
#include <fstream>
#include "Halfedge.h"

constexpr float MIN = std::numeric_limits<float>::min();
constexpr float MAX = std::numeric_limits<float>::max();

void Halfedge::InitHalfedgeWithOffFile(const std::string filename)
{
	std::ifstream OffFile(filename, std::ios::in);
	if (!OffFile.is_open())
	{
		std::cout << "Failed to open the file." << std::endl;
	}
	std::string off;
	OffFile >> off >> num_points_ >> num_faces_ >> num_edges_;
	float x, y, z;
	size_t index1, index2, index3;
	for (size_t i = 0; i < num_points_; ++i)
	{
		OffFile >> x >> y >> z;
		add_point(x, y, z);
	}
	size_t tmp;//面中顶点数目
	OffFile >> tmp;
	if (tmp == 3)
	{
		OffFile >> index1 >> index2 >> index3;
		add_face(index1, index2, index3);
		for (size_t i = 1; i < num_faces_; ++i)
		{
			OffFile >> tmp >> index1 >> index2 >> index3;
			add_face(index1, index2, index3);
		}
	}
	else if (tmp == 4)
	{
		size_t index4;
		OffFile >> index1 >> index2 >> index3 >> index4;
		add_face(index1, index2, index3, index4);
		for (size_t i = 1; i < num_faces_; ++i)
		{
			OffFile >> tmp >> index1 >> index2 >> index3 >> index4;
			add_face(index1, index2, index3, index4);
		}
	}
}

void Halfedge::add_point(float x, float y, float z)
{
	//传入坐标，插入对应点
	HalfedgePoint tmp{ x, y, z };
	point_.push_back(tmp);
	num_points_++;
}

size_t Halfedge::add_edge(size_t index1, size_t index2, size_t index3, size_t index_face, bool inside)
{
	//index1 index2为该边的两个端点在edge中的索引 index3为面中另外一个点的索引
	//index_face为该边所属的面在face中的索引
	//半边: index1 -> index2 下一个半边：index2 -> index3

	//首先查询判断该半边是否存在
	auto flg = map_endpoints_to_edge.find(std::make_pair(index1, index2));
	if (flg == map_endpoints_to_edge.end())
	{
		//not found
		struct HalfedgeEdge tmp;
		if (inside)
		{
			tmp.index_in_face = index_face;
			tmp.index_pair_edge = num_edges_ + 1;
			//得到index_next_edge
			auto query_next = map_endpoints_to_edge.find(std::make_pair(index2, index3));
			if (query_next == map_endpoints_to_edge.end())
				//not found
				//因为处理顺序固定，因此通过当前的num_of_edges确定该半边的索引--下一个半边
				tmp.index_next_edge = num_edges_ + 2;
			else
				tmp.index_next_edge = (*query_next).second;
		}
		else
		{
			tmp.index_pair_edge = num_edges_ - 1;
		}
		tmp.index_in_point = index2;

		//插入当前半边
		edge_.push_back(tmp);
		map_endpoints_to_edge.insert({ std::make_pair(index1, index2), num_edges_ });
		point_[index1].index_in_edge = num_edges_;
		num_edges_++;
		return num_edges_ - 1;
	}
	else
	{
		//既然已经存在，那么其second value即为索引值
		point_[index1].index_in_edge = (*flg).second;
		auto res = (*flg).second;
		//此时处理其所在面以及面中下一条边
		if (inside)
		{
			edge_[res].index_in_face = index_face;
			auto query_next = map_endpoints_to_edge.find(std::make_pair(index2, index3));
			if (query_next == map_endpoints_to_edge.end())
			{
				//not found
				edge_[res].index_next_edge = num_edges_;
			}
			else
				edge_[res].index_next_edge = (*query_next).second;
		}
		return res;
	}
}

void Halfedge::add_face(size_t index1, size_t index2, size_t index3)
{
	struct HalfedgeFace tmp;
	tmp.index_any_edge = add_edge(index1, index2, index3, num_faces_, 1);
	add_edge(index2, index1, index3, num_faces_, 0);
	add_edge(index2, index3, index1, num_faces_, 1);
	add_edge(index3, index2, index1, num_faces_, 0);
	add_edge(index3, index1, index2, num_faces_, 1);
	add_edge(index1, index3, index2, num_faces_, 0);
	num_faces_++;
	face_.push_back(tmp);
}

void Halfedge::add_face(size_t index1, size_t index2, size_t index3, size_t index4)
{

	struct HalfedgeFace tmp;
	tmp.index_any_edge = add_edge(index1, index2, index3, num_faces_, 1);
	add_edge(index2, index1, index4, num_faces_, 0);
	add_edge(index2, index3, index4, num_faces_, 1);
	add_edge(index3, index2, index1, num_faces_, 0);
	add_edge(index3, index4, index1, num_faces_, 1);
	add_edge(index4, index3, index2, num_faces_, 0);
	add_edge(index4, index1, index2, num_faces_, 1);
	add_edge(index1, index4, index3, num_faces_, 0);
	num_faces_++;
	face_.push_back(tmp);

	/*insert_face(index1, index2, index3);
	insert_face(index3, index4, index1);*/
}

void Halfedge::add_face(std::vector<size_t>& PointIndex)
{
	size_t num_point = PointIndex.size();

}


void Halfedge::find_adj_verts(size_t index, std::vector<size_t>& res)const
{
	//在res中保存point[index]的相邻顶点的索引
	size_t tmp_edge = point_[index].index_in_edge;
	size_t tmp_point;
	size_t start_point = index;//固定为index 起始半边的起点
	size_t start_edge;
	do {
		//从当前起始半边开始找到其所属面中最后的半边索引
		start_edge = tmp_edge;//start_edge不断更新
		tmp_point = edge_[start_edge].index_in_point;//起始半边的终点
		res.push_back(tmp_point);
		while (tmp_point != start_point)
		{
			//更新tmp_point tmp_edge
			tmp_edge = edge_[tmp_edge].index_next_edge;
			tmp_point = edge_[tmp_edge].index_in_point;
		}
		//更新tmp_edge为其对边
		tmp_edge = edge_[tmp_edge].index_pair_edge;

	} while (point_[index].index_in_edge != tmp_edge);
}

void Halfedge::find_adj_edges(size_t index, std::vector<size_t>& res)const
{
	size_t start_edge = point_[index].index_in_edge;
	size_t tmp_edge = edge_[start_edge].index_next_edge;
	while (tmp_edge != start_edge)
	{
		if (edge_[tmp_edge].index_in_point == index)
		{
			res.push_back(tmp_edge);
			tmp_edge = edge_[tmp_edge].index_pair_edge;
			res.push_back(tmp_edge);
		}
		else
		{
			tmp_edge = edge_[tmp_edge].index_next_edge;
		}
	}

}

void Halfedge::test()
{
	std::cout << "----Test the HaleEdge Class----" << std::endl;
	std::cout << "顶点数量：" << num_points_ << std::endl;
	std::cout << "半边数量：" << num_edges_ << std::endl;
	std::cout << "面数量：" << num_faces_ << std::endl;

	std::vector<size_t> test;
	find_adj_verts(0, test);
	std::cout << "0号顶点相邻顶点：" << test.size() << "个" << std::endl;
	for (size_t i = 0; i < test.size(); ++i)
	{
		std::cout << point_[test[i]].x << " " << point_[test[i]].y << " " << point_[test[i]].z << std::endl;
	}
	test.clear();
	find_adj_edges(0, test);
	std::cout << "0号顶点所在半边：" << std::endl;
	for (size_t i = 0; i < test.size(); ++i)
	{
		std::cout << edge_[test[i]].index_pair_edge << std::endl;
	}

}

std::vector<float> Halfedge::ReturnPointCoord(size_t index)const
{
	std::vector<float> res;
	res.push_back(point_[index].x);
	res.push_back(point_[index].y);
	res.push_back(point_[index].z);
	return res;
}

std::vector<size_t> Halfedge::ReturnPointIndexInFace(size_t index)const
{
	size_t start_edge = face_[index].index_any_edge;
	size_t tmp_edge = start_edge;
	std::vector<size_t> res;
	do
	{
		res.push_back(edge_[tmp_edge].index_in_point);
		tmp_edge = edge_[tmp_edge].index_next_edge;
	} while (tmp_edge != start_edge);
	return res;
}

std::vector<float> Halfedge::ReturnAdjEdges(size_t index)const
{
	//首先得到所有邻接边的的索引 然后得到各边终点 传回终点坐标即可
	std::vector<size_t> tmp;
	find_adj_edges(index, tmp);
	std::vector<float> res;
	size_t v;
	for (size_t i = 0; i < tmp.size(); ++i)
	{
		v = edge_[tmp[i]].index_in_point;
		if (v != index)
		{
			res.push_back(point_[v].x);
			res.push_back(point_[v].y);
			res.push_back(point_[v].z);
		}
	}
	return res;
}

std::vector<float> Halfedge::ReturnBoundingBox()const
{
	float min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = min_y = min_z = MAX;
	max_x = max_y = max_z = MIN;

	for (size_t i = 0; i < point_.size(); ++i)
	{
		max_x = std::max(point_[i].x, max_x);
		max_y = std::max(point_[i].y, max_y);
		max_z = std::max(point_[i].z, max_z);

		min_x = std::min(point_[i].x, min_x);
		min_y = std::min(point_[i].y, min_y);
		min_z = std::min(point_[i].z, min_z);
	}

	std::vector<float> res{ min_x,max_x,min_y,max_y,min_z,max_z };
	return res;
}