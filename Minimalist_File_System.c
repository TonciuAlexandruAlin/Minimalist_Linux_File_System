#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

void touch (Dir* parent, char* name) {

	File* current = parent->head_children_files;
	// aloc memorie pentru un nou fisier
	File *x = (File*) malloc (sizeof(File));
	x->name = (char*) malloc ((strlen(name) + 1) * sizeof(char));
	x->parent = parent;

	// daca nu exista niciun fisier
	if (current == NULL){
		
		strcpy(x->name, name);
		x->next = NULL;
		x->parent = parent;
		parent->head_children_files = x;
		return;
	}

	// merg la sfarsitul listei
	while(current->next){
		// verific daca fisierul este deja in lista
		if(strcmp(current->name, name) == 0){
			printf("File already exists\n");
			free(x->name);
			free(x);
			return;
		}
		current = current->next;
	}

	// verific daca ultimul fisier are numele "name"
	if(strcmp(current->name, name) == 0){
		printf("File already exists\n");
		free(x->name);
		free(x);
		return;
	}
	// creez fisierul la sfarsitul listei
	else{
		strcpy(x->name, name);
		x->next = NULL;
		x->parent = parent;
		current->next = x;
	}
	
}

void mkdir (Dir* parent, char* name) {

	Dir* current = parent->head_children_dirs;
	// aloc memorie pentru un nou director
	Dir *x = (Dir*) malloc (sizeof(Dir));
	x->name = (char*) malloc ((strlen(name) + 1) * sizeof(char));
	x->head_children_files = NULL;
	x->head_children_dirs = NULL;
	x->parent = parent;

	// daca nu mai exista niciun director
	if (current == NULL){
		
		strcpy(x->name, name);
		x->next = NULL;
		x->parent = parent;
		parent->head_children_dirs = x;
		return;
	}

	// merg la sfarsitul listei
	while(current->next){
		// verific daca directorul este deja in lista
		if(strcmp(current->name, name) == 0){
			printf("Directory already exists\n");
			free(x->name);
			free(x);
			return;
		}
		current = current->next;
	}

	// verific daca ultimul director are numele "name"
	if(strcmp(current->name, name) == 0){
		printf("Directory already exists\n");
		free(x->name);
		free(x);
		return;
	}
	// creez directorul la sfarsitul listei
	else{
		strcpy(x->name, name);
		x->next = NULL;
		x->parent = parent;
		current->next = x;
	}
	
}

void ls (Dir* parent) {

	Dir* currDir = parent->head_children_dirs;
	File* currFile = parent->head_children_files;

	// afisez directoarele
	while(currDir){

		printf("%s\n", currDir->name);
		currDir = currDir->next;
	}

	// afisez fisierele
	while(currFile){

		printf("%s\n", currFile->name);
		currFile = currFile->next;
	}

}

void rm (Dir* parent, char* name) {

	File* current = parent->head_children_files;

	// daca lista e goala
	if(current == NULL){

		printf("Could not find the file\n");
		return;
	}

	// daca primul fisier este elementul care trebuie sters
	if(strcmp(current->name, name) == 0){

		parent->head_children_files = current->next;
		free(current->name);
		free(current);
		return;
	}

	// parcurg lista
	while(current->next){

		// verific daca exista elementul de sters in lista
		if(strcmp(current->next->name, name) == 0){
			
			File* aux = current->next;
			current->next = current->next->next;
			free(aux->name);
			free(aux);
			return;
		}
		current = current->next;
	}

	// daca nu exista elementul in lista
	printf("Could not find the file\n");

}

void rmdir (Dir* parent, char* name) {

	Dir* current = parent->head_children_dirs;

	// daca lista este goala
	if(current == NULL){

		printf("Could not find the dir\n");
		return;
	}

	// daca primul director este elementul care trebuie sters
	if(strcmp(current->name, name) == 0){

		parent->head_children_dirs = current->next;
		free(current->name);
		// reinitializez mereu contorul cu head-ul directoarelor si sterg recursiv
		// tot ce se afla intr-un director.
		for(Dir* i = current->head_children_dirs; i != NULL; i = current->head_children_dirs){

			rmdir(current, i->name);

		}

		// reinitializez mereu contorul cu head-ul fisierelor si sterg toate 
		// fisierele aflate intr-un director
		for(File* i = current->head_children_files; i != NULL; i = current->head_children_files){
			
			rm(current, i->name);
		}
		free(current);
		return;
	}

	// parcurg lista si fac acelasi lucru ca mai sus
	while(current->next){

		if(strcmp(current->next->name, name) == 0){
			
			Dir* aux = current->next;
			current->next = current->next->next;
			free(aux->name);
			for(Dir* i = aux->head_children_dirs; i != NULL; i = aux->head_children_dirs){

				rmdir(aux, i->name);
			}

			for(File* i = aux->head_children_files; i != NULL; i = aux->head_children_files){

				rm(aux, i->name);
			}
			free(aux);
			return;
		}
		current = current->next;
	}

	printf("Could not find the dir\n");

}

