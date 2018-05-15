# Teler
Teler is a distributed, streamlined version control system (VCS) for the student use case.
## Building
To build `teler`, navigate to the root of the repository and run
```
make
```
This will place a `teler` executable within `./bin/`. Copy this file, if desired, to a directory in your local path (e.g. `~/.local/bin`).
### Dependencies
* [clang](https://clang.llvm.org/)
* [openssl](https://www.openssl.org/)
* [zlib](https://www.zlib.net/)
## Basic Usage
| Command | Description |
| --- | --- |
| `teler init` | Create a empty `teler` repository at the current working directory. |
| Push | Track, save, and tag all changes as a commit. |
| Pull | Load the latest commit into the root of the repository. Use with the `-c` option allows the user to specify a certain commit to load. |
| History | Print to `stdout` the history of commits of the repository, latest to earliest. |
## Usage Example
First navigate to a desired directory in which you would like to create a `teler` repository. Once within the directory,
execute
```
teler init
```
to initialize an empty `teler` repository. You will be prompted to enter your name as well as your email address for author tracking purposes.

Once you have reached a place where you would like to commit your changes, simply run
```
teler push
```
which will prompt you to enter a commit message. This will save all work as a commit into the repository.

To view the history of commits, run
```
teler history
```

If you would like to revert the repository to one of these previous commits, run
```
teler pull -c <commit>
```
where `<commit>` is the hash of the commit found by running `teler history`. If you would simply like to revert the repository to the latest
commit, omit the `-c` flag. **NOTE:** Running `teler pull` will remove all files within the repository which are not committed.
## Contributors
Hadley Luker [lukerhad] and Zachary J. Susag [susagzac]
