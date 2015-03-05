#include "print_header.h"
#include "server.h"

int main(int argc, char *argv[]) {
	server *s = new server(5001);
	int x;
	x = s->bringup();
	cout<<x<<endl;
}
