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
typedef struct Edge{		//存边的信息 
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
	int et[maxn], tg[maxn];
	int maxt, lim = 4000000, ans, anum;
	int ansq[maxn], anst[maxn];

public:
	Subway() {}
	Subway(string file) {
		strcpy(path, file.data());
	}
	void AddEdge(int, int);		//增加新边
	int GetLineID(const char *);//得到线路的编号 
	int AddStation(const char*, int);//增加新站
	int Init(string);					//从文件读取地铁站信息
	vector<int> inters(vector<int>, vector<int>);
	int CalVoid(DI, DI);
	void BfsVoidInit(queue<DI>&, map<DI,int>&, map<DI,int>&, map<DI,DI>&, int);
	bool JudgeVoid(map<DI,DI>&, DI, DI);
	void BfsVoidFun1(queue<DI>&, map<DI,int>&, map<DI,int>&, map<DI,DI>&, int);
	void BfsVoidFun2(queue<DI>&, map<DI,int>&, map<DI,int>&, map<DI,DI>&, int start, int, vector<string>&);
	void Bfs2(int, int);
	bool JudgeInt(int*, int, int);
	int Bfs1(int, int, bool, int*);
	void Dfs1(int, int);
	void Dfs2Fun1(int&, int&, int&, int&);
	void Dfs2Fun2(int&, int&, int&, int*, int*);
	void Dfs2(int, int, int);
	void do_main(string);		//查询地铁线路信息 
	void do_b(string, string);	//查询从地铁站1到地铁站2的最短路径 
	void do_a(string);			//遍历北京地铁 
	void usage();				//使用说明（在没有参数输入时调用） 
};

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

