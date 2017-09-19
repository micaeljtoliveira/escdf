/* Copyright (C) 2016-2017 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *
 * This file is part of ESCDF.
 *
 * ESCDF is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, version 2.1 of the License, or (at your option) any
 * later version.
 *
 * ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 */

#include <check.h>
#include <unistd.h>

#include "attributes.h"
#include "utils_hdf5.h"
#include "escdf_handle.h"


#define FILE_R "check_attribute_test_file_r.h5"
#define FILE_W "check_attribute_test_file_w.h5"


#define NONE           0
#define SCALAR_BOOL    1
#define SCALAR_UINT    2
#define SCALAR_INT     3
#define SCALAR_DOUBLE  4
#define SCALAR_STRING  5
#define DIM1           7
#define DIM2           8
#define ARRAY_BOOL     9
#define ARRAY_UINT    10
#define ARRAY_INT     11
#define ARRAY_DOUBLE  12
#define ARRAY_STRING  13


const _attribute_t attribute_none = {
        NONE, "none", ESCDF_DT_NONE, 0, NULL
};

const _attribute_t attribute_scalar_bool = {
        SCALAR_BOOL, "scalar_bool", ESCDF_DT_BOOL, 0, NULL
};

const _attribute_t attribute_scalar_uint = {
        SCALAR_UINT, "scalar_uint", ESCDF_DT_UINT, 0, NULL
};

const _attribute_t attribute_scalar_int = {
        SCALAR_INT, "scalar_int", ESCDF_DT_INT, 0, NULL
};

const _attribute_t attribute_scalar_double = {
        SCALAR_DOUBLE, "scalar_double", ESCDF_DT_DOUBLE, 0, NULL
};

const _attribute_t attribute_scalar_string = {
        SCALAR_STRING, "scalar_string", ESCDF_DT_STRING, 0, NULL
};


const _attribute_t attribute_dim1 = {
        DIM1, "dim1", ESCDF_DT_UINT, 0, NULL
};

const _attribute_t attribute_dim2 = {
        DIM2, "dim2", ESCDF_DT_UINT, 0, NULL
};

const _attribute_t *array_dims[] = {&attribute_dim1, &attribute_dim2};

const _attribute_t attribute_array_bool = {
        ARRAY_BOOL, "array_bool", ESCDF_DT_BOOL, 2, array_dims
};

const _attribute_t attribute_array_uint = {
        ARRAY_UINT, "array_uint", ESCDF_DT_UINT, 2, array_dims
};

const _attribute_t attribute_array_int = {
        ARRAY_INT, "array_int", ESCDF_DT_INT, 2, array_dims
};

const _attribute_t attribute_array_double = {
        ARRAY_DOUBLE, "array_double", ESCDF_DT_DOUBLE, 2, array_dims
};

const _attribute_t attribute_array_string = {
        ARRAY_STRING, "array_string", ESCDF_DT_STRING, 2, array_dims
};

static bool scalar_bool = true;
static unsigned int scalar_uint = 1;
static int scalar_int = 2;
static double scalar_double = 3.0;
static hsize_t dims[] = {2, 3};
static unsigned int array_uint[2][3] = {{1, 2, 3},
                                        {4, 5, 6}};
static unsigned int array_int[2][3] = {{1, 2, 3},
                                       {4, 5, 6}};
static double array_double[2][3] = {{0.00, 0.00, 0.00},
                                    {0.25, 0.25, 0.25}};

static escdf_handle_t *handle_r = NULL, *handle_w = NULL;
static _attribute_data_t *data_dims[2] = {NULL, NULL};
static _attribute_data_t *data = NULL;


/******************************************************************************
 * Setup and teardown                                                         *
 ******************************************************************************/

void file_setup(void)
{
    hid_t file_id, root_id;

    /* create file */
    file_id = H5Fcreate(FILE_R, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);

    /* write attributes */
    utils_hdf5_write_bool(root_id, attribute_scalar_bool.name, scalar_bool);
    utils_hdf5_write_attr(root_id, attribute_scalar_uint.name, H5T_NATIVE_UINT, NULL, 0, H5T_NATIVE_UINT, &scalar_uint);
    utils_hdf5_write_attr(root_id, attribute_scalar_int.name, H5T_NATIVE_INT, NULL, 0, H5T_NATIVE_DOUBLE, &scalar_int);
    utils_hdf5_write_attr(root_id, attribute_scalar_double.name, H5T_NATIVE_DOUBLE, NULL, 0, H5T_NATIVE_DOUBLE, &scalar_double);

    utils_hdf5_write_attr(root_id, attribute_dim1.name, H5T_NATIVE_HSIZE, NULL, 0, H5T_NATIVE_UINT, &dims[0]);
    utils_hdf5_write_attr(root_id, attribute_dim2.name, H5T_NATIVE_HSIZE, NULL, 0, H5T_NATIVE_UINT, &dims[1]);

    utils_hdf5_write_attr(root_id, attribute_array_uint.name, H5T_NATIVE_UINT, dims, 2, H5T_NATIVE_UINT, array_uint);
    utils_hdf5_write_attr(root_id, attribute_array_int.name, H5T_NATIVE_INT, dims, 2, H5T_NATIVE_INT, array_int);
    utils_hdf5_write_attr(root_id, attribute_array_double.name, H5T_NATIVE_DOUBLE, dims, 2, H5T_NATIVE_DOUBLE, array_double);

    H5Gclose(root_id);
    H5Fclose(file_id);

    handle_r = escdf_open(FILE_R, NULL);
    handle_w = escdf_create(FILE_W, NULL);
}

