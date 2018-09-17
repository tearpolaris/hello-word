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

//======================================================================
//                POST TRANSISITON                                    //
//======================================================================
#include <stdio.h>
#include <stdlib.h>
#define MAX_STRING_LENGTH 6

struct package
{
	char* id;
	int weight;
};

typedef struct package package;

struct post_office
{
	int min_weight;
	int max_weight;
	package* packages;
	int packages_count;
};

typedef struct post_office post_office;

struct town
{
	char* name;
	post_office* offices;
	int offices_count;
};

typedef struct town town;



void print_all_packages(town t) {
    printf("%c:\n", *(t.name));
    for (int i=0; i < offices_count; i++) {
        printf("\t%d:\n",i);
        for (int j=0; j < t.offices->packages_count; j++) {
            printf("\t\t%c\n", *(t.offices->packages->id);
            t.offices->packages->id++;
        }
        printf("\t\t%c\n");
        
    }
}

void send_all_acceptable_packages(town* source, int source_office_index, town* target, int target_office_index) {	
    for (int i=0; i < source[source_office_index].offices->packages_count; i++) {
        if ((source[source_office_index].offices->packages->weight >= target[tart_get_office_index].offices->min_weight) && (source[source_office_index].offices->packages->weight <= target[target_office_index].offices->max_weight)) {
            target[target_office_index].offices->packages_count++;
            source[source_office_index].offices->packages_count--;
            target[target_office_index].offices->packages[packages_count-1].weight = source[source_office_index].offices->packages->weight;
            *(target[target_office_index].offices->packages[packages_count-1].id) = *(source[source_office_index].offices->packages->id);
            
        }
    }
}

town town_with_most_packages(town* towns, int towns_count) {
}

town* find_town(town* towns, int towns_count, char* name) {
}

int main()
{
	int towns_count;
	scanf("%d", &towns_count);
	town* towns = malloc(sizeof(town)*towns_count);
	for (int i = 0; i < towns_count; i++) {
		towns[i].name = malloc(sizeof(char) * MAX_STRING_LENGTH);
		scanf("%s", towns[i].name);
		scanf("%d", &towns[i].offices_count);
		towns[i].offices = malloc(sizeof(post_office)*towns[i].offices_count);
		for (int j = 0; j < towns[i].offices_count; j++) {
			scanf("%d%d%d", &towns[i].offices[j].packages_count, &towns[i].offices[j].min_weight, &towns[i].offices[j].max_weight);
			towns[i].offices[j].packages = malloc(sizeof(package)*towns[i].offices[j].packages_count);
			for (int k = 0; k < towns[i].offices[j].packages_count; k++) {
				towns[i].offices[j].packages[k].id = malloc(sizeof(char) * MAX_STRING_LENGTH);
				scanf("%s", towns[i].offices[j].packages[k].id);
				scanf("%d", &towns[i].offices[j].packages[k].weight);
			}
		}
	}
	int queries;
	scanf("%d", &queries);
	char town_name[MAX_STRING_LENGTH];
	while (queries--) {
		int type;
		scanf("%d", &type);
		switch (type) {
		case 1:
			scanf("%s", town_name);
			town* t = find_town(towns, towns_count, town_name);
			print_all_packages(*t);
			break;
		case 2:
			scanf("%s", town_name);
			town* source = find_town(towns, towns_count, town_name);
			int source_index;
			scanf("%d", &source_index);
			scanf("%s", town_name);
			town* target = find_town(towns, towns_count, town_name);
			int target_index;
			scanf("%d", &target_index);
			send_all_acceptable_packages(source, source_index, target, target_index);
			break;
		case 3:
			printf("Town with the most number of packages is %s\n", town_with_most_packages(towns, towns_count).name);
			break;
		}
	}
	return 0;
}
