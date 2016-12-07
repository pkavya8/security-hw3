-------------------------------------------------------------------------------------------------------------------------------------
Kavya Premkumar
UNI - kp2652
------------------------------------------------------------------------------------------------------------------------------------- 
Commands: 
The program accepts a config file whice can contain lines of the form :
DocumentRoot <path>
OutputArea <path>
Alias <path> <path> [Upto 20]

Aliases are allowed only for one directory:
Eg: 
Correct : Alias \foo \bar
Wrong : Alias \foo\bar \baz

DocumentRoot and OutputArea must be present in input. Aliases are optional.
The document root, output area and alias values are expected to be given with no trailing "/". 
Eg : 
Correct : \foo
Wrong  : \foo\

The Alias is expected to be given with trailing "/". 
Eg : 
Correct : \foo\
Wrong : \foo

Therefore, Correct input for document root is : DocumentRoot "/home/abc"
Correct input for output area is : OutputArea "/home/abcd"
Correct input for alias is : Alias "/home/" "/bar"

NOTE : No input sanitization is done and hence the paths given are assumed to be correct.

The program takes standard input that can contain two files names separated by one whitespace. The program checks for the input file name to be present in the DocumentRoot. If it is present, it copies the file to the destination file name in the OutputArea. If it does not exist in DocumentRoot, it checks for aliases and resolves the path. If that does not exist too, it prints an error. The program handles cases for . and .. in input file names. 

The OutputArea is confined using chroot(). This is done by forking a process and execing to copy the files. chroot() takes care of not letting the files to be copies outside the outputarea even in the presence of "." and "..".

The program stops accepting input on Ctrl+C.
----------------------------------------------------------------------------------------------------------------------------------------------

Test cases :

In all the test files, I first change directory to /tmp/kp2652. 

test1.txt checks for happy path where all inputs are legal.
test2.txt has an invalid argument for password. However it would still work if the keyfile, encrypt and decrypt files exist.
test3.txt fails because there is no keyfile set before encrypt or decrypt. It also has an unknown command.
test4.txt checks for various legal and illegal arguments like the ones mentioned aove.
test5.txt creates a directory and changes into it. However it cannot encrypt if the files are not present in the new directory.
test6.txt checks illegal and legal arguments for mkdir and cd.
----------------------------------------------------------------------------------------------------------------------------------------------
Makefile :

make test creates a folder in /tmp/kp2652 and copies all my files to it.
It then executes the code with all the test files. The resulting encrypted and decrypted files are also stored in /tmp/kp2652 since I change directory in all test files.

make clean removes this folder. 