void file_teardown(void)
{
    escdf_close(handle_r);
    escdf_close(handle_w);
    unlink(FILE_R);
    unlink(FILE_W);
}


void scalar_setup()
{
    file_setup();
}

void scalar_teardown(void)
{
    file_teardown();
    _attribute_data_free(data);
    data = NULL;
}


void array_setup(void)
{
    file_setup();
    data_dims[0] = _attribute_data_new(&attribute_dim1, NULL);
    data_dims[1] = _attribute_data_new(&attribute_dim2, NULL);
    _attribute_data_set(data_dims[0], &dims[0]);
    _attribute_data_set(data_dims[1], &dims[1]);
}

void array_teardown(void)
{
    file_teardown();
    _attribute_data_free(data_dims[0]);
    _attribute_data_free(data_dims[1]);
    _attribute_data_free(data);
    data = NULL;
    data_dims[0] = NULL;
    data_dims[1] = NULL;
}




/******************************************************************************
 * Tests                                                                      *
 ******************************************************************************/

START_TEST(test_attribute_sizeof_none)
{
    ck_assert(_attribute_sizeof(&attribute_none) == 0);
}
END_TEST

START_TEST(test_attribute_sizeof_bool)
{
    ck_assert(_attribute_sizeof(&attribute_scalar_bool) == sizeof(bool));
}
END_TEST

START_TEST(test_attribute_sizeof_uint)
{
    ck_assert(_attribute_sizeof(&attribute_scalar_uint) == sizeof(unsigned int));
}
END_TEST

START_TEST(test_attribute_sizeof_int)
{
    ck_assert(_attribute_sizeof(&attribute_scalar_int) == sizeof(int));
}
END_TEST

START_TEST(test_attribute_sizeof_double)
{
    ck_assert(_attribute_sizeof(&attribute_scalar_double) == sizeof(double));
}
END_TEST

START_TEST(test_attribute_sizeof_string)
{
    ck_assert(_attribute_sizeof(&attribute_scalar_string) == sizeof(char));
}
END_TEST

START_TEST(test_attribute_hdf5_disk_type_none)
{
    ck_assert(_attribute_hdf5_disk_type(&attribute_none) == 0);
}
END_TEST

START_TEST(test_attribute_hdf5_disk_type_bool)
{
    ck_assert(_attribute_hdf5_disk_type(&attribute_scalar_bool) == H5T_C_S1);
}
END_TEST

START_TEST(test_attribute_hdf5_disk_type_uint)
{
    ck_assert(_attribute_hdf5_disk_type(&attribute_scalar_uint) == H5T_NATIVE_UINT);
}
END_TEST

START_TEST(test_attribute_hdf5_disk_type_int)
{
    ck_assert(_attribute_hdf5_disk_type(&attribute_scalar_int) == H5T_NATIVE_INT);
}
END_TEST

START_TEST(test_attribute_hdf5_disk_type_double)
{
    ck_assert(_attribute_hdf5_disk_type(&attribute_scalar_double) == H5T_NATIVE_DOUBLE);
}
END_TEST

START_TEST(test_attribute_hdf5_disk_type_string)
{
    ck_assert(_attribute_hdf5_disk_type(&attribute_scalar_string) == H5T_C_S1);
}
END_TEST

START_TEST(test_attribute_hdf5_mem_type_none)
{
    ck_assert(_attribute_hdf5_mem_type(&attribute_none) == 0);
}
END_TEST

START_TEST(test_attribute_hdf5_mem_type_bool)
{
    ck_assert(_attribute_hdf5_mem_type(&attribute_scalar_bool) == H5T_C_S1);
}
END_TEST

START_TEST(test_attribute_hdf5_mem_type_uint)
{
    ck_assert(_attribute_hdf5_mem_type(&attribute_scalar_uint) == H5T_NATIVE_UINT);
}
END_TEST

