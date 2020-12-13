#include <stdlib.h> // atoi, rand, qsort, malloc
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

t_line *upper_hull( t_point *points, int num_point, t_point p1, t_point pn, t_line *lines, int *num_line, int *capacity);

float distance( float a, float b, float c, t_point p);

void separate_points( t_point *points, int num_point, t_point from, t_point to, t_point *s1, t_point *s2, int *n1, int *n2);

void print_header(char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}

void print_footer(void)
{
	printf( "dev.off()\n");
}

int cmp_x( const void *p1, const void *p2)
{
	t_point *p = (t_point *)p1;
	t_point *q = (t_point *)p2;
	
	float diff = p->x - q->x;
	
    return ((diff >= 0.0) ? ((diff > 0.0) ? +1 : 0) : -1);
}

void print_points( t_point *points, int num_point)
{
	int i;
	printf( "\n#points\n");
	
	for (i = 0; i < num_point; i++)
		printf( "points(%d,%d)\n", points[i].x, points[i].y);
}

void print_line_segments( t_line *lines, int num_line)
{
	int i;

	printf( "\n#line segments\n");
	
	for (i = 0; i < num_line; i++)
		printf( "segments(%d,%d,%d,%d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
}

t_line *convex_hull(t_point *points, int num_point, int *num_line);

int main( int argc, char **argv)
{
	float x, y;
	int num_point; // number of points
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]);
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points;
	points = (t_point *)malloc( sizeof(t_point) * num_point);
	assert( points != NULL);
	
	srand( time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
	
		points[i].x = x;
		points[i].y = y;
 	}

	fprintf( stderr, "%d points created!\n", num_point);
	
	// sort the points by their x coordinate
	qsort( points, num_point, sizeof(t_point), cmp_x);

	print_header( "convex.png");
	
	print_points( points, num_point);
	
	// convex hull algorithm
	int num_line;
	t_line *lines = convex_hull( points, num_point, &num_line);
	
	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
	
	print_footer();
	
	free( points);
	free( lines);
	
	return 0;
}

float distance (float a , float b, float c, t_point p)
{
	float tmp;
	tmp = a * p.x + b * p.y - c;
	if (tmp < 0)
		return (-tmp);
	else
		return tmp;
}
void separate_points( t_point *points, int num_point, t_point from, t_point to, t_point *s1, t_point *s2, int *n1, int *n2)
{
	int i;
	int j = 0;
	int k = 0;
	int a, b, c;
	a = to.y - from.y;
	b = from.x - to.x;
	c = (from.x * to.y) - (to.x * from.y);
	for (i = 0;i < num_point;i ++)
	{
		if (points[i].x * a + points[i].y * b - c > 0)
		{
			s1[j] = points[i];
			j += 1;
		}
		else if(points[i].x * a + points[i].y * b - c < 0)
		{
			s2[k] = points[i];
			k += 1;
		}
	}
	*n1 = j;
	*n2 = k;
}
t_line *convex_hull(t_point *points, int num_point, int *num_line)
{
	int capacity = 10;

	t_line *lines = (t_line *)malloc(capacity * sizeof(t_line));
	*num_line = 0;

	t_point *s1 = (t_point *)malloc(sizeof(t_point) * num_point);
	assert(s1 != NULL);

	t_point *s2 = (t_point *)malloc(sizeof(t_point) * num_point);
	assert(s2 != NULL);

	int n1, n2; // number of points in s1, s2, respectivelyleftmost point (p1)

	separate_points(points, num_point, points[0], points[num_point - 1], s1, s2, &n1, &n2);

	lines = upper_hull( s1, n1, points[0], points[num_point-1], lines, num_line, &capacity);
	lines = upper_hull( s2, n2, points[num_point-1], points[0], lines, num_line, &capacity);

	free(s1);
	free(s2);

	return lines;
}

t_line *upper_hull (t_point *points, int num_point, t_point p1, t_point pn, t_line *lines, int *num_line, int *capacity)
{
	t_point *s11 = (t_point *)malloc(sizeof(t_point) * num_point);
	t_point *s12 = (t_point *)malloc(sizeof(t_point) * num_point);
	int n11, n12;
	int i, tmp;
	int max;
	int a, b, c;
	a = pn.y - p1.y;
	b = p1.x - pn.x;
	c = (p1.x * pn.y) - (p1.y * pn.x);

	if (*num_line == *capacity)
	{
		*capacity += 10;
		lines = (t_line *)realloc(lines,(*capacity) * sizeof(t_line));
		fprintf(stderr,"\nrealloc (%d)!\n",*capacity);
	}
	if (num_point == 0)
	{
		lines[*num_line].from = p1;
		lines[*num_line].to = pn;
		*num_line += 1;
	}
	else
	{
		max = 0;
		for(i = 0;i < num_point;i++)
		{
			if(distance(a,b,c,points[i]) > max)
			{
				max = distance(a,b,c,points[i]);
				tmp = i;
			}
		}
		separate_points(points, num_point, p1,points[tmp],s11,s12,&n11,&n12);
		lines = upper_hull(s11,n11,p1,points[tmp],lines, num_line, capacity);
		separate_points(points, num_point, points[tmp],pn,s11,s12,&n11,&n12);
		lines = upper_hull(s11,n11,points[tmp],pn,lines, num_line, capacity);
	}

	free (s11);
	free (s12);

	return lines;
}