void cd(Dir** target, char *name) {

	// verific daca urc in ierarhia directoarelor ca sa pot sa ma duc in parent
	if(strcmp(name, "..") == 0){

		// daca ma aflu in root
		if((*target)->parent == NULL){
			
			return;
		}
		
		*target = (*target)->parent;
		return;
	}

	Dir* current = (*target)->head_children_dirs;

	// parcurg lista
	while(current){

		if(strcmp(current->name, name) == 0){

			// schimba directorul curent cu directorul target 
			*target = current;
			return;
		}
		current = current->next;
	}

	// daca nu gasesc directorul care trebuie schimbat
	printf("No directories found!\n");

}

char *pwd (Dir* target) {

	char* rez;
	// cazul de baza in care ajung la root
	if(strcmp(target->name, "home") == 0){

		// aloc memorie sirului care trebuie returnat si adaug "/home" in sir
		rez = (char*) malloc ((strlen(target->name) + 2) * sizeof(char));
		strcpy(rez,"/");
		strcat(rez, target->name);
	}
	else{
		
		// apelez functia recursiv pana ajung la root
		rez = pwd(target->parent);
		// realoc memorie pentru a avea loc tot sirul in variabila rezultat
		rez = (char*) realloc (rez, strlen(rez) + strlen(target->name) + 2);
		// adaug "/" + numele directorului in sir
		strcat(rez, "/");
		strcat(rez, target->name);
	}

	return rez;

}

void stop (Dir* target) {

	// apelez recursiv rmdir pe root pentru a se elibera toata memoria
	Dir* current = (Dir*) malloc (sizeof(Dir));
	current->head_children_dirs = target;
	rmdir(current, target->name);
	free(current);

}

void tree (Dir* target, int level) {

	Dir* currDir = target->head_children_dirs;
	File* currFile = target->head_children_files;

	// parcurg directoarele
	while(currDir){

		// pentru fiecare level adaug 4 spatii alaturi de numele directorului si
		// apelez recursiv functia pentru toate elementele dintr-un director
		for(int i = 0; i < level; i++){
			printf("    ");
		}
		printf("%s\n", currDir->name);
		tree(currDir, level + 1);
		currDir = currDir->next;
		
	}

	// parcurg fisierele
	while(currFile){

		// pentru fiecare level adaug 4 spatii alaturi de numele fisierului
		// dintr-un director
		for(int i = 0; i < level; i++){
			printf("    ");
		}
		printf("%s\n", currFile->name);
		currFile = currFile->next;
	}
	
}

void mv(Dir* parent, char* oldname, char* newname) {

	Dir* currDir = parent->head_children_dirs;
	File* currFile = parent->head_children_files;
	Dir* copieDir;
	File* copieFile;

	int contorDirNewname = 0, contorDirOldName = 0, contorFileNewName = 0, contorFileOldName = 0;

	//verific daca se regasesc deja elementele care trebuiesc schimbate
	if(currDir != NULL){

		//verific daca se gaseste pe prima pozitie elementul cu numele "newname"
		if(strcmp(currDir->name, newname) == 0){

			printf("File/Director already exists\n");
			return;
		}
		
		//parcurg lista ca sa verific daca exista deja newname 
		while(currDir->next != NULL){

			//daca il gasesc, printez si ies din functie
			if(strcmp(currDir->next->name, newname) == 0){

				printf("File/Director already exists\n");
				return;
			}

			currDir = currDir->next;
		}

	}

	if(currFile != NULL){

		//verific daca se gaseste pe prima pozitie elementul cu numele "newname"
		if(strcmp(currFile->name, newname) == 0){

			printf("File/Director already exists\n");
			contorFileNewName = 1;
			return;
		}
		
		//parcurg lista ca sa verific daca exista deja newname
		while(currFile->next != NULL){

			//daca il gasesc, printez si ies din functie
			if(strcmp(currFile->next->name, newname) == 0){

				printf("File/Director already exists\n");
				return;
			}

			currFile = currFile->next;
		}
		
	}

	//resetez iteratoarele ca sa pot parcurge listele din nou
	currDir = parent->head_children_dirs;
	currFile = parent->head_children_files;

	//daca lista exista
	if (currDir != NULL){

		//verific daca gasesc oldname pe prima pozitie
		if(strcmp(currDir->name, oldname) == 0){

			if(currDir->next != NULL){
				
				//copiez nodul si schimb head-ul
				copieDir = currDir;
				parent->head_children_dirs = currDir->next;
				contorDirOldName = 1;

			}
			else{
				
				//daca e singur in lista, schimb doar numele 
				free(currDir->name);
				currDir->name = (char*) malloc ((strlen(newname) + 1) * sizeof(char));
				strcpy(currDir->name, newname);
				return;
			}

		}
		// parcurg lista
		while(currDir->next != NULL){

			//daca exista oldname
			if(strcmp(currDir->next->name, oldname) == 0){
				
				//copiez nodul si fac legaturile 
				copieDir = currDir->next;
				currDir->next = currDir->next->next;
				contorDirOldName = 1;
			}
			currDir = currDir->next;
		}

	}

	//daca lista exista
	if (currFile != NULL){

		//verific daca gasesc oldname pe prima pozitie
		if(strcmp(currFile->name, oldname) == 0){

			if(currFile->next != NULL){
				
				//copiez nodul si schimb headul
				copieFile = currFile;
				parent->head_children_files = currFile->next;
				contorFileOldName = 1;

			}
			else{
				
				//daca e singur in lista doar schimb numele
				free(currFile->name);
				currFile->name = (char*) malloc ((strlen(newname) + 1) * sizeof(char));
				strcpy(currFile->name, newname);
				return;
			}

		}
		//parcurg lista
		while(currFile->next != NULL){

			//daca exista oldname
			if(strcmp(currFile->next->name, oldname) == 0){

				//copiez nodul si fac legaturile 
				copieFile = currFile->next;
				currFile->next = currFile->next->next;
				contorFileOldName = 1;
			}
			currFile = currFile->next;
		}
	}
	
	//daca am gasit directorul cu oldname
	if(contorDirOldName == 1){

		//adaug nodul la coada listei, setez nextul newname-ului cu null si schimb numele.
		currDir->next = copieDir;
		currDir->next->next = NULL;
		strcpy(currDir->next->name, newname);
		return;
	}
	
	//daca am gasit directorul cu oldname
	if(contorFileOldName == 1){

		//adaug nodul la coada listei, setez nextul newname-ului cu null si schimb numele.
		currFile->next = copieFile;
		currFile->next->next = NULL;
		strcpy(currFile->next->name, newname);
		return;
	}

	//daca nu gasesc fisierul/directorul
	printf("File/Director not found\n");
}

