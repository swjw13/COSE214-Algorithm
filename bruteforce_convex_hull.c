#include <stdlib.h> // atoi, rand, malloc, realloc
#include <stdio.h>
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

////////////////////////////////////////////////////////////////////////////////
void print_header( char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer( void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points( t_point *points, int num_point);

/*
#line segments
segments(7107,2909,7107,2909)
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments( t_line *lines, int num_line);

// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of line segments that forms the convex hull
// return value : set of line segments that forms the convex hull
t_line *convex_hull( t_point *points, int num_point, int *num_line);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int num_point; // number of points
	int num_line; // number of lines
	
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

	t_point *points = (t_point *) malloc( num_point * sizeof( t_point));
		
	t_line *lines;

	// making n points
	srand( time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		points[i].x = x;
		points[i].y = y;
 	}

	fprintf( stderr, "%d points created!\n", num_point);

	print_header( "convex.png");
	
	print_points( points, num_point);
	
	lines = convex_hull( points, num_point, &num_line);

	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
		
	print_footer();
	
	free( points);
	free( lines);

	return 0;
}

void print_points(t_point *points, int num_point){
	int i;
	printf("\n#points\n");
	for(i = 0 ;i < num_point;i++){
		printf("points(%d,%d)\n",points[i].x, points[i].y);
	}
}

void print_line_segments(t_line *lines, int num_line){
	int i;
	printf("\n#line segements\n");
	for(i = 0;i < num_line;i++){
		printf("segments(%d,%d,%d,%d)\n",lines[i].from.x,lines[i].from.y,lines[i].to.x,lines[i].to.y);	
	}
}

t_line *convex_hull(t_point *points, int num_point, int *num_line)
{
	int i, j, k, l;
	int tmp;
	int p, q, r;
	int line_num = 0;
	t_line *first = (t_line* ) malloc (10 * sizeof(t_line));

	for(i=0;i<num_point;i++)
	{
		for(j = i + 1;j < num_point;j++)
		{
			tmp = -2;
			p = points[j].y - points[i].y;
			q = points[j].x - points[i].x;
			r = points[i].y * points[j].x - points[i].x * points[j].y;

			for(k = 0;k < num_point;k++)
			{
				if(k != i && k != j)
				{
					if(tmp == -2)
					{
						if(p * points[k].x - q * points[k].y + r > 0)
							tmp = 1;
						else
							tmp = -1;
					}
					else
					{
						if(p * points[k].x - q * points[k].y + r > 0 && tmp == -1)
						{
							tmp = -10;
							break;
						}
						else if(p * points[k].x - q * points[k].y + r < 0 && tmp == 1)
						{
							tmp = -10;
							break;
						}
					}

				}
			}

			if (tmp != -10)
			{
				if(line_num % 10 == 0 && line_num != 0)
				{
					first = (t_line *)realloc(first, (line_num + 10) * sizeof(t_line));
					fprintf(stderr,"\nrealloc (%d)!\n",line_num);
				}
				first[line_num].from = points[i];
				first[line_num].to = points[j];
				line_num += 1;
			}
		}
	}
	*num_line = line_num;
	return first;
}
