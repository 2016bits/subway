//软件工程组队项目——地铁
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#define maxn 1000
#define inf 1000000000

using namespace std;
typedef pair<int, int> DI;
typedef pair<int*, int> PDI;
typedef struct edge{		//存边的信息 
	int num;				//当前边的编号 
	int prior;				//上一条邻接边的编号 
}EDGE;
typedef struct node{
	int ecount;		//ecount表示站台所连接的编号最大的边的编号
	int enumb;		//enumb表示该地铁站所连接的边的条数 
	char str[64];	//存站名
	vector<int> v, con;		// v保存地铁站对应的线路编号
}NODE;

class Subway {
private:
	int line_num, stat_num, edge_num;
	//line_num表示线路的条数，stat_num表示地铁站的个数，edge_num表示边的个数
	char path[256];
	char lines[128][64];
	vector<int> line_n[128];	//存地铁线路所包含的地铁站的编号
	map<string, int> stat_refl;	//存地铁站名称和编号的对应关系 
	map<DI, PDI> paths;
	NODE station[maxn];			//存放所有的站点信息
	EDGE edge[maxn*maxn >> 1];	//存放所有边的信息
	int et[maxn], tg[maxn], maxtime, lim = 4000000;
	int ansq[maxn], ans, anst[maxn], an;
	
	void AddEdge(int, int);		//增加新边
	int GetLineID(const char *);//得到线路的编号 
	int AddStation(const char*, int);//增加新站
	int init();					//从文件读取地铁站信息
	vector<int> inters(vector<int>, vector<int>);
	void BFS2(int, int);
	int BFS(int, int, bool, int*);
	void OutputInfo(int*, int);
	void DFS(int, int, int);
	void DFS2(int, int);

public:
	Subway() {}
	Subway(string file) {
		strcpy(path, file.data());
	}
	bool initialize(string);
	void do_main(string);
	void do_b(string, string);
	void do_c(string, string);
	void do_c(string);
	void usage();
};
int main(int argc, char* argv[])
{
	string subway_data("Beijing_subway.txt");
	Subway subway(subway_data);
	string cmd_exit("exit");
	if (subway)
}