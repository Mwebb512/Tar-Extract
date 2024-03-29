CS360 Lab #5 -- Tarc
James S. Plank
CS360
Lab written in February, 2019.
Lab directory: /home/jplank/cs360/labs/Lab-5-Tarc
Lab writeup: http://web.eecs.utk.edu/~jplank/plank/classes/cs360/360/labs/Lab-5-Tarc/index.html
There is a makefile in makefile. It assumes that tarc.c is in the src directory.
Here's a video that TA Sonaz Jazny made to help in 2023: https://www.youtube.com/watch?v=0oz7jRsuHjI.
What you submit
You submit the file tarc.c. The TA's will copy this to their own src directory on the lab machines, and then compile with:
UNIX> make -f /home/jplank/cs360/labs/Lab-5-Tarc/makefile
You may use Libfdr for this assignment.
Introduction
Tarc and tarx are your first real systems programs. They will mimic the functionality of the program tar, which bundles up files and directories into a single file, and which can then use that file to recreate the files and directories.
You will write tarc in Lab 5, and tarx in Lab 6. Tarc, stands for "tar create", and works similarly to "tar cf". Tarx, stands for "tar extract", and works similarly to "tar xf".

Both of these are explained in great detail in the two lab writeups, with plenty of examples.

Description of bin/tarc
Tarc takes a single argument on the command line. This should be the name of a directory. It can be an absolute pathname, or a relative pathname. Tarc will then create a tarfile, which has enough information to recreate the directory and all of its contents. It prints this tarfile on standard output.
The tarfile has a specific format. For each file and directory, it will print the following information:

The size of the file's name, as a four-byte integer in little endian.
The file's name. No null character.
The file's inode, as an eight byte long in little endian.
If this is the first time that we have encountered a file with that inode, then it will print the following additional information:
The file's mode, as a four byte integer in little endian.
The file's last modification time, in seconds, as an eight byte long in little endian.
Finally, if the file is a file, and not a directory, then it prints the following final information:
The file's size, as an eight byte long in little endian.
The file's bytes.
This is enough information for you to recreate the directory and its contents, including hard links. A few things of note:
As in the prsize lecture, you should ignore "." and "..".
You should also ignore symbolic links -- if you encounter one, simply omit it from the tarfile.
Although you don't have to generate your tarfile to be an exact copy of my tarfile, it has to be readable by my programs. More on that below.
In the tarfile, a directory must appear before any files in that directory. However, otherwise, directories and files may appear in any order.
If the specified directory is a relative or absolute pathname with the '/' character in it, then you need to split that pathname into a prefix and a suffix. In your tarfile, you should only include the suffixes.
Remember that when you extract files from a tarfile (either a regular tar file or ones created by bin/tarc), the inodes will be different from what the were in the tar file.
To explore bin/tarc go ahead and work in a fresh directory. Obviously, do this on a lab machine:
UNIX> ls
UNIX>  # First, unpack d1.tar in the lab directory.  This is a regular tar file.
UNIX> tar xpfv /home/plank/cs360/labs/Lab-5-Tarc/d1.tar
d1/
d1/f1.txt
d1/f2.txt
d1/f3.txt
d1/sub/
d1/sub/f1.txt
UNIX> # Next, let's look at the contents of d1.  You can see f1.txt, f2.txt and f3.txt.
UNIX> # f1.txt and f3.txt are hard links.  F2.txt and sub/f1.txt have different protections:
UNIX> ls -li
total 0
894388406 drwxr-xr-x. 3 plank loci 55 Feb 16 10:00 d1
UNIX> ls -li d1
total 12
894428207 -rw-r--r--. 2 plank loci  3 Feb 13 17:00 f1.txt
894750231 -rw-------. 1 plank loci  6 Feb 14 08:51 f2.txt
894428207 -rw-r--r--. 2 plank loci  3 Feb 13 17:00 f3.txt
963609657 drwxr-xr-x. 2 plank loci 19 Feb 14 09:44 sub
UNIX> ls -li d1/sub
total 4
963609658 -r--------. 1 plank loci 5 Feb 15 18:04 f1.txt
UNIX>  # Let's see what's in those fx.txt files:
UNIX> grep . d1/f?.txt
d1/f1.txt:Hi
d1/f2.txt:There
d1/f3.txt:Hi
UNIX> cat d1/sub/f1.txt
THOR
UNIX> 
Now, I'll create a tarfile using bin/tarc in the lab directory, and I'll examine its contents using xxd (read the man page).
UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/tarc d1 > d1.tarc
UNIX> xxd -g 1 d1.tarc
0000000: 02 00 00 00 64 31 b6 48 4f 35 00 00 00 00 ed 41  ....d1.HO5.....A
0000010: 00 00 78 25 68 5c 00 00 00 00 09 00 00 00 64 31  ..x%h\........d1
0000020: 2f 66 31 2e 74 78 74 2f e4 4f 35 00 00 00 00 a4  /f1.txt/.O5.....
0000030: 81 00 00 71 93 64 5c 00 00 00 00 03 00 00 00 00  ...q.d\.........
0000040: 00 00 00 48 69 0a 09 00 00 00 64 31 2f 66 32 2e  ...Hi.....d1/f2.
0000050: 74 78 74 17 ce 54 35 00 00 00 00 80 81 00 00 66  txt..T5........f
0000060: 72 65 5c 00 00 00 00 06 00 00 00 00 00 00 00 54  re\............T
0000070: 68 65 72 65 0a 09 00 00 00 64 31 2f 66 33 2e 74  here.....d1/f3.t
0000080: 78 74 2f e4 4f 35 00 00 00 00 06 00 00 00 64 31  xt/.O5........d1
0000090: 2f 73 75 62 39 84 6f 39 00 00 00 00 ed 41 00 00  /sub9.o9.....A..
00000a0: c5 7e 65 5c 00 00 00 00 0d 00 00 00 64 31 2f 73  .~e\........d1/s
00000b0: 75 62 2f 66 31 2e 74 78 74 3a 84 6f 39 00 00 00  ub/f1.txt:.o9...
00000c0: 00 00 81 00 00 61 45 67 5c 00 00 00 00 05 00 00  .....aEg\.......
00000d0: 00 00 00 00 00 54 48 4f 52 0a                    .....THOR.
UNIX> 
We should be able to account for every byte in this file. To wit:
Bytes 0-3: Filename size = 0x00000002 = 2.
Bytes 4-5: Filename: 0x64 0x31 = "d1".
Bytes 6-13: Inode: 0x00000000354f48b6 = 894388406.
Bytes 14-17: Mode: 0x000041ed = 100 000 111 101 101. (Directory)
Bytes 18-25: Modification time = 0x000000005c682578 = 1550329208.
Bytes 26-29: Filename size = 0x00000009 = 9.
Bytes 30-38: Filename: "d1/f1.txt".
Bytes 39-46: Inode: 0x00000000354fe42f = 894428207.
Bytes 47-50: Mode: 0x000081a4 = 001 000 000 110 100 100.
Bytes 51-58: Modification time = 0x000000005c649371 = 1550095217.
Bytes 59-66: File size = 0x0000000000000003 = 3.
Bytes 67-69: The bytes: 0x48, 0x69, 0x0a = "Hi\n".
Bytes 70-73: Filename size = 0x00000009 = 9.
Bytes 74-82: Filename: "d1/f2.txt".
Bytes 83-90: Inode: 0x000000003554ce17 = 894750231.
Bytes 91-94: Mode: 0x00008180 = 001 000 000 110 000 000.
Bytes 95-102: Modification time = 0x000000005c657266 = 1550152294.
Bytes 103-110: File size = 0x0000000000000006 = 6.
Bytes 111-116: The bytes: 0x54, 0x68, 0x65, 0x72, 0x65, 0x0a= "There\n".
Bytes 117-120: Filename size = 0x00000009 = 9.
Bytes 121-129: Filename: "d1/f3.txt".
Bytes 130-137: Inode: 0x00000000354fe42f = 894428207.
Since this file has the same inode as "f1.txt", it's a hard link, we're done.
Bytes 138-141: Filename size = 0x00000006 = 6
Bytes 142-147: Filename: "d1/sub".
Bytes 148-155: Inode: 0x00000000396f8439 = 963609657.
Bytes 156-159: Mode: 0x000041ed = 100 000 111 101 101. (Directory)
Bytes 160-167: Modification time = 0x000000005c657ec5 = 1550155461.
Bytes 168-171: Filename size = 0x0000000d = 13.
Bytes 172-184: Filename: "d1/sub/f1.txt".
Bytes 185-192: Inode: 0x00000000396f843a = 963609658.
Bytes 193-196: Mode: 0x00008100 = 001 000 000 100 000 000.
Bytes 197-204: Modification time = 0x000000005c674561 = 1550271841.
Bytes 205-212: File size = 0x0000000000000005 = 5.
Bytes 213-217: The bytes: 0x54, 0x48, 0x4f, 0x52, 0x0a = "THOR\n".
Spend some time on the list above and how it maps to the bytes of the tarfile. You need to understand this exactly to do the lab.
Now, suppose I specify d2, but with a prefix -- "././d1/../d1" -- that specifies the same directory as before, but in a different way. As you'll notice, the two tarfiles are identical, because the prefix has been stripped off:

UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/tarc ././d1/../d1 > d1a.tarc
UNIX> diff d1.tarc d1a.tarc
UNIX> 
Suppose that you recreate the directory d1 from its original tar file. You'll see that the names, modes, bytes and modification times are guaranteed to be the same as the first time, but but the inodes will be different. We can see that reflected in the tarfile.
UNIX> rm -rf d1 d1a.tarc
UNIX> tar xpfv /home/plank/cs360/labs/Lab-5-Tarc/d1.tar
d1/
d1/f1.txt
d1/f2.txt
d1/f3.txt
d1/sub/
d1/sub/f1.txt
UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/tarc ././d1/../d1 > d1b.tarc
UNIX> diff d1.tarc d1b.tarc
Binary files d1.tarc and d1b.tarc differ
UNIX> xxd -g 1 d1.tarc | head -n 6
0000000: 02 00 00 00 64 31 b6 48 4f 35 00 00 00 00 ed 41  ....d1.HO5.....A // -- inode of d1
0000010: 00 00 78 25 68 5c 00 00 00 00 09 00 00 00 64 31  ..x%h\........d1
0000020: 2f 66 31 2e 74 78 74 2f e4 4f 35 00 00 00 00 a4  /f1.txt/.O5..... // -- inode of d1/f1.txt
0000030: 81 00 00 71 93 64 5c 00 00 00 00 03 00 00 00 00  ...q.d\.........
0000040: 00 00 00 48 69 0a 09 00 00 00 64 31 2f 66 32 2e  ...Hi.....d1/f2.
0000050: 74 78 74 17 ce 54 35 00 00 00 00 80 81 00 00 66  txt..T5........f // -- inode of d1/f2.txt
UNIX> xxd -g 1 d1b.tarc | head -n 6
0000000: 02 00 00 00 64 31 28 14 52 3d 00 00 00 00 ed 41  ....d1(.R=.....A // -- inode of d1
0000010: 00 00 78 25 68 5c 00 00 00 00 09 00 00 00 64 31  ..x%h\........d1
0000020: 2f 66 31 2e 74 78 74 29 14 52 3d 00 00 00 00 a4  /f1.txt).R=..... // -- inode of d1/f1.txt
0000030: 81 00 00 71 93 64 5c 00 00 00 00 03 00 00 00 00  ...q.d\.........
0000040: 00 00 00 48 69 0a 09 00 00 00 64 31 2f 66 32 2e  ...Hi.....d1/f2.
0000050: 74 78 74 2a 14 52 3d 00 00 00 00 80 81 00 00 66  txt*.R=........f // -- inode of d1/f2.txt
UNIX> 
The mrd program
The program bin/mrd is a program that will create a directory full of files and directories that exhibit the range of behaviors that we're expecting you to handle with bin/tarc. It is called as follows:
usage: mrd seed dir-level fpd randprot(n|y|d) linkperc stem
The arguments are:

