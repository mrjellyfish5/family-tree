#include <criterion/criterion.h>
#include <familytree/familyTree.h>

Test(person, create_and_add) {
	FT_FamilyTree *tree = ft_create();
	cr_assert_not_null(tree, "tree should be created");

	FT_Person *p = ft_person_add_as_root(tree, FT_SEX_FEMALE, "Alice");
	cr_assert(ft_person_get_generation(p) == 0, "gen should be zero");

	cr_assert(ft_person_get_sex(p) == FT_SEX_FEMALE);

	const char *name = ft_person_get_name(p);
	cr_assert_str_eq(name, "Alice");

	ft_destroy(tree);
}

Test(person, parent_child_relationship) {
	FT_FamilyTree *t = ft_create();
	FT_Person *p = ft_person_add_as_root(t, FT_SEX_MALE, "Parent");
	FT_Person *c = ft_person_add_as_child(p, FT_SEX_FEMALE, "Child");

	FT_Person *father = ft_person_get_father(c);
	cr_assert_eq(father, p);

	const DA_DynamicArray *children = ft_person_get_children(p);
	cr_assert_eq(children->size, 1);
	cr_assert_eq(children->data[0], c);

	FT_Status rc = ft_remove_parent_child(p, c);
	cr_assert_eq(rc, FT_STATUS_OK);

	father = ft_person_get_father(c);
	cr_assert_eq(father, NULL);

	cr_assert_eq(ft_person_get_children(p)->size, 0);

	ft_destroy(t);

}

Test(person, spouse_relationship) {
	FT_FamilyTree *t = ft_create();
	FT_Person *husband = ft_person_add_as_root(t, FT_SEX_MALE, "Husband");
	FT_Person *wife = ft_person_add_as_spouse(husband, FT_SEX_FEMALE, "Wife");

	const DA_DynamicArray *relationships = ft_person_get_relationships(husband);
	cr_assert_eq(relationships->size, 1);
	cr_assert_eq(relationships->data[0], wife);

	FT_Status rc = ft_remove_relationship(husband, wife);
	cr_assert_eq(rc, FT_STATUS_OK);

	cr_assert_eq(ft_person_get_relationships(husband)->size, 0);

	FT_Person *c = ft_person_add_as_child(husband, FT_SEX_MALE, "Son");
	ft_add_parent_child(wife, c);

	const DA_DynamicArray *r2 = ft_person_get_relationships(husband);
	cr_assert_eq(r2->size, 1);
	cr_assert_eq(r2->data[0], wife);

	ft_destroy(t);

}
