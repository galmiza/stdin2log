# Welcome

Stdin2log is a **developer tool** that helps manage the **logs of command line applications**.  
Basically it converts the text stream on the standard input (stdin) into a log file structure with support for **log rotation** and **log compression**.

# Usage

Pipe the stdout/stderr stream of your command line application into stdin2log (more about options later).

```
$ myVerboseApp | ./stdin2log -o /tmp/myLog -s 16777216 -n 4 -z 9
```

After some time, your logs may look like this:

```
$ ls -l /tmp/myLog*
-rw-r--r--  1 guillaume  staff  12484818 Nov 10 15:34 myLog.log   # current log being populated by stdin2log (always under 16MB)
-rw-r--r--  1 guillaume  staff    369180 Nov 10 14:08 myLog.24.log.gz # \
-rw-r--r--  1 guillaume  staff    277013 Nov 10 14:01 myLog.25.log.gz # |_ the 4 latest compressed logs (each with a maximum uncompressed size of 16MB)
-rw-r--r--  1 guillaume  staff    457202 Nov 10 13:55 myLog.26.log.gz # |
-rw-r--r--  1 guillaume  staff    398216 Nov 10 12:10 myLog.27.log.gz # /
```

# Options

### (o)utput

Basename of the logs.  
Logs file will be named \<basename\>.\<index\>.log.gz if compression is activated, \<basename\>.\<index\>.log if compression is disabled.

### (s)ize

Maximum size of an uncompressed log file in bytes.

### (n)umber

Maximum number of log files.  
The oldest log files are dropped as new log files are created.

### compre(z)ion

Level of compression.  
From 1 (fastest) to 9 (best).

# Compilation

Stdin2log is written in c++.  
It doesn't rely on any framework or platform but you need to compile it for the target architecture.

### Install g++ compiler

On Debian

```
$ apt-get update -y && apt-get upgrade -y
$ apt-get install -y g++
```

On Centos

```
$ yum update -y && yum upgrade -y
$ yum install -y gcc-c++
```

### Compile

```
$ g++ -std=c++17 main.cpp -o stdin2log
```

Yeah that's it. C++ isn't that horrible after all, right?

# Test

Check it runs properly and without error.

```
$ echo -e "first line\nsecond one\nok one more" | ./stdin2log -o logTest
$ ls -l logTest* # show the logs
```

Feed some random strings into stdin2log.

```
$ dd if=/dev/urandom bs=1024 count=256 | hexdump | ./stdin2log -o test1 -n 6 -s $((1024*32)) -z 9 # set compression, limit to 6 files
$ dd if=/dev/urandom bs=1024 count=256 | hexdump | ./stdin2log -o test2 -n 2 -s $((1024*32)) # disable compression, limit to 2 files
$ ls -l test* # show the logs
-rw-r--r--  1 guillaume  staff  11577 Nov 10 16:24 test1.23.log.gz  # \
-rw-r--r--  1 guillaume  staff  11566 Nov 10 16:24 test1.24.log.gz  # |
-rw-r--r--  1 guillaume  staff  11533 Nov 10 16:24 test1.25.log.gz  # |
-rw-r--r--  1 guillaume  staff  11545 Nov 10 16:24 test1.26.log.gz  # |_ first test, 6 compressed log files
-rw-r--r--  1 guillaume  staff  11508 Nov 10 16:24 test1.27.log.gz  # |
-rw-r--r--  1 guillaume  staff  11570 Nov 10 16:24 test1.28.log.gz  # |
-rw-r--r--  1 guillaume  staff    232 Nov 10 16:24 test1.log    # /
-rw-r--r--  1 guillaume  staff  32760 Nov 10 16:24 test2.27.log # \
-rw-r--r--  1 guillaume  staff  32760 Nov 10 16:24 test2.28.log # |_ second test, 2 uncompressed log files
-rw-r--r--  1 guillaume  staff    232 Nov 10 16:24 test2.log    # /
$ rm test1.*.log.gz test2.*.log test*.log # clean the mess
```

# Roadmap

* Handle errors
* Add helper (-h)
* Support units for parameter size (e.g. 4m or 3kB)
* Option to add formatted date into filename
