#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// words.txt 에서 단어들을 입력받아 word 형식으로 저장 한 후 ngram 구조체의 wordarray에 순서대로 저장
typedef struct {
	char data[100];
	int len;
} word;

typedef struct {
	int i;
	int max_number;
	word** wordarray;
}ngram;

//word구조체를 만들어주는 함수
word *newword(char *data, int len)
{
	word *ptr = (word *)malloc(sizeof(word));

	if (ptr == NULL)
		return NULL;
	else
	{
		strcpy(ptr->data, data);
		ptr->len = len;
		return ptr;
	}
}

// 저장하려는 색인 파일의 이름을 만들어 주는 함수
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

// a 안에 b가 있는지 확인. 나타난 횟수를 return.
int checkin(char* a, char *b)
{
	int check = 0;
	char *am = a;
	char *bm = b;
	char* ptr;
	while (1)
	{
		ptr = strstr(am, bm);
		if (ptr == NULL)
			break;
		else
		{
			check += 1;
			am++;
		}
	}
	return check;
}

// word들을 arr에 길이 순서대로 저장을 해 저장한다.
void quick_sort(word *arr[], int start, int end)
{
	if (start >= end)
		return;

	int pivot = start;
	int i = pivot + 1;
	int j = end;
	int len1, len2;
	word *tmp;

	while (i <= j)
	{
		while (i <= end && (arr[i]->len <= arr[pivot]->len))
			i++;
		while (j > start && (arr[j]->len >= arr[pivot]->len))
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

int main()
{
	int i, j, k;
	int check_len;
	int len1_index = 0;
	int check_ifin, tmp;
	int save_num[26][26] = { 0, };
	char check[3];
	char name[15];				// 색인파일을 만들어 줄 때 이름 저장을 위한 배열
	word *newnode;
	word *len1[30];

	ngram *field[26][26];

	//각각의 칸마다 ngram을 위한 node를 만든다. 해당 노드 안에 array를 넣어 주어 단어들을 저장한다.
	for (i = 0; i < 26; i++)							
	{
		for (j = 0; j < 26; j++)
		{
			field[i][j] = (ngram *)malloc(sizeof(ngram));
			field[i][j]->i = 0;
			field[i][j]->max_number = 100;
			field[i][j]->wordarray = (word **)malloc(sizeof(word*) * (field[i][j]->max_number));
		}
	}

	char str[300];
	FILE *ptr = fopen("words.txt", "r");

	if (ptr == NULL)
	{
		printf("file open error!\n");
		return 0;
	}
	else
	{
		while (1)
		{
			i = 0;
			fgets(str, sizeof(str), ptr);
			if (feof(ptr))
				break;

			if (strlen(str) == 2) {				//길이가 1인 단어는 따로 저장
				newnode = newword(str, check_len);
				len1[len1_index] = newnode;
				len1_index += 1;
			}
			else
			{
				while (str[i + 1] != '\n')
				{
					check[0] = str[i];
					check[1] = str[i + 1];
					check[2] = '\0';					//문자열 중 겹치는 부분이 있는지 조사. 중복되는 입력을 줄이기 위함

					check_ifin = 0;
					tmp = i + 1;
					while (str[tmp + 1] != '\n' && str[tmp + 1] != '\0')
					{
						if ((str[tmp] == check[0]) && str[tmp + 1] == check[1])
						{
							check_ifin += 1;
							break;
						}
						tmp++;
					}
					
					if (check_ifin == 0)
					{
						check_len = strlen(str);
						newnode = newword(str, check_len);

						if (field[str[i] - 'a'][str[i + 1] - 'a']->i >= field[str[i] - 'a'][str[i + 1] - 'a']->max_number)			//해당 ngram 필드 안의 arr길이를 넘어서면 realloc을 해준다.
						{
							field[str[i] - 'a'][str[i + 1] - 'a']->max_number += 100;
							field[str[i] - 'a'][str[i + 1] - 'a']->wordarray = (word **)realloc(field[str[i] - 'a'][str[i + 1] - 'a']->wordarray, (sizeof(word*) * field[str[i] - 'a'][str[i + 1] - 'a']->max_number));
						}

						field[str[i] - 'a'][str[i + 1] - 'a']->wordarray[field[str[i] - 'a'][str[i + 1] - 'a']->i] = newnode;
						field[str[i] - 'a'][str[i + 1] - 'a']->i += 1;

						save_num[str[i] - 'a'][str[i + 1] - 'a'] += 1;
					}
					i++;
				}
			}
		}
		fclose(ptr);
	}
	//=====================================================파일로부터 읽어와 ngram을 만드는 과정====================================================================

	FILE *nf = fopen("len1.txt", "w");
	if (nf == NULL)
	{
		printf("open error!");
		return 0;
	}

	for (i = 0; i < len1_index; i++)
	{
		fprintf(nf, "%s", len1[i]->data);
	}
	fclose(nf);

	for (i = 0; i < 26; i++)
	{
		for (j = 0; j < 26; j++)					//각각의 ngram file들을 만들어준다
		{
			make_file(name, i + 'a', j + 'a');

			FILE *nf = fopen(name, "w");
			if (nf == NULL)
			{
				printf("open error");
				return 0;
			}

			for (k = 0; k < save_num[i][j]; k++)
			{
				fprintf(nf, "%s", field[i][j]->wordarray[k]->data);
			}
			fclose(nf);
		}
	}

	for (i = 0; i < 26; i++)
	{
		for (j = 0; j < 26; j++)
		{
			for (k = 0; k < field[i][j]->i; k++)
			{
				free(field[i][j]->wordarray[k]);
			}
			free(field[i][j]->wordarray);
			free(field[i][j]);
		}
	}
	for (i = 0; i < len1_index; i++)
	{
		free(len1[i]);
	}
}