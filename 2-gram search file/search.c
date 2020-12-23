#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	2

typedef struct {
	int how_much_same;
	int same;
	char data[300];
} word;

typedef struct {
	int max_size;
	int total;
	word **arr;
}wordlist;

void quick_sort(word *arr[], int start, int end)
{
	if (start >= end)
		return;

	int pivot = start;
	int i = pivot + 1;
	int j = end;
	word *tmp;

	while (i <= j)			
	{
		while (i <= end && (arr[i]->how_much_same <= arr[pivot]->how_much_same))
			i++;
		while (j > start && (arr[j]->how_much_same >= arr[pivot]->how_much_same))
			j--;

		if (i > j)
		{
			tmp = arr[j];
			arr[j] = arr[pivot];
			arr[pivot] = tmp;
		}
		else
		{
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
		}
	}

	quick_sort(arr, start, j - 1);
	quick_sort(arr, j + 1, end);
}

int bar(int a)				//절대값을 리턴해주는 함수
{
	if (a < 0)
		return -a;
	else
		return a;
}

int __GetMin3(int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return min;
}

int __GetMin4(int a, int b, int c, int d)
{
	int min = __GetMin3(a, b, c);
	return (min > d) ? d : min;
}

int min_editdistance(char *str1, char *str2)
{
	int n = strlen(str1);
	int m = strlen(str2);

	int i, j;
	int d[n + 1][m + 1];
	int tmp;
	int a, b, c, t;

	for (i = 0; i <= n; i++)
		d[i][0] = i;
	for (j = 0; j <= m; j++)
		d[0][j] = j;				//distance배열의 초기값 설정

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
				}
				else										//해당 문자열이 다르면 subtitute사용
				{
					c = d[i - 1][j - 1] + SUBSTITUTE_COST;
					tmp = __GetMin3(a, b, c);
					d[i][j] = tmp;
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
					}
					else
					{
						c = d[i - 1][j - 1] + SUBSTITUTE_COST;
						tmp = __GetMin4(a, b, c, t);
						d[i][j] = tmp;
					}
				}
				else
				{
					if (str1[i - 1] == str2[j - 1])
					{
						c = d[i - 1][j - 1];
						tmp = __GetMin3(a, b, c);
						d[i][j] = tmp;
					}
					else
					{
						c = d[i - 1][j - 1] + SUBSTITUTE_COST;
						tmp = __GetMin3(a, b, c);
						d[i][j] = tmp;
					}
				}
			}
		}
	}
	return d[n][m];
}

// 색인 파일들을 open할 떄 해당 파일 이름을 만들어 주는 함수. 
void make_file(char* file, char a, char b)
{
	file[0] = a;
	file[1] = b;
	file[2] = '.';
	file[3] = 't';
	file[4] = 'x';
	file[5] = 't';
	file[6] = '\0';
}

word *makeword(char *d, int t)
{
	word *ptr = (word *)malloc(sizeof(word));

	if (ptr == NULL)
		return NULL;
	else
	{
		strcpy(ptr->data, d);
		ptr->how_much_same = t;
		ptr->same = 1;
		return ptr;
	}
}