seed for srand48(). Yes, it's a bad RNG, but it's fine for this purpose.
dir-level: this is the maximum number of nested subdirectories. They will always be a chain of directories of length dir-level. There may be more than one.
fpd: Maximum files per directory.
randprot: If 'y', set the protection bits of files randomly. The "user read" bit and the "world read" will always be set. If 'd', also set the protection bits of directories randomly. The "user/world read" and "user/world execute" bits will always be set.
linkperc the percentage of times that the program will create hard links to existing files rather than new files.
stem this is the directory into which to put the files.
Here's an example:
UNIX> ls
UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/mrd
usage: mrd seed dir-level fpd randprot(n|y|d) linkperc stem
UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/mrd 2 1 15 y 50 ex1
UNIX> ls -li --full-time ex1
total 52
553947704 -rwx-w-r--. 1 plank loci 74 2009-07-13 23:07:50.000000000 -0400 BBQEF
553947709 -r-x---r--. 3 plank loci  2 2009-01-12 15:53:33.000000000 -0500 dBVq4ob3
553947709 -r-x---r--. 3 plank loci  2 2009-01-12 15:53:33.000000000 -0500 eIPt2ytg1e7gwhmc
553947706 -r-x--xr-x. 3 plank loci  3 2009-03-31 15:06:37.000000000 -0400 Frhxr
553947707 -r-----r-x. 1 plank loci 69 2009-10-23 07:25:54.000000000 -0400 HHOSU2o5
553947706 -r-x--xr-x. 3 plank loci  3 2009-03-31 15:06:37.000000000 -0400 JikTtd0eo
553947710 -r---wxr--. 2 plank loci 70 2009-05-14 15:08:31.000000000 -0400 kQV5
553947706 -r-x--xr-x. 3 plank loci  3 2009-03-31 15:06:37.000000000 -0400 mAVDOy
553947710 -r---wxr--. 2 plank loci 70 2009-05-14 15:08:31.000000000 -0400 sAEuZwlRtaTPX5v
553947711 -r-----r-x. 1 plank loci 11 2009-10-05 07:53:15.000000000 -0400 wb31yIuVqOuDZ
553772035 -r-----r--. 1 plank loci 50 2009-01-14 21:20:06.000000000 -0500 yqOzTZXl3SZ94Vkv
553947708 -r-x-w-r-x. 1 plank loci 45 2009-10-22 00:43:57.000000000 -0400 YTn9k
553947709 -r-x---r--. 3 plank loci  2 2009-01-12 15:53:33.000000000 -0500 ZUl
UNIX> 
As you can see, it created a directory ex1 with 13 files, many of which are hard links to each other (e.g. dBVq4ob3 and eIPt2ytg1e7gwhmc). The bytes in the files are random, BTW, so if you want to look at them, use xxd.
One of the nice things about bin/mrd is that if you give it the same parameters, it always creates the same directory structure with the same file/directory names and the same modification/access times. Therefore, you can use it to help test your bin/tarc programs.

In the following example, I specify random protections on directories too:

UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/mrd 2 2 5 d 50 ex2
UNIX> ls -li ex2
total 4
846068234 dr-x-w-r-x. 2 plank loci 45 Apr 21  2009 iqOuDZX
674147657 dr-xrw-r-x. 2 plank loci 26 Jun 17  2009 JfHOSU2o56Nxb31y
895575575 dr-x---r-x. 2 plank loci 51 Jun 25  2009 kQV5
615228457 -r-----r--. 2 plank loci 73 Apr  9  2009 S0eo1jYBQ
756542011 drwx-w-r-x. 2 plank loci  6 Oct 29  2009 XxUlqOhUik
UNIX> ls -li ex2/iqOuDZX
total 8
615228457 -r-----r--. 2 plank loci 73 Apr  9  2009 eIPt2ytg1e7gwhmc
895575577 -r--r--r-x. 3 plank loci 15 Sep 17  2009 zRtaTPX5v
UNIX> ls -li ex2/JfHOSU2o56Nxb31y
total 4
895575577 -r--r--r-x. 3 plank loci 15 Sep 17  2009 gdBVq4ob376lA
UNIX> ls -li ex2/kQV5
total 12
895575577 -r--r--r-x. 3 plank loci 15 Sep 17  2009 al3SZ94Vkv3aOCTn
895575578 -r--r-xr-x. 2 plank loci 52 May  4  2009 DVD
895575578 -r--r-xr-x. 2 plank loci 52 May  4  2009 tZUqO
UNIX> ls -li ex2/XxUlqOhUik
total 0
UNIX> 
You'll note that ex2/S0eo1jYBQ and ex2/iqOuDZX/eIPt2ytg1e7gwhmc are hard links to each other.
One thing about these directories -- when the protection has changed so that the write bit isn't set, you can't delete them, even with "rm -rf", without changing the protection back. For example, if I try to delete ex2, I get an error:

UNIX> rm -rf ex2
rm: cannot remove 'ex2/JfHOSU2o56Nxb31y/gdBVq4ob376lA': Permission denied
rm: cannot remove 'ex2/iqOuDZX/eIPt2ytg1e7gwhmc': Permission denied
rm: cannot remove 'ex2/iqOuDZX/zRtaTPX5v': Permission denied
rm: cannot remove 'ex2/kQV5/al3SZ94Vkv3aOCTn': Permission denied
rm: cannot remove 'ex2/kQV5/tZUqO': Permission denied
rm: cannot remove 'ex2/kQV5/DVD': Permission denied
UNIX> 
To delete it, I need to first recursively set the protections to something I can delete:
UNIX> chmod -R 0755 ex2
UNIX> rm -rf ex2
UNIX> 
The tarf program
You call bin/tarf in one of two ways:
bin/tarf < tarfile       -- Prints out information about the tarfile.
bin/tarf debug < tarfile -- Also prints out information while reading the tarfile.
The information that bin/tarf prints out is sorted by filename. Here's an example. I'm going to recreate d1 again, so that it will have different inodes than before:
UNIX> tar xpfv /home/plank/cs360/labs/Lab-5-Tarc/d1.tar
d1/
d1/f1.txt
d1/f2.txt
d1/f3.txt
d1/sub/
d1/sub/f1.txt
UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/tarc d1 > d1.tarc
UNIX> 
Now, I call bin/tarf on it, and it prints out information about all of the files/directories, sorted by inode:
UNIX> /home/plank/cs360/labs/Lab-5-Tarc/bin/tarf < d1.tarc
Inode 362606622 Mode 40755 Hash 00000000 Mtime 1550329208
Name: d1

