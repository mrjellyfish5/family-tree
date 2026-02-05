#ifndef FAMILYTREE_FAMILYTREE_H
#define FAMILYTREE_FAMILYTREE_H

#include <dynamicarray/dynamicArray.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	FT_STATUS_OK,
	FT_STATUS_PERSON_NOT_FOUND,
	FT_STATUS_FAGGOT,
	FT_STATUS_RELATIONSHIP_EXISTS,
	FT_STATUS_UNMATCHED_TREE
} FT_Status;
typedef enum {
	FT_SEX_MALE,
	FT_SEX_FEMALE,
	FT_SEX_UNKNOWN
} FT_Sex;
typedef struct FT_FamilyTree FT_FamilyTree;
typedef struct FT_Person FT_Person;
typedef int64_t FT_Generation;
#define FT_INVALID_GENERATION ((FT_Generation)INT64_MAX)

// create/destroy tree
FT_FamilyTree *ft_create(void);
void ft_destroy(FT_FamilyTree *tree);

// add/remove person
FT_Person *ft_person_add_as_root(FT_FamilyTree *tree, FT_Sex sex, const char *name);
FT_Person *ft_person_add_as_child(FT_Person *parent, FT_Sex sex, const char *name);
FT_Person *ft_person_add_as_parent(FT_Person *child, FT_Sex sex, const char *name);
FT_Person *ft_person_add_as_spouse(FT_Person *spouse, FT_Sex sex, const char *name);
FT_Status ft_person_remove(FT_Person *p);

// finders (NULL if not found)
FT_Person *ft_person_find_by_name(FT_FamilyTree *tree, const char *name);

// relationship management
FT_Status ft_add_parent_child(FT_Person *parent, FT_Person *child);
FT_Status ft_remove_parent_child(FT_Person *parent, FT_Person *child);
FT_Status ft_add_relationship(FT_Person *p1, FT_Person *p2);
FT_Status ft_remove_relationship(FT_Person *p1, FT_Person *p2);

// checks if ID exists
// bool ft_id_exists(FT_FamilyTree *tree, FT_Person *p);

// accessors
FT_Generation ft_person_get_generation(const FT_Person *p);
FT_Sex ft_person_get_sex(const FT_Person *p);
const char *ft_person_get_name(const FT_Person *p);
FT_Person *ft_person_get_father(const FT_Person *p);
FT_Person *ft_person_get_mother(const FT_Person *p);
const DA_DynamicArray *ft_person_get_children(const FT_Person *p);
DA_DynamicArray *ft_couple_get_children(const FT_Person *p1, const FT_Person *p2);
const DA_DynamicArray *ft_person_get_relationships(const FT_Person *p);
DA_DynamicArray *ft_person_get_siblings(const FT_Person *p);

// serialization (JSON)
int ft_serialize_json(FT_FamilyTree *tree, const char *filepath);
FT_FamilyTree *ft_deserialize_json(const char *filepath);

#ifdef __cplusplus
}
#endif

#endif // FAMILYTREE_FAMILYTREE_H
