Space Shooter
=============
This is a small game I developed for testing the Hummingbird2D game framework.

Compiling & running
===================
```
# Make directory for project
mkdir space-shooter
cd space-shooter

# Clone necessary repos
git clone -b TFG https://github.com/Galbar/hummingbird.git
git clone -b TFG https://github.com/Galbar/hummingbird-MOGL.git MOGL
git clone https://github.com/Galbar/space-shooter.git space-shooter
git clone https://github.com/g-truc/glm.git

# Compile framework
cd hummingbird
make

# Compile SFML-based extension
cd ../MOGL
make

# Compile game
cd ../space-shooter
make

# Run game
LD_LIBRARY_PATH=../hummingbird/lib:../MOGL/lib ./game
```

Controls
========
`W`, `A`, `S`, `D`: Move around

`Mouse pointer`: aim

`Mouse left button`: shoot (click for one, keep pressed for spray)