int main()
{
	int i = 0;	//색인 파일로부터 읽어들이는 단어의 길이를 측정하는 데 사용하는 변수
	int j, len1,len2, sub;
	int if_in_node, distance;
	char input[300];		//사용자가 찾고자 하는 단어를 저장하는 배열
	char check[300];		//색인 파일로부터 읽어들인 단어를 저장하는 배열
	char openfile[10];		//색인 파일을 읽을 때 사용하는 배열
	word* newword;

	printf("input words: ");
	scanf("%s",input);	
	len1 = strlen(input);

	//우선순위 단어들의 후보들을 넣어놓는 배열. 초기 길이를 200으로 잡아두고 점점 길이를 늘려나간다.
	wordlist *ls = (wordlist *)malloc(sizeof(wordlist));
	ls->total = 0;
	ls->max_size = 200;
	ls->arr = (word **)malloc(sizeof(word *) * 200); 


	if (strlen(input) == 1)				//길이가 1인 단어가 들어왔을 경우 해당 단어가 들어가 있는 파일들만을 열어본다.
	{
		newword = makeword(input, 0);
		ls->arr[ls->total] = newword;
		ls->total++;

		for (j = 0; j < 26; j++)
		{
			make_file(openfile, input[0], j + 'a');

			FILE *fp = fopen(openfile, "r");
			if (fp == NULL)
			{
				printf("file open error!\n");
				return 0;
			}
			while (1)
			{
				fgets(check, sizeof(check), fp);
				if (feof(fp))
					break;
				check[strlen(check) - 1] = '\0';
				len2 = strlen(check);
				if (len2 == 2)
				{
					distance = min_editdistance(input, check);
					newword = makeword(check, distance);
					if (ls->total >= ls->max_size)
					{
						ls->max_size += 200;
						ls->arr = (word **)realloc(ls->arr, sizeof(word *) * (ls->max_size));
					}
					ls->arr[ls->total] = newword;
					ls->total += 1;
				}
			}
			fclose(fp);
		}
		for (j = 0; j < 26; j++)
		{
			make_file(openfile, j + 'a', input[0]);

			FILE *fp = fopen(openfile, "r");
			if (fp == NULL)
			{
				printf("file open error!\n");
				return 0;
			}
			while (1)
			{
				fgets(check, sizeof(check), fp);
				if (feof(fp))
					break;
				check[strlen(check) - 1] = '\0';
				len2 = strlen(check);
				if (len2 == 2)
				{
					distance = min_editdistance(input, check);
					newword = makeword(check, distance);
					if (ls->total >= ls->max_size)
					{
						ls->max_size += 200;
						ls->arr = (word **)realloc(ls->arr, sizeof(word *) * (ls->max_size));
					}
					ls->arr[ls->total] = newword;
					ls->total += 1;
				}
			}
			fclose(fp);
		}
	}
	else
	{
		while (input[i + 1] != '\0' && input[i+1] != '\n')
		{
			make_file(openfile, input[i], input[i + 1]);
			FILE *fp = fopen(openfile,"r");
			if (fp == NULL)
			{
				printf("file open error!\n");
				return 0;
			}
			while (1)
			{
				if_in_node = 0;		// 후보문자로 추려진 단어가 이미 나온 것인지 체크하는 변수

				fgets(check, sizeof(check), fp);
				if (feof(fp))
					break;

				check[strlen(check) - 1] = '\0';				//파일에서 읽어 온 단어의 개행문자 제거
				len2 = strlen(check);

				sub = bar(len2 - len1);

				if (sub < 3)				// 입력한 단어와 길이차이가 2 이하인 단어들이 1차 후보가 된다. 
				{
					distance = min_editdistance(input, check);
					if (distance < 3)				//1차 후보 중에서 edit distance가 3 이하인 단어들을 가지고 최종 후보군을 만들어준다. 
					{
						for (j = 0; j < ls->total; j++)
						{
							if (strcmp(ls->arr[j]->data, check) == 0)
							{
								if_in_node++;							//전체 문자 후보 안에 같은 문자가 있을 경우. 노드를 만들지 않고 node안의 몇 개가 있는지에 대한 값만 1 늘려준다. 
								ls->arr[j]->same += 1;
								break;
							}
						}

						if (if_in_node == 0)							//전체 문자 후보 안에 같은문자가 없는 경우는 노드를 만든다.
						{
							newword = makeword(check, distance);
							if (ls->total >= ls->max_size)
							{
								ls->max_size += 2000;
								ls->arr = (word **)realloc(ls->arr, sizeof(word *) * (ls->max_size));				//길이가 늘어날 수록 ls->arr의 크기를 재할당 해준다. 
							}

							ls->arr[ls->total] = newword;
							ls->total += 1;
						}
					}
				}
			}
			fclose(fp);
			i++;
		}
	}
//=========================================================후보군을 만드는 과정========================================================================================
	
	quick_sort(ls->arr, 0, ls->total - 1);
	
	printf("is this what you find?\n");
	for (j = 0; j < 10; j++)
	{
		if (ls->arr[j] == NULL)
			break;
		printf("%s\n",ls->arr[j]->data);
	}


	for (j = 0; j < ls->total; j++)
	{
		free(ls->arr[j]);
	}
	free(ls->arr);
	free(ls);
}