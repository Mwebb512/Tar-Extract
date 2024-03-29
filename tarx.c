//Matthew Webb
//tarx.c 
//C code that mimics how the OS extracts a tar file
//3/27/24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
#include <unistd.h>
#include <time.h>
#include "jrb.h"
#include "fields.h"
#include "dllist.h"
#include <dirent.h>


//all the info required
typedef struct file_info{
	int filename_size, mode;
    long inode, size;
    long mtime;
    char filename[256];
    struct file_info *link;
    unsigned char* data;
} Info;

// Function to create directory structure and files from tarfile
int extract_tar(Info *tar_file, JRB inodes) {

    int exists;
    int filename_size, mode;
    struct stat buf, buf2;
    long mtime;
    time_t atime;
    struct utimbuf new_times; 
    long size, inode;
    
    
    //get size of name
    if(fread(&filename_size, sizeof(int), 1, stdin) != 1){
       // printf("returned\n"); 

        if (feof(stdin)) {
           return -1; 
        }else{
            fprintf(stderr, "Error with Filename size %d \n");
        }
    }
    tar_file->filename_size = filename_size;

    //check if name size is valid
    if(filename_size <= 0) {
        fprintf(stderr, "Error with Filename size %d \n", filename_size);
        exit(0);
    }
    
    
    //get name  
    char path2[256];
    if (fread(&path2, sizeof(char), filename_size, stdin) != filename_size){
        fprintf(stderr, "Bad tarc file.  Couldn't read name\n");
        exit(1);
    }

  
    //copy the name to struct
    path2[filename_size] = '\0';
    strcpy(tar_file->filename, path2);
    
    //get inode
    if(fread(&inode, sizeof(long), 1, stdin) != 1){
        fprintf(stderr, "Bad tarc file   %s. Couldn't read inode\n", tar_file->filename);
        exit(1);
    };
    tar_file->inode = inode;

    //check to see if inode exist, if it does we need link
    JRB find = jrb_find_int(inodes, inode);
    if(find != NULL){
        tar_file->link = (Info*) jrb_val(find).v;
        return 0;
    }

    //if no link set link to NULL
    tar_file->link = NULL;
    jrb_insert_int(inodes, inode, new_jval_v(tar_file));

    

    //get the mode
    if(fread(&mode, sizeof(int), 1, stdin) != 1){
        fprintf(stderr, "Error with mode %d \n", mode);
        exit(1);
    }
    tar_file->mode = mode;
    
   
    //get mtime 
    if(fread(&mtime, sizeof(long), 1, stdin) != 1){
        fprintf(stderr, "Error with mtime %d \n", mtime);
        exit(1);
    }

    //mtime = mtime;
    //new_times.modtime = mtime;
    //tar_file->mtime = new_times.modtime;
    tar_file->mtime = mtime;

    //is directory?
    if(S_ISDIR(mode)){
       return 0; 
    }
    
    //get the file size
    if(fread(&size, sizeof(long), 1, stdin) != 1){
        fprintf(stderr, "Error with File size %d \n", size);
        exit(1);
    }
    tar_file->size = size;

    //check the file
    if (size <= 0 || size >= 100000000) {
        fprintf(stderr, "issue with file size %d \n", size);
        exit(1);
    }

    //get the data of the file
    unsigned char data[size];
    if(fread(&data, sizeof(unsigned char), size, stdin) != size){
        fprintf(stderr, "Error with data %s \n", tar_file->filename);
        exit(1);
    }

    //copy the data to the file struct
    tar_file->data = (unsigned char*) malloc(sizeof(unsigned char) * size);
    memcpy(tar_file->data, data, size);

   // printf("Inode: %d ", buf.st_ino); 
   // printf("mode: %o ", buf.st_mode); 
    //printf("Size: %ld\n", size); 
   // printf("Modification Time: %d\n", buf.st_mtime); 
   // printf("Name: %s\n", path2);


}

//creates the directories and files
void create_directories(JRB inodes, Dllist files, Dllist directories, Info *tf){

    struct utimbuf new_times; 
    Dllist tmp;
    
    //create directories first
    dll_traverse(tmp, directories){
        Info* tf2 = (Info*) tmp->val.v;
        
        //check if direcrtory was made
        if (mkdir(tf2->filename, 0777) == -1) {
            perror("failed to make dir");
            exit(EXIT_FAILURE);
        }
	}

    //create files
    dll_traverse(tmp, files){
       
        Info* tf2 = (Info*) tmp->val.v;

        // Create a hard link if the file already had an inode
        if (tf2->link != NULL) {
            link(tf2->link->filename, tf2->filename);
            
        }else{
            //printf("data: %s\n", tf2->filename);
            //printf("data: %s\n", tf2->data);
            //printf("size of data: %d\n", sizeof(tf2->data));
            //printf("data: %d\n", tf2->size);

            // Write the file contents
            FILE *file = fopen(tf2->filename, "wb");

            //printf("open\n");

            //write the data to the file
            fwrite(tf2->data, sizeof(unsigned char), tf2->size, file);
            fclose(file);
            //printf("made\n");

            // Set the file mode
            chmod(tf2->filename, tf2->mode);
        
            //set the time
            new_times.modtime = tf2->mtime;
            new_times.actime = tf2->mtime;
            utime(tf2->filename, &new_times);
            // Set the file modification time
            //utime(tf2->filename, tf2->mtime);
        }
    }
    //printf("made\n");

    //need to reset the modification times for the directories since they changed when we mad the files and also need to set the protections
    dll_traverse(tmp, directories){
        Info* tf2 = (Info*) tmp->val.v;
        chmod(tf2->filename, tf2->mode);
        
        new_times.modtime = tf2->mtime;
        new_times.actime = tf2->mtime;
        utime(tf2->filename, &new_times);
        //utime(tf2->filename, &tf2->mtime);

        free(tf2);
    }
   
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
     //   fprintf(, "Usage: %s <tarfile>\n", argv[0]);
      //  return EXIT_FAILURE;
    }

    int exists;
    int filename_size;
    int inode;
    
    //char *tarfile = argv[1];
    struct stat buf, buf2;
    int mode;
    off_t size;
    JRB inodes = make_jrb();
    Dllist files = new_dllist();
    Dllist directories = new_dllist();
    Dllist tmp;

    //time
    time_t mtime;
    time_t atime;
    struct utimbuf new_times; 
    Info *tf2;

   // FILE *fp; // = fopen(tarfile, "r");

    //if (fp == NULL) {
       // perror("Error opening tarfile");
        //exit(EXIT_FAILURE);
    //}

    while(1){

        
        tf2 = (Info*) malloc(sizeof(Info));

        //printf("made\n");
        
        if(extract_tar(tf2, inodes) == -1){
            free(tf2);
            break;
        }

        //printf("data: %s\n", tf2->filename);
        //printf("data: %s\n", tf2->data);
        //printf("size of data: %d\n", sizeof(tf2->data));
        //printf("data: %d\n\n", tf2->size);

        //printf("data: %s\n", tar_file->filename);

        //add to directory or files
        if(S_ISDIR(tf2->mode)){
            dll_append(directories, new_jval_v(tf2));
        }else{
            dll_append(files, new_jval_v(tf2));
        }

       
    }
    
    //create directories
    create_directories(inodes, files, directories, tf2);
    //extract_tar(path2, fp);

    //free memory
    dll_traverse(tmp, files){
        Info* tf = (Info*) tmp->val.v;
        free(tf->data);
        free(tf);
    }
   // fclose(fp);
    free_dllist(files);
    free_dllist(directories);
    jrb_free_tree(inodes);

    return 1;
}