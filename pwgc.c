#include <stdio.h>
#include <stdlib.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename( FILE *fp, int state)
{
	int a, b, c, d;

	d = (state & 1);
	c = ((state >> 1) & 1);
	b = ((state >> 2) & 1);
	a = ((state >> 3) & 1);
	fprintf(fp, "%d \"<%d%d%d%d>\"\n",state + 1,a,b,c,d);
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc( int state, int *p, int *w, int *g, int *c)
{
	*c = (state & 1);
	*g = ((state>>1) & 1);
	*w = ((state>>2) & 1);
	*p = ((state>>3) & 1);
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state)
{
	int a, b, c, d;
	get_pwgc(state, &a,&b,&c,&d);

	if ((b == c) && (b != a))
		return 1;
	else if ((c == d) && (c != a))
		return 1;
	else
		return 0;
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition(int state1, int state2)
{
	int a, b, c, d;
	get_pwgc(state1, &a, &b, &c, &d);
	if ((is_dead_end(state2)) == 1)
		return 0;
	else if (is_dead_end(state1) == 1)
		return 0;
	else if ((state1 ^ state2) == 8)
		return 1;
	else if (((state1 ^ state2) == 12) && !(a^b))
		return 1;
	else if (((state1 ^ state2) == 10) && !(a^c))
		return 1;
	else if (((state1 ^ state2) == 9) && !(a^d))
		return 1;
	else
		return 0;
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state)
{
	int tmp = state ^ 8;
	return tmp;
}

// 상태 변경: 농부, 늑대 이동
//return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state)
{
	int a, b, c, d;
	int tmp = state ^ 12;
	get_pwgc(tmp, &a,&b,&c,&d);
	int p = is_possible_transition(state, tmp);
	if (p == 0)
		return -1;
	else
		return tmp;
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state)
{
	int p, w, g, c;
	int tmp = state ^ 10;
	get_pwgc(tmp, &p, &w, &g, &c);
	int a = is_possible_transition(state, tmp);
	if (a == 0)
		return -1;
	else
		return tmp;
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state)
{
	int p, w, g, c;
	int tmp = state ^ 9;
	get_pwgc(tmp, &p, &w, &g, &c);
	int a = is_possible_transition(state, tmp);
	if (a == 0)
		return -1;
	else
		return tmp;
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int level, int state)
{
	int i;
	for (i=0;i<level;i++)
	{
		if(visited[i] == state)
			return 1;
	}
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
static void print_states( int visited[], int count)
{
	int i, a, b, c, d;
	for(i=0;i<count;i++)
	{
		get_pwgc(visited[i],&a,&b,&c,&d);
		printf("<%d%d%d%d>\n",a,b,c,d);
	}
}

// recursive function
void dfs_main( int state, int goal_state, int level, int visited[])
{
	int a, b, c, d;
	int p;
	get_pwgc(state, &a, &b, &c, &d);

	printf("cur state is <%d%d%d%d>  (level %d)\n",a,b,c,d,level);
	visited[level] = state;
	level += 1;

	if(state == goal_state)
	{
		printf("goal-state found!\n");
		print_states(visited, level);
		printf("\n");
	}
	else
	{
		p = changeP(state);
		if (p == goal_state)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n",a,b,c,d,level-1);
		}
		else if (is_dead_end(p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> is dead-end\n",a,b,c,d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (is_visited(visited, level, p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> has been visited\n",a,b,c,d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}

		
		p = changePW(state);
		if (p == goal_state)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}
		else if (is_dead_end(p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> is dead-end\n", a, b, c, d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (is_visited(visited, level, p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> has been visited\n", a, b, c, d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (p != -1)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}
		
		p = changePG(state);
		if (p == goal_state)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}
		else if (is_dead_end(p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> is dead-end\n", a, b, c, d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (is_visited(visited, level, p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> has been visited\n", a, b, c, d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (p != -1)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}

		p = changePC(state);
		if (p == goal_state)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}
		else if (is_dead_end(p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> is dead-end\n", a, b, c, d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (is_visited(visited, level, p) == 1)
		{
			get_pwgc(p, &a, &b, &c, &d);
			printf("		next stage <%d%d%d%d> has been visited\n", a, b, c, d);
			get_pwgc(state, &a, &b, &c, &d);
		}
		else if (p != -1)
		{
			dfs_main(p, goal_state, level, visited);
			printf("back to <%d%d%d%d> (level %d)\n", a, b, c, d, level-1);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16])
{
	int i, j, a;
	for(i = 0;i<16;i++)
	{
		for(j = 0; j < 16; j++)
		{
			if(is_possible_transition(i, j) == 1)
				graph[i][j] = 1;
		}
	}
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num)
{
	int i, j;
	for(i=0;i<num;i++)
	{
		for(j = 0; j < 16;j++)
		{
			printf(" %d ",graph[i][j]);
		}
		printf("\n\n");
	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char *filename, int graph[][16], int num)
{
	FILE *fp = fopen(filename, "w");
	int i, j;

	fprintf(fp, "*Vertices %d\n", num);
	for (i = 0; i < num; i++)
	{
		print_statename(fp, i);
	}
	fprintf(fp, "*Edges\n");

	for (i = 0; i < num; i++)
	{
		for (j = i;j < 16; j++)
		{
			if (graph[i][j] == 1)
			{
				fprintf(fp, "%3d %3d\n",i+1, j+1);
			}
		}
	}
	fclose(fp);
}

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state)
{
	int level = 0;
	int visited[16] = {0,}; // 방문한 정점을 저장
	
	dfs_main( init_state, goal_state, level, visited); 
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력 (only for debugging)
	print_graph( graph, 16);
	
	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}