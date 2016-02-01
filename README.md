Space Shooter
=============
This is a small game I developed for testing the Hummingbird2D game framework.

Compiling & running
===================
```
# Make directory for project
mkdir /root/folder/
cd /root/folder/

# Clone necessary repos
git clone https://github.com/Galbar/Hummingbird2D.git Hum2D
git clone https://github.com/Galbar/Hummingbird2D-SFML.git SFML
git clone https://github.com/Galbar/space-shooter.git space-shooter

# Compile framework
cd Hum2D
make

# Compile SFML-based extension
cd ../SFML
make

# Compile game
cd ../space-shooter
make

# Add libraries to the linker path
export LD_LIBRARY_PATH=/root/folder/Hum2D/lib/:/root/folder/SFML/lib/

# Run game
./game
```

Controls
========
`W`, `A`, `S`, `D`: Move around

`Mouse pointer`: aim

`Mouse left button`: shoot (click for one, keep pressed for spray)
