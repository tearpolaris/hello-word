#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct triangle
{
	int a;
	int b;
	int c;
};
typedef struct triangle triangle;

void sort_by_area(triangle* tr, int n) {
	/**
	* Sort an array a of the length n
	*/
    double Smax = 0, S1 = 0;
    double p0, p1;
    int tmp_a, tmp_b, tmp_c, max_position;
    for (int i = 0; i < n; i++) {
        max_position = i;
        p0 = tr[max_position].a + tr[max_position].b + tr[max_position].c;
        Smax = sqrt(p0*(p0-tr[max_position].a)*(p0-tr[max_position].b)*(p0-tr[max_position].c));
        for (int j = i+1; j < n-i; j++) {
          p1 = tr[j].a + tr[j].b + tr[j].c; 
          S1 = sqrt(p1*(p1-tr[j].a)*(p1-tr[j].b)*(p1-tr[j].c));
        
          if (S1 < Smax ) {          
            tmp_a = tr[j].a;
            tmp_b = tr[j].b;
            tmp_c = tr[j].c;
            tr[j].a = tr[max_position].a;
            tr[j].b = tr[max_position].b;
            tr[j].c = tr[max_position].c;
            tr[max_position].a = tmp_a;
            tr[max_position].b = tmp_b;
            tr[max_position].c = tmp_c;
            max_position = j;
         }
    }
}
}
int main()
{
	int n;
	scanf("%d", &n);
	triangle *tr = malloc(n * sizeof(triangle));
	for (int i = 0; i < n; i++) {
		scanf("%d%d%d", &tr[i].a, &tr[i].b, &tr[i].c);
	}
	sort_by_area(tr, n);
	for (int i = 0; i < n; i++) {
		printf("%d %d %d\n", tr[i].a, tr[i].b, tr[i].c);
	}
	return 0;
}
