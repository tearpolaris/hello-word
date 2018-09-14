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
    double S0 = 0, S1 = 0;
    double p0, p1;
    int tmp_a, tmp_b, tmp_c;
    for (int i = 0; i < n; i++) {
        max_position = i;
        p0 = tr[max_position].a + tr[max_position].b + tr[max_position].c;
        Smax = sqrt(p0*(p0-a)*(p0-b)*(p0-c));
        for (int j = i+1; j < n-i; j++) {
          p1 = tr[j].a + tr[j].b + tr[j].c; 
          S1 = sqrt(p1*(p1-a)*(p1-b)*(p1-c));
        
          if (S1 < Smax ) {          
            tr[j].a = tmp_a;
            tr[j].b = tmp_b;
            tr[j].c = tmp_c;
            tr[j].a = tr[max_position].a;
            tr[j].b = tr[max_position].b;
            tr[j].c = tr[max_position].c;
            tr[max_position].a = tmp_a;
            tr[max_position].b = tmp_b;
            ttr[max_position].c = tmp_c;
            max_position = j;
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
