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

int bar(int a)				//���밪�� �������ִ� �Լ�
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
		d[0][j] = j;				//distance�迭�� �ʱⰪ ����

	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= m; j++)
		{
			a = d[i][j - 1] + INSERT_COST;
			b = d[i - 1][j] + DELETE_COST;

			if ((i == 1) || (j == 1))						//transpose�� �ȵǴ� ���
			{
				if ((str1[i - 1]) == (str2[j - 1]))			//�ش� ���ڿ��� ������ match�� ���
				{
					c = d[i - 1][j - 1];
					tmp = __GetMin3(a, b, c);
					d[i][j] = tmp;
				}
				else										//�ش� ���ڿ��� �ٸ��� subtitute���
				{
					c = d[i - 1][j - 1] + SUBSTITUTE_COST;
					tmp = __GetMin3(a, b, c);
					d[i][j] = tmp;
				}
			}
			else
			{
				if ((str1[i - 2] == str2[j - 1]) && (str1[i - 1] == str2[j - 2]))		//transpose�� �ִ� ���
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

// ���� ���ϵ��� open�� �� �ش� ���� �̸��� ����� �ִ� �Լ�. 
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
	int i = 0;	//���� ���Ϸκ��� �о���̴� �ܾ��� ���̸� �����ϴ� �� ����ϴ� ����
	int j, len1,len2, sub;
	int if_in_node, distance;
	char input[300];		//����ڰ� ã���� �ϴ� �ܾ �����ϴ� �迭
	char check[300];		//���� ���Ϸκ��� �о���� �ܾ �����ϴ� �迭
	char openfile[10];		//���� ������ ���� �� ����ϴ� �迭
	word* newword;

	printf("input words: ");
	scanf("%s",input);	
	len1 = strlen(input);

	//�켱���� �ܾ���� �ĺ����� �־���� �迭. �ʱ� ���̸� 200���� ��Ƶΰ� ���� ���̸� �÷�������.
	wordlist *ls = (wordlist *)malloc(sizeof(wordlist));
	ls->total = 0;
	ls->max_size = 200;
	ls->arr = (word **)malloc(sizeof(word *) * 200); 


	if (strlen(input) == 1)				//���̰� 1�� �ܾ ������ ��� �ش� �ܾ �� �ִ� ���ϵ鸸�� �����.
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
				if_in_node = 0;		// �ĺ����ڷ� �߷��� �ܾ �̹� ���� ������ üũ�ϴ� ����

				fgets(check, sizeof(check), fp);
				if (feof(fp))
					break;

				check[strlen(check) - 1] = '\0';				//���Ͽ��� �о� �� �ܾ��� ���๮�� ����
				len2 = strlen(check);

				sub = bar(len2 - len1);

				if (sub < 3)				// �Է��� �ܾ�� �������̰� 2 ������ �ܾ���� 1�� �ĺ��� �ȴ�. 
				{
					distance = min_editdistance(input, check);
					if (distance < 3)				//1�� �ĺ� �߿��� edit distance�� 3 ������ �ܾ���� ������ ���� �ĺ����� ������ش�. 
					{
						for (j = 0; j < ls->total; j++)
						{
							if (strcmp(ls->arr[j]->data, check) == 0)
							{
								if_in_node++;							//��ü ���� �ĺ� �ȿ� ���� ���ڰ� ���� ���. ��带 ������ �ʰ� node���� �� ���� �ִ����� ���� ���� 1 �÷��ش�. 
								ls->arr[j]->same += 1;
								break;
							}
						}

						if (if_in_node == 0)							//��ü ���� �ĺ� �ȿ� �������ڰ� ���� ���� ��带 �����.
						{
							newword = makeword(check, distance);
							if (ls->total >= ls->max_size)
							{
								ls->max_size += 2000;
								ls->arr = (word **)realloc(ls->arr, sizeof(word *) * (ls->max_size));				//���̰� �þ ���� ls->arr�� ũ�⸦ ���Ҵ� ���ش�. 
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
//=========================================================�ĺ����� ����� ����========================================================================================
	
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