-- Note -- 
Simple multithreading with 101 commits

-- Original readme --

Coursework project by Martynas Antipenkovas
Game physics programming and game graphics programming modules

Warning:
1) This program has an option for updating symbols, which might cause long first-time compilations as it might be resolving those 
symbols. Disabling this (In Visual Studio 2019 (tested)Tools->Debugging->Symbols->Symbol file (.pdb) locations and the unchecking) might 
cause unresolved symbols issues, which could lead to memory access violation when opening or closing application, but as they are 
inconsistent restarting the application should solve these issues.

2)OpenGL libraries are in the include folder

3)If the .exe is not present the program must be built and then be placed in "HovercraftCW_MA" folder as it uses relative path to find 
the OpenGL library files

4)While creating this project there were several cases of when the project just freezes on launch - possible solution close (ctrl+c)
in the console terminal

5) 'Multithreaded' branch is the same as master, but 'Multithreaded_independant' is different, this branch contains code for running the
game in independant threads at their own speeds instead of going once per frame. The reason why it is not included in final project is
because it would still cause problems and is not stable enough. Main problem - collisions don't always work as expected 
(for example, aligning hovercrafts back with a plane collider and then rotating will let hovercraft go with no struggle)

Youtube video - https://youtu.be/pPoYNK6I3yM

Executables:
- HovercraftCW_MA_SINGLE_THREAD.exe for signlethreaded experience
- HovercraftCW_MA_MULTITHREADED.exe for multithreaded experience

Control scheme (that is also visible in the splash screen):
Debugging:
F3 - Enable Debug mode
F4 - Disable Debug mode
T - Print out items in the grid
Camera:
F5 - Fixed point
F6 - Top to down
F7 - Third person
Intergration:
[ (F9 in single thread project) - Explicit Eulers
] (F10 in single thread project) - Semi Explicit Eulers
\ (F11 in single thread project) - Velocity Verlet
Movement:
W - Forward
S - Backwards
E - Rotate Right
Q - Rotate  Left

[For Testing purposes]
D - Move Right
A - Move Left

General:
Esc - Close application
N - Enable MSAA X8
M - Dissable MSAA X8
, and . - Rotate Light
