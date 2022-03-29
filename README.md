# Sludge
Tape ARchive, or tar, is a file format and a utility used to store and manipulate collections of files. The original intention behind the tar archive was to aggregate the contents of many files to improve the read/write performance of tape drives. These days we use it to create archives to store and transmit multiple files as a single file. Let's write a simplified version of tar. Note that you are NOT reimplementing tar. Let's call this tar-like utility sludge.
You may NOT simply wrap the functionality of another archiving tool, but must instead implement it from scratch and in C. You are allowed to use any standard libraries. sludge
should be able to recognize command-line options for creating a new archive with a specified name and a list of files, adding to an existing archive with a specified name and a list of files, listing the contents of an existing archive, and extracting some or all of the files from an existing archive.
We should be able to interact with sludge as follows:
List the contents of the archive_name.sludge:
./sludge -l archive_name.sludge
Add files file.4 and file.5 to archive_name.sludge (if archive_name.sludge doesn't exist, it is created):
./sludge -a archive_name.sludge file.4 file.5
Extract all files from archive_name.sludge:
./sludge -e archive_name.sludge
Extract only file.2 and file.5 from archive_name.sludge:
./sludge -e archive_name.sludge file.2 file.5

Let's say we have a single file that is 20 bytes long and we want to archive it and the archive doesn't yet exist, we'd do something like:
./sludge -a archive_name.sludge file.1
File archive_name.sludge should be created and have the following structure:
+---------------------------+
| name: "file.1"            |
| size: 20                  |
+---------------------------+
| <file.1 data>             |
|                           |
+---------------------------+

So now, if we add a second (200 byte) file to the archive with:
./sludge -a archive_name.sludge file.2
the resulting archive should look like:
+---------------------------+
| name: "file.1"            |
| size: 20                  |
+---------------------------+
| <file.1 data>             |
|                           |
+---------------------------+
| name: "file.2"            |
| size: 200                 |
+---------------------------+
| <file.2 data>             |
|                           |
+---------------------------+


