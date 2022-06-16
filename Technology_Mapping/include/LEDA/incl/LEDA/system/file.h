/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  file.h
+
+
+  Copyright (c) 1995-2007
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.3 $  $Date: 2007/02/28 07:44:16 $

#ifndef __LEDA_FILE__
#define __LEDA_FILE__

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 520402
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>
#include <LEDA/core/list.h>

LEDA_BEGIN_NAMESPACE

extern __exportF void   get_time(int& ye,int& mo,int& da, int& h,int& m,int& s);
extern __exportF string time_string();

extern __exportF void play_wave_file(string fname);

extern __exportF bool create_process(string cmd);
extern __exportF bool create_process_wait(string cmd);
extern __exportF void get_command_line(int& argc, char**& argv);
extern __exportF void message_box(const char* msg, const char* label);

extern __exportF bool apm_get_status(int& status, int& percent, int& minutes);
extern __exportF void get_host_info(char* name, unsigned long sz, 
                                                list<unsigned long>& idlist);


/*{\Manpage {file} {} {Files and Directories}  }*/

/*{\Mtext
The following functions are declared in \<LEDA/file.h\>.
}*/

extern __exportF string set_directory(string new_dir);
/*{\Mfuncl     sets the current working directory to |new_dir| and 
               returns the name of the old cwd. }*/

extern __exportF string get_directory();
/*{\Mfuncl     returns the name of the current working directory. }*/

extern __exportF string get_home_directory();
/*{\Mfuncl     returns the name of the user's home directory. }*/

extern __exportF string get_directory_delimiter();
/*{\Mfuncl     returns the character that delimits directory names in a path
               (i.e.~``$\backslash$'' on Windows and ``/'' on Unix).}*/

extern __exportF void append_directory_delimiter(string& dir);
/*{\Mfuncl     appends the directory delimiter to |dir| if |dir| does not 
               already end with the delimiter.}*/

extern __exportF void remove_trailing_directory_delimiter(string& dir);
/*{\Mfuncl     removes the directory delimiter from |dir| if |dir| ends with 
               it.}*/

extern __exportF list<string> get_directories(string dir);
/*{\Mfuncl     returns the list of names of all sub-directories in 
               directory |dir|. }*/

extern __exportF list<string> get_files(string dir);
/*{\Mfuncl     returns the list of names of all regular files in 
               directory |dir|. }*/

extern __exportF list<string> get_files(string dir, string pattern);
/*{\Mfuncl     returns the list of names of all regular files in 
               directory |dir| matching pattern. }*/

extern __exportF list<string> get_entries(string dir);
/*{\Mfuncl     returns the list of all entries (directory and files) of 
               directory |dir|. }*/

extern __exportF bool create_directory(string fname);
/*{\Mfuncl     creates a directory with  name |dname|, returns
               |true| on success. }*/

extern __exportF bool is_directory(string fname);
/*{\Mfuncl     returns true if |fname| is the path name of a directory
               and false otherwise. }*/

extern __exportF bool is_file(string fname);
/*{\Mfuncl     returns true if |fname| is the path name of a regular file
               and false otherwise. }*/

extern __exportF bool is_link(string fname);
/*{\Mfuncl     returns true if |fname| is the path name of a symbolic link
               and false otherwise. }*/

extern __exportF int size_of_file(string fname);
/*{\Mfuncl     returns the size of file |fname| in bytes. }*/

extern __exportF string tmp_file_name();
/*{\Mfuncl     returns a unique name for a temporary file. }*/

extern __exportF bool delete_file(string fname);
/*{\Mfuncl     deletes file |fname| returns true on success and 
               false otherwise. }*/

extern __exportF string first_file_in_path(string fname, string path, 
                                                         char sep = ':');
/*{\Mfuncl     searches all directories in string $path$ (separated by $sep$)
               for the first directory $dir$ that contains a file with
               name $fname$ and returns $dir/fname$ (the empty string
               if no such directory is contained in path). }*/



#if LEDA_ROOT_INCL_ID == 520402
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



LEDA_END_NAMESPACE

#endif
