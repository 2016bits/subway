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
	int emax;		//emax表示站台所连接的编号最大的边的编号
	int enumb;		//enumb表示该地铁站所连接的边的条数 
	char str[64];	//存站名
	vector<int> v, con;		// v保存地铁站对应的线路编号
}NODE;

const string subway_data("Beijing_subway.txt");

//定义Bsearch类的函数
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

public:
	Subway() {}
	Subway(string file) {
		strcpy(path, file.data());
	}
	void AddEdge(int, int);		//增加新边
	int GetLineID(const char *);//得到线路的编号 
	int AddStation(const char*, int);//增加新站
	int init();					//从文件读取地铁站信息
	vector<int> inters(vector<int>, vector<int>);
	int cal(DI, DI);
	void BfsInit(queue<DI>&, map<DI,int>&, map<DI,int>&, map<DI,DI>&, int);
	bool judge(map<DI,DI>&, DI, DI);
	void BfsFun1(queue<DI>&, map<DI,int>&, map<DI,int>&, map<DI,DI>&, int);
	void BfsFun2(queue<DI>&, map<DI,int>&, map<DI,int>&, map<DI,DI>&, int start, int, vector<string>&);
	void BFS(int, int);
	bool initialize(string);	//读取文件中的地铁站信息 
	void do_main(string);		//查询地铁线路信息 
	void do_b(string, string);	//查询从地铁站1到地铁站2的最短路径 
	void do_a(string);			//遍历北京地铁 
	void usage();				//使用说明（在没有参数输入时调用） 
};

//定义类的私有函数 
void Subway::AddEdge(int first, int end)
{//增加新边
	++edge_num;					//每次增加新边，编号自动+1
	edge[edge_num].num = end;	//记录新边的终点
	edge[edge_num].prior = station[first].emax;
	++station[first].enumb;		//记录站台连接的边数 
	station[first].emax = edge_num;//记录站台所连接的边的编号
	//记录地铁站所连接的边的前一个站台的信息（x是该边的起点，y是该边的终点）
	station[end].con.push_back(first);	
}

int Subway::GetLineID(const char *s)
{//由线路名得到对应的编号 
	for (int i = 1; i <= line_num; ++i) {
		if (strcmp(lines[i], s) == 0)
			return i;
	}
	return 0;
}

int Subway::AddStation(const char *s, int ref = -1)
{//增加新站
	string str(s);
	int num;
	if (stat_refl.find(str) != stat_refl.end()) {
		num = stat_refl[str];
		if (ref > 0) {
			station[num].v.push_back(ref);
		}
	}
	else {
		++stat_num;
		strcpy(station[stat_num].str, s);
		station[stat_num].v.clear();
		station[stat_num].con.clear();
		station[stat_num].v.push_back(ref);
		station[stat_num].emax = 0;
		station[stat_num].enumb = 0;
		stat_refl[str] = stat_num;		//保存地铁站对应的编号
		num = stat_num;
	}
	return num;
}
int Subway::init()
{//从文件读取地铁站信息
	char s[64];
	FILE* f = fopen(path, "r");		//读取保存地铁信息的文本文件
	if (f == NULL) {
		printf("Error: fail to open file '%s'.\n", subway_data.data());
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
		fclose(f);
		return 0;
	}
}

vector<int> Subway::inters(vector<int> x, vector<int> y)
{//找到x和y的公共部分 
	vector<int>::iterator it1, it2;
	it1 = x.begin();
	it2 = y.begin();
	vector<int> res;
	res.clear();
	while (it1 != x.end() && it2 != y.end()) {
		if (*it1 == *it2) {
			res.push_back(*it1);
			++it1;
			++it2;
		}
		else if (*it1 < *it2) ++it1;
		else ++it2;
	}
	return res;
}

int Subway::cal(DI x, DI y)
{
	vector<int> a = inters(station[x.second].v, station[x.first].v);
	vector<int> b = inters(station[y.second].v, station[y.first].v);
	vector<int> c = inters(a, b);
	int csize = c.size();
	return(csize != 1);
}

void Subway::BfsInit(queue<DI>&q, map<DI,int>&u, map<DI,int>&v, map<DI,DI>&dpa, int start)
{
	while (!q.empty())
		q.pop();
	u.clear();
	v.clear();
	dpa.clear();
	for (int stat = station[start].emax; stat; stat = edge[stat].prior) {
		int num = edge[stat].num;
		DI di(start, num);
		q.push(di);
		dpa[di] = DI(0, start);
		u[di] = 0;
		v[di] = 1;
	}
}