int main () {
	
	int boolean = 1;
	// aloc memorie pentru sirul in care citesc comenzile
	char *s = (char*) malloc (MAX_INPUT_LINE_SIZE * sizeof(char));
	// creez root-ul si initialiez toate campurile lui
	Dir *home = (Dir*) malloc(sizeof(Dir));
	home->head_children_dirs = NULL;
	home->head_children_files = NULL;
	home->parent = NULL;
	home->name = (char*) malloc(5 * sizeof(char));
	strcpy(home->name, "home");
	// creez o copie cu care voi efectua toate operatiile
	Dir *curr = home;

	do
	{
		// citesc linie cu linie de la tastatura
		fgets(s, MAX_INPUT_LINE_SIZE, stdin);
		// daca citesc comanda "touch" apelez functia touch
		if(strncmp(s, "touch", 5) == 0) {

			char* token = strtok(s, " ");
			char* token2 = strtok(NULL, " \n");
			if (token2 != NULL){

				touch(curr, token2);
			}
		}
		// daca citesc comanda "mkdir" apelez functia mkdir
		else if(strncmp(s, "mkdir", 5) == 0){

			char* token = strtok(s, " ");
			char* token2 = strtok(NULL, " \n");
			if (token2 != NULL){

				mkdir(curr, token2);
			}
		}
		// daca citesc comanda "ls" apelez functia ls
		else if(strncmp(s, "ls", 2) == 0){

			ls(curr);
		}
		// daca citesc comanda "rmdir" apelez functia rmdir
		else if(strncmp(s,"rmdir",5) == 0){

			char* token = strtok(s, " ");
			char* token2 = strtok(NULL, " \n");
			if (token2 != NULL){

				rmdir(curr, token2);
			}
		}
		// daca citesc comanda "rm" apelez functia rm
		else if(strncmp(s, "rm", 2) == 0){
			
			char* token = strtok(s, " ");
			char* token2 = strtok(NULL, " \n");
			if (token2 != NULL){

				rm(curr, token2);
			}
		}
		// daca citesc comanda "cd" apelez functia cd
		else if(strncmp(s, "cd", 2) == 0){

			char* token = strtok(s, " ");
			char* token2 = strtok(NULL, " \n");
			if (token2 != NULL){

				cd(&curr, token2);
			}
		}
		// daca citesc comanda "pwd" apelez functia pwd
		else if(strncmp(s, "pwd", 3) == 0){

			char* rez = pwd(curr);
			printf("%s\n", rez);
			free(rez);
		}
		// daca citesc comanda "tree" apelez functia tree
		else if(strncmp(s, "tree", 4) == 0){

			tree(curr, 0);
		}
		// daca citesc comanda "mv" apelez functia mv
		else if(strncmp(s, "mv", 2) == 0){
			char* token = strtok(s, " ");
			char* token2 = strtok(NULL, " ");
			char* token3 = strtok(NULL, " \n");
			if (token2 != NULL && token3 != NULL){

				mv(curr, token2, token3);
			}
		}
		// daca citesc comanda stop, apelez functia stop si dezaloc memoria
		// pentru sirul in care citesc comenzile
		else if(strncmp(s, "stop", 4) == 0){
			
			stop(home);
			free(s);
			// ies din do while
			break;
		}

	} while (boolean == 1);
	
	return 0;
}