Inode 362606623 Mode 100644 Hash 0b87f2c0 Mtime 1550095217
Name: d1/f1.txt
Name: d1/f3.txt

Inode 362606624 Mode 100600 Hash d363a247 Mtime 1550152294
Name: d1/f2.txt

Inode 425644584 Mode 40755 Hash 00000000 Mtime 1550155461
Name: d1/sub

Inode 425644585 Mode 100400 Hash 0e17e06c Mtime 1550271841
Name: d1/sub/f1.txt

UNIX> 
You see that the two hard links are printed together. Also, I calculate a hash of each file's contents. Directories have hash values of zero. Tarf is nice because if you have two tarfiles for the same directory, even though the tarfiles may differ (because of the order in which you specify files and directories), the output of bin/tarf will be the same. If I'm bored someday, I'll demonstrate it. For now, just accept that it's true.
If you call bin/tarf with "debug", it prints out the information as it reads it in from the tarfile. This may be helpful to you for debugging when your tarfiles have errors.

The gradescript goes through the following steps for gradescript i:

It calls my bin/tarc on the file in Gradescript-Examples/i. It puts the tarfile into tmp-i-correct-tarfile.txt.
It calls your bin/tarc on the file in Gradescript-Examples/i. It puts the tarfile into tmp-i-your-tarfile.txt.
When it makes that call, it limits you to ten file descriptors, so be aware of that. (It does that by calling your program with the program bin/limit-shell) in the lab directory. You can read the source code of that if you care to in src/limit-shell.c.
If your program had an error, it will stop there.
It calls bin/tarf on my tarfile. That goes into tmp-i-correct-stdout.txt.
It calls bin/tarf on your tarfile. That goes into tmp-i-your-stdout.txt.
It tests to see that those two files are identical.
Ten Open Files
On the issue of open files -- I'm limiting your process to 10 open files, because I want you to pay attention to keeping that number low. For that reason you need to:
Call fclose() when you're done with a file that you have opened with fopen().
Call closedir() when you're done with a directory that you have opened with opendir().
Test the return values of fopen() and opendir(), and if they are NULL, then use perror() to print the error, and then exit. This will save you countless hours of headaches on segmentation violations.
To test yourself while you're debugging, do one of the following using csh or bash:
UNIX> csh -c "limit descriptors 10; bin/tarc d1 > d1.tarc"
or using bash
UNIX> bash -c "ulimit -n 10; bin/tarc d1 > d1.tarc"
That way you execute the command in a sub-shell that is limited to 10 open files, but you don't limit your own shell to 10 open files.
Useful System Calls
You will most definitely need to use the following system calls. Make sure you read their man pages:
lstat(). Obviously, stat()/lstat() have their own lecture, so make sure you go through those lecture notes. You'll be making use of the st_mtime field of the struct stat, which on some machines is not straightforward. Fortunately, on our lab machines, it is straightforward, so I encourage you to read the man page and develop your code on a lab machine. If you don't, just be aware that different machines have different mechanisms regarding st_mtime. For example, on my Macintosh (OS X 10.11.6), the struct stat does not have st_mtime, but instead has:
struct timespec st_mtimespec;
Later in the man page, it says:

The time-related fields of struct stat are as follows:

st_atime         Time when file data last accessed.  Changed by the mknod(2),
                 utimes(2) and read(2) system calls.

st_mtime         Time when file data last modified.  Changed by the mknod(2),
                 utimes(2) and write(2) system calls.
So, what gives? Well, if you look at /usr/include/sys/stat.h, you'll see:

#define st_mtime st_mtimespec.tv_sec
This means that you can use st_mtime as if it is a member of the struct stat, but it is a little confusing, isn't it?

chmod(). This is nice and straightforward.
