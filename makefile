all: firstTest secondTest
	
firstTest:
	gcc -o testOne storage_mgr.c dberror.c test_assign1_1.c

secondTest:
	gcc -o testTwo storage_mgr.c dberror.c test_assign1_2.c


