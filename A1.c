const char *mypath[] = {
	"./",
	"/usr/bin/",
	"/bin/",
	NULL
};
/* To be embedded in a suitable function ... */
	while (...){
		/* Wait for input */
		printf ("prompt> ");
		fgets (...);
	while (( ... = strsep (...)) != NULL){
		...
	}
	/* If necessary locate executable using mypath array */
	/* Launch executable */
	if (fork () == 0) {
	...
	execv (...);
		...
	} 
	else {
		wait(…);
	}
}
