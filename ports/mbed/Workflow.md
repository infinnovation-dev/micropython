```
  +------------------------------+
  |github:micropython/micropython|
  |Upstream MicroPython project  |
  |..............................|
  |Discuss general issues        |
  +------------------------------+
            |           ^
            |           | PR for general stuff
  git fetch |    +--------------------------------------+
  git merge |    |github:infinnovation/micropython      |
            |    |Public repo for the port (mbed branch)|
            |    |......................................|
            |    |Issues for mbed port                  |
            |    +--------------------------------------+
            |           ^
            |           | git push
  +-------------------------+
  |offline:micropython (git)|
  |.........................|
  |Develop python tools etc.|
  |Sync with upstream       |
  +-------------------------+
                ^  |
  mbedsync pull |  | mbedsync push
                |  v
  +-----------------------------------+
  |offline:micropython-{dev,repl} (hg)|
  |...................................|
  |Develop C/C++ code                 |
  +-----------------------------------+
            ^  |            ^
  hg fetch  |  | hg push    | copy Makefile
  hg update |  |        +--------------------+
            |  |        |micropython-repl.zip|
            |  |        |From mbed IDE       |
            |  |        +--------------------+
            |  |\           ^
            |  | \          |
            |  |  -----------------.
            |  v            |      v
  +---------------------+   |   +-------------------------------+
  |hg:micropython-dev   |   |   |hg:micropython{,-repl} (Public)|
  |on developer.mbed.org|   |   |on developer.mbed.org          |
  +---------------------+   |   +-------------------------------+
          ^  |              |
  Publish |  | Update       | Export from IDE
          |  v              |
  +---------------------------+
  |IDE:micropython-dev        |
  |within IDE:micropython-repl|
  |...........................|
  |Check compilation          |
  +---------------------------+
```