bool Subway::judge(map<DI,DI>&dpa, DI x, DI y)
{
	bool flag1 = true, flag2 = true;
	if (abs(dpa[x].first) != AddStation("大望路"))
		flag1 = false;
	else if (y.second != AddStation("高碑店"))
		flag1 = false;
	if (abs(dpa[x].first) != AddStation("高碑店"))
		flag2 = false;
	else if (y.second != AddStation("大望路"))
		flag2 = false;
	return (flag1 || flag2);
}

void Subway::BfsFun1(queue<DI>&q, map<DI,int>&u, map<DI,int>&v, map<DI,DI>&dpa, int start)
{
	while(!q.empty()) {
		DI x = q.front();
		int step = u[x];
		int d = v[x];
		q.pop();
		for (int stat = station[x.second].emax; stat; stat = edge[stat].prior) {
			DI y(x.second, edge[stat].num);
			int stp = cal(x, y) + step;
			if (dpa[x] != DI(0, start) && judge(dpa, x, y)) {
				++stp;
			}
			bool flag = false;
			if (u.find(y) == u.end())
				flag = true;
			else if (stp < u[y])
				flag = true;
			else if (stp == u[y] && (v.find(y) == v.end() || d + 1 < v[y]))
				flag = true;
			if (flag) {
				q.push(y);
				v[y] = v[x] + 1;
				u[y] = stp;
				dpa[y] = x;
			}
		}
	}
}

void Subway::BfsFun2(queue<DI>&q, map<DI,int>&u, map<DI,int>&v, map<DI,DI>&dpa, int start, int end, vector<string>&stat_name)
{
	int mx = inf, md = inf, m = 0;
	for (int i = 0; i < station[end].con.size(); ++i) {
		int stat = station[end].con[i];
		DI temp(stat, end);
		if (u[temp] < mx || u[temp] < mx && v[temp] < md) {
			mx = u[temp];
			md = v[temp];
			m = stat;
		}
	}
	DI y(m, end), x(end, end);
	while(y != DI(0, start)) {
		string temp(station[y.second].str);
		vector<int> tk = inters(station[x.second].v, station[x.first].v);
		if (inters(inters(station[y.second].v, station[y.first].v), tk).size() != 1) {
			temp += string("换乘") + string(lines[tk[0]]);
		}
		if (dpa[y] != DI(0, end) && judge(dpa, y, x)) {
			temp += string("换乘") + string(lines[tk[0]]);
		} 
		stat_name.push_back(temp);
		x = y;
		y = dpa[y];
	}
	stat_name.push_back(station[y.second].str);
	printf("%llu\n", stat_name.size());
	for (unsigned int i = 1; i <= stat_name.size(); ++i)
		printf("%s\n", stat_name[stat_name.size() - i].c_str());
}

void Subway::BFS(int start, int end)
{
	queue<DI> q;
	vector<string> stat_name;
	map<DI, int> u, v;
	map<DI, DI> dpa;
	stat_name.clear();
	BfsInit(q, u, v, dpa, start);
	BfsFun1(q, u, v, dpa, start);
	BfsFun2(q, u, v, dpa, start, end, stat_name);
}
//定义类的私有函数 
//定义类的共有函数
bool Subway::initialize(string file = string(""))
{//读取文件中的地铁站信息 
	if (file != "") strcpy(path, file.data());
	return (init() == 1);
}

void Subway::do_main(string s)
{//查询地铁线路信息 
	int num = GetLineID(s.data());
	if (num) {
		for (unsigned int i = 0; i < line_n[num].size(); ++i)
			printf("%s\n", station[line_n[num][i]].str);
	}
	else printf("line %s was not found.\n", s.c_str());
}

void Subway::do_a(string s)
{//从s出发遍历北京地铁站 
	if (stat_refl.find(s) != stat_refl.end()) {
		
	}
	else {
		fprintf(stderr, "station '%s' was not found.\n", s.c_str());
	}
}

void Subway::do_b(string s1, string s2)
{//查询从地铁站s1至地铁站s2的最短路径 
	if (stat_refl.find(s1) != stat_refl.end() && stat_refl.find(s2) != stat_refl.end()) {
		int start = stat_refl[s1];
		int end = stat_refl[s2];
		if (start == end) {
			printf("you arrived.\n");
			return;
		}
		else {
			BFS(start, end);
		}
	}
	else {
		fprintf(stderr, "station was not found.\n");
	}
}
//定义类的共有函数

Subway subway(subway_data);
int main(int argc, char* argv[])
{
	string cmd_exit("exit");
	if (subway.initialize()) {
		cout << "Initialization failed." << endl;
		return 1;
	}
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
	else if (argc == 3 && !strcmp(argv[1], "/a")) {
		string ref(argv[2]);
		subway.do_a(ref);
	}
	else if (argc == 4 && !strcmp(argv[1], "/b")) {
		string ref1(argv[2]);
		string ref2(argv[3]);
		subway.do_b(ref1, ref2);
	}
	return 0;
}