cmake_minimum_required(VERSION 3.17)
project(pa_assign_opt C)

set(CMAKE_C_STANDARD 11)

add_executable(pa_assign_opt btree_mgr.c buffer_mgr.c buffer_mgr_stat.c dberror.c expr.c storage_mgr.c test_assign4_1.c)