#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv){
	int n, in, out;
	char buf[1024];
	if(argc < 3) {
		write(2, "Usage : copy file1 file2\n", 25);
		return -1;
	}

	if((in = open(argv[1], O_RDWR|O_APPEND)) < 0 ) {
		perror(argv[1]);
		return -1;
	}
	if((out = open(argv[2], O_RDONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR)) < 0 ) {
		perror(argv[2]);
		return -1;
	}

	while ((n = read(out, buf, sizeof(buf))) > 0 )
		write(in, buf, n);


	close(out);
	close(in);

	return 0;

}
