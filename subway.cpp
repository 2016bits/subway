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

const string subway_data("Beijing_subway.txt");

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
	int et[maxn], tg[maxn], maxtime, lim;
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
	Subway() {lim = 4000000;}
	Subway(string file) {
		lim = 4000000;
		strcpy(path, file.data());
	}
	bool initialize(string);
	void do_main(string);
	void do_b(string, string);
	void do_c(string, string);
	void do_c(string);
	void usage();
};

//定义类的私有函数 
void Subway::AddEdge(int first, int end)
{
	++edge_num;					//每次增加新边，编号自动+1
	edge[edge_num].num = end;	//记录新边的终点
	edge[edge_num].prior = station[first].ecount;
	++station[first].enumb;		//记录站台连接的边数 
	station[first].ecount = edge_num;//记录站台所连接的边的编号
	//记录地铁站所连接的边的前一个站台的信息（x是该边的起点，y是该边的终点）
	station[end].con.push_back(first);	
}

int Subway::GetLineID(const char *s)
{
	for (int i = 1; i <= line_num; ++i) {
		if (!strcmp(lines[i], s))
			return i;
	}
	return 0;
}

int Subway::AddStation(const char *s, int ref = -1)
{
	string str(s);
	int num;
	if (stat_refl.find(str) != stat_refl.end()) {
		num = stat_refl[str];
		if (num > 0) {
			station[num].v.push_back(ref);
		}
	}
	else {
		++stat_num;
		strcpy(station[stat_num].str, s);
		station[stat_num].v.clear();
		station[stat_num].con.clear();
		station[stat_num].v.push_back(ref);
		station[stat_num].ecount = 0;
		station[stat_num].enumb = 0;
		stat_refl[str] = stat_num;		//保存地铁站对应的编号
		num = stat_num;
	}
	return num;
}
int Subway::init()
{
	char s[64];
	FILE* f = fopen(path, "r");		//读取保存地铁信息的文本文件
	if (f == NULL) {
		printf("Error: file '%s' does not exist or access permission denied.\n", subway_data.data());
		return 1;
	}
	else {
		line_num = stat_num = edge_num = 0;//将路线条数、站台个数、边的个数初始化
		while (fscanf(f, "%s", s, 64) != EOF) {	//从文件f中逐行读取信息至字符串s 
			++line_num;		//每次线路编号+1 
			s[63] = '\0'; 
			strcpy(lines[line_num], s);
			int num;
			fscanf(f, "%d", &num);
			bool cir = false;		//记录是否是环线 
			if (num < 0) {
				num = -num;
				cir = true;
			}
			for (int i = 1; i <= num; ++i) {
				fscanf(f, "%s", s, 64);
				bool flag = false;
				if (s[strlen(s) - 1] == '*') {
					s[strlen(s) - 1] = '\0';
					flag = true;
				}
				int id = AddStation(s, line_num);	//地铁站对应的编号
				line_n[line_num].push_back(id);		//记录每条线路包含的地铁站的编号 
				if (i != 1) {
					AddEdge(line_n[line_num][i-2], line_n[line_num][i-1]);
					if (flag == false) {
						AddEdge(line_n[line_num][i-1], line_n[line_num][i-2]);
					} 
				} 
				if (cir) {
					fscanf(f, "%s", s, 64);
					bool flag = false;
					if (s[strlen(s) - 1] == '*') {
						s[strlen(s) - 1] = '\0';
						flag = true;
					}
					int numb = AddStation(s);
					AddEdge(line_n[line_num][num-1], numb);
					if (flag == false)
						AddEdge(numb, line_n[line_num][num-1]);
				}
			}
		} 
		fclose(f);
		return 0;
	}
}
//定义类的私有函数 
//定义类的共有函数
bool Subway::initialize(string file = string(""))
{
	if (file != "") strcpy(path, file.data());
	return (init() == 1);
}

void Subway::do_main(string s)
{
	int k = GetLineID(s.data());
	if (k) {
		for (unsigned int i = 0; i < line_n[k].size(); ++i)
			printf("%s\n", station[line_n[k][i]].str);
	}
	else printf("line %s was not found.\n", s.c_str());
}

//定义类的共有函数

Subway subway(subway_data);
int main(int argc, char* argv[])
{
	string cmd_exit("exit");
	if (subway.initialize())
		return 1;
	bool flag = true;
	if (argc == 1) {
		flag = false;
		string s;
		while (true) {
			cin >> s;
			if (s == cmd_exit) break;
			subway.do_main(s);
		}
	}
}