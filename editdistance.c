#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

int num = 1;

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!)
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
static void backtrace_main(int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8]);

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)
void print_matrix(int *op_matrix, int col_size, int n, int m)
{
	int i, j;
	char sidt[6];
	int tmp;
	int num;
	for (i = n; i >= 1; i--)
	{
		for (j = 1; j <= m; j++)
		{
			num = 0;
			tmp = op_matrix[col_size * i + j];
			if ((tmp & 8) == 8)
			{
				sidt[num] = 'M';
				num += 1;
			}
			if ((tmp & 4) == 4)
			{
				sidt[num] = 'S';
				num += 1;
			}
			if ((tmp & 1) == 1)
			{
				sidt[num] = 'I';
				num += 1;
			}
			if ((tmp & 2) == 2)
			{
				sidt[num] = 'D';
				num += 1;
			}
			if ((tmp & 16) == 16)
			{
				sidt[num] = 'T';
				num += 1;
			}
			sidt[num] = '\0';
			printf("%-7s", sidt);
		}
		printf("\n");
	}
	printf("\n");
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance(char *str1, char *str2);

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력
void print_alignment( char align_str[][8], int level)
{
	int i;
	
	for (i = level; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m)
{
	char align_str[n+m][8]; // n+m strings
	
	backtrace_main( op_matrix, col_size, str1, str2, n, m, 0, align_str);
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	fprintf( stderr, "INSERT_COST = %d\n", INSERT_COST);
	fprintf( stderr, "DELETE_COST = %d\n", DELETE_COST);
	fprintf( stderr, "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	fprintf( stderr, "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);
	
	while( fscanf( stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str1, str2);
		printf( "==============================\n");
		
		num = 1;

		distance = min_editdistance( str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}

int min_editdistance(char *str1, char *str2)
{
	int n = strlen(str1);
	int m = strlen(str2);

	int i, j;
	int d[n + 1][m + 1];
	int op_matrix[(n + 1)*(m + 1)];
	int tmp;
	int a, b, c, t;

	for (i = 0; i <= n; i++)
		d[i][0] = i;
	for (j = 0; j <= m; j++)
		d[0][j] = j;				//distance배열의 초기값 설정
	for (i = 0; i < (n + 1)*(m + 1); i++)
		op_matrix[i] = 0;				//op_matrix 를 0으로 초기화
	for (i = 1; i <= m; i++)
		op_matrix[i] = INSERT_OP;
	for (i = m + 1; i <= (m + 1) * n; i += (m + 1))
		op_matrix[i] = DELETE_OP;			//한쪽 문자열이 없는 경우를 넣어줌

	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= m; j++)
		{
			a = d[i][j - 1] + INSERT_COST;
			b = d[i - 1][j] + DELETE_COST;

			if ((i == 1) || (j == 1))						//transpose가 안되는 경우
			{
				if ((str1[i - 1]) == (str2[j - 1]))			//해당 문자열이 같으면 match를 사용
				{
					c = d[i - 1][j - 1];
					tmp = __GetMin3(a, b, c);
					d[i][j] = tmp;
					if (tmp == d[i - 1][j - 1])
						op_matrix[(m + 1) * i + j] += MATCH_OP;
					if (tmp == (d[i][j - 1] + INSERT_COST))
						op_matrix[(m + 1) * i + j] += INSERT_OP;
					if (tmp == (d[i - 1][j] + DELETE_COST))
						op_matrix[(m + 1) * i + j] += DELETE_OP;
				}
				else										//해당 문자열이 다르면 subtitute사용
				{
					c = d[i - 1][j - 1] + SUBSTITUTE_COST;
					tmp = __GetMin3(a, b, c);
					d[i][j] = tmp;
					if (tmp == (d[i - 1][j - 1] + SUBSTITUTE_COST))
						op_matrix[(m + 1) * i + j] += SUBSTITUTE_OP;
					if (tmp == (d[i][j - 1] + INSERT_COST))
						op_matrix[(m + 1) * i + j] += INSERT_OP;		
					if (tmp == (d[i - 1][j] + DELETE_COST))
						op_matrix[(m + 1) * i + j] += DELETE_OP;			
				}
			}
			else
			{
				if ((str1[i - 2] == str2[j - 1]) && (str1[i - 1] == str2[j - 2]))		//transpose가 있는 경우
				{
					t = d[i - 2][j - 2] + TRANSPOSE_COST;
					if (str1[i - 1] == str2[j - 1])
					{
						c = d[i - 1][j - 1];
						tmp = __GetMin4(a, b, c, t);
						d[i][j] = tmp;
						if (tmp == d[i - 1][j - 1])
							op_matrix[(m + 1) * i + j] += MATCH_OP;
						if (tmp == (d[i][j - 1] + INSERT_COST))
							op_matrix[(m + 1) * i + j] += INSERT_OP;
						if (tmp == (d[i - 1][j] + DELETE_COST))
							op_matrix[(m + 1) * i + j] += DELETE_OP;
						if (tmp == (d[i - 2][j - 2] + TRANSPOSE_COST))
							op_matrix[(m + 1) * i + j] += TRANSPOSE_OP;
					}
					else
					{
						c = d[i - 1][j - 1] + SUBSTITUTE_COST;
						tmp = __GetMin4(a, b, c, t);
						d[i][j] = tmp;
						if (tmp == (d[i - 1][j - 1] + SUBSTITUTE_COST))
							op_matrix[(m + 1) * i + j] += SUBSTITUTE_OP;
						if (tmp == (d[i][j - 1] + INSERT_COST))
							op_matrix[(m + 1) * i + j] += INSERT_OP;
						if (tmp == (d[i - 1][j] + DELETE_COST))
							op_matrix[(m + 1) * i + j] += DELETE_OP;
						if (tmp == (d[i - 2][j - 2] + TRANSPOSE_COST))
							op_matrix[(m + 1) * i + j] += TRANSPOSE_OP;
					}
				}
				else
				{
					if (str1[i - 1] == str2[j - 1])
					{
						c = d[i - 1][j - 1];
						tmp = __GetMin3(a, b, c);
						d[i][j] = tmp;
						if (tmp == d[i - 1][j - 1])
							op_matrix[(m + 1) * i + j] += MATCH_OP;
						if (tmp == (d[i][j - 1] + INSERT_COST))
							op_matrix[(m + 1) * i + j] += INSERT_OP;
						if (tmp == (d[i - 1][j] + DELETE_COST))
							op_matrix[(m + 1) * i + j] += DELETE_OP;
					}
					else
					{
						c = d[i - 1][j - 1] + SUBSTITUTE_COST;
						tmp = __GetMin3(a, b, c);
						d[i][j] = tmp;
						if (tmp == (d[i - 1][j - 1] + SUBSTITUTE_COST))
							op_matrix[(m + 1) * i + j] += SUBSTITUTE_OP;
						if (tmp == (d[i][j - 1] + INSERT_COST))
							op_matrix[(m + 1) * i + j] += INSERT_OP;
						if (tmp == (d[i - 1][j] + DELETE_COST))
							op_matrix[(m + 1) * i + j] += DELETE_OP;
					}
				}
			}
		}
	}

	print_matrix(op_matrix, m + 1, n, m);

	backtrace(op_matrix, m + 1, str1, str2, n, m);

	return d[n][m];
}

static void backtrace_main(int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8])
{
	int tmp = op_matrix[col_size * n + m];
	if (tmp == 0)
	{
		printf("[%d]=======================\n", num);				//global 변수 num 사용
		print_alignment(align_str, level - 1);
		printf("\n");
		num += 1;
	}
	else
	{
		if ((tmp & 4) == 4)				//substitute
		{
			align_str[level][0] = str1[n - 1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m - 1];
			align_str[level][5] = '\0';

			backtrace_main(op_matrix, col_size, str1, str2, n - 1, m - 1, level + 1, align_str);
		}
		if ((tmp & 8) == 8)				//match
		{
			align_str[level][0] = str1[n - 1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m - 1];
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n - 1, m - 1, level + 1, align_str);
		}
		if ((tmp & 1) == 1)				//insert
		{
			align_str[level][0] = '*';
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m - 1];
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n, m - 1, level + 1, align_str);
		}
		if ((tmp & 2) == 2)				//delete
		{
			align_str[level][0] = str1[n - 1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = '*';
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n - 1, m, level + 1, align_str);;
		}
		if ((tmp & 16) == 16)				//transpose
		{
			align_str[level][0] = str1[n - 2];
			align_str[level][1] = str1[n - 1];
			align_str[level][2] = ' ';
			align_str[level][3] = '-';
			align_str[level][4] = ' ';
			align_str[level][5] = str2[m - 2];
			align_str[level][6] = str2[m - 1];
			align_str[level][7] = '\0';
			backtrace_main(op_matrix, col_size, str1, str2, n - 2, m - 2, level + 1, align_str);
		}
	}
}