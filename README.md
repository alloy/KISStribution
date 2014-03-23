A simple way to distribute a bundle of files in one binary.

* Unpacks data, either pure tar or lz4 compressed tar, to a temporary directory
  from which actual program is run.
* No need to compile when creating a distribution bundle. Only a linker is
  required to add the data and create the final executable.
