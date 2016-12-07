-------------------------------------------------------------------------------------------------------------------------------------
Kavya Premkumar
UNI - kp2652
------------------------------------------------------------------------------------------------------------------------------------- 
Commands: 
The program accepts a config file whice can contain lines of the form :
DocumentRoot <path>
OutputArea <path>
Alias <path> <path> [Upto 20]

DocumentRoot and OutputArea must be present in input. Aliases are optional.

Aliases are allowed only for one directory:
Eg: 
Correct : Alias /foo /bar
Wrong : Alias /foo/bar /baz

The document root, output area and alias values are expected to be given with no trailing "/". 
Eg : 
Correct : /foo
Wrong  : /foo/

The Alias is expected to be given with trailing "/". 
Eg : 
Correct : /foo/
Wrong : /foo

Therefore, Correct input for document root is : DocumentRoot "/home/abc"
Correct input for output area is : OutputArea "/home/abcd"
Correct input for alias is : Alias "/home/" "/bar"

NOTE : No input sanitization is done and hence the paths given are assumed to be correct.

The program takes standard input that can contain two files names separated by one whitespace. The program checks for the input file name to be present in the DocumentRoot. If it is present, it copies the file to the destination file name in the OutputArea. If it does not exist in DocumentRoot, it checks for aliases and resolves the path. If that does not exist too, it prints an error. The program handles cases for . and .. in input file names. However if at anypoint the input resolves outside the document root, I return error even if it comes back into the root after that.

The OutputArea is confined using chroot(). This is done by forking a process and execing to copy the files. chroot() takes care of not letting the files to be copied outside the outputarea even in the presence of "." and "..".

The program stops accepting input on Ctrl+C.
----------------------------------------------------------------------------------------------------------------------------------------------
Makefile :
make creates hwcopy.o.
Run program as ./hwcopy -c configfile [With your own configfile]
NOTE : program needs to be run as root or sudo.