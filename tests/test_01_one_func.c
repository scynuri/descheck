int the_global_var;

int the_one_func(void)
{
	int the_local_obj = 0x34; //default value ... just too see if it's remembered in dwarf info
	return the_local_obj;
}

int main(int argc, char * argv[])
{
	int main_local_variable = 0x77;
	return main_local_variable + the_one_func() + the_global_var;
}