START_TEST(test_attribute_hdf5_mem_type_int)
{
    ck_assert(_attribute_hdf5_mem_type(&attribute_scalar_int) == H5T_NATIVE_INT);
}
END_TEST

START_TEST(test_attribute_hdf5_mem_type_double)
{
    ck_assert(_attribute_hdf5_mem_type(&attribute_scalar_double) == H5T_NATIVE_DOUBLE);
}
END_TEST

START_TEST(test_attribute_hdf5_mem_type_string)
{
    ck_assert(_attribute_hdf5_mem_type(&attribute_scalar_string) == H5T_C_S1);
}
END_TEST

START_TEST(test_attribute_is_present_true)
{
    ck_assert(_attribute_is_present(&attribute_scalar_uint, handle_r->group_id) == true);
}
END_TEST

START_TEST(test_attribute_is_present_false)
{
    ck_assert(_attribute_is_present(&attribute_none, handle_r->group_id) == false);
}
END_TEST


START_TEST(test_attribute_data_new_scalar_bool)
{
    ck_assert( (data = _attribute_data_new(&attribute_scalar_bool, NULL)) != NULL);
}
END_TEST

START_TEST(test_attribute_data_set_scalar_bool)
{
    data = _attribute_data_new(&attribute_scalar_bool, NULL);
    ck_assert( _attribute_data_set(data, &scalar_bool) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_attribute_data_get_scalar_bool)
{
    bool value = !scalar_bool;

    data = _attribute_data_new(&attribute_scalar_bool, NULL);
    _attribute_data_set(data, &scalar_bool);
    ck_assert(_attribute_data_get(data, &value) == ESCDF_SUCCESS);
    ck_assert(value == scalar_bool);
}
END_TEST

START_TEST(test_attribute_data_read_scalar_bool)
{
    bool value = !scalar_bool;

    data = _attribute_data_new(&attribute_scalar_bool, NULL);
    ck_assert( _attribute_data_read(data, handle_r->group_id) == ESCDF_SUCCESS);
    _attribute_data_get(data, &value);
    ck_assert(value == scalar_bool);
}
END_TEST

START_TEST(test_attribute_data_write_scalar_bool)
{
    data = _attribute_data_new(&attribute_scalar_bool, NULL);
    _attribute_data_set(data, &scalar_bool);
    ck_assert( _attribute_data_write(data, handle_w->group_id) == ESCDF_SUCCESS);
}
END_TEST



START_TEST(test_attribute_data_new_array_uint)
{
    ck_assert((data = _attribute_data_new(&attribute_array_uint, data_dims)) != NULL);
}
END_TEST

START_TEST(test_attribute_data_set_array_uint)
{
    data = _attribute_data_new(&attribute_array_uint, data_dims);
    ck_assert( _attribute_data_set(data, array_uint) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_attribute_data_get_array_uint)
{
    unsigned int value[2][3] = {{10, 11, 12},
                                {13, 14, 15}};

    data = _attribute_data_new(&attribute_array_uint, data_dims);
    _attribute_data_set(data, array_uint);
    ck_assert(_attribute_data_get(data, value) == ESCDF_SUCCESS);
    ck_assert(value[0][0] == array_uint[0][0]);
    ck_assert(value[0][1] == array_uint[0][1]);
    ck_assert(value[0][2] == array_uint[0][2]);
    ck_assert(value[1][0] == array_uint[1][0]);
    ck_assert(value[1][1] == array_uint[1][1]);
    ck_assert(value[1][2] == array_uint[1][2]);
}
END_TEST

START_TEST(test_attribute_data_read_array_uint)
{
    unsigned int value[2][3] = {{10, 11, 12},
                                {13, 14, 15}};

    data = _attribute_data_new(&attribute_array_uint, data_dims);
    ck_assert( _attribute_data_read(data, handle_r->group_id) == ESCDF_SUCCESS);
    _attribute_data_get(data, value);
    ck_assert(value[0][0] == array_uint[0][0]);
    ck_assert(value[0][1] == array_uint[0][1]);
    ck_assert(value[0][2] == array_uint[0][2]);
    ck_assert(value[1][0] == array_uint[1][0]);
    ck_assert(value[1][1] == array_uint[1][1]);
    ck_assert(value[1][2] == array_uint[1][2]);
}
END_TEST

START_TEST(test_attribute_data_write_array_uint)
{
    data = _attribute_data_new(&attribute_array_uint, data_dims);
    _attribute_data_set(data, array_uint);
    ck_assert( _attribute_data_write(data, handle_w->group_id) == ESCDF_SUCCESS);
}
END_TEST



Suite * make_attributes_suite(void)
{
    Suite *s;
    TCase *tc_attribute_sizeof, *tc_attribute_hdf5_disk_type, *tc_attribute_hdf5_mem_type, *tc_attribute_is_present;
    TCase *tc_attribute_data_scalar_bool;
    TCase *tc_attribute_data_array_uint;

    s = suite_create("Attributes");

    tc_attribute_sizeof = tcase_create("Attribute size of");
    tcase_add_checked_fixture(tc_attribute_sizeof, NULL, NULL);
    tcase_add_test(tc_attribute_sizeof, test_attribute_sizeof_none);
    tcase_add_test(tc_attribute_sizeof, test_attribute_sizeof_bool);
    tcase_add_test(tc_attribute_sizeof, test_attribute_sizeof_uint);
    tcase_add_test(tc_attribute_sizeof, test_attribute_sizeof_int);
    tcase_add_test(tc_attribute_sizeof, test_attribute_sizeof_double);
    tcase_add_test(tc_attribute_sizeof, test_attribute_sizeof_string);
    suite_add_tcase(s, tc_attribute_sizeof);

    tc_attribute_hdf5_disk_type = tcase_create("Attribute HDF5 disk type");
    tcase_add_checked_fixture(tc_attribute_hdf5_disk_type, NULL, NULL);
    tcase_add_test(tc_attribute_hdf5_disk_type, test_attribute_hdf5_disk_type_none);
    tcase_add_test(tc_attribute_hdf5_disk_type, test_attribute_hdf5_disk_type_bool);
    tcase_add_test(tc_attribute_hdf5_disk_type, test_attribute_hdf5_disk_type_uint);
    tcase_add_test(tc_attribute_hdf5_disk_type, test_attribute_hdf5_disk_type_int);
    tcase_add_test(tc_attribute_hdf5_disk_type, test_attribute_hdf5_disk_type_double);
    tcase_add_test(tc_attribute_hdf5_disk_type, test_attribute_hdf5_disk_type_string);
    suite_add_tcase(s, tc_attribute_hdf5_disk_type);

    tc_attribute_hdf5_mem_type = tcase_create("Attribute HDF5 mem type");
    tcase_add_checked_fixture(tc_attribute_hdf5_mem_type, NULL, NULL);
    tcase_add_test(tc_attribute_hdf5_mem_type, test_attribute_hdf5_mem_type_none);
    tcase_add_test(tc_attribute_hdf5_mem_type, test_attribute_hdf5_mem_type_bool);
    tcase_add_test(tc_attribute_hdf5_mem_type, test_attribute_hdf5_mem_type_uint);
    tcase_add_test(tc_attribute_hdf5_mem_type, test_attribute_hdf5_mem_type_int);
    tcase_add_test(tc_attribute_hdf5_mem_type, test_attribute_hdf5_mem_type_double);
    tcase_add_test(tc_attribute_hdf5_mem_type, test_attribute_hdf5_mem_type_string);
    suite_add_tcase(s, tc_attribute_hdf5_mem_type);

    tc_attribute_is_present = tcase_create("Attribute is present");
    tcase_add_checked_fixture(tc_attribute_is_present, file_setup, file_teardown);
    tcase_add_test(tc_attribute_is_present, test_attribute_is_present_true);
    tcase_add_test(tc_attribute_is_present, test_attribute_is_present_false);
    suite_add_tcase(s, tc_attribute_is_present);

    tc_attribute_data_scalar_bool = tcase_create("Attribute scalar bool");
    tcase_add_checked_fixture(tc_attribute_data_scalar_bool, scalar_setup, scalar_teardown);
    tcase_add_test(tc_attribute_data_scalar_bool, test_attribute_data_new_scalar_bool);
    tcase_add_test(tc_attribute_data_scalar_bool, test_attribute_data_set_scalar_bool);
    tcase_add_test(tc_attribute_data_scalar_bool, test_attribute_data_get_scalar_bool);
    tcase_add_test(tc_attribute_data_scalar_bool, test_attribute_data_read_scalar_bool);
    tcase_add_test(tc_attribute_data_scalar_bool, test_attribute_data_write_scalar_bool);
    suite_add_tcase(s, tc_attribute_data_scalar_bool);

    tc_attribute_data_array_uint = tcase_create("Attribute array uint");
    tcase_add_checked_fixture(tc_attribute_data_array_uint, array_setup, array_teardown);
    tcase_add_test(tc_attribute_data_array_uint, test_attribute_data_new_array_uint);
    tcase_add_test(tc_attribute_data_array_uint, test_attribute_data_set_array_uint);
    tcase_add_test(tc_attribute_data_array_uint, test_attribute_data_get_array_uint);
    tcase_add_test(tc_attribute_data_array_uint, test_attribute_data_read_array_uint);
    tcase_add_test(tc_attribute_data_array_uint, test_attribute_data_write_array_uint);
    suite_add_tcase(s, tc_attribute_data_array_uint);

    return s;
}