#define _POSIX_C_SOURCE 200809L

#include <familytree/familyTree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t INIT_TREE_SIZE = 1;
const size_t INIT_CHILDREN_SIZE = 1;
const size_t INIT_RELATIONSHIPS_SIZE = 1;

struct FT_FamilyTree {
	DA_DynamicArray *people;
	FT_Generation max_gen;
	FT_Generation min_gen;
};
struct FT_Person {
	FT_FamilyTree *tree;
	FT_Generation generation;
	FT_Sex sex;
	char *name;
	FT_Person *father;
	FT_Person *mother;
	DA_DynamicArray *children;
	DA_DynamicArray *relationships;
};

static FT_Person *ft_person_add(FT_FamilyTree *tree, FT_Generation gen, FT_Sex sex, const char *name);

//static FT_Person *ft_person_find_by_id(FT_FamilyTree *tree, FT_ID id);

// create tree
FT_FamilyTree *ft_create(void) {
	FT_FamilyTree *tree = malloc(sizeof(*tree));
	*tree = (FT_FamilyTree){
		.people = da_create(INIT_TREE_SIZE)
	};
	return tree;
}

// destroy tree
void ft_destroy(FT_FamilyTree *tree) {
	while (tree->people->size > 0) {
		ft_person_remove(tree->people->data[0]);
	}
	da_destroy(tree->people);
	free(tree);
}

// add person
static FT_Person *ft_person_add(FT_FamilyTree *tree, FT_Generation gen, FT_Sex sex, const char *name) {
	FT_Person *p = malloc(sizeof(*p));
	*p = (FT_Person){
		.tree = tree,
		.generation = gen,
		.sex = sex,
		.name = strdup(name),
		.children = da_create(INIT_CHILDREN_SIZE),
		.relationships = da_create(INIT_RELATIONSHIPS_SIZE)
	};

	if (gen > tree->max_gen)
		tree->max_gen = gen;
	if (gen < tree->min_gen)
		tree->min_gen = gen;

	da_add(tree->people, p);

	return p;
}
FT_Person *ft_person_add_as_root(FT_FamilyTree *tree, FT_Sex sex, const char *name) {
	if (tree->people->size != 0)
		return NULL;
	return ft_person_add(tree, 0, sex, name);
}
FT_Person *ft_person_add_as_child(FT_Person *parent, FT_Sex sex, const char *name) {
	if (parent == NULL)
		return NULL;

	FT_Person *child = ft_person_add(parent->tree, parent->generation - 1, sex, name);
	FT_Status status = ft_add_parent_child(parent, child);
	if (status != FT_STATUS_OK) {
		ft_person_remove(child);
		return NULL;
	}
	return child;
}
FT_Person *ft_person_add_as_parent(FT_Person *child, FT_Sex sex, const char *name) {
	if (child == NULL)
		return NULL;

	FT_Person *parent = ft_person_add(child->tree, child->generation + 1, sex, name);
	FT_Status status = ft_add_parent_child(parent, child);
	if (status != FT_STATUS_OK) {
		ft_person_remove(parent);
		return NULL;
	}
	return parent;
}
FT_Person *ft_person_add_as_spouse(FT_Person *spouse, FT_Sex sex, const char *name) {
	if (spouse == NULL)
		return NULL;

	FT_Person *p = ft_person_add(spouse->tree, spouse->generation, sex, name);
	FT_Status status = ft_add_relationship(p, spouse);
	if (status != FT_STATUS_OK) {
		ft_person_remove(p);
		return NULL;
	}
	return p;
}
	
// remove person
FT_Status ft_person_remove(FT_Person *p) {
	size_t i;

	if (!p)
		return FT_STATUS_PERSON_NOT_FOUND;
	
	// remove relationships
	if (p->father != NULL)
		ft_remove_parent_child(p->father, p);
	if (p->mother != NULL)
		ft_remove_parent_child(p->mother, p);
	for (i = 0; i < p->children->size; i++) {
		ft_remove_parent_child(p, p->children->data[i]);
	}

	da_remove(p->tree->people, p);

	free(p->name);
	da_destroy(p->children);
	da_destroy(p->relationships);
	free(p);

	return FT_STATUS_OK;
}

// finders
FT_Person *ft_person_find_by_name(FT_FamilyTree *tree, const char *name) {
	size_t i;
	for (i = 0; i < tree->people->size; i++) {
		if (!strcmp(((FT_Person*) tree->people->data[i])->name, name)) 
			return tree->people->data[i];
	}
	return NULL;
}

