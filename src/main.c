#include <familytree/familyTree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_tree(FT_Person *focus);

int main(void) {
	FT_FamilyTree *tree = ft_create();

	FT_Person *people[10];
       	people[0] = ft_person_add_as_root(tree, FT_SEX_MALE, "Cody Tibbetts");
	people[1] = ft_person_add_as_parent(people[0], FT_SEX_MALE, "Dean Tibbetts Jr.");
	people[2] = ft_person_add_as_parent(people[0], FT_SEX_FEMALE, "Julee Tibbetts");
	people[3] = ft_person_add_as_child(people[1], FT_SEX_MALE, "Blake Tibbetts");
	ft_add_parent_child(people[2], people[3]);
	people[4] = ft_person_add_as_parent(people[1], FT_SEX_MALE, "Dean Tibbetts Sr.");
	people[5] = ft_person_add_as_parent(people[1], FT_SEX_FEMALE, "Teresa Rogers");
	people[6] = ft_person_add_as_parent(people[2], FT_SEX_MALE, "Art Volz");
	people[7] = ft_person_add_as_parent(people[2], FT_SEX_FEMALE, "Arlee Volz");
	people[8] = ft_person_add_as_child(people[6], FT_SEX_MALE, "Jon Volz");
	ft_add_parent_child(people[7], people[8]);

	print_tree(people[0]);

	ft_destroy(tree);

	return 0;
}



void print_tree(FT_Person *focus) {
	DA_DynamicArray *generations = da_create(1);
}

