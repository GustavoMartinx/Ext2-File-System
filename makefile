super:
	gcc ext2_fs.h ext2super.c
	./a.out

dir:
	gcc ext2_fs.h read_dir.c
	./a.out