// parent-child management
FT_Status ft_add_parent_child(FT_Person *parent, FT_Person *child) {
	if (!parent || !child)
		return FT_STATUS_PERSON_NOT_FOUND;
	if (parent->tree != child->tree)
		return FT_STATUS_UNMATCHED_TREE;

	// give child new parent
	if (parent->sex == FT_SEX_MALE) {
		if (child->father != NULL)
			ft_remove_parent_child(child->father, child);
		child->father = parent;
	} else {
		if (child->mother != NULL)
			ft_remove_parent_child(child->mother, child);
		child->mother = parent;
	}
	ft_add_relationship(child->father, child->mother);

	// give parent new child
	da_add(parent->children, child);

	return FT_STATUS_OK;
}
FT_Status ft_remove_parent_child(FT_Person *parent, FT_Person *child) {
	if (!parent || !child)
		return FT_STATUS_PERSON_NOT_FOUND;
	if (parent->tree != child->tree)
		return FT_STATUS_UNMATCHED_TREE;

	// remove parent from child
	if (parent == child->father) 
		child->father = NULL;
	else if (parent == child->mother)
		child->mother = NULL;
	
	// remove child from parent
	da_remove(parent->children, child);

	return FT_STATUS_OK;
}

// relationship management
FT_Status ft_add_relationship(FT_Person *p1, FT_Person *p2) {
	if (!p1 || !p2)
		return FT_STATUS_PERSON_NOT_FOUND;
	if (p1->sex == p2->sex)
		return FT_STATUS_FAGGOT;
	if (da_find_index(p1->relationships, p2) != p2->relationships->size)
		return FT_STATUS_RELATIONSHIP_EXISTS;
	if (p1->tree != p2->tree)
		return FT_STATUS_UNMATCHED_TREE;

	da_add(p1->relationships, p2);
	da_add(p2->relationships, p1);

	return FT_STATUS_OK;
}
FT_Status ft_remove_relationship(FT_Person *p1, FT_Person *p2) {
	if (!p1 || !p2)
		return FT_STATUS_PERSON_NOT_FOUND;
	if (p1->tree != p2->tree)
		return FT_STATUS_UNMATCHED_TREE;
	
	da_remove(p1->relationships, p2);
	da_remove(p2->relationships, p1);
	
	return FT_STATUS_OK;
}

// accessors
FT_Generation ft_person_get_generation(const FT_Person *p) {
	if (!p)
		return FT_INVALID_GENERATION;
	return p->generation;
}
FT_Sex ft_person_get_sex(const FT_Person *p) {
	if (!p)
		return FT_SEX_UNKNOWN;
	return p->sex;
}
const char *ft_person_get_name(const FT_Person *p) {
	if (!p)
		return NULL;
	return p->name;
}
FT_Person *ft_person_get_father(const FT_Person *p) {
	if (!p)
		return NULL;
	return p->father;
}
FT_Person *ft_person_get_mother(const FT_Person *p) {
	if (!p)
		return NULL;
	return p->mother;
}
const DA_DynamicArray *ft_person_get_children(const FT_Person *p) {
	if (!p)
		return NULL;
	return p->children;
}
DA_DynamicArray *ft_couple_get_children(const FT_Person *p1, const FT_Person *p2) {
	if (!p1 || !p2 || p1->tree != p2->tree)
		return NULL;

	DA_DynamicArray *children = da_create(1);
	for (size_t i = 0; i < p1->children->size; i++) {
		if (da_find_index(p2->children, p1->children->data[i]) != p2->children->size)
			da_add(children, p1->children->data[i]);
	}
	return children;
}
const DA_DynamicArray *ft_person_get_relationships(const FT_Person *p) {
	if (!p)
		return NULL;
	return p->relationships;
}
DA_DynamicArray *ft_person_get_siblings(const FT_Person *p) {
	if (!p)
		return NULL;

	if (p->father && p->mother) {
		return ft_couple_get_children(p->father, p->mother);
	}
	DA_DynamicArray *siblings = da_create(1);
	if (p->father) {
		for (size_t i = 0; i < p->father->children->size; i++) {
			da_add(siblings, p->father->children->data[i]);
		}
		return siblings;
	}
	if (p->mother) {
		for (size_t i = 0; i < p->mother->children->size; i++) {
			da_add(siblings, p->mother->children->data[i]);
		}
		return siblings;
	}

	da_add(siblings, p);
	return siblings;
}