int Subway::Init(string file = string(""))
{//从文件读取地铁站信息
	if (file != "") strcpy(path, file.data());
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

int Subway::CalVoid(DI x, DI y)
{
	vector<int> a = inters(station[x.second].v, station[x.first].v);
	vector<int> b = inters(station[y.second].v, station[y.first].v);
	vector<int> c = inters(a, b);
	int csize = c.size();
	return(csize != 1);
}

void Subway::BfsVoidInit(queue<DI>&q, map<DI,int>&u, map<DI,int>&v, map<DI,DI>&dpa, int start)
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

bool Subway::JudgeVoid(map<DI,DI>&dpa, DI x, DI y)
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

void Subway::BfsVoidFun1(queue<DI>&q, map<DI,int>&u, map<DI,int>&v, map<DI,DI>&dpa, int start)
{
	while(!q.empty()) {
		DI x = q.front();
		int step = u[x];
		int d = v[x];
		q.pop();
		for (int stat = station[x.second].emax; stat; stat = edge[stat].prior) {
			DI y(x.second, edge[stat].num);
			int stp = CalVoid(x, y) + step;
			if (dpa[x] != DI(0, start) && JudgeVoid(dpa, x, y)) {
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

void Subway::BfsVoidFun2(queue<DI>&q, map<DI,int>&u, map<DI,int>&v, map<DI,DI>&dpa, int start, int end, vector<string>&stat_name)
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
		if (dpa[y] != DI(0, end) && JudgeVoid(dpa, y, x)) {
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

void Subway::Bfs2(int start, int end)
{
	if (start == end) {
		printf("you arrived.\n");
		return;
	}
	queue<DI> q;
	vector<string> stat_name;
	map<DI, int> u, v;
	map<DI, DI> dpa;
	stat_name.clear();
	BfsVoidInit(q, u, v, dpa, start);
	BfsVoidFun1(q, u, v, dpa, start);
	BfsVoidFun2(q, u, v, dpa, start, end, stat_name);
}

bool Subway::JudgeInt(int* f, int x, int y)
{
	bool flag = true;
	bool flag1 = true, flag2 = true;
	if (f[x] == 0)
		flag = false;
	else if (f[abs(f[x])] == 0)
		flag = false;
	if (abs(f[abs(f[x])]) != AddStation("大望路"))
		flag1 = false;
	else if (y == AddStation("高碑店"))
		flag1 = false;
	if (abs(f[abs(f[x])]) == AddStation("高碑店"))
		flag2 = false;
	else if (y == AddStation("大望路"))
		flag2 = false;
	flag = (flag && (flag1 || flag2));
	return flag;
}

int Subway::Bfs1(int s, int t, bool pt = true, int *ret = NULL) {
		if (s == t)
		{
			printf("已到达\n");
			return 0;
		}
		queue<int> q;
		vector<string> ans;
		ans.clear();
		while (!q.empty()) q.pop();
		int v[maxn], u[maxn], f[maxn], tr[maxn];
		memset(v, 0x3e, sizeof(v));
		memset(u, 0x3e, sizeof(u));
		q.push(s);
		v[s] = 0;
		u[s] = 0;
		f[s] = 0;
		while (!q.empty()) {
			int x = q.front();
			int step = u[x];
			int d = v[x];
			q.pop();
			for (int p = station[x].emax; p; p = edge[p].prior) {
				int y = edge[p].num;
				vector<int> tmp = inters(station[x].v, station[y].v);
				int stp = (inters(inters(station[x].v, station[abs(f[x])].v), tmp).size() != 1) + step;
				if (f[x] == 0) stp = 0;
				if (JudgeInt(f, x, y)) {
					++stp;
				}
				bool flag = v[x] + 1 < v[y];
				if (flag) {
					q.push(y);
					v[y] = v[x] + 1;
					u[y] = stp;
					f[y] = x;
					if (stp > step) f[y] = -f[y], tr[y] = tmp[0];
					if (y == t) {
						int nflag = 0;
						while (y) {
							string tmp(station[y].str);
							if (nflag) tmp += string("换乘") + string(lines[nflag]);
							if (f[y] < 0) nflag = tr[y]; else nflag = 0;
							ans.push_back(tmp);
							if (!pt) *(ret++) = y;
							y = abs(f[y]);
						}
						if (pt) {
							printf("%llu\n", ans.size());
							for (unsigned int i = 1; i <= ans.size(); ++i) printf("%s\n", ans[ans.size() - i].c_str());
						}
						return (int)ans.size();
					}
				}
			}
		}
		return -1;
	}

void Subway::Dfs1(int ref1, int ref2)
{
	tg[ref1] = 1;
	int stat = station[ref1].emax;
	while(stat) {
		int num = edge[stat].num;
		if (num != ref2 && station[num].enumb < 3 && !tg[num]) {
			Dfs1(num, ref1);
		}
		stat = edge[stat].prior;
	}
}

void Subway::Dfs2Fun1(int&ref1, int&ref2, int&ref3, int&nnum)
{
	int stat = station[ref1].emax;
	while (stat) {
		int temp = edge[stat].num;
		if (et[temp] == 0 && (temp != ref3 || ref2 == stat_num - 1) && tg[temp]) {
			nnum = -temp;
			anst[anum] = temp;
			++anum;
			++et[temp];
			Dfs2(temp, ref2 + 1, ref3);
			--et[temp];
			--anum;
		}
		stat = edge[stat].prior;
	}
	if (nnum == 0) {
		int stat = station[ref1].emax;
		while (stat) {
			int temp = edge[stat].num;
			if (et[temp] == 0 && (temp != ref3 || ref2 == stat_num - 1)) {
				nnum = temp;
				anst[anum] = temp;
				++anum;
				++et[temp];
				Dfs2(temp, ref2 + 1, ref3);
				--et[temp];
				--anum;
			}
			stat = edge[stat].prior;
		}
	}
}

void Subway::Dfs2Fun2(int&ref1, int&ref2, int&ref3, int*mmax, int*mmin)
{
	for (int y = 1; y <= stat_num; ++y) {
		if (et[y] == 0 && (y != ref3 || ref2 == stat_num - 1)) {
			int *tmp;
			int cnt = 0;
			if (paths.find(DI(ref1, y)) == paths.end()) {
				tmp = (int*)malloc(stat_num * sizeof(int));
				cnt = Bfs1(ref1, y, false, tmp);
				paths[DI(ref1, y)] = PDI(tmp, cnt);
			}
			if (cnt - 1 < mmax[0]) {
				mmax[1] = mmax[0];
				mmin[1] = mmin[0];
				mmax[0] = cnt - 1;
				mmin[0] = y;
			}
			else if (cnt - 1 < mmax[1]) {
				mmax[1] = cnt - 1;
				mmin[1] = y;
			}
		}
	}
}

void Subway::Dfs2(int ref1, int ref2, int ref3)
{
	if (ref2 == stat_num && ref1 == ref3) {
		if (anum < ans) {
			ans = anum;
			memcpy(ansq, anst, anum * sizeof(int));
		}
		return;
	}
	++maxt;
	if (maxt > lim)
		return;
	if (anum + stat_num - ref2 >= ans || maxt > lim)
		return;
	int nnum = 0;
	if (nnum == 0) {
		int mmax[2] = { 100100000,100100000 };
		int mmin[2] = { -1,-1 };
		Dfs2Fun2(ref1, ref2, ref3, mmax, mmin);
		int ant = anum;
		for (int i = 0; i < 2; ++i) {
			if (i > 0 && mmin[i] == -1) continue;
			if (i == 0 && mmin[i] == -1)
			{
				mmin[i] = ref3;
				if (paths.find(DI(ref1, mmin[i])) == paths.end()) {
					int *tmp = (int*)malloc(stat_num * sizeof(int));
					int cnt = Bfs1(ref1, mmin[i], false, tmp);
					paths[DI(ref1, mmin[i])] = PDI(tmp, cnt);
				}
			}
			int *kk = paths[DI(ref1, mmin[i])].first;
			int ks = paths[DI(ref1, mmin[i])].second;
			int nc = 0;
			for (int j = 1; j < ks; ++j) {
				int y = kk[ks - 1 - j];
				if (et[y] <= station[y].enumb) {
					anst[anum++] = y;
					if (et[y] == 0) ++nc;
					++et[y];
				}
				else {
					anum = ant;
					for (int k = 1; k < j; ++k) --et[kk[ks-k-1]];
					return;
				}
			}
			Dfs2(mmin[i], ref2 + nc, ref3);
			anum = ant;
			for (int j = 1; j < ks; ++j) --et[kk[ks-j-1]];
		}
	}
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
		memset(et, 0, sizeof(et));
		memset(tg, 0, sizeof(tg));
		maxt = 0;
		for (int i = 1; i <= stat_num; ++i) {
			if (station[i].enumb == 1 && !tg[i]) {
				Dfs1(i, 0);
			}
		}
		anum = 1, ans = 1001;
		int temp = stat_refl[s];
		anst[0] = temp;
		paths.clear();
		Dfs2(temp, 0, temp);
		printf("%d\n", ans);
		for (int i = 0; i < ans; ++i) {
			printf("%s\n", station[ansq[i]].str);
		}
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
		Bfs2(start, end);
	}
	else {
		fprintf(stderr, "station was not found.\n");
	}
}

Subway subway(subway_data);
int main(int argc, char* argv[])
{
	string cmd_exit("exit");
	if (subway.Init()) {
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
		flag = false;
		string ref(argv[2]);
		subway.do_a(ref);
	}
	else if (argc == 4 && !strcmp(argv[1], "/b")) {
		flag = false;
		string ref1(argv[2]);
		string ref2(argv[3]);
		subway.do_b(ref1, ref2);
	}
	if (flag) {
		printf("your input is fault.\n");
	}
	return 0;
}