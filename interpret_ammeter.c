/* default */
	dnum0 = 2;
	ch = 1;
	strcpy(flname,"current.txt");
    /***********************************/
    /*** 0, Interpret the arguments  ***/
    /***********************************/
	while((argc > 1) && (argv[1][0] == '-')) {
		switch (argv[1][1]){
		case 'd':
			if(argv[1][2]=='\0') {
			--argc;
			++argv;
			dnum0 = atoi(argv[1]);
			}
			else dnum0 = atoi(&argv[1][2]);
			break;
		case 'n':
			if(argv[1][2]=='\0') {
			--argc;
			++argv;
			ch = atoi(argv[1]);
			}
			else ch = atoi(&argv[1][2]);
			break;
		case 'f':
			if(argv[1][2]=='\0') {
			--argc;
			++argv;
			strcpy(flname,argv[1]);
			}
			break;
		default :
			fprintf(stderr, "Bad option: %s\n", argv[1]);
		}
		--argc;  // decriment the count
		++argv;  // move to next option
}
